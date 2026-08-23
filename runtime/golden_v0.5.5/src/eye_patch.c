#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "MinHook.h"

#define AFX_TIMESTAMP  0x6A58EC87u
#define AFX_IMAGE_SIZE 0x0023F000u
#define AFX_DORENDERVIEW_RVA 0x00022340u

/*
   IMPORTANT: exact HLAE 2.191.1 CViewSetup_csgo copy constructor at
   AfxHookSource.dll RVA 0x00014BA0 copies through byte 0x197.
   Therefore sizeof(CViewSetup_csgo) for this exact binary is 0x198 bytes.
   The earlier 0x14C was only our camera-analysis capture prefix.
*/
#define VIEW_BYTES 0x198u
#define OFF_ORIGIN 0x0B8u
#define OFF_ANGLES 0x0C4u

#define OFF_FORCE_CACHE 0x30u
#define OFF_DO_RENDER_COUNT 0x50u

#define HALF_EYE_SEP 1.2598425f
#define DEG2RAD 0.01745329251994329577f

typedef void (__attribute__((thiscall)) *DoRenderView_t)(
    void *afxStreamsThis,
    void *fn,
    void *gameThis,
    void *gameEdx,
    const void *view,
    const void *hudView,
    int nClearFlags,
    int whatToDraw
);

static DoRenderView_t g_original = NULL;
static HANDLE g_log = INVALID_HANDLE_VALUE;
static CRITICAL_SECTION g_logLock;
static volatile LONG g_loggedPairs = 0;

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
    char buf[1024];
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
    _snwprintf(root, MAX_PATH-1, L"%sCSGOVR_HLAE_EYE_PATCH", tmp);
    root[MAX_PATH-1] = 0;
    CreateDirectoryW(root, NULL);
    _snwprintf(pidDir, MAX_PATH-1, L"%s\\PID_%lu", root, (unsigned long)GetCurrentProcessId());
    pidDir[MAX_PATH-1] = 0;
    CreateDirectoryW(pidDir, NULL);
    _snwprintf(logPath, MAX_PATH-1, L"%s\\eye_patch.log", pidDir);
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
        log_fmt("ABORT AfxHookSource timestamp mismatch actual=0x%08X expected=0x%08X",
                nt->FileHeader.TimeDateStamp, AFX_TIMESTAMP);
        return 0;
    }
    if (nt->OptionalHeader.SizeOfImage != AFX_IMAGE_SIZE) {
        log_fmt("ABORT AfxHookSource SizeOfImage mismatch actual=0x%08X expected=0x%08X",
                nt->OptionalHeader.SizeOfImage, AFX_IMAGE_SIZE);
        return 0;
    }
    return 1;
}

static int verify_dorenderview_bytes(uint8_t *p) {
    static const uint8_t head[] = {
        0x55,0x8B,0xEC,0x6A,0xFF,0x68
    };
    static const uint8_t countSeq[] = {
        0x8B,0xD9,0x8B,0x53,0x50,0x42,0x80,0x7B,0x54,0x00,0x89,0x53,0x50
    };
    /* Exact binary also touches view+0x148 in DoRenderView when forcing
       m_bCacheFullSceneState. This guard makes sure we are on the audited ABI. */
    static const uint8_t view148Seq[] = {
        0x8A,0x86,0x48,0x01,0x00,0x00
    };
    if (memcmp(p, head, sizeof(head)) != 0) return 0;
    if (memcmp(p + 0x28, countSeq, sizeof(countSeq)) != 0) return 0;
    if (memcmp(p + 0x123, view148Seq, sizeof(view148Seq)) != 0) return 0;
    return 1;
}

/* Source/Valve AngleVectors right-vector convention:
   X forward, Y left, Z up. At yaw=pitch=roll=0, right=(0,-1,0). */
static void angle_right(const float *ang, float out[3]) {
    float pitch = ang[0] * DEG2RAD;
    float yaw   = ang[1] * DEG2RAD;
    float roll  = ang[2] * DEG2RAD;

    float sp = sinf(pitch), cp = cosf(pitch);
    float sy = sinf(yaw),   cy = cosf(yaw);
    float sr = sinf(roll),  cr = cosf(roll);

    out[0] = -sr * sp * cy + cr * sy;
    out[1] = -sr * sp * sy - cr * cy;
    out[2] = -sr * cp;
}

