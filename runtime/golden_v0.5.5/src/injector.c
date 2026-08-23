#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <wchar.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((LONG)(Status)) >= 0)
#endif

typedef LONG NTSTATUS;
typedef NTSTATUS (NTAPI *NtQueryInformationProcessFn)(HANDLE, ULONG, PVOID, ULONG, PULONG);

typedef struct _CSGOVR_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} CSGOVR_UNICODE_STRING;

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

static int query_remote_command_line(HANDLE process, wchar_t **out_cmdline) {
    if (!out_cmdline) return 0;
    *out_cmdline = NULL;
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) return 0;
    NtQueryInformationProcessFn NtQueryInformationProcess =
        (NtQueryInformationProcessFn)GetProcAddress(ntdll, "NtQueryInformationProcess");
    if (!NtQueryInformationProcess) return 0;

    /* ProcessCommandLineInformation = 60 on supported Windows 10/11 builds. */
    ULONG need = 0;
    NTSTATUS st = NtQueryInformationProcess(process, 60u, NULL, 0, &need);
    if (need < sizeof(CSGOVR_UNICODE_STRING) || need > (1024u * 1024u)) return 0;

    BYTE *buf = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)need + sizeof(wchar_t) * 2u);
    if (!buf) return 0;
    st = NtQueryInformationProcess(process, 60u, buf, need, &need);
    if (!NT_SUCCESS(st)) { HeapFree(GetProcessHeap(), 0, buf); return 0; }

    CSGOVR_UNICODE_STRING *us = (CSGOVR_UNICODE_STRING*)buf;
    if (!us->Buffer || us->Length == 0 || (us->Length & 1u) != 0) {
        HeapFree(GetProcessHeap(), 0, buf); return 0;
    }

    SIZE_T chars = (SIZE_T)us->Length / sizeof(wchar_t);
    wchar_t *copy = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (chars + 1u) * sizeof(wchar_t));
    if (!copy) { HeapFree(GetProcessHeap(), 0, buf); return 0; }
    memcpy(copy, us->Buffer, us->Length);
    copy[chars] = 0;
    HeapFree(GetProcessHeap(), 0, buf);
    *out_cmdline = copy;
    return 1;
}

static uintptr_t remote_module_base(DWORD pid, const wchar_t *name) {
    HANDLE s = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (s == INVALID_HANDLE_VALUE) return 0;
    MODULEENTRY32W me; ZeroMemory(&me, sizeof(me)); me.dwSize = sizeof(me);
    uintptr_t out = 0;
    if (Module32FirstW(s, &me)) {
        do {
            if (_wcsicmp(me.szModule, name) == 0) { out = (uintptr_t)me.modBaseAddr; break; }
        } while (Module32NextW(s, &me));
    }
    CloseHandle(s);
    return out;
}

int main(void) {
    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv || argc < 3) {
        fwprintf(stderr, L"usage: injector.exe <pid> <dll-path>\n");
        if (argv) LocalFree(argv);
        return 2;
    }
    DWORD pid = wcstoul(argv[1], NULL, 10);
    const wchar_t *dll = argv[2];
    if (!pid || GetFileAttributesW(dll) == INVALID_FILE_ATTRIBUTES) {
        fwprintf(stderr, L"invalid pid or dll path\n");
        LocalFree(argv);
        return 3;
    }

    HANDLE p = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ, FALSE, pid);
    if (!p) { fwprintf(stderr, L"OpenProcess failed: %lu\n", GetLastError()); LocalFree(argv); return 4; }

    wchar_t *remoteCmd = NULL;
    if (!query_remote_command_line(p, &remoteCmd)) {
        fwprintf(stderr, L"SAFETY ABORT: unable to verify target command line; refusing injection.\n");
        CloseHandle(p); LocalFree(argv); return 20;
    }
    if (!has_exact_insecure_arg(remoteCmd)) {
        fwprintf(stderr, L"SAFETY ABORT: target process was not launched with exact -insecure argument.\n");
        HeapFree(GetProcessHeap(), 0, remoteCmd);
        CloseHandle(p); LocalFree(argv); return 21;
    }
    wprintf(L"[SAFETY] -insecure confirmed in target command line.\n");
    HeapFree(GetProcessHeap(), 0, remoteCmd);

    SIZE_T bytes = (wcslen(dll) + 1) * sizeof(wchar_t);
    void *remote = VirtualAllocEx(p, NULL, bytes, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    if (!remote) { fwprintf(stderr, L"VirtualAllocEx failed: %lu\n", GetLastError()); CloseHandle(p); LocalFree(argv); return 5; }
    if (!WriteProcessMemory(p, remote, dll, bytes, NULL)) {
        fwprintf(stderr, L"WriteProcessMemory failed: %lu\n", GetLastError()); VirtualFreeEx(p, remote, 0, MEM_RELEASE); CloseHandle(p); LocalFree(argv); return 6;
    }

    HMODULE localK32 = GetModuleHandleW(L"kernel32.dll");
    FARPROC localLL = GetProcAddress(localK32, "LoadLibraryW");
    uintptr_t localOffset = (uintptr_t)localLL - (uintptr_t)localK32;
    uintptr_t remoteK32 = remote_module_base(pid, L"kernel32.dll");
    if (!remoteK32) { fwprintf(stderr, L"remote kernel32 not found\n"); VirtualFreeEx(p, remote, 0, MEM_RELEASE); CloseHandle(p); LocalFree(argv); return 7; }
    LPTHREAD_START_ROUTINE remoteLL = (LPTHREAD_START_ROUTINE)(remoteK32 + localOffset);

    HANDLE th = CreateRemoteThread(p, NULL, 0, remoteLL, remote, 0, NULL);
    if (!th) { fwprintf(stderr, L"CreateRemoteThread failed: %lu\n", GetLastError()); VirtualFreeEx(p, remote, 0, MEM_RELEASE); CloseHandle(p); LocalFree(argv); return 8; }
    WaitForSingleObject(th, 15000);
    DWORD code = 0; GetExitCodeThread(th, &code);
    wprintf(L"LoadLibraryW returned 0x%08lX\n", (unsigned long)code);
    CloseHandle(th);
    VirtualFreeEx(p, remote, 0, MEM_RELEASE);
    CloseHandle(p);
    LocalFree(argv);
    if (!code) return 9;
    return 0;
}
