#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#include <windows.h>
#include <shellapi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <d3d9.h>
#include "MinHook.h"
#include "promoted_rvas.h"

#define AFX_TIMESTAMP  0x6A58EC87u
#define AFX_IMAGE_SIZE 0x0023F000u
#define MAX_UNIQUE_SURFACES 16
#define MAX_LOGGED_PUSHES 192

typedef void (__cdecl *PushWrapper_t)(IDirect3DSurface9 *replacementSurface);

static PushWrapper_t g_originalPush = NULL;
static HANDLE g_log = INVALID_HANDLE_VALUE;
static CRITICAL_SECTION g_logLock;
static volatile LONG g_pushCount = 0;
static volatile LONG g_probeCount = 0;

typedef struct SurfaceKey_s {
    UINT width;
    UINT height;
    D3DFORMAT format;
    D3DMULTISAMPLE_TYPE msaa;
    DWORD quality;
} SurfaceKey;

static SurfaceKey g_seen[MAX_UNIQUE_SURFACES];
static LONG g_seenCount = 0;

static int has_exact_insecure_arg(const wchar_t *cmdline) {
    if (!cmdline || !*cmdline) return 0;
    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(cmdline, &argc);
    if (!argv) return 0;
    int found = 0;
    for (int i = 0; i < argc; ++i) {
        if (_wcsicmp(argv[i], L"-insecure") == 0) { found = 1; break; }
    }
    LocalFree(argv);
    return found;
}

static void log_line(const char *s) {
    if (g_log == INVALID_HANDLE_VALUE || !s) return;
    DWORD n = (DWORD)strlen(s), w = 0;
    EnterCriticalSection(&g_logLock);
    WriteFile(g_log, s, n, &w, NULL);
    WriteFile(g_log, "\r\n", 2, &w, NULL);
    FlushFileBuffers(g_log);
    LeaveCriticalSection(&g_logLock);
}

static void log_fmt(const char *fmt, ...) {
    char buf[2048];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf)-1, fmt, ap);
    va_end(ap);
    buf[sizeof(buf)-1] = 0;
    log_line(buf);
}

