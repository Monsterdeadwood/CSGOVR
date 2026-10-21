#pragma once
#include "native_memory.hpp"
#include "controller_input.hpp"
// Exact offline target: native game UI, buy wheel and engine event consumer.
// No OS input, fabricated purchases, or replacement menu. The target's event
// stride is 24 bytes (engine+29cbab ADD ESI,18), unlike the older public SDK.
namespace native_menu {
using namespace nova;
#ifdef NOVA_LAB_UNIVERSAL
struct Event {I32 type,tick,data[4];};
static_assert(sizeof(Event)==24,"Exact target InputEvent ABI");
using Count=I32(__thiscall*)(void*);
using Data=const Event*(__thiscall*)(void*);
using Fire=bool(__thiscall*)(void*,void*);
using Visible=bool(__thiscall*)(void*);
static handcore_native::NativeMemory memory;
static U32 clientBase{},engineBase{},inputBase{},inputObject{};
static Count originalCount{};static Data originalData{};static Fire originalFire{};
static U32 *countSlot{},*dataSlot{},*fireSlot{};
static volatile I32 installed{},buyOpen{},queueBusy{},inputAllowed{};
static volatile I32 gameOpen{};static U32 gameUiObject{};static Visible gameUiVisible{};
static Log logg;
static Event queue[32]{},merged[256]{};static U32 readAt{},writeAt{},queued{},cachePoll{},cacheCount{};
static bool cacheReady{},axisArmed{},cursorReady{};static U32 previousMode{};
static I32 cursorX=-1,cursorY=-1;static U32 width{},height{},menuSerial{};
static float pointerX{},pointerY{};static U64 pointerMs{};
static I32 escapeCode{};
static controller_input::Trigger confirm;
static controller_input::ButtonEdges buttons;
static bool open(){return load32(&buyOpen)!=0;}
static bool game_open(){return load32(&gameOpen)!=0;}
static void refresh_game_ui(){
 if(!gameUiObject||!gameUiVisible)return;bool shown=gameUiVisible((void*)(uintptr_t)gameUiObject);
 if(shown!=game_open()){store32(&gameOpen,shown?1:0);logg.msg(shown?"UI_GAME_OPEN":"UI_GAME_CLOSED",++menuSerial,GetCurrentThreadId());}
}
static bool swap(U32*slot,U32 expected,U32 replacement){
 U32 actual=0,old=0;if(!memory.read(U32((uintptr_t)slot),&actual,4)||actual!=expected||!VirtualProtect(slot,4,0x40,&old))return false;
 bool done=__atomic_compare_exchange_n(slot,&expected,replacement,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE);
 U32 ignored=0;bool protectedAgain=VirtualProtect(slot,4,old,&ignored)!=0;return done&&protectedAgain;
}
static bool enqueue(Event event){
 if(!compare32(&queueBusy,0,1))return false;bool okay=false;
 if(event.type==3&&queued){U32 last=(writeAt+31)&31;if(queue[last].type==3){queue[last]=event;okay=true;}}
 if(!okay&&queued<32){queue[writeAt]=event;writeAt=(writeAt+1)&31;queued++;okay=true;}
 store32(&queueBusy,0);return okay;
}
static void click(I32 code){
 if(!compare32(&queueBusy,0,1))return;bool okay=queued<=30;
 if(okay){queue[writeAt]={0,0,{code,0,0,0}};writeAt=(writeAt+1)&31;queue[writeAt]={1,0,{code,0,0,0}};writeAt=(writeAt+1)&31;queued+=2;}
 store32(&queueBusy,0);if(okay)logg.msg("UI_NATIVE_CLICK",code,menuSerial);
}
static bool __fastcall fire_hook(void*self,void*,void*event){
 U32 vt=0,fn=0;if(memory.read(U32((uintptr_t)event),&vt,4)&&vt==engineBase+0x4b6f54&&memory.read(vt+4,&fn,4)&&fn==engineBase+0x210f10){
  using Name=const char*(__thiscall*)(void*);const char*name=((Name)(uintptr_t)fn)(event);char text[64]{};
  if(memory.read(U32((uintptr_t)name),text,63)){
   if(streq(text,"buymenu_open")){store32(&buyOpen,1);menuSerial++;logg.msg("UI_BUY_OPEN",menuSerial,GetCurrentThreadId());}
   else if(streq(text,"buymenu_close")||streq(text,"server_spawn")||streq(text,"game_newmap")){store32(&buyOpen,0);store32(&inputAllowed,0);logg.msg("UI_BUY_CLOSED",menuSerial,GetCurrentThreadId());}
  }
 }
 return originalFire(self,event);
}
static I32 __fastcall count_hook(void*self,void*){
 I32 count=originalCount(self);
 if(!load32(&installed)||U32((uintptr_t)self)!=inputObject||U32((uintptr_t)__builtin_return_address(0))!=engineBase+0x29cb90)return count;
 U32 poll=0,tick=0;if(!memory.read(inputObject+0x5770,&poll,4)||!memory.read(inputObject+0x5768,&tick,4))return count;
 if(cacheReady&&poll==cachePoll)return I32(cacheCount);cacheReady=false;
 if(count<0||count>240||!compare32(&queueBusy,0,1))return count;
 Event next{};bool have=false;
 while(queued){next=queue[readAt];readAt=(readAt+1)&31;queued--;if(next.type==1||((open()||game_open())&&load32(&inputAllowed))){have=true;break;}}
 store32(&queueBusy,0);if(!have)return count;
 const Event*native=originalData(self);
 if(count&&!memory.read(U32((uintptr_t)native),merged,U32(count)*sizeof(Event)))return count;
 next.tick=I32(tick);merged[count]=next;cacheCount=U32(count)+1;cachePoll=poll;cacheReady=true;return I32(cacheCount);
}
static const Event*__fastcall data_hook(void*self,void*){
 const Event*native=originalData(self);U32 poll=0;
 if(load32(&installed)&&U32((uintptr_t)self)==inputObject&&U32((uintptr_t)__builtin_return_address(0))==engineBase+0x29cb9d&&cacheReady&&memory.read(inputObject+0x5770,&poll,4)&&poll==cachePoll)return merged;
 return native;
}
static bool initialize(U32 client,U32 engine,U32 input){
 if(load32(&installed))return true;clientBase=client;engineBase=engine;inputBase=input;
 if(logg.file==BAD_HANDLE)logg.open(L"nova_ui.log");
 U32 table=0,manager=0,managerTable=0,functions[2]{};U8 stride[3]{};
 if(!input||!memory.read(client+0x534d6b8,&inputObject,4)||!memory.read(inputObject,&table,4)||table!=input+0x30ea0||!memory.read(table+80,functions,8)||functions[0]!=input+0x34a0||functions[1]!=input+0x34b0||!memory.read(engine+0x29cbab,stride,3)||stride[0]!=0x83||stride[1]!=0xc6||stride[2]!=0x18){logg.msg("UI_WAIT_INPUT_IDENTITY",table,inputObject,input);return false;}
 using Factory=void*(__cdecl*)(const char*,int*);auto factory=(Factory)GetProcAddress((void*)(uintptr_t)engine,"CreateInterface");
 U32 gameTable=0,visible=0,prefix=0;if(!factory)return false;
 gameUiObject=U32((uintptr_t)factory("VEngineVGui001",nullptr));
 if(!memory.read(gameUiObject,&gameTable,4)||gameTable!=engine+0x4d1eb0||!memory.read(gameTable+8,&visible,4)||visible!=engine+0x2b15c0||!memory.read(visible,&prefix,4)||prefix!=0x8530498b){logg.msg("UI_GAME_QUERY_REJECTED",gameTable,visible);return false;}
 gameUiVisible=(Visible)(uintptr_t)visible;
 if(!memory.read(client+0x5334790,&manager,4)||!memory.read(manager,&managerTable,4)||managerTable!=engine+0x4b6e3c){logg.msg("UI_WAIT_EVENT_MANAGER",manager,managerTable,engine);return false;}
 U32 fire=0;if(!memory.read(managerTable+36,&fire,4)||fire!=engine+0x211a50)return false;
 using FromName=I32(__thiscall*)(void*,const char*);using ToName=const char*(__thiscall*)(void*,I32);
 U32 from=0,to=0;if(!memory.read(table+42*4,&from,4)||from!=input+0x2b60||!memory.read(table+40*4,&to,4)||to!=input+0x2b20)return false;
 if(((FromName)(uintptr_t)from)((void*)(uintptr_t)inputObject,"MOUSE1")!=107||!streq(((ToName)(uintptr_t)to)((void*)(uintptr_t)inputObject,107),"MOUSE1"))return false;
 escapeCode=((FromName)(uintptr_t)from)((void*)(uintptr_t)inputObject,"ESCAPE");
 if(escapeCode<=0||escapeCode>=107||!streq(((ToName)(uintptr_t)to)((void*)(uintptr_t)inputObject,escapeCode),"ESCAPE")){logg.msg("UI_ESCAPE_IDENTITY_REJECTED",U32(escapeCode));return false;}
 originalCount=(Count)(uintptr_t)functions[0];originalData=(Data)(uintptr_t)functions[1];originalFire=(Fire)(uintptr_t)fire;
 countSlot=(U32*)(uintptr_t)(table+80);dataSlot=(U32*)(uintptr_t)(table+84);fireSlot=(U32*)(uintptr_t)(managerTable+36);
 if(!swap(countSlot,functions[0],U32((uintptr_t)count_hook)))return false;
 if(!swap(dataSlot,functions[1],U32((uintptr_t)data_hook))){swap(countSlot,U32((uintptr_t)count_hook),functions[0]);return false;}
 if(!swap(fireSlot,fire,U32((uintptr_t)fire_hook))){swap(dataSlot,U32((uintptr_t)data_hook),functions[1]);swap(countSlot,U32((uintptr_t)count_hook),functions[0]);return false;}
 store32(&installed,1);logg.msg("UI_NATIVE_EVENTS_ADMITTED",sizeof(Event),inputObject,manager);return true;
}
static void extent(U32 w,U32 h){width=w;height=h;}
static void update(const controller_input::Sample&s,U64 now,bool active){
 U32 mode=game_open()?2:open()?1:0;
 if(mode!=previousMode){confirm.release();buttons.release();axisArmed=cursorReady=false;cursorX=cursorY=-1;pointerX=float(width)*.5f;pointerY=float(height)*.5f;pointerMs=now;previousMode=mode;}
 bool ready=mode&&active&&(s.actionMask&2)&&width&&height;
 store32(&inputAllowed,ready?1:0);
 float x=controller_input::axis(s.rightX),y=controller_input::axis(s.rightY),m=float(sqrt(x*x+y*y));
 float lx=controller_input::axis(s.leftX),ly=controller_input::axis(s.leftY),lm=float(sqrt(lx*lx+ly*ly));
 if(!ready)axisArmed=false;else if(m<.25f&&lm<.25f)axisArmed=true;
 if(lm>m){x=lx;y=ly;m=lm;}
 if(ready&&axisArmed&&mode==1&&m>.35f){
  // Stock native 16:9 wheel: same cursor geometry for every weapon/category.
  I32 nextX=I32(float(width)*.315f+float(height)*.215f*x/m),nextY=I32(float(height)*.346f-float(height)*.215f*y/m);
  if(nextX!=cursorX||nextY!=cursorY){enqueue({3,0,{2,nextX,nextY,0}});cursorX=nextX;cursorY=nextY;cursorReady=true;logg.msg("UI_RADIAL_CURSOR",U32(nextX),U32(nextY),s.sample);}
 }
 if(ready&&mode==2){
  U64 elapsed=now>=pointerMs?now-pointerMs:0;if(elapsed>50)elapsed=50;
  if(axisArmed&&m>.25f){float speed=float(height)*.9f*float(elapsed)*.001f*(m-.25f)/.75f;pointerX+=x/m*speed;pointerY-=y/m*speed;}
  if(pointerX<0)pointerX=0;if(pointerX>float(width-1))pointerX=float(width-1);
  if(pointerY<0)pointerY=0;if(pointerY>float(height-1))pointerY=float(height-1);
  I32 nextX=I32(pointerX),nextY=I32(pointerY);
  if(nextX!=cursorX||nextY!=cursorY){enqueue({3,0,{2,nextX,nextY,0}});cursorX=nextX;cursorY=nextY;cursorReady=true;}
 }
 pointerMs=now;
 int edge=confirm.update(s,now,ready);if(edge>0&&cursorReady)click(107);
 U32 down=0,up=0;buttons.update(s,now,ready,down,up);
 if(cursorReady&&(down&(controller_input::A|controller_input::RightClick)))click(107);
}
static void shutdown(){
 if(!load32(&installed))return;store32(&installed,0);store32(&buyOpen,0);store32(&gameOpen,0);
 bool a=swap(fireSlot,U32((uintptr_t)fire_hook),U32((uintptr_t)originalFire));
 bool b=swap(dataSlot,U32((uintptr_t)data_hook),U32((uintptr_t)originalData));
 bool c=swap(countSlot,U32((uintptr_t)count_hook),U32((uintptr_t)originalCount));logg.msg("UI_NATIVE_HOOKS_RESTORED",a,b,c);
}
#else
static inline bool open(){return false;}
static inline void extent(U32,U32){}
#endif
}
