#pragma once
// Fresh, narrow declarations of public Windows ABI. No C5 renderer/transport dependencies.
#include <stdint.h>
#include <stddef.h>
#ifdef NOVA_PORTABLE_TEST
#define WINCALL
#define WINIMPORT
#ifndef __cdecl
#define __cdecl
#endif
#else
#define WINCALL __stdcall
#define WINIMPORT __declspec(dllimport)
#endif
#define NOVA_EXPORT extern "C" __declspec(dllexport)
using U8=uint8_t; using U16=uint16_t; using U32=uint32_t; using U64=uint64_t;
using I32=int32_t; using I64=int64_t; using DWORD=uint32_t; using UINT=uint32_t; using LONG=int32_t;
using HRESULT=int32_t; using BOOL=int32_t; using ULONG=uint32_t; using BYTE=uint8_t;
using HANDLE=void*; using HMODULE=void*;using HWND=void*;using SIZE_T=uintptr_t;
using LPTHREAD_START_ROUTINE=DWORD(WINCALL*)(void*);
struct GUID {U32 Data1;U16 Data2,Data3;U8 Data4[8];};
using REFIID=const GUID&;
struct LUID {U32 LowPart;I32 HighPart;};
struct RECT {I32 left,top,right,bottom;};
struct FILETIME {U32 low,high;};
struct MEMORY_BASIC_INFORMATION {void*BaseAddress,*AllocationBase;U32 AllocationProtect;SIZE_T RegionSize;U32 State,Protect,Type;};
struct DXGI_ADAPTER_DESC1 {wchar_t Description[128];U32 VendorId,DeviceId,SubSysId,Revision;SIZE_T DedicatedVideoMemory,DedicatedSystemMemory,SharedSystemMemory;LUID AdapterLuid;U32 Flags;};
struct MODULEENTRY32W {U32 dwSize,th32ModuleID,th32ProcessID,GlblcntUsage,ProccntUsage;U8*modBaseAddr;U32 modBaseSize;HMODULE hModule;wchar_t szModule[256],szExePath[260];};
static inline bool ok(HRESULT r){return r>=0;} static inline bool failed(HRESULT r){return r<0;}
static const HANDLE BAD_HANDLE=(HANDLE)(intptr_t)-1;
extern "C" {
WINIMPORT void* WINCALL GetProcAddress(HMODULE,const char*);
WINIMPORT HMODULE WINCALL LoadLibraryW(const wchar_t*);
WINIMPORT HMODULE WINCALL GetModuleHandleW(const wchar_t*);
WINIMPORT U32 WINCALL GetModuleFileNameW(HMODULE,wchar_t*,U32);
WINIMPORT U32 WINCALL GetEnvironmentVariableW(const wchar_t*,wchar_t*,U32);
WINIMPORT U32 WINCALL GetCurrentProcessId();
WINIMPORT U32 WINCALL GetCurrentThreadId();
WINIMPORT U64 WINCALL GetTickCount64();
WINIMPORT void WINCALL Sleep(U32);
WINIMPORT BOOL WINCALL VirtualProtect(void*,SIZE_T,U32,U32*);
WINIMPORT SIZE_T WINCALL VirtualQuery(const void*,MEMORY_BASIC_INFORMATION*,SIZE_T);
WINIMPORT void* WINCALL VirtualAlloc(void*,SIZE_T,U32,U32);
WINIMPORT BOOL WINCALL VirtualFree(void*,SIZE_T,U32);
WINIMPORT HANDLE WINCALL CreateFileW(const wchar_t*,U32,U32,void*,U32,U32,HANDLE);
WINIMPORT BOOL WINCALL WriteFile(HANDLE,const void*,U32,U32*,void*);
WINIMPORT BOOL WINCALL ReadFile(HANDLE,void*,U32,U32*,void*);
WINIMPORT BOOL WINCALL FlushFileBuffers(HANDLE);
WINIMPORT BOOL WINCALL CloseHandle(HANDLE);
WINIMPORT HANDLE WINCALL CreateFileMappingW(HANDLE,void*,U32,U32,U32,const wchar_t*);
WINIMPORT HANDLE WINCALL OpenFileMappingW(U32,BOOL,const wchar_t*);
WINIMPORT void* WINCALL MapViewOfFile(HANDLE,U32,U32,U32,SIZE_T);
WINIMPORT BOOL WINCALL UnmapViewOfFile(const void*);
WINIMPORT HANDLE WINCALL OpenProcess(U32,BOOL,U32);
WINIMPORT BOOL WINCALL QueryFullProcessImageNameW(HANDLE,U32,wchar_t*,U32*);
WINIMPORT BOOL WINCALL GetProcessTimes(HANDLE,FILETIME*,FILETIME*,FILETIME*,FILETIME*);
WINIMPORT void* WINCALL VirtualAllocEx(HANDLE,void*,SIZE_T,U32,U32);
WINIMPORT BOOL WINCALL VirtualFreeEx(HANDLE,void*,SIZE_T,U32);
WINIMPORT BOOL WINCALL WriteProcessMemory(HANDLE,void*,const void*,SIZE_T,SIZE_T*);
WINIMPORT HANDLE WINCALL CreateRemoteThread(HANDLE,void*,SIZE_T,LPTHREAD_START_ROUTINE,void*,U32,U32*);
WINIMPORT U32 WINCALL WaitForSingleObject(HANDLE,U32);
WINIMPORT BOOL WINCALL GetExitCodeThread(HANDLE,U32*);
WINIMPORT HANDLE WINCALL CreateThread(void*,SIZE_T,LPTHREAD_START_ROUTINE,void*,U32,U32*);
WINIMPORT BOOL WINCALL DisableThreadLibraryCalls(HMODULE);
WINIMPORT U32 WINCALL GetLastError();
WINIMPORT HANDLE WINCALL CreateToolhelp32Snapshot(U32,U32);
WINIMPORT BOOL WINCALL Module32FirstW(HANDLE,MODULEENTRY32W*);
WINIMPORT BOOL WINCALL Module32NextW(HANDLE,MODULEENTRY32W*);
WINIMPORT BOOL WINCALL GetFileInformationByHandle(HANDLE,void*);
WINIMPORT void WINCALL ExitProcess(UINT);
WINIMPORT void WINCALL OutputDebugStringA(const char*);
WINIMPORT int __cdecl _snprintf(char*,size_t,const char*,...);
WINIMPORT void* __cdecl memcpy(void*,const void*,size_t);
WINIMPORT void* __cdecl memset(void*,int,size_t);
WINIMPORT double __cdecl sin(double);WINIMPORT double __cdecl cos(double);WINIMPORT double __cdecl tan(double);
WINIMPORT double __cdecl atan2(double,double);WINIMPORT double __cdecl sqrt(double);
}
static inline U32 load32(const volatile I32*p){return (U32)__atomic_load_n(p,__ATOMIC_ACQUIRE);}
static inline void store32(volatile I32*p,I32 n){__atomic_store_n(p,n,__ATOMIC_RELEASE);}
static inline bool compare32(volatile I32*p,I32 old,I32 newer){return __atomic_compare_exchange_n(p,&old,newer,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE);}
static inline U32 inc32(volatile I32*p){return (U32)__atomic_add_fetch(p,1,__ATOMIC_ACQ_REL);}
static inline void barrier(){__atomic_thread_fence(__ATOMIC_SEQ_CST);}
static inline bool streq(const char*a,const char*b){while(*a&&*b&&*a==*b){a++;b++;}return *a==*b;}
static inline size_t strnlen_(const char*s,size_t max){size_t i=0;for(;i<max&&s[i];i++){}return i;}
static inline size_t wlen_(const wchar_t*s){size_t n=0;while(s[n])n++;return n;}
static inline void wcat_(wchar_t*dst,const wchar_t*src,size_t cap){size_t i=0;while(i+1<cap&&dst[i])i++;size_t j=0;while(i+1<cap&&src[j])dst[i++]=src[j++];dst[i]=0;}
static inline void decimal_(wchar_t*out,U32 n){wchar_t rev[16];size_t i=0;do{rev[i++]=wchar_t(L'0'+n%10);n/=10;}while(n&&i<15);size_t k=0;while(i)out[k++]=rev[--i];out[k]=0;}
struct Log {
 HANDLE file=BAD_HANDLE;
 bool open(const wchar_t*name,const wchar_t*overrideDir=nullptr){wchar_t dir[800]={};U32 n=GetEnvironmentVariableW(L"NOVA_LOGDIR",dir,720);if((!n||n>=720)&&overrideDir){size_t len=wlen_(overrideDir);if(len>0&&len<720){for(size_t i=0;i<len;i++)dir[i]=overrideDir[i];n=(U32)len;}}if(n>0&&n<720){wcat_(dir,L"\\",800);wcat_(dir,name,800);file=CreateFileW(dir,0x40000000u,7,nullptr,2,0x80,nullptr);}return file!=BAD_HANDLE;}
 void msg(const char*tag,U64 a=0,U64 b=0,U64 c=0){char line[256];int n=_snprintf(line,sizeof(line),"NOVA_%s tick=%llu a=%llu b=%llu c=%llu\r\n",tag,(unsigned long long)GetTickCount64(),(unsigned long long)a,(unsigned long long)b,(unsigned long long)c);if(n<=0)return;OutputDebugStringA(line);if(file!=BAD_HANDLE){U32 written=0;WriteFile(file,line,U32(n<255?n:255),&written,nullptr);}}
 void close(){if(file!=BAD_HANDLE){CloseHandle(file);file=BAD_HANDLE;}}
};
