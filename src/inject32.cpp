#include "platform.hpp"
// NOVA's one-time explicit offline-only loader. No stealth, no API hooking.
static Log logg;
static U32 pid(){wchar_t s[24]={};if(!GetEnvironmentVariableW(L"NOVA_PID",s,24))return 0;U32 n=0;for(int i=0;s[i];i++){if(s[i]<L'0'||s[i]>L'9')return 0;n=n*10+U32(s[i]-L'0');}return n;}
static bool same_ci(const wchar_t*a,const wchar_t*b){while(*a&&*b){wchar_t x=*a++,y=*b++;if(x>=L'A'&&x<=L'Z')x+=32;if(y>=L'A'&&y<=L'Z')y+=32;if(x!=y)return false;}return !*a&&!*b;}
static bool same_name(const wchar_t*full,const wchar_t*shortname){const wchar_t*p=full;for(size_t i=0;full[i];i++)if(full[i]==L'\\'||full[i]==L'/')p=full+i+1;return same_ci(p,shortname);}
static U32 run(){logg.open(L"nova_inject.log");U32 target=pid();wchar_t expected[1024]{},dll[1024]{};
 if(!target||!GetEnvironmentVariableW(L"NOVA_GAME",expected,1024)||!GetEnvironmentVariableW(L"NOVA_DLL",dll,1024))return 1;
 HANDLE proc=OpenProcess(0x0002|0x0008|0x0010|0x0020|0x0400|0x1000,0,target);if(!proc)return 2;
 wchar_t actual[1024]{};U32 size=1024;
 if(!QueryFullProcessImageNameW(proc,0,actual,&size)||!same_ci(actual,expected)){logg.msg("GAME_PATH_NOT_EXACT",target);CloseHandle(proc);return 3;}
 // A locally-resolved kernel export may live in kernelbase, not kernel32. Identify owner by VirtualQuery.
 HMODULE module=GetModuleHandleW(L"kernel32.dll");void*loader=GetProcAddress(module,"LoadLibraryW");
 MEMORY_BASIC_INFORMATION m{};if(!loader||VirtualQuery(loader,&m,sizeof(m))!=sizeof(m)){CloseHandle(proc);return 4;}
 wchar_t ownerPath[1024]{};if(!GetModuleFileNameW((HMODULE)m.AllocationBase,ownerPath,1024)){CloseHandle(proc);return 5;}
 HANDLE snap=CreateToolhelp32Snapshot(0x8|0x10,target);if(snap==BAD_HANDLE){CloseHandle(proc);return 6;}
 MODULEENTRY32W ent{};ent.dwSize=sizeof(ent);uintptr_t remote=0;
 BOOL next=Module32FirstW(snap,&ent);
 while(next){if(same_name(ownerPath,ent.szModule)){remote=(uintptr_t)ent.modBaseAddr;break;}next=Module32NextW(snap,&ent);}
 CloseHandle(snap);if(!remote){CloseHandle(proc);return 7;}
 uintptr_t remoteLoader=remote+(uintptr_t)loader-(uintptr_t)m.AllocationBase;
 size_t length=(wlen_(dll)+1)*sizeof(wchar_t);
 void*mem=VirtualAllocEx(proc,nullptr,length,0x3000,0x04);SIZE_T written=0;
 if(!mem||!WriteProcessMemory(proc,mem,dll,length,&written)||written!=length){if(mem)VirtualFreeEx(proc,mem,0,0x8000);CloseHandle(proc);return 8;}
 HANDLE thread=CreateRemoteThread(proc,nullptr,0,(LPTHREAD_START_ROUTINE)remoteLoader,mem,0,nullptr);
 if(!thread){VirtualFreeEx(proc,mem,0,0x8000);CloseHandle(proc);return 9;}
 U32 wait=WaitForSingleObject(thread,15000);U32 exit=0;
 bool done=wait==0&&GetExitCodeThread(thread,&exit);
 if(done)VirtualFreeEx(proc,mem,0,0x8000); // never free live remote thread argument
 CloseHandle(thread);CloseHandle(proc);
 logg.msg("REMOTE_LOAD",target,done,exit);logg.close();return done&&exit?0:10;
}
extern "C" void mainCRTStartup(){int ec=run();ExitProcess(ec);}