static void open_log(void) {
    wchar_t tmp[MAX_PATH], root[MAX_PATH], pidDir[MAX_PATH], logPath[MAX_PATH];
    DWORD n = GetTempPathW(MAX_PATH, tmp);
    if (!n || n >= MAX_PATH) return;
    _snwprintf(root, MAX_PATH-1, L"%sCSGOVR_T0_RT_FORMAT_COMPAT", tmp);
    root[MAX_PATH-1] = 0;
    CreateDirectoryW(root, NULL);
    _snwprintf(pidDir, MAX_PATH-1, L"%s\\PID_%lu", root, (unsigned long)GetCurrentProcessId());
    pidDir[MAX_PATH-1] = 0;
    CreateDirectoryW(pidDir, NULL);
    _snwprintf(logPath, MAX_PATH-1, L"%s\\compat.log", pidDir);
    logPath[MAX_PATH-1] = 0;
    g_log = CreateFileW(logPath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
                        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

static int verify_pe(HMODULE m) {
    if (!m) return 0;
    uint8_t *base = (uint8_t*)m;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER*)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;
    IMAGE_NT_HEADERS32 *nt = (IMAGE_NT_HEADERS32*)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;
    if (nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) return 0;
    if (nt->FileHeader.TimeDateStamp != AFX_TIMESTAMP) {
        log_fmt("ABORT timestamp actual=0x%08X expected=0x%08X", nt->FileHeader.TimeDateStamp, AFX_TIMESTAMP);
        return 0;
    }
    if (nt->OptionalHeader.SizeOfImage != AFX_IMAGE_SIZE) {
        log_fmt("ABORT SizeOfImage actual=0x%08X expected=0x%08X", nt->OptionalHeader.SizeOfImage, AFX_IMAGE_SIZE);
        return 0;
    }
    return 1;
}

static int verify_push_signature(const uint8_t *p) {
    if (!p) return 0;
    if (CSGOVR_PUSH_WRAPPER_SIG_LEN < 6u || CSGOVR_PUSH_WRAPPER_SIG_LEN > 64u) return 0;
    return memcmp(p, CSGOVR_PUSH_WRAPPER_SIG, CSGOVR_PUSH_WRAPPER_SIG_LEN) == 0;
}

static const char *fmt_name(D3DFORMAT f) {
    switch (f) {
        case D3DFMT_X8R8G8B8: return "X8R8G8B8";
        case D3DFMT_A8R8G8B8: return "A8R8G8B8";
        case D3DFMT_A8B8G8R8: return "A8B8G8R8";
        case D3DFMT_X8B8G8R8: return "X8B8G8R8";
        case D3DFMT_A2B10G10R10: return "A2B10G10R10";
        case D3DFMT_A2R10G10B10: return "A2R10G10B10";
        case D3DFMT_A16B16G16R16F: return "A16B16G16R16F";
        default: return "OTHER";
    }
}

/* Microsoft D3D9 -> D3D11 OpenSharedResource contract maps these D3D9
   layouts to the documented DXGI R8G8B8A8, R10G10B10A2 and R16G16B16A16F
   shared formats. Other D3D9 formats may still be shareable D3D9<->D3D9,
   but are not classified DIRECT_OK for the cross-API T0 path here. */
static int cross_api_format_allowed(D3DFORMAT f) {
    return f == D3DFMT_A8B8G8R8 ||
           f == D3DFMT_A2B10G10R10 ||
           f == D3DFMT_A16B16G16R16F;
}

static int same_key(const SurfaceKey *a, const D3DSURFACE_DESC *d) {
    return a->width == d->Width && a->height == d->Height &&
           a->format == d->Format && a->msaa == d->MultiSampleType &&
           a->quality == d->MultiSampleQuality;
}

static int claim_unique_surface(const D3DSURFACE_DESC *d) {
    LONG n = g_seenCount;
    for (LONG i = 0; i < n && i < MAX_UNIQUE_SURFACES; ++i) {
        if (same_key(&g_seen[i], d)) return 0;
    }
    if (n >= MAX_UNIQUE_SURFACES) return 0;
    LONG slot = InterlockedIncrement(&g_seenCount) - 1;
    if (slot < 0 || slot >= MAX_UNIQUE_SURFACES) return 0;
    g_seen[slot].width = d->Width;
    g_seen[slot].height = d->Height;
    g_seen[slot].format = d->Format;
    g_seen[slot].msaa = d->MultiSampleType;
    g_seen[slot].quality = d->MultiSampleQuality;
    return 1;
}

static void release_texture(IDirect3DTexture9 **p) {
    if (p && *p) { IDirect3DTexture9_Release(*p); *p = NULL; }
}

static void probe_surface(IDirect3DSurface9 *surface, void *caller) {
    if (!surface) return;

    D3DSURFACE_DESC desc;
    HRESULT hrDesc = IDirect3DSurface9_GetDesc(surface, &desc);
    LONG pushNo = InterlockedIncrement(&g_pushCount);
    if (FAILED(hrDesc)) {
        if (pushNo <= MAX_LOGGED_PUSHES)
            log_fmt("PUSH n=%ld tid=%lu caller=%p surface=%p GetDesc=0x%08lX",
                    pushNo, GetCurrentThreadId(), caller, surface, (unsigned long)hrDesc);
        return;
    }

    if (pushNo <= MAX_LOGGED_PUSHES) {
        log_fmt("PUSH n=%ld tid=%lu caller=%p surface=%p %ux%u fmt=%u/%s msaa=%u quality=%lu usage=0x%08lX pool=%u",
                pushNo, GetCurrentThreadId(), caller, surface,
                desc.Width, desc.Height, (unsigned)desc.Format, fmt_name(desc.Format),
                (unsigned)desc.MultiSampleType, (unsigned long)desc.MultiSampleQuality,
                (unsigned long)desc.Usage, (unsigned)desc.Pool);
    }

    if (!claim_unique_surface(&desc)) return;
    LONG probeNo = InterlockedIncrement(&g_probeCount);

    IDirect3DDevice9 *dev = NULL;
    HRESULT hrDev = IDirect3DSurface9_GetDevice(surface, &dev);
    if (FAILED(hrDev) || !dev) {
        log_fmt("CAPS probe=%ld surface=%p GetDevice=0x%08lX VERDICT=NO_DEVICE",
                probeNo, surface, (unsigned long)hrDev);
        return;
    }

    IDirect3DDevice9Ex *devEx = NULL;
    HRESULT hrEx = IDirect3DDevice9_QueryInterface(dev, &IID_IDirect3DDevice9Ex, (void**)&devEx);

    D3DDEVICE_CREATION_PARAMETERS cp;
    ZeroMemory(&cp, sizeof(cp));
    HRESULT hrCp = IDirect3DDevice9_GetCreationParameters(dev, &cp);

    IDirect3D9 *d3d = NULL;
    HRESULT hrD3d = IDirect3DDevice9_GetDirect3D(dev, &d3d);

    D3DDISPLAYMODE dm;
    ZeroMemory(&dm, sizeof(dm));
    HRESULT hrMode = E_FAIL;
    HRESULT hrCarrierFormat = E_FAIL;
    HRESULT hrConversion = E_FAIL;
    if (SUCCEEDED(hrCp) && SUCCEEDED(hrD3d) && d3d) {
        hrMode = IDirect3D9_GetAdapterDisplayMode(d3d, cp.AdapterOrdinal, &dm);
        if (SUCCEEDED(hrMode)) {
            hrCarrierFormat = IDirect3D9_CheckDeviceFormat(
                d3d, cp.AdapterOrdinal, cp.DeviceType, dm.Format,
                D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A8B8G8R8);
        }
        hrConversion = IDirect3D9_CheckDeviceFormatConversion(
            d3d, cp.AdapterOrdinal, cp.DeviceType, desc.Format, D3DFMT_A8B8G8R8);
    }

    HANDLE directHandle = NULL;
    IDirect3DTexture9 *directTex = NULL;
    HRESULT hrDirectCreate = E_FAIL;
    if (SUCCEEDED(hrEx) && devEx && desc.MultiSampleType == D3DMULTISAMPLE_NONE) {
        hrDirectCreate = IDirect3DDevice9_CreateTexture(
            dev, desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET,
            desc.Format, D3DPOOL_DEFAULT, &directTex, &directHandle);
    }

    HANDLE carrierHandle = NULL;
    IDirect3DTexture9 *carrierTex = NULL;
    HRESULT hrCarrierCreate = E_FAIL;
    if (SUCCEEDED(hrEx) && devEx && SUCCEEDED(hrCarrierFormat)) {
        hrCarrierCreate = IDirect3DDevice9_CreateTexture(
            dev, desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET,
            D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &carrierTex, &carrierHandle);
    }

    int directAllowedFmt = cross_api_format_allowed(desc.Format);
    int directOk = SUCCEEDED(hrEx) && desc.MultiSampleType == D3DMULTISAMPLE_NONE &&
                   directAllowedFmt && SUCCEEDED(hrDirectCreate) && directHandle != NULL;
    int carrierOk = SUCCEEDED(hrEx) && SUCCEEDED(hrCarrierFormat) &&
                    SUCCEEDED(hrConversion) && SUCCEEDED(hrCarrierCreate) && carrierHandle != NULL;

    const char *verdict = directOk ? "A1A_DIRECT_D3D9_SIDE_READY" :
                          carrierOk ? "A1B_GPU_CARRIER_D3D9_SIDE_READY" :
                                      "D3D9_SIDE_INCONCLUSIVE_OR_BLOCKED";

    log_fmt("CAPS probe=%ld surface=%p fmt=%u/%s msaa=%u exQI=0x%08lX creation=0x%08lX getD3D=0x%08lX mode=0x%08lX adapter=%u devType=%u adapterFmt=%u crossFmtAllowed=%d directCreate=0x%08lX directHandle=%p carrierFmtCheck=0x%08lX conversionCheck=0x%08lX carrierCreate=0x%08lX carrierHandle=%p VERDICT=%s",
            probeNo, surface, (unsigned)desc.Format, fmt_name(desc.Format),
            (unsigned)desc.MultiSampleType, (unsigned long)hrEx,
            (unsigned long)hrCp, (unsigned long)hrD3d, (unsigned long)hrMode,
            SUCCEEDED(hrCp) ? cp.AdapterOrdinal : 0xFFFFFFFFu,
            SUCCEEDED(hrCp) ? (unsigned)cp.DeviceType : 0xFFFFFFFFu,
            SUCCEEDED(hrMode) ? (unsigned)dm.Format : 0xFFFFFFFFu,
            directAllowedFmt, (unsigned long)hrDirectCreate, directHandle,
            (unsigned long)hrCarrierFormat, (unsigned long)hrConversion,
            (unsigned long)hrCarrierCreate, carrierHandle, verdict);

    /* This control never binds, copies, reads or submits either texture. */
    release_texture(&carrierTex);
    release_texture(&directTex);
    if (d3d) IDirect3D9_Release(d3d);
    if (devEx) IDirect3DDevice9Ex_Release(devEx);
    IDirect3DDevice9_Release(dev);
}

static void __cdecl hook_push(IDirect3DSurface9 *replacementSurface) {
    void *caller = __builtin_return_address(0);
    g_originalPush(replacementSurface);
    probe_surface(replacementSurface, caller);
}

static DWORD WINAPI worker_thread(LPVOID unused) {
    (void)unused;
    InitializeCriticalSection(&g_logLock);
    open_log();

    log_line("CSGOVR T0 RT FORMAT COMPAT CONTROL starting");
    log_line("MODE=OBSERVE_PUSH_REPLACEMENT_SURFACE_AND_UNBOUND_CAPABILITY_PROBES");
    log_line("NO SetRenderTarget / NO StretchRect / NO pixel readback / NO OpenXR / NO camera mutation");
    log_fmt("PromotionColdResultSHA256=%s", CSGOVR_PROMOTION_COLD_RESULT_SHA256);

    if (!has_exact_insecure_arg(GetCommandLineW())) {
        log_line("SAFETY ABORT exact -insecure argument absent");
        return 20;
    }

    HMODULE afx = NULL;
    for (int i = 0; i < 300 && !afx; ++i) {
        afx = GetModuleHandleW(L"AfxHookSource.dll");
        if (!afx) Sleep(100);
    }
    if (!afx) { log_line("ABORT AfxHookSource.dll not loaded"); return 21; }
    if (!verify_pe(afx)) return 22;

    uint8_t *target = (uint8_t*)afx + CSGOVR_PUSH_WRAPPER_RVA;
    if (!verify_push_signature(target)) {
        log_fmt("ABORT promoted Push wrapper signature mismatch target=%p RVA=0x%08X sigLen=%u",
                target, CSGOVR_PUSH_WRAPPER_RVA, CSGOVR_PUSH_WRAPPER_SIG_LEN);
        return 23;
    }

    log_fmt("PROMOTED PUSH wrapper target=%p RVA=0x%08X signatureLen=%u",
            target, CSGOVR_PUSH_WRAPPER_RVA, CSGOVR_PUSH_WRAPPER_SIG_LEN);

    MH_STATUS st = MH_Initialize();
    if (st != MH_OK && st != MH_ERROR_ALREADY_INITIALIZED) {
        log_fmt("ABORT MH_Initialize status=%d", (int)st);
        return 24;
    }
    st = MH_CreateHook((LPVOID)target, (LPVOID)&hook_push, (LPVOID*)&g_originalPush);
    if (st != MH_OK) { log_fmt("ABORT MH_CreateHook status=%d", (int)st); return 25; }
    st = MH_EnableHook((LPVOID)target);
    if (st != MH_OK) { log_fmt("ABORT MH_EnableHook status=%d", (int)st); return 26; }

    log_line("HOOK ACTIVE: run the existing GOLDEN/HLAE recording sequence only; correlate this log with the existing v0.5.5 eye_patch.log.");
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved) {
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinst);
        HANDLE th = CreateThread(NULL, 0, worker_thread, NULL, 0, NULL);
        if (th) CloseHandle(th);
    }
    return TRUE;
}