static void __attribute__((thiscall)) hook_DoRenderView(
    void *afxStreamsThis,
    void *fn,
    void *gameThis,
    void *gameEdx,
    const void *view,
    const void *hudView,
    int nClearFlags,
    int whatToDraw
) {
    int currentCount = *(volatile int*)((uint8_t*)afxStreamsThis + OFF_DO_RENDER_COUNT);
    int pass = currentCount + 1;
    int forceCache = *(volatile uint8_t*)((uint8_t*)afxStreamsThis + OFF_FORCE_CACHE) ? 1 : 0;

    /* Proven HLAE v0.5.1 recording sequence:
       pass 1 forceCache=1 = CSGOVRLEFT
       pass 2 forceCache=1 = CSGOVRRIGHT
       pass 3 forceCache=0 = normal on-screen game view.
       Modify ONLY the two HLAE recording passes. */
    if (view && forceCache && (pass == 1 || pass == 2)) {
        __attribute__((aligned(64))) uint8_t localView[VIEW_BYTES];
        memcpy(localView, view, VIEW_BYTES);

        float *origin = (float*)(localView + OFF_ORIGIN);
        float *angles = (float*)(localView + OFF_ANGLES);
        float right[3];
        angle_right(angles, right);

        float sign = (pass == 1) ? -1.0f : +1.0f; /* left = -right, right = +right */
        float oldx = origin[0], oldy = origin[1], oldz = origin[2];

        origin[0] += sign * right[0] * HALF_EYE_SEP;
        origin[1] += sign * right[1] * HALF_EYE_SEP;
        origin[2] += sign * right[2] * HALF_EYE_SEP;

        LONG seen = InterlockedIncrement(&g_loggedPairs);
        if (seen <= 24) {
            uint32_t tail = *(const uint32_t*)((const uint8_t*)view + 0x194);
            log_fmt("EYE pass=%d forceCache=%d yaw=%.4f pitch=%.4f roll=%.4f "
                    "right=(%.5f,%.5f,%.5f) origin=(%.5f,%.5f,%.5f)->(%.5f,%.5f,%.5f) tail194=0x%08X",
                    pass, forceCache, angles[1], angles[0], angles[2],
                    right[0], right[1], right[2],
                    oldx, oldy, oldz, origin[0], origin[1], origin[2], tail);
        }

        g_original(afxStreamsThis, fn, gameThis, gameEdx,
                   localView, hudView, nClearFlags, whatToDraw);
        return;
    }

    g_original(afxStreamsThis, fn, gameThis, gameEdx,
               view, hudView, nClearFlags, whatToDraw);
}

static DWORD WINAPI worker_thread(LPVOID unused) {
    (void)unused;
    InitializeCriticalSection(&g_logLock);
    open_log();

    log_line("CSGOVR HLAE EYE PATCH v0.5.5 starting");
    log_line("MODE: exact HLAE DoRenderView hook; local CViewSetup copies only; pass3 untouched");

    if (!has_exact_insecure_arg(GetCommandLineW())) {
        log_line("SAFETY ABORT: exact -insecure argument not present.");
        return 20;
    }
    log_line("SAFETY GATE PASS: exact -insecure confirmed inside target process");

    HMODULE afx = NULL;
    for (int i = 0; i < 300 && !afx; ++i) {
        afx = GetModuleHandleW(L"AfxHookSource.dll");
        if (!afx) Sleep(100);
    }
    if (!afx) {
        log_line("ABORT: AfxHookSource.dll not loaded.");
        return 21;
    }

    if (!verify_pe(afx)) return 22;

    uint8_t *target = (uint8_t*)afx + AFX_DORENDERVIEW_RVA;
    if (!verify_dorenderview_bytes(target)) {
        log_fmt("ABORT: DoRenderView machine-byte guard failed at %p RVA=0x%X",
                target, AFX_DORENDERVIEW_RVA);
        return 23;
    }

    log_fmt("AfxHookSource base=%p timestamp=0x%08X imageSize=0x%08X", afx, AFX_TIMESTAMP, AFX_IMAGE_SIZE);
    log_fmt("DoRenderView target=%p RVA=0x%X countOffset=0x%X forceCacheOffset=0x%X",
            target, AFX_DORENDERVIEW_RVA, OFF_DO_RENDER_COUNT, OFF_FORCE_CACHE);
    log_fmt("CViewSetup exact full copy size=0x%X (copy ctor evidence through byte 0x197)", VIEW_BYTES);
    log_fmt("Eye model: halfSep=%.7f Source units; pass1 LEFT=-right; pass2 RIGHT=+right", HALF_EYE_SEP);

    MH_STATUS st = MH_Initialize();
    if (st != MH_OK && st != MH_ERROR_ALREADY_INITIALIZED) {
        log_fmt("ABORT MH_Initialize status=%d", (int)st);
        return 24;
    }

    st = MH_CreateHook((LPVOID)target, (LPVOID)&hook_DoRenderView, (LPVOID*)&g_original);
    if (st != MH_OK) {
        log_fmt("ABORT MH_CreateHook status=%d", (int)st);
        return 25;
    }

    st = MH_EnableHook((LPVOID)target);
    if (st != MH_OK) {
        log_fmt("ABORT MH_EnableHook status=%d", (int)st);
        return 26;
    }

    log_line("HOOK ACTIVE");
    log_line("Waiting for HLAE recording: only forceCache pass1/pass2 will receive LEFT/RIGHT origin offsets.");
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
