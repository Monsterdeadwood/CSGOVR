#pragma once
#include "native_memory.hpp"
#include "controller_input.hpp"
#include "native_menu.hpp"
#include "grenade_classes.hpp"
#include "ray_optics.hpp"
#include "optic_surface_profiles.hpp"
#include "fps_policy.hpp"
#include "perf.hpp"
// Additive console/status channel. The old image/pose ABI stays at version 3.
namespace native_game_control {
using namespace nova;
#ifdef NOVA_LAB_UNIVERSAL
struct alignas(8) Control {
 U32 magic,version,bytes,pid;
 volatile I32 request,ack,result,stateSequence;
 U32 commandBytes,thread;U64 tick;
 I32 health,team;U32 life;I32 observer;
 U32 checksum,bones,handActive,handReason;U64 handFrame;
 Vec3 observedPalm,targetPalm;float observedAxes[9];char model[64],command[512];
};
static_assert(sizeof(Control)==728&&offsetof(Control,command)==212&&offsetof(Control,handFrame)==80,"Console control ABI");
static HANDLE map{};static Control*shared{};static handcore_native::NativeMemory memory;
static U32 clientBase{},ownerThread{},expectedHost{};static U64 lastState{};
static Log inputLog;
static U32 queueAddress{};
// Exact target's public ExecuteClientCmd wrapper passes ECX/EDX plus two
// stack arguments and then ADD ESP,8. This private LTCG ABI is NOT __fastcall
// (which would make the callee pop those arguments). Preserve its actual ABI.
__declspec(naked) static void __cdecl queue_text(U32,const char*){
 __asm {
  push ebp
  mov ebp,esp
  xor ecx,ecx
  mov edx,[ebp+12]
  push 0
  push 0
  call dword ptr [ebp+8]
  add esp,8
  pop ebp
  ret
 }
}
static char commandStorage[8][514]{};
static bool stopGameRequested=false;
static bool testNativeScreen=false;
static volatile I32 outputIntake=1,visualRecoil=1;
static HANDLE buttonsMap{};static controller_input::Channel*buttons{};static controller_input::Trigger trigger;
static controller_input::ButtonEdges buttonEdges,menuEdges;static controller_input::AxisGate turnGate;
static controller_input::MenuGesture menuGesture;
static controller_input::WalkHold walkHold;
struct RecenterRequest {U64 epoch{},sample{},atMs{};};
static RecenterRequest recenterRequest;static volatile I32 recenterBusy{};static U32 recenterSerial{};
static bool take_recenter(const Tracking&t){
 if(!t.valid||!t.focused||!compare32(&recenterBusy,0,1))return false;
 U64 now=GetTickCount64();const auto&r=recenterRequest;
 bool expired=r.epoch&&(r.epoch!=t.epoch||now<r.atMs||now-r.atMs>1000);
 bool take=r.epoch&&!expired&&t.sampleId>=r.sample;
 if(expired||take)recenterRequest={};store32(&recenterBusy,0);return take;
}
struct ViewReference {U64 atMs{},epoch{},poseSample{},poseMs{};float pitch{},yaw{},headYaw{};Vec3 throwDirection{},gripDelta{};bool throwValid{};};
static ViewReference viewReference;static volatile I32 referenceBusy{};static U32 inputSystemBase{};
static volatile I32 grenadeSteering{1},grenadeOrigin{1},additionalReadBatch{1};
static bool directionalBridge=true,maskNativeDirections=true;
static I32 lastShotCommand{};static U64 lastShotEpoch{},lastShotMs{};
static void reference(const Tracking&t,const void*view,Vec3 center,Quat anchor){
 if(!t.valid||!t.focused||!view||!compare32(&referenceBusy,0,1))return;
 ViewReference next{};next.atMs=GetTickCount64();next.epoch=t.epoch;memcpy(&next.pitch,(const U8*)view+0xc4,4);memcpy(&next.yaw,(const U8*)view+0xc8,4);
 Basis head=head_basis(qmul(conjugate(normalized(anchor)),normalized(t.eye[0].rot)));next.headYaw=head.forward.x*head.forward.x+head.forward.y*head.forward.y>.02f?deg(atan2(head.forward.y,head.forward.x)):viewReference.headYaw;
 next.poseSample=t.sampleId;next.poseMs=t.sampledMs;
 if(t.hand[1].active&&t.hand[1].valid&&handcore::valid_quat(t.hand[1].rot)&&handcore::finite(t.hand[1].pos,30)&&handcore::finite(center,30)){
  Basis body=source_basis({next.pitch,next.yaw,0});Quat q=qmul(conjugate(normalized(anchor)),normalized(t.hand[1].rot));
  // Same universal anatomical frame as the rendered palm: its finger-forward
  // axis is -grip Y. No firearm muzzle geometry or per-weapon hand offsets.
  next.throwDirection=basis_apply(body,to_source(rotate(q,{0,-1,0})));
  next.gripDelta=scale(basis_apply(body,to_source(rotate(conjugate(normalized(anchor)),sub(t.hand[1].pos,center)))),UNITS_PER_METRE);
  next.throwValid=handcore::finite(next.throwDirection)&&absf(length(next.throwDirection)-1)<.01f&&handcore::finite(next.gripDelta)&&length(next.gripDelta)<80;
 }
 viewReference=next;store32(&referenceBusy,0);
}
static bool read_reference(ViewReference&out){if(!compare32(&referenceBusy,0,1))return false;out=viewReference;store32(&referenceBusy,0);return out.atMs&&GetTickCount64()>=out.atMs&&GetTickCount64()-out.atMs<=250;}
static bool ui_free(){
 U32 object=0,table=0,mode=0;return !native_menu::game_open()&&inputSystemBase&&memory.read(clientBase+0x534d6b8,&object,4)&&memory.read(object,&table,4)&&table==inputSystemBase+0x30ea0&&memory.read(object+0x6044,&mode,4)&&mode!=4&&mode!=8&&mode!=16;
}
static bool flat_frame(){
 if(testNativeScreen||native_menu::open()||native_menu::game_open())return true;U32 player=0;I32 health=0,observer=-1;U8 life=255;
 if(!memory.read(clientBase+0x52bf6d8,&player,4)||!player)return true;
 return !memory.read(player+0x100,&health,4)||!memory.read(player+0x25f,&life,1)||!memory.read(player+0x3388,&observer,4)||health<=0||life||observer;
}
// This component runs only in the owned VR game. Suppress mouse camera input
// across headset focus changes; native menu mouse handling uses a separate path.
static bool mouse_camera_blocked(){return shared&&!stopGameRequested;}
static void screen_extent(U32 width,U32 height){native_menu::extent(width,height);auto*c=__atomic_load_n(&buttons,__ATOMIC_ACQUIRE);if(c&&width&&height&&width<=16384&&height<=16384)store32(&c->screenExtent,I32(width|(height<<16)));}
static void feedback(U32 kind,I32 command,U64 epoch,U32 weapon=0,float value=1){
 auto*c=__atomic_load_n(&buttons,__ATOMIC_ACQUIRE);if(!c||!epoch)return;
 if(!compare32(&c->feedbackBusy,0,1)){__atomic_fetch_add(&c->hapticDropped,1u,__ATOMIC_RELAXED);return;}
 U64 now=GetTickCount64();if(kind==controller_input::Shot&&lastShotEpoch==epoch&&lastShotCommand==command&&now>=lastShotMs&&now-lastShotMs<=20){store32(&c->feedbackBusy,0);return;}
 if(kind==controller_input::Shot){lastShotEpoch=epoch;lastShotCommand=command;lastShotMs=now;}
 c->feedback={c->feedback.serial+1,GetTickCount64(),epoch,command,kind,weapon,value};store32(&c->feedbackBusy,0);
}
static void button_commands(U32 down,U32 up,const controller_input::Sample&s){
 struct Hold {U32 bit;const char*press;const char*release;};
 static const Hold holds[]={{controller_input::A,"+jump\n","-jump\n"},{controller_input::X,"+reload\n","-reload\n"},{controller_input::RightClick,"+duck\n","-duck\n"},{controller_input::Use,"+use\n","-use\n"},{controller_input::LeftClick,"+showscores\n","-showscores\n"}};
 for(const auto&h:holds){if(up&h.bit)queue_text(queueAddress,h.release);if(down&h.bit)queue_text(queueAddress,h.press);}
 int walking=walkHold.update(down,up);
 if(walking){queue_text(queueAddress,walking>0?"+speed\n":"-speed\n");inputLog.msg(walking>0?"XR_WALK_ON":"XR_WALK_OFF",walkHold.held,s.sample,s.epoch);}
 if(down&controller_input::B)queue_text(queueAddress,"invnext\n");
 if(down&controller_input::Y){queue_text(queueAddress,"drop\n");inputLog.msg("XR_DROP_WEAPON",s.sample,s.epoch);feedback(controller_input::WeaponInput,0,s.epoch);}
 if(down&controller_input::X)feedback(controller_input::ReloadInput,0,s.epoch);
 if(down&controller_input::B)feedback(controller_input::WeaponInput,0,s.epoch);
 if(down||up)inputLog.msg("XR_BUTTON_EDGES",down,up,s.sample);
}
static U64 nextButtonsTry{};
static void trigger_edge(int edge,const controller_input::Sample&sample){
 if(edge&&queueAddress){queue_text(queueAddress,edge>0?"+attack\n":"-attack\n");inputLog.msg(edge>0?"XR_TRIGGER_NATIVE_ATTACK_DOWN":"XR_TRIGGER_NATIVE_ATTACK_UP",sample.sample,sample.epoch,U32(sample.trigger*1000));}
}
static void release_buttons(){trigger_edge(trigger.release(),controller_input::Sample{});button_commands(0,buttonEdges.release(),controller_input::Sample{});menuEdges.release();menuGesture.cancel();turnGate.armed=false;}
static void poll_buttons(){
 if(!buttons&&GetTickCount64()>=nextButtonsTry){
  nextButtonsTry=GetTickCount64()+1000;wchar_t name[64]{};controller_input::name(name,GetCurrentProcessId());
  buttonsMap=OpenFileMappingW(0xf001f,0,name);if(buttonsMap)__atomic_store_n(&buttons,(controller_input::Channel*)MapViewOfFile(buttonsMap,0xf001f,0,0,sizeof(controller_input::Channel)),__ATOMIC_RELEASE);
  if(buttonsMap&&!buttons){CloseHandle(buttonsMap);buttonsMap=nullptr;}
 }
 controller_input::Sample sample{};
 if(buttons&&buttons->magic==controller_input::MAGIC&&buttons->version==controller_input::VERSION&&buttons->bytes==sizeof(controller_input::Channel)&&buttons->gamePid==GetCurrentProcessId()&&buttons->hostPid==expectedHost)controller_input::read(buttons,sample);
 bool ready=shared->health>0&&shared->life==0&&shared->observer==0&&shared->handActive&&!stopGameRequested&&ui_free()&&!flat_frame();
 int edge=trigger.update(sample,GetTickCount64(),ready);
 trigger_edge(edge,sample);
 bool live=controller_input::live(sample,GetTickCount64())&&!stopGameRequested;bool free=ui_free();
 if(buttons)store32(&buttons->gameUiOpen,(!free||flat_frame())?1:0);
 U32 menuDown=0,menuUp=0;menuEdges.update(sample,GetTickCount64(),live,menuDown,menuUp);
 auto menuAction=menuGesture.update(sample,GetTickCount64(),live);
 if(native_menu::open()&&(menuDown&(controller_input::Menu|controller_input::RightGrip|controller_input::B))){queue_text(queueAddress,"buymenu 0\n");if(menuDown&controller_input::Menu)menuGesture.cancel();}
 else if((menuDown&controller_input::RightGrip)&&free&&shared->health>0&&!shared->life&&!shared->observer){queue_text(queueAddress,"buymenu 1\n");feedback(controller_input::MenuInput,0,sample.epoch);inputLog.msg("XR_BUY_TOGGLE",1,sample.sample);}
 else if(menuAction==controller_input::MenuGesture::LongPress||(menuAction==controller_input::MenuGesture::ShortPress&&!free)){
  queue_text(queueAddress,native_menu::game_open()?"gameui_hide\n":"gameui_activate\n");feedback(controller_input::MenuInput,0,sample.epoch);inputLog.msg("XR_MENU_TOGGLE",native_menu::game_open()?0:1,sample.sample);
 }else if(menuAction==controller_input::MenuGesture::ShortPress&&free&&shared->health>0&&!shared->life&&!shared->observer&&compare32(&recenterBusy,0,1)){
  recenterRequest={sample.epoch,sample.sample,GetTickCount64()};store32(&recenterBusy,0);
  inputLog.msg("XR_RECENTER_REQUEST",++recenterSerial,sample.sample,sample.epoch);feedback(controller_input::MenuInput,0,sample.epoch);
 }
 if(native_menu::game_open()&&(menuDown&controller_input::B))queue_text(queueAddress,"gameui_hide\n");
 native_menu::update(sample,GetTickCount64(),live);
 bool gameReady=live&&shared->health>0&&shared->life==0&&shared->observer==0&&free&&!flat_frame();
 U32 down=0,up=0;buttonEdges.update(sample,GetTickCount64(),gameReady,down,up);down&=~controller_input::Menu;
 button_commands(down,up,sample);
 ViewReference reference{};bool turnReady=gameReady&&(sample.actionMask&2)&&read_reference(reference)&&reference.epoch==sample.epoch;
 int turn=turnGate.update(sample.rightX,turnReady);
 if(turn){U32 degrees=buttons->snapDegrees;if(degrees!=15&&degrees!=30&&degrees!=45)degrees=30;float yaw=reference.yaw-turn*float(degrees);while(yaw>180)yaw-=360;while(yaw<-180)yaw+=360;
  static char turnStorage[8][96]{};static U32 turnSerial=0;char*text=turnStorage[(turnSerial++)&7];_snprintf(text,96,"setang %.6f %.6f 0\n",double(reference.pitch),double(yaw));queue_text(queueAddress,text);inputLog.msg("XR_SNAP_TURN_QUEUED",degrees,turn>0?1:2,sample.sample);
 }
}
// The optic's physical image is independent of this native gameplay state.
// Ask the original weapon to enter its first scope level through CUserCmd,
// before the stock verified copy/CRC. Never write player or weapon fields.
static bool equipped_optic(U32 player,U32&vm,U32&weapon,U32&checksum){
 U32 vh=0,wh=0,entry=0,serial=0,owner=0,wrapper=0,header=0;
 if(!player||!memory.read(player+0x3308,&vh,4)||vh==0xffffffffu||!memory.read(player+0x2f08,&wh,4)||wh==0xffffffffu)return false;
 entry=clientBase+0x4e051cc+(vh&65535)*16;
 if(!memory.read(entry,&vm,4)||!vm||!memory.read(entry+4,&serial,4)||serial!=(vh>>16)||!memory.read(vm+0x29d8,&owner,4)||owner!=wh)return false;
 entry=clientBase+0x4e051cc+(wh&65535)*16;
 if(!memory.read(entry,&weapon,4)||!weapon||!memory.read(entry+4,&serial,4)||serial!=(wh>>16)||!memory.read(vm+0x2950,&wrapper,4)||!wrapper||!memory.read(wrapper,&header,4)||!header||!memory.read(header+8,&checksum,4))return false;
 return ray_optics::profile(checksum)!=nullptr;
}
static void automatic_scope(void*cmd,U32 player){
 U32 vm=0,weapon=0,checksum=0,level=0;U8 resume=0;
 if(!equipped_optic(player,vm,weapon,checksum)||!memory.read(weapon+0x33e0,&level,4)||level>2||!memory.read(player+0x9976,&resume,1))return;
 U32 bits=0;memcpy(&bits,(U8*)cmd+48,4);
 bool request=level==0&&!resume;bits&=~2048u;if(request)bits|=2048u;memcpy((U8*)cmd+48,&bits,4);
 static U32 previousWeapon=0;static bool previousRequest=false;
 if(previousWeapon!=weapon||request!=previousRequest){inputLog.msg("O6_NATIVE_SCOPE_REQUEST",request,level,checksum);previousWeapon=weapon;previousRequest=request;}
}
static void apply_command(void*cmd,U32 player){
 auto*c=__atomic_load_n(&buttons,__ATOMIC_ACQUIRE);controller_input::Sample sample{};ViewReference reference{};
 I32 health=0,observer=-1;U8 life=255;
 if(!c||!cmd||!player||stopGameRequested||flat_frame()||!memory.read(player+0x100,&health,4)||health<=0||!memory.read(player+0x25f,&life,1)||life||!memory.read(player+0x3388,&observer,4)||observer||!controller_input::read(c,sample)||!controller_input::live(sample,GetTickCount64())||!(sample.actionMask&1)||!ui_free()||!read_reference(reference)||reference.epoch!=sample.epoch)return;
 automatic_scope(cmd,player);
 // Controlled offline FPS experiment only (not in the default 31 profile).
 // Both ABBA variants receive fire solely from the same live XR sample.
 // This prevents desktop mouse input from silently changing the workload.
 if(nova_fps::on(nova_fps::TestVrFireOnly)){
  U32 bits=0;memcpy(&bits,(U8*)cmd+48,4);bits&=~1u;if(sample.trigger>=.55f)bits|=1u;memcpy((U8*)cmd+48,&bits,4);
  // The existing native verified-command parser/CRC owns these exact target
  // fields (muzzle_core Key::select/subtype). Freeze loadout only in this test.
  U32 select=0;memcpy(&select,(U8*)cmd+56,4);if(select){static U32 blocked=0;if(++blocked<=4||blocked%120==0)inputLog.msg("F2_TEST_WEAPON_SELECTION_BLOCKED",select,blocked);}
  U32 zero=0;memcpy((U8*)cmd+56,&zero,4);memcpy((U8*)cmd+60,&zero,4);
 }
 float forward=0,side=0;controller_input::movement(sample.leftX,sample.leftY,reference.headYaw,forward,side);
 if(forward==0&&side==0&&!maskNativeDirections)return;memcpy((U8*)cmd+36,&forward,4);memcpy((U8*)cmd+40,&side,4);
 U32 before=0,after=0;memcpy(&before,(U8*)cmd+48,4);after=before;
 if(maskNativeDirections){
  after&=~(8u|16u|512u|1024u);
  if(directionalBridge){if(forward>.01f)after|=8u;else if(forward<-.01f)after|=16u;if(side>.01f)after|=1024u;else if(side<-.01f)after|=512u;}
  memcpy((U8*)cmd+48,&after,4);
 }
 static U64 nextMoveLog=0;U64 now=GetTickCount64();if(now>=nextMoveLog){nextMoveLog=now+100;char text[280];int n=_snprintf(text,sizeof(text),"NOVA_G3_MOVE tick=%llu bridge=%u mask=%u forward=%.5f side=%.5f before=%u after=%u\r\n",now,directionalBridge,maskNativeDirections,double(forward),double(side),before,after);U32 written=0;if(n>0&&inputLog.file!=BAD_HANDLE)WriteFile(inputLog.file,text,U32(n<279?n:279),&written,nullptr);}
 static U32 moves=0;if(++moves<5||moves%120==0){char text[200];int n=_snprintf(text,200,"NOVA_XR_ANALOG_MOVE sample=%llu forward=%.4f side=%.4f headYaw=%.4f\r\n",sample.sample,double(forward),double(side),double(reference.headYaw));U32 done=0;if(n>0&&inputLog.file!=BAD_HANDLE)WriteFile(inputLog.file,text,U32(n<199?n:199),&done,nullptr);}
}
static bool apply_grenade(void*cmd,U32 player,grenade_control::Input&out){
 if(!load32(&grenadeSteering)||!cmd||!player||stopGameRequested||flat_frame()||!ui_free())return false;
 ViewReference reference{};controller_input::Sample sample{};auto*c=__atomic_load_n(&buttons,__ATOMIC_ACQUIRE);U64 now=GetTickCount64();
 if(!c||!read_reference(reference)||!reference.throwValid||!controller_input::read(c,sample)||!controller_input::live(sample,now)||reference.epoch!=sample.epoch||now<reference.poseMs||now-reference.poseMs>250)return false;
 U32 handle=0,weapon=0,serial=0,vt=0;
 if(!memory.read(player+0x2f08,&handle,4)||handle==0xffffffffu)return false;
 U32 entry=clientBase+0x4e051cc+(handle&0xffff)*16;
 if(!memory.read(entry,&weapon,4)||!weapon||!memory.read(entry+4,&serial,4)||serial!=(handle>>16)||!memory.read(weapon,&vt,4))return false;
 U32 kind=0;for(;kind<6;kind++)if(vt==clientBase+grenade_control::grenadeVtables[kind])break;if(kind==6)return false;
 U32 vmh=0,vm=0,ownerH=0,owner=0;
 if(!memory.read(player+0x3308,&vmh,4)||vmh==0xffffffffu)return false;
 U32 vme=clientBase+0x4e051cc+(vmh&0xffff)*16;
 if(!memory.read(vme,&vm,4)||!vm||!memory.read(vme+4,&serial,4)||serial!=(vmh>>16)||!memory.read(vm+0x29dc,&ownerH,4)||ownerH==0xffffffffu)return false;
 U32 oe=clientBase+0x4e051cc+(ownerH&0xffff)*16;
 if(!memory.read(oe,&owner,4)||owner!=player||!memory.read(oe+4,&serial,4)||serial!=(ownerH>>16))return false;
 Angles before{};memcpy(&before,(U8*)cmd+12,12);Vec3 d=reference.throwDirection;
 Angles target{deg(atan2(-d.z,sqrt(d.x*d.x+d.y*d.y))),deg(atan2(d.y,d.x)),0};
 if(!handcore::finite(before)||!handcore::finite(target))return false;
 float forward=0,side=0;memcpy(&forward,(U8*)cmd+36,4);memcpy(&side,(U8*)cmd+40,4);
 float delta=rad(before.y-target.y),co=float(cos(delta)),si=float(sin(delta));
 float movedForward=co*forward+si*side,movedSide=co*side-si*forward;
 memcpy((U8*)cmd+12,&target,12);memcpy((U8*)cmd+36,&movedForward,4);memcpy((U8*)cmd+40,&movedSide,4);
 out={player,weapon,ownerH,handle,kind,reference.epoch,reference.poseSample,reference.poseMs,reference.gripDelta};
 // Regenerate the directional bits in this command's new yaw basis as well.
 if(directionalBridge){U32 bits=0;memcpy(&bits,(U8*)cmd+48,4);bits&=~(8u|16u|512u|1024u);if(movedForward>.01f)bits|=8u;else if(movedForward<-.01f)bits|=16u;if(movedSide>.01f)bits|=1024u;else if(movedSide<-.01f)bits|=512u;memcpy((U8*)cmd+48,&bits,4);}
 static U64 nextLog=0;if(now>=nextLog){nextLog=now+100;I32 number=0;memcpy(&number,(U8*)cmd+4,4);char text[400];int n=_snprintf(text,sizeof(text),"NOVA_G3_GRENADE_COMMAND tick=%llu cmd=%d sample=%llu epoch=%llu native=(%.5f,%.5f) target=(%.5f,%.5f) movement=(%.5f,%.5f)\r\n",now,number,reference.poseSample,reference.epoch,double(before.p),double(before.y),double(target.p),double(target.y),double(movedForward),double(movedSide));U32 written=0;if(n>0&&inputLog.file!=BAD_HANDLE)WriteFile(inputLog.file,text,U32(n<399?n:399),&written,nullptr);}
 return true;
}
static void initialize(U8*client,U8*engine,U32 hostPid){
 if(shared||!client||!engine)return;memory.base=clientBase=U32((uintptr_t)client);
 using Factory=void*(__cdecl*)(const char*,int*);
 auto factory=(Factory)GetProcAddress(engine,"CreateInterface");if(!factory)return;
 U32 object=U32((uintptr_t)factory("VEngineClient014",nullptr)),vt=0,execute=0;
 if(!memory.read(object,&vt,4)||!memory.read(vt+108*4,&execute,4)||execute!=U32((uintptr_t)engine)+0xb6660)return;
 const U8 prefix[]={0x55,0x8b,0xec,0x8b,0x55,0x08,0x33,0xc9,0x6a,0,0x6a,0,0xe8};U8 actual[20]{};I32 relative=0;
 if(!memory.read(execute,actual,sizeof(actual)))return;
 for(U32 k=0;k<sizeof(prefix);k++)if(actual[k]!=prefix[k])return;
 memcpy(&relative,actual+13,4);U32 target=U32(execute+17+relative);
 if(target!=U32((uintptr_t)engine)+0x1db910||actual[17]!=0x83||actual[18]!=0xc4||actual[19]!=8)return;
 wchar_t name[64]{};const wchar_t*base=L"Local\\NOVA_GAME_CONTROL_";U32 n=0;while(base[n]){name[n]=base[n];n++;}decimal_(name+n,GetCurrentProcessId());
 map=CreateFileMappingW(BAD_HANDLE,nullptr,4,0,sizeof(Control),name);
 if(!map||GetLastError()==183){if(map)CloseHandle(map);map=nullptr;return;}
 shared=(Control*)MapViewOfFile(map,0xf001f,0,0,sizeof(Control));if(!shared){CloseHandle(map);map=nullptr;return;}
 expectedHost=hostPid;inputSystemBase=U32((uintptr_t)GetModuleHandleW(L"inputsystem.dll"));inputLog.open(L"nova_input.log");queueAddress=target;shared->version=1;shared->bytes=sizeof(Control);shared->pid=GetCurrentProcessId();barrier();shared->magic=0x4347564e;
}
static void state(){
 if(!shared||GetTickCount64()-lastState<50)return;lastState=GetTickCount64();
 I32 seq=I32(load32(&shared->stateSequence));store32(&shared->stateSequence,seq+1);barrier();
 shared->tick=lastState;shared->thread=ownerThread;shared->health=0;shared->team=0;shared->life=255;shared->observer=-1;shared->checksum=shared->bones=0;shared->model[0]=0;
 U32 p=0,h=0,entry=0,vm=0,serial=0,wrapper=0,header=0,magic=0;
 if(memory.read(clientBase+0x52bf6d8,&p,4)&&p){
  memory.read(p+0x100,&shared->health,4);memory.read(p+0xf4,&shared->team,4);U8 life=255;memory.read(p+0x25f,&life,1);shared->life=life;memory.read(p+0x3388,&shared->observer,4);
  if(memory.read(p+0x3308,&h,4)&&h!=0xffffffffu){entry=clientBase+0x4e051cc+(h&0xffff)*16;
   if(memory.read(entry,&vm,4)&&vm&&memory.read(entry+4,&serial,4)&&serial==(h>>16)&&memory.read(vm+0x2950,&wrapper,4)&&memory.read(wrapper,&header,4)&&memory.read(header,&magic,4)&&magic==0x54534449){
    memory.read(header+8,&shared->checksum,4);memory.read(header+0x9c,&shared->bones,4);memory.read(header+12,shared->model,64);shared->model[63]=0;
   }
  }
 }
 barrier();store32(&shared->stateSequence,seq+2);
}
static void poll(U32 renderOwner){
 if(!shared||!queueAddress||!renderOwner)return;U32 thread=GetCurrentThreadId();if(thread!=renderOwner)return;
 nova_perf::Scope timing(nova_perf::GameControl);
 if(!ownerThread){ownerThread=renderOwner;inputLog.msg("F2_NATIVE_CONTROL_GRAPHICS_OWNER",ownerThread);}if(ownerThread!=thread)return;
 U32 request=load32(&shared->request);
 if(request!=load32(&shared->ack)){
  U32 n=shared->commandBytes;I32 result=2;char*text=commandStorage[request&7];
  if(n>0&&n<=510){memcpy(text,shared->command,n);barrier();bool okay=request==load32(&shared->request);
   for(U32 k=0;k<n;k++)if(U8(text[k])<32||U8(text[k])>126)okay=false;
   if(okay){text[n]='\n';text[n+1]=0;
    inputLog.msg("F2_CONTROL_DISPATCH",request,n,thread);
    if(inputLog.file!=BAD_HANDLE){U32 done=0;WriteFile(inputLog.file,text,n+1,&done,nullptr);}
    if(n==18&&streq(text,"__NOVA_STOP_GAME__\n")){stopGameRequested=true;inputLog.msg("F2_NATIVE_STOP_REQUEST",request,thread);}
    else if(streq(text,"__NOVA_NATIVE_SCREEN_ON__\n"))testNativeScreen=true;
    else if(streq(text,"__NOVA_NATIVE_SCREEN_OFF__\n"))testNativeScreen=false;
    else if(streq(text,"__NOVA_CAMERA_BASE__\n"))stable_camera::mode(true);
    else if(streq(text,"__NOVA_CAMERA_STOCK__\n"))stable_camera::mode(false);
    else if(streq(text,"__NOVA_GRENADE_ON__\n")){store32(&grenadeSteering,1);inputLog.msg("G3_GRENADE_STEERING",1);}
    else if(streq(text,"__NOVA_GRENADE_OFF__\n")){store32(&grenadeSteering,0);inputLog.msg("G3_GRENADE_STEERING",0);}
    else if(streq(text,"__NOVA_GRENADE_ORIGIN_ON__\n")){store32(&grenadeOrigin,1);inputLog.msg("G4_GRENADE_ORIGIN",1);}
    else if(streq(text,"__NOVA_GRENADE_ORIGIN_OFF__\n")){store32(&grenadeOrigin,0);inputLog.msg("G4_GRENADE_ORIGIN",0);}
    else if(streq(text,"__NOVA_DIRECTION_BITS_ON__\n")){directionalBridge=maskNativeDirections=true;inputLog.msg("G3_DIRECTION_MODE",1);}
    else if(streq(text,"__NOVA_DIRECTION_BITS_OFF__\n")){directionalBridge=maskNativeDirections=false;inputLog.msg("G3_DIRECTION_MODE",0);}
    else if(streq(text,"__NOVA_DIRECTION_BITS_ZERO__\n")){directionalBridge=false;maskNativeDirections=true;inputLog.msg("G3_DIRECTION_MODE",2);}
    else if(n>=14&&text[0]=='_'&&text[1]=='_'&&text[2]=='N'&&text[3]=='O'&&text[4]=='V'&&text[5]=='A'&&text[6]=='_'&&text[7]=='F'&&text[8]=='P'&&text[9]=='S'&&text[10]=='_'){
     U32 value=0,k=11;for(;k<n&&text[k]>='0'&&text[k]<='9';k++)value=value*10+U32(text[k]-'0');
     if(k>11&&k+2==n&&text[k]=='_'&&text[k+1]=='_'&&(value&~4095u)==0){nova_fps::set(value);nova_perf::set_enabled(!(value&nova_fps::ProfilingOff));inputLog.msg("F1_POLICY",value,nova_fps::skippedPalm,nova_fps::skippedMenu);}else result=2;
    }
    else if(streq(text,"__NOVA_OUTPUT_INTAKE_ON__\n")){store32(&outputIntake,1);inputLog.msg("R1_OUTPUT_INTAKE",1);}
    else if(streq(text,"__NOVA_OUTPUT_INTAKE_OFF__\n")){store32(&outputIntake,0);inputLog.msg("R1_OUTPUT_INTAKE",0);}
    else if(streq(text,"__NOVA_VISUAL_RECOIL_ON__\n")){store32(&visualRecoil,1);inputLog.msg("R1_VISUAL_RECOIL",1);}
    else if(streq(text,"__NOVA_VISUAL_RECOIL_OFF__\n")){store32(&visualRecoil,0);inputLog.msg("R1_VISUAL_RECOIL",0);}
    else if(streq(text,"__NOVA_READ_BATCH_ON__\n")){store32(&additionalReadBatch,1);inputLog.msg("G5_ADDITIONAL_READ_BATCH",1);}
    else if(streq(text,"__NOVA_READ_BATCH_OFF__\n")){store32(&additionalReadBatch,0);inputLog.msg("G5_ADDITIONAL_READ_BATCH",0);}
    else queue_text(queueAddress,text);
    result=0;
   }
  }
  store32(&shared->result,result);store32(&shared->ack,I32(request));
 }
 static U64 nextUiTry=0;if(!load32(&native_menu::installed)&&GetTickCount64()>=nextUiTry){nextUiTry=GetTickCount64()+1000;native_menu::initialize(clientBase,U32((uintptr_t)GetModuleHandleW(L"engine.dll")),U32((uintptr_t)GetModuleHandleW(L"inputsystem.dll")));}
 native_menu::refresh_game_ui();
 state();poll_buttons();
}
static bool stop_requested(){return stopGameRequested;}
static void queue_quit(){stopGameRequested=false;if(queueAddress)queue_text(queueAddress,"quit\n");}
static void hand(U64 frame,bool active,U32 reason,Vec3 observed,Vec3 target,const handcore::Rotation&axes){
 if(!shared)return;I32 seq=I32(load32(&shared->stateSequence));store32(&shared->stateSequence,seq+1);barrier();
 shared->handActive=active?1:0;shared->handReason=reason;shared->handFrame=frame;shared->observedPalm=observed;shared->targetPalm=target;
 memcpy(shared->observedAxes,axes.a,sizeof(axes.a));
 barrier();store32(&shared->stateSequence,seq+2);
}
#else
static inline void initialize(U8*,U8*,U32){}
static inline void poll(){}
static inline void hand(U64,bool,U32,Vec3,Vec3,const handcore::Rotation&){}
#endif
}
