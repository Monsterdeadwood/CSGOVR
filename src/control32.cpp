#include "platform.hpp"
#include "wire.hpp"
// NOVA control plane: one owned mapping, cooperative stop; no game file changes.
using namespace nova;
static U32 game_pid(){wchar_t s[24]{};U32 n=GetEnvironmentVariableW(L"NOVA_PID",s,24);if(n<1||n>=24)return 0;U32 x=0;for(U32 i=0;i<n;++i){if(s[i]<L'0'||s[i]>L'9')return 0;x=x*10+(U32)(s[i]-L'0');}return x;}
extern "C" void mainCRTStartup(){
 U32 pid=game_pid();if(!pid)ExitProcess(1);
 wchar_t name[64]{};mapping_name(name,pid);
 HANDLE h=OpenFileMappingW(0xf001f,0,name);if(!h)ExitProcess(2);
 Shared*s=(Shared*)MapViewOfFile(h,0xf001f,0,0,sizeof(Shared));
 if(!s){CloseHandle(h);ExitProcess(3);}
 U32 ok_=(s->magic==MAGIC&&s->version==VERSION&&s->bytes==sizeof(Shared)&&s->gamePid==pid)?1:0;
 if(ok_)store32(&s->stop,1);
 UnmapViewOfFile(s);CloseHandle(h);ExitProcess(ok_?0:4);
}
