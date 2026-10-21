#pragma once
#include "native_memory.hpp"
#include "muzzle_core.hpp"
#include "muzzle_exact.hpp"
#include "fx_core.hpp"
#include "weapon_body.hpp"
#include "perf.hpp"
#ifdef NOVA_LAB_UNIVERSAL
#include "grenade_exact.hpp"
#endif
// Local-listen-server command bridge and owned viewmodel attachment outputs.
// No viewangle write, synthetic bullet/effect or latest-pose read in a shot hook.
namespace muzzle_native {
using namespace nova;using namespace handcore;using namespace muzzle;
using Mem=handcore_native::NativeMemory;
static Mem*mem{};static Driver<Mem>*driver{};static U32 modules[4]{},renderThread{};
static bool initialized{},permanentFailure{},muzzleOff{},universalMuzzle{};static U64 nextTry{},lastStatus{};
static volatile I32 live{},stopped{},handOn{},faulted{};static Cell<U64> trackingEpoch;
static Log log;static volatile I32 logBusy{};
static Cell<Frame> frameCell;static Cell<Peer> peerCell;static History history;
static volatile I32 boneCalls{},boneExpanded{},frameOkay{},frameReject{},commands{},commandsAllowed{},commandsBlocked{},serverBound{},angleClient{},angleServer{},originClient{},originServer{},shotReject{},clamps{},serverTraces{},observerRejected{},logDropped{};
static Frame building{};static Pose firstMuzzle{};static U32 firstCounter{};static bool firstOkay{},secondOkay{};
static U64 expandedAtBegin{};static MuzzleReason lastMuzzleReason=MuzzleReason::NoLease;
static ModelMuzzle modelMuzzle;
namespace body_authority {
static bool supported(U32);static bool valid_source(const Frame&);static bool is_rendering();
static bool owns_commands(const Identity&);
static bool command_source(const Identity&,U32,U32,const Key&,Frame&);
static bool fx_target(const Frame&,Frame&);
static void output(Matrix*,int,bool);static void begin(const Tracking&,Vec3,Angles,Vec3,Quat);
}

// These scopes contain checked reads and copied arithmetic only. They end
// before native methods, setters or rendering are invoked. Nested identity
// admissions retain their full value/ownership checks and 16-hit permission cap.
struct AdmissionBatch {
 bool active{};
 AdmissionBatch(){
#ifdef NOVA_LAB_UNIVERSAL
  active=mem&&load32(&native_game_control::additionalReadBatch)&&mem->thread==GetCurrentThreadId();
  if(active)mem->begin_read_batch();
#endif
 }
 ~AdmissionBatch(){if(active)mem->end_read_batch();}
};
struct FxFrame {Frame source{};weapon_fx::Attachments attachments{};};
static Cell<FxFrame> fxCell;static weapon_fx::Attachments firstFx{},buildingFx{};
static bool firstFxOkay{},secondFxOkay{};static volatile I32 fxMatrixCalls{},fxAnglesCalls{},fxPublished{},fxRejected{};
struct ClassEntry {U32 vtable{},tag{};};struct ClassCache {U32 count{};ClassEntry entries[64]{};};
static Cell<ClassCache> classCache;
static bool native_gun_class(U32 vt,bool server,U32&tag){
 ClassCache cache{};if(!classCache.read(cache))return false;
 for(U32 i=0;i<cache.count;i++)if(cache.entries[i].vtable==vt){tag=cache.entries[i].tag;return tag!=0;}
 bool okay=gun_class(*mem,modules[server?1:0],muzzle_exact::modules[server?1:0].size,vt,server,tag);
 if(cache.count<64){cache.entries[cache.count++]={vt,okay?tag:0};classCache.write(cache);}
 return okay;
}
template<class T> static bool rd(U32 a,T&out){return mem&&a&&mem->read(a,&out,sizeof(out));}
static U32 addr(const void*p){return U32((uintptr_t)p);}
static bool writeable(void*p,size_t n){if(!mem||!p||!n||U64(addr(p))+n>0x100000000ull)return false;int resident=native_memory_access::probe(p,n,true);if(resident)return resident>0;MEMORY_BASIC_INFORMATION m{};if(VirtualQuery(p,&m,sizeof(m))<sizeof(m)||m.State!=0x1000||(m.Protect&0x100))return false;U32 prot=m.Protect&0xff;return (prot==4||prot==8||prot==0x40||prot==0x80)&&U64(addr(p))+n<=U64((uintptr_t)m.BaseAddress)+m.RegionSize;}

namespace visual_recoil {
static Angles punch{};static float scale=0;static int eye=-1;static bool drawn[2]{};
static U32 variable=0;static U64 nextCvar=0;static float nativeScale=0;
static U32 flagHeader{},flagChecksum{},flagCount{},boneFlags[256]{};
struct Parameters {Identity id{};U64 epoch{},atMs{};float scale{};};
static Cell<Parameters> parameterCell;
static float read_scale(){
 U64 now=GetTickCount64();if(now<nextCvar)return nativeScale;nextCvar=now+1000;
 if(!variable){
  U32 lib=addr(GetModuleHandleW(L"vstdlib.dll"));if(!lib)return 0;
  using Factory=void*(__cdecl*)(const char*,int*);auto factory=(Factory)GetProcAddress((HMODULE)(uintptr_t)lib,"CreateInterface");if(!factory)return 0;
  U32 object=addr(factory("VEngineCvar007",nullptr)),table=0,method=0;
  const U8 expected[]={0x55,0x8b,0xec,0x8b,0x01,0x56,0xff,0x75,0x08,0xff,0x50,0x38,0x8b,0xf0,0x85,0xf6};U8 code[16]{};
  if(!rd(object,table)||!rd(table+16*4,method)||method!=lib+0x4430||!mem->read(method,code,sizeof(code)))return 0;
  for(U32 i=0;i<16;i++)if(code[i]!=expected[i])return 0;
  using Find=void*(__thiscall*)(void*,const char*);U32 value=addr(((Find)(uintptr_t)method)((void*)(uintptr_t)object,"weapon_recoil_scale")),name=0;char text[20]{};
  if(!value||!rd(value+12,name)||!mem->read(name,text,20)||!streq(text,"weapon_recoil_scale"))return 0;variable=value;
 }
 U32 parent=0,bits=0;if(!rd(variable+28,parent)||!rd(parent+44,bits))return 0;bits^=parent;float result=0;memcpy(&result,&bits,4);
 if(!finite_in_range(result,20)||result<0)return 0;nativeScale=result;return result;
}
static void reset(){punch={};scale=0;eye=-1;drawn[0]=drawn[1]=false;}
static void begin(U32 player){
 if(load32(&native_game_control::visualRecoil)){
  Angles value{};float factor=read_scale();if(factor>0&&rd(player+0x303c,value)&&finite(value)){punch=value;scale=factor;}
 }
 parameterCell.write({building.id,building.epoch,GetTickCount64(),scale});
}
static bool command_punch(const Identity&id,U64 epoch,Angles&value,float&factor){
 Parameters p{};U64 now=GetTickCount64();
 if(!parameterCell.read(p)||!same_identity(p.id,id)||p.epoch!=epoch||now<p.atMs||now-p.atMs>250)return false;
 factor=p.scale;value={};return factor<=0||(rd(id.player+0x303c,value)&&finite(value));
}
static bool delta(const Pose&base,Matrix&out){return driver&&weapon_body::recoil_delta(base,driver->gripWorld,punch,scale,out);}
static bool active(){return scale>0&&(absf(punch.p)+absf(punch.y)+absf(punch.r))>.000001f;}
static void output(Matrix*out,int count,bool success){
 if(!active()||!success||eye<0||eye>1||!driver||!driver->leased||!out||!modelMuzzle.checksum||modelMuzzle.checksum!=building.modelChecksum||count<I32(modelMuzzle.count)||modelMuzzle.count>256||!driver->checkpoint())return;
 U32 cache=0,alias=0;if(!rd(driver->lease.vm+0x2914,cache)||!rd(driver->lease.vm+0x26a8,alias))return;
 U64 a=addr(out),end=a+modelMuzzle.count*48,n=modelMuzzle.count*48;
 if((a<cache+n&&end>cache)||(a<alias+n&&end>alias)||!writeable(out,n))return;
 Matrix barrel=compose(out[modelMuzzle.bone],modelMuzzle.local),d{};
 if(!matrix_valid(barrel)||!delta(matrix_pose(barrel),d))return;
 if(flagHeader!=driver->lease.header||flagChecksum!=modelMuzzle.checksum||flagCount!=modelMuzzle.count){
  U32 bo=0,size=0;flagHeader=0;
  if(!rd(driver->lease.header+76,size)||!rd(driver->lease.header+0xa0,bo)||bo>size||modelMuzzle.count*216>size-bo)return;
  for(U32 i=0;i<modelMuzzle.count;i++)if(!rd(driver->lease.header+bo+i*216+160,boneFlags[i]))return;
  flagHeader=driver->lease.header;flagChecksum=modelMuzzle.checksum;flagCount=modelMuzzle.count;
 }
 Matrix value[256]{};for(U32 i=0;i<modelMuzzle.count;i++){value[i]=out[i];if(!(boneFlags[i]&0x600))continue;if(!matrix_valid(out[i]))return;value[i]=compose(d,out[i]);if(!matrix_valid(value[i]))return;}
 memcpy(out,value,n);drawn[eye]=true;
}
}

static void line(const char*s,int n){if(n<=0)return;if(!compare32(&logBusy,0,1)){inc32(&logDropped);return;}if(log.file!=BAD_HANDLE){U32 done=0;WriteFile(log.file,s,U32(n),&done,nullptr);}store32(&logBusy,0);}
static void note(const char*tag,U64 a=0,U64 b=0,U64 c=0){char text[300];int n=_snprintf(text,sizeof(text),"NOVA_H2_%s tick=%llu a=%llu b=%llu c=%llu\r\n",tag,GetTickCount64(),a,b,c);line(text,n>299?299:n);}
static bool module_identity(unsigned index){
 U32 base=modules[index],pe=0,sig=0,ts=0,sz=0;U16 mz=0,machine=0,kind=0;
 return rd(base,mz)&&mz==0x5a4d&&rd(plus(base,0x3c),pe)&&pe>=64&&pe<0x10000&&rd(plus(base,pe),sig)&&sig==0x4550&&rd(plus(base,pe+4),machine)&&machine==0x14c&&rd(plus(base,pe+8),ts)&&ts==muzzle_exact::modules[index].timestamp&&rd(plus(base,pe+24),kind)&&kind==0x10b&&rd(plus(base,pe+24+56),sz)&&sz==muzzle_exact::modules[index].size;
}
static bool exact(){
 for(unsigned i=0;i<4;i++)if(!module_identity(i)){note("MODULE_IDENTITY_FAIL",i);return false;}
 for(const auto&g:muzzle_exact::guards){U8 actual[128]{},wanted[128]{};U32 base=modules[g.module];if(g.n>128||!mem->read(plus(base,g.rva),actual,g.n))return false;memcpy(wanted,g.b,g.n);
  for(U32 r=0;r<g.nrel;r++){U32 value=0;memcpy(&value,wanted+g.rel[r],4);value=base+(value-0x10000000u);memcpy(wanted+g.rel[r],&value,4);}
  for(U32 n=0;n<g.n;n++)if(actual[n]!=wanted[n]){note("EXACT_WINDOW_FAIL",g.module,g.rva,n);return false;}
 }return true;
}
static bool resolve(U32 h,bool server,U32&entity){
 if(h==0xffffffffu)return false;U32 base=modules[server?1:0],entry=plus(base,(server?0xab05bc:0x4e051cc)+U32(h&0xffffu)*(server?24:16));U32 serial=0;
 return rd(entry,entity)&&entity&&rd(plus(entry,4),serial)&&serial==(h>>16);
}
static int entity_index(U32 entity,bool server){
 if(!server){I32 n=0;return rd(plus(entity,0x64),n)&&n>0&&n<8192?n:0;}
 U32 globals=0,base=0,edict=0;if(!rd(plus(modules[1],0xa968e0),globals)||!rd(plus(globals,0x60),base)||!base||!rd(plus(entity,0x1c),edict)||edict<base||(edict-base)%16||(edict-base)/16>=8192)return 0;return int((edict-base)/16);
}
// This read is intentionally independent from Driver::identity(), which owns
// mutable render-side diagnostics. No command/server thread enters that driver.
static bool local_identity(Identity&out,U32&playerIndex,U32&weaponIndex,U32*classTag=nullptr,U32*modelSum=nullptr){
 AdmissionBatch batch;
 U32 b=modules[0],vt=0,p=0,owner=0,vmh=0,wh=0,active=0,cs=0,tag=0;I32 hp=0,observer=0;U8 life=255;
 if(!rd(plus(b,0x52bf6d8),p)||!p||!rd(p,vt)||vt!=plus(b,0xbfcbcc)||!rd(plus(p,0x25f),life)||life||!rd(plus(p,0x100),hp)||hp<=0||!rd(plus(p,0x3388),observer)||observer)return false;
 out.player=p;if(!rd(plus(p,0x3308),vmh)||!resolve(vmh,false,out.vm)||!rd(out.vm,vt)||vt!=plus(b,0xbe09f4))return false;out.vmHandle=vmh;
 if(!rd(plus(out.vm,0x29dc),out.ownerHandle)||!resolve(out.ownerHandle,false,owner)||owner!=p||!rd(plus(out.vm,0x29d8),wh)||!rd(plus(p,0x2f08),active)||wh!=active||!resolve(wh,false,out.weapon)||!rd(out.weapon,vt))return false;out.weaponHandle=wh;
 if(universalMuzzle){if(!native_gun_class(vt,false,tag))return false;}
 else if(vt!=plus(b,0xc95140))return false;
 if(!rd(plus(out.vm,0x258),out.model)||!rd(plus(out.vm,0x2950),out.wrapper)||!rd(out.wrapper,out.header)||!rd(plus(out.header,8),cs))return false;
 if(!universalMuzzle&&cs!=1475988351u)return false;
 U32 magic=0,version=0,size=0,count=0;
 if(universalMuzzle&&(!rd(out.header,magic)||magic!=0x54534449u||!rd(plus(out.header,4),version)||version<44||version>49||!rd(plus(out.header,76),size)||size<0x100||size>32u*1024u*1024u||!rd(plus(out.header,0x9c),count)||count<1||count>256))return false;
 int pi=entity_index(p,false),wi=entity_index(out.weapon,false);if(pi<=0||pi>64||wi<=0||wi==pi)return false;playerIndex=U32(pi);weaponIndex=U32(wi);if(classTag)*classTag=tag;if(modelSum)*modelSum=cs;return true;
}
static bool current_local(const Frame&f){Identity i{};U32 pi=0,wi=0,tag=0,cs=0;return body_authority::valid_source(f)&&local_identity(i,pi,wi,&tag,&cs)&&same_identity(i,f.id)&&pi==f.playerIndex&&wi==f.weaponIndex&&(!universalMuzzle||(tag==f.weaponClass&&cs==f.modelChecksum));}
static bool pointer_free(bool server,U32 player){
 U32 value=0;
 if(server){if(!rd(plus(player,0x105c),value))return false;}
 else{U32 obj=0,vt=0,fn=0;if(!rd(plus(modules[0],0x534d6b8),obj)||!rd(obj,vt)||vt!=plus(modules[3],0x30ea0)||!rd(plus(vt,272),fn)||fn!=plus(modules[3],0x4db0)||!rd(plus(obj,0x6044),value))return false;}
 return value!=4&&value!=8&&value!=16;
}
static bool server_peer(U32 self,const Frame&source,Peer&out){
 U32 vt=0,wh=0,w=0,ownerH=0,owner=0,tag=0;I32 hp=0;U8 life=255;
 if(!rd(self,vt)||(vt!=plus(modules[1],0x8fcc20)&&vt!=plus(modules[1],0x919ec0)&&vt!=plus(modules[1],0x91a964))||!rd(plus(self,0x249),life)||life||!rd(plus(self,0x230),hp)||hp<=0)return false;
 if(!rd(plus(self,0x954),wh)||!resolve(wh,true,w)||!rd(w,vt)||!rd(plus(w,0x930),ownerH)||!resolve(ownerH,true,owner)||owner!=self)return false;
 if(universalMuzzle){if(!native_gun_class(vt,true,tag)||tag!=source.weaponClass)return false;}
 else if(vt!=plus(modules[1],0x93e974))return false;
 int pi=entity_index(self,true),wi=entity_index(w,true);if(pi<=0||pi>64||U32(pi)!=source.playerIndex||wi<=0||wi==pi||U32(wi)!=source.weaponIndex)return false;
 out={self,ownerH,w,wh,U32(pi),U32(wi),source.id.ownerHandle,source.id.weaponHandle,GetTickCount64(),tag};return pointer_free(true,self);
}
struct alignas(16) Ray {float start[4],delta[4],startOffset[4],extents[4];U32 transform;U8 isRay,isSwept,pad[10];};
static_assert(sizeof(Ray)==80,"Exact x86 Ray storage");
struct Trace {U8 b[84];};struct Filter {U32 vtable,skip,group,extra;};
using TraceFn=void(__thiscall*)(void*,const Ray*,U32,void*,Trace*);
using CreateFn=void(__thiscall*)(void*,int,float,bool);
using MimicFn=void(__thiscall*)(void*,int,void*,void*);
using StartFn=void(__thiscall*)(void*,void*,void*);
using EyeFn=const Angles*(__thiscall*)(void*);
using ShootFn=Vec3*(__thiscall*)(void*,Vec3*);
using BonesFn=bool(__thiscall*)(void*,Matrix*,int,int,float);
using AttachmentMatrixFn=bool(__thiscall*)(void*,int,Matrix*);
using AttachmentAnglesFn=bool(__thiscall*)(void*,int,Vec3*,Angles*);
static AttachmentMatrixFn originalAttachmentMatrix{};static AttachmentAnglesFn originalAttachmentAngles{};
static TraceFn originalTrace{};static void*traces[2]{};static BonesFn originalBones{};static CreateFn originalCreate{};static MimicFn originalMimic{};static StartFn originalStart{};static EyeFn originalEye[2]{};static ShootFn originalShoot[2]{};
static bool trace_segment(bool server,U32 self,Vec3 start,Vec3 end,TraceResult&result){
 float n=length(sub(end,start));if(!traces[server]||!originalTrace||!finite(start)||!finite(end)||!(n>.0001f&&n<120.f))return false;
 Ray ray{};ray.start[0]=start.x;ray.start[1]=start.y;ray.start[2]=start.z;Vec3 d=sub(end,start);ray.delta[0]=d.x;ray.delta[1]=d.y;ray.delta[2]=d.z;ray.isRay=ray.isSwept=1;
 Filter f{plus(modules[server?1:0],server?0x8dce68:0xbea9c4),self,0,0};Trace r{};
 // Call the original method, not our observation hook. Native filtering/raycast,
 // no extra FireBullet/FX or replacement of the original shot trace.
 originalTrace(traces[server],&ray,0x4600400b,&f,&r);return decode_trace(r.b,sizeof(r.b),result);
}
static U32 client_crc(const U8*b){
 static constexpr U8 offset[]={4,8,12,24,36,40,44,48,52,56,60,64,68,70},size[]={4,4,12,12,4,4,4,4,1,4,4,4,2,2};U32 c=0xffffffffu;
 for(unsigned k=0;k<14;k++)for(unsigned j=0;j<size[k];j++){c^=b[offset[k]+j];for(int bit=0;bit<8;bit++)c=(c>>1)^(0xedb88320u&U32(0-I32(c&1)));}return ~c;
}
#ifdef NOVA_LAB_UNIVERSAL
struct GrenadeCommand {Key key{};grenade_control::Input source{};Peer peer{};U64 madeMs{};U32 crc{},bound{};};
struct GrenadeHistory {
 Cell<GrenadeCommand> entries[256];
 bool put(const GrenadeCommand&c){return c.key.number>0&&entries[U32(c.key.number)%256].write(c);}
 bool get(I32 n,GrenadeCommand&c){return n>0&&entries[U32(n)%256].read(c)&&c.key.number==n;}
 bool bind(const Key&k,const Peer&p){
  auto&cell=entries[U32(k.number)%256];if(!compare32(&cell.busy,0,1))return false;
  bool okay=key_same(cell.value.key,k,true);if(okay){cell.value.peer=p;cell.value.bound=1;}store32(&cell.busy,0);return okay;
 }
};
static GrenadeHistory grenadeHistory;
static bool current_grenade(const grenade_control::Input&i){
 U32 player=0,weapon=0,owner=0,h=0,vt=0;I32 hp=0;U8 life=255;
 return i.kind<6&&rd(modules[0]+0x52bf6d8,player)&&player==i.player&&rd(player+0x100,hp)&&hp>0&&rd(player+0x25f,life)&&!life&&
  resolve(i.ownerHandle,false,owner)&&owner==player&&rd(player+0x2f08,h)&&h==i.weaponHandle&&resolve(h,false,weapon)&&weapon==i.weapon&&
  rd(weapon,vt)&&vt==modules[0]+grenade_control::grenadeVtables[i.kind]&&pointer_free(false,player);
}
static bool grenade_peer(U32 player,const grenade_control::Input&i,Peer&out){
 U32 vt=0,h=0,weapon=0,ownerH=0,owner=0;I32 hp=0;U8 life=255;
 if(i.kind>=6||!rd(player,vt)||(vt!=modules[1]+0x8fcc20&&vt!=modules[1]+0x919ec0&&vt!=modules[1]+0x91a964)||!rd(player+0x230,hp)||hp<=0||!rd(player+0x249,life)||life||
  !rd(player+0x954,h)||!resolve(h,true,weapon)||!rd(weapon,vt)||vt!=modules[1]+grenade_control::emitGuards[i.kind].vtable||
  !rd(weapon+0x930,ownerH)||!resolve(ownerH,true,owner)||owner!=player||!pointer_free(true,player))return false;
 int pi=entity_index(player,true),wi=entity_index(weapon,true);
 if(pi<=0||pi>64||pi!=entity_index(i.player,false)||wi<=0||wi==pi||wi!=entity_index(i.weapon,false))return false;
 out={player,ownerH,weapon,h,U32(pi),U32(wi),i.ownerHandle,i.weaponHandle,GetTickCount64(),i.kind+1};return true;
}
using GrenadeEmitFn=void(__thiscall*)(void*,Vec3,Angles,Vec3,Vec3,void*);
static GrenadeEmitFn originalGrenadeEmit[6]{};
static bool grenade_exact(){
 for(const auto&g:grenade_control::emitGuards){
  U8 actual[320]{},wanted[320]{};if(g.bytes>320||g.relocationCount>16||!mem->read(modules[1]+g.rva,actual,g.bytes))return false;
  memcpy(wanted,g.code,g.bytes);for(U32 k=0;k<g.relocationCount;k++){
   U32 offset=g.relocations[k],v=0;if(offset+4>g.bytes)return false;memcpy(&v,wanted+offset,4);v=modules[1]+(v-0x10000000u);memcpy(wanted+offset,&v,4);
  }
  for(U32 k=0;k<g.bytes;k++)if(actual[k]!=wanted[k]){note("G4_GRENADE_EXACT_FAIL",g.rva,k);return false;}
 }return true;
}
static bool grenade_origin(U32 kind,U32 weapon,U32 player,Vec3&origin){
 U32 cmd=0,global=0;U8 raw[0x44]{};Key k{};GrenadeCommand c{};Peer peer{};U64 now=GetTickCount64(),epoch=0;
 auto reject=[&](U32 reason){note("G4_GRENADE_ORIGIN_RETAINED",kind,reason,U32(k.number));return false;};
 if(!load32(&live)||load32(&stopped)||!load32(&native_game_control::grenadeOrigin))return false;
 if(!player||!rd(player+0x1034,cmd)||!cmd||!rd(modules[1]+0xba18ac,global)||global!=player||!mem->read(cmd,raw,sizeof(raw))||!parse(raw,sizeof(raw),k))return reject(1);
 if(!grenadeHistory.get(k.number,c)||!c.bound||c.source.kind!=kind||!no_pointer(k)||k.select||!key_same(c.key,k,true)||now<c.madeMs||now-c.madeMs>2000)return reject(2);
 if(!current_grenade(c.source)||!grenade_peer(player,c.source,peer)||peer.weapon!=weapon||!peer_same(c.peer,peer))return reject(3);
 native_game_control::ViewReference reference{};
 if(!trackingEpoch.read(epoch)||epoch!=c.source.epoch||!native_game_control::read_reference(reference)||reference.epoch!=c.source.epoch||!native_game_control::ui_free()||
  now<c.source.poseMs||now-c.source.poseMs>500||!finite(c.source.gripDelta)||length(c.source.gripDelta)>=80)return reject(4);
 Vec3 eye{};if(!originalShoot[1]||originalShoot[1]((void*)(uintptr_t)player,&eye)!=&eye||!finite(eye))return reject(5);
 Vec3 target=add(eye,c.source.gripDelta),delta=sub(target,eye);float n=length(delta);TraceResult tr{};
 if(!traces[1]||!originalTrace||n>=80)return reject(6);
 Ray ray{};ray.start[0]=eye.x;ray.start[1]=eye.y;ray.start[2]=eye.z;ray.delta[0]=delta.x;ray.delta[1]=delta.y;ray.delta[2]=delta.z;
 ray.extents[0]=ray.extents[1]=ray.extents[2]=2;ray.isSwept=n>.0001f;
 // Exact native grenade setup (58F850): MASK 0208400B, hull [-2,+2],
 // CTraceFilterSimple 8DCE68. This keeps the native solid/entity filtering.
 Filter filter{modules[1]+0x8dce68,player,0,0};Trace native{};originalTrace(traces[1],&ray,0x208400b,&filter,&native);
 if(!decode_trace(native.b,sizeof(native.b),tr)||tr.startsolid||tr.allsolid)return reject(7);
 Vec3 accepted=target;if(tr.fraction<.99999f&&n>.0001f){float t=tr.fraction-.25f/n;if(t<0)t=0;accepted=add(eye,scale(delta,t));}
 if(!finite(accepted))return reject(8);
 Vec3 stock=origin;origin=accepted;
 char text[950];int z=_snprintf(text,sizeof(text),"NOVA_G4_GRENADE_EMIT kind=%u cmd=%d sample=%llu epoch=%llu stock=(%.6f,%.6f,%.6f) eye=(%.6f,%.6f,%.6f) grip=(%.6f,%.6f,%.6f) accepted=(%.6f,%.6f,%.6f) fraction=%.7f hull=2 mask=0208400B poseAgeMs=%llu\r\n",kind,k.number,c.source.sample,c.source.epoch,double(stock.x),double(stock.y),double(stock.z),double(eye.x),double(eye.y),double(eye.z),double(target.x),double(target.y),double(target.z),double(accepted.x),double(accepted.y),double(accepted.z),double(tr.fraction),now-c.source.poseMs);line(text,z>949?949:z);
 return true;
}
template<U32 Kind> static void __fastcall grenade_emit_hook(void*self,void*,Vec3 source,Angles angles,Vec3 velocity,Vec3 spin,void*player){
 grenade_origin(Kind,addr(self),addr(player),source);
 originalGrenadeEmit[Kind](self,source,angles,velocity,spin,player);
}
#endif
struct Context {bool inputActive{},inCreate{},mimic{},controlledGrenade{},blocked{},angles{},origin{};U32 input{},commandPtr{},self{},family{},cmdNumber{},sourceThread{};I32 sequence{};U64 angleMs{},originMs{};Command candidate{},shot{};Vec3 finalOrigin{};Angles returned{};
#ifdef NOVA_LAB_UNIVERSAL
 grenade_control::Input grenadeInput{};
#endif
};
struct Thread {volatile I32 id{};Context side[2]{};};static Thread threads[16];
static Context*context(bool server){U32 id=GetCurrentThreadId();if(!id)return nullptr;for(auto&t:threads)if(load32(&t.id)==id)return &t.side[server];for(auto&t:threads)if(compare32(&t.id,0,I32(id)))return &t.side[server];return nullptr;}
static bool fx_attachment(U32 renderable,int index,Matrix&out,U64&pair){
 if(!universalMuzzle||!load32(&live)||load32(&stopped)||!load32(&handOn)||!renderable)return false;
 FxFrame f{};U64 epoch=0,now=GetTickCount64();
 if(!fxCell.read(f)||renderable!=plus(f.source.id.vm,4)||!fresh_frame(f.source,now)||!trackingEpoch.read(epoch)||epoch!=f.source.epoch||!current_local(f.source)||!pointer_free(false,f.source.id.player)||f.attachments.checksum!=f.source.modelChecksum||!weapon_fx::get(f.attachments,index,out))return false;
 if(f.source.poseDomain){
  Frame target{};
  if(body_authority::fx_target(f.source,target)){
   Matrix delta=compose(weapon_body::matrix(target.barrel),weapon_body::inverse(weapon_body::matrix(f.source.barrel)));
   Matrix moved=compose(delta,out);if(!matrix_valid(moved))return false;out=moved;
   static volatile I32 rebased{};U32 n=inc32(&rebased);
   if(n<=6||n%240==0)note("FX_BODY_REBASED",target.poseDomain,target.sample,f.source.sample);
   pair=target.pair;return true;
  }
 }
 pair=f.source.pair;return true;
}
static bool __fastcall attachment_matrix_hook(void*self,void*,int index,Matrix*out){
 bool okay=originalAttachmentMatrix(self,index,out);Matrix value{};U64 pair=0;
 if(okay&&writeable(out,sizeof(*out))&&fx_attachment(addr(self),index,value,pair)){
  Vec3 before=position(*out);*out=value;U32 n=inc32(&fxMatrixCalls);
  if(n<5||n%240==0){Vec3 p=position(value);char b[420];int z=_snprintf(b,sizeof(b),"NOVA_FX_ATTACHMENT_MATRIX index=%d pair=%llu stock=(%.6f,%.6f,%.6f) native=(%.6f,%.6f,%.6f) scope=OWNED_CALLER_OUTPUT_ONLY\r\n",index,pair,double(before.x),double(before.y),double(before.z),double(p.x),double(p.y),double(p.z));line(b,z>419?419:z);}
 }return okay;
}
static bool __fastcall attachment_angles_hook(void*self,void*,int index,Vec3*out,Angles*angles){
 bool okay=originalAttachmentAngles(self,index,out,angles);Matrix value{};U64 pair=0;
 if(okay&&writeable(out,sizeof(*out))&&writeable(angles,sizeof(*angles))&&fx_attachment(addr(self),index,value,pair)){Pose p=matrix_pose(value);*out=p.p;*angles=p.a;inc32(&fxAnglesCalls);}
 return okay;
}
static bool input_packet(U32 self,int sequence,U32 cmd){U32 vt=0,ring=0;I32 n=0;return sequence>0&&rd(self,vt)&&(vt==plus(modules[0],0xbd8d60)||vt==plus(modules[0],0xc14cec))&&rd(plus(self,0xf0),ring)&&plus(ring,U32(sequence%150)*100)==cmd&&rd(plus(cmd,4),n)&&n==sequence;}
#ifdef NOVA_LAB_UNIVERSAL
using MouseFn=void(__thiscall*)(void*,int,Angles*,void*,float,float);
static MouseFn originalMouse{};
static void __fastcall mouse_hook(void*self,void*,int slot,Angles*view,void*cmd,float x,float y){
 U32 table=0;bool owned=rd(addr(self),table)&&(table==modules[0]+0xbd8d60||table==modules[0]+0xc14cec);
 if(!load32(&live)||load32(&stopped)||!owned||slot!=0||!native_game_control::mouse_camera_blocked()){originalMouse(self,slot,view,cmd,x,y);return;}
 // MouseMove has already drained its accumulators. Leave its copied angles
 // untouched, before the stock verified-command copy and checksum are made.
 if(writeable(cmd,72)){U32 zero=0;memcpy((U8*)cmd+68,&zero,4);}
 if(x!=0||y!=0){static U32 blocked=0;U32 n=++blocked;if(n<5||n%240==0){char text[240];int z=_snprintf(text,sizeof(text),"NOVA_XR_MOUSE_CAMERA_BLOCKED count=%u dx=%.5f dy=%.5f thread=%u\r\n",n,double(x),double(y),GetCurrentThreadId());line(text,z>239?239:z);}}
}
#endif
static void __fastcall create_hook(void*self,void*,int sequence,float time,bool active){
 Context*c=context(false);if(!load32(&live)||load32(&stopped)||!c||c->inCreate){originalCreate(self,sequence,time,active);return;}
 c->inputActive=active;c->inCreate=true;c->mimic=c->controlledGrenade=false;c->blocked=false;c->sequence=sequence;c->input=addr(self);c->sourceThread=GetCurrentThreadId();c->candidate={};c->candidate.madeMs=GetTickCount64();
 originalCreate(self,sequence,time,active);c->inCreate=false;if(!c->mimic&&!c->controlledGrenade)return;
 U8 actual[100]{},verified[104]{};U32 ring=0;
 if(!input_packet(addr(self),sequence,c->commandPtr)||!mem->read(c->commandPtr,actual,100)||!rd(plus(addr(self),0xf4),ring)||!mem->read(plus(ring,U32(sequence%150)*104),verified,104)){note("COMMAND_VERIFY_READ_FAILED",sequence);return;}
 Key a{},v{};U32 expected=0;memcpy(&expected,verified+100,4);U32 crc=client_crc(actual);
 if(!parse(actual,100,a)||!parse(verified,100,v)||!key_same(a,v)||crc!=expected||client_crc(verified)!=expected||(c->blocked&&(a.buttons&1))){note("NATIVE_VERIFIED_COPY_OR_CRC_MISMATCH",sequence,crc,expected);store32(&faulted,1);store32(&live,0);return;}
 if(c->controlledGrenade){static U32 verifiedGrenades=0;if(++verifiedGrenades<=3||verifiedGrenades%120==0)note("G3_GRENADE_COMMAND_VERIFIED",sequence,crc,verifiedGrenades);}
#ifdef NOVA_LAB_UNIVERSAL
 if(c->controlledGrenade){GrenadeCommand g{};g.key=a;g.source=c->grenadeInput;g.madeMs=c->candidate.madeMs;g.crc=crc;grenadeHistory.put(g);}
#endif
 if(!c->mimic)return;
 c->candidate.key=a;c->candidate.crc=crc;if(!no_pointer(a)||a.select)c->candidate.allowed=0;
 bool stored=history.put(c->candidate);U32 n=inc32(&commands);if(c->candidate.allowed)inc32(&commandsAllowed);else inc32(&commandsBlocked);
 if(n<=6||n%180==0||(a.buttons&1)){char b[650];int z=_snprintf(b,sizeof(b),"NOVA_H2_COMMAND number=%d tick=%d crc=%08x allowed=%u attack=%u sample=%llu pair=%llu sourceAgeMs=%llu peer=%08x stored=%u cameraAnglesUNCHANGED=(%.5f,%.5f,%.5f)\r\n",a.number,a.tick,crc,c->candidate.allowed,a.buttons&1,c->candidate.source.sample,c->candidate.source.pair,GetTickCount64()>=c->candidate.source.capturedMs?GetTickCount64()-c->candidate.source.capturedMs:~U64(0),c->candidate.expectedPeer.player,stored,double(a.view.p),double(a.view.y),double(a.view.r));line(b,z>649?649:z);}
}
static void mimic_done(void*self,int slot,void*cmd,void*first,U32 ret){
 if(!load32(&live)||load32(&stopped))return;Context*c=context(false);
 if(!c||!c->inputActive||!c->inCreate||c->input!=addr(self)||c->sourceThread!=GetCurrentThreadId()||ret!=plus(modules[0],0x305cc1)||slot!=0||cmd!=first||!input_packet(addr(self),c->sequence,addr(cmd))||!writeable(cmd,100))return;
#ifdef NOVA_LAB_UNIVERSAL
 U32 controlPlayer=0,controlVt=0;if(rd(plus(modules[0],0x52bf6d8),controlPlayer)&&rd(controlPlayer,controlVt)&&controlVt==plus(modules[0],0xbfcbcc)&&pointer_free(false,controlPlayer)){native_game_control::apply_command(cmd,controlPlayer);c->controlledGrenade=native_game_control::apply_grenade(cmd,controlPlayer,c->grenadeInput);if(c->controlledGrenade)c->commandPtr=addr(cmd);}
#endif
 if(!load32(&handOn))return;
 Identity id{};U32 pi=0,wi=0,sum=0;if(!local_identity(id,pi,wi,nullptr,&sum)||!pointer_free(false,id.player))return;
 U8 raw[100]{};if(!mem->read(addr(cmd),raw,100))return;Key k{};if(!parse(raw,100,k)||!no_pointer(k))return;
 Frame f{};Peer p{};U64 currentEpoch=0,now=GetTickCount64();bool useBody=body_authority::supported(sum)&&body_authority::owns_commands(id);
 bool fgot=useBody?body_authority::command_source(id,pi,wi,k,f):frameCell.read(f),pgot=peerCell.read(p);bool gen=trackingEpoch.read(currentEpoch);
 bool frameGood=fgot&&gen&&f.epoch==currentEpoch&&same_identity(f.id,id)&&f.playerIndex==pi&&f.weaponIndex==wi&&fresh_frame(f,now);
 bool permit=frameGood&&pgot&&peer_fresh(p,f,now)&&!k.select;
 if(permit&&(k.buttons&1)){TraceResult tr{};permit=trace_segment(false,id.player,f.bodyEye,f.barrel.p,tr)&&tr.valid&&!tr.startsolid&&!tr.allsolid&&tr.fraction>=.99999f;if(!permit)note("COMMAND_BARREL_OBSTRUCTED_ATTACK_SUPPRESSED",k.number);}
 // Missing/blocked admitted gun barrel input does not silently shoot at mouse center.
 // The stock engine still handles ammo/cooldown/attack edge and all other input.
 if(!permit){U32 b=k.buttons&~1u;memcpy((U8*)cmd+0x30,&b,4);c->blocked=true;}
 if(!frameGood){f={};f.id=id;f.playerIndex=pi;f.weaponIndex=wi;}
 c->candidate.source=f;c->candidate.expectedPeer=p;c->candidate.allowed=permit?1:0;c->commandPtr=addr(cmd);c->mimic=true;
}
static void __fastcall mimic_hook(void*self,void*,int slot,void*cmd,void*first){
 U32 ret=U32((uintptr_t)__builtin_return_address(0));originalMimic(self,slot,cmd,first);mimic_done(self,slot,cmd,first,ret);
}
static void __fastcall start_hook(void*self,void*,void*player,void*cmd){
 originalStart(self,player,cmd);if(!load32(&live)||load32(&stopped)||!player||!cmd)return;
 U32 current=0,global=0;U8 raw[0x44]{};if(!rd(plus(addr(player),0x1034),current)||current!=addr(cmd)||!rd(plus(modules[1],0xba18ac),global)||global!=addr(player)||!mem->read(addr(cmd),raw,sizeof(raw)))return;
 Key k{};Command c{};Peer p{};if(!parse(raw,sizeof(raw),k))return;
#ifdef NOVA_LAB_UNIVERSAL
 GrenadeCommand grenade{};if(grenadeHistory.get(k.number,grenade)&&key_same(grenade.key,k,true)&&GetTickCount64()>=grenade.madeMs&&GetTickCount64()-grenade.madeMs<=2000&&current_grenade(grenade.source)&&grenade_peer(addr(player),grenade.source,p))grenadeHistory.bind(k,p);
#endif
 if(!load32(&handOn)||!history.get(k.number,c)||!key_same(c.key,k,true)||GetTickCount64()<c.madeMs||GetTickCount64()-c.madeMs>2000||!current_local(c.source)||!server_peer(addr(player),c.source,p))return;
 if(history.bind(k.number,k,p)){peerCell.write(p);U32 n=inc32(&serverBound);if(n<5||n%180==0||(k.buttons&1))note("LOCAL_SERVER_COMMAND_BOUND",k.number,k.tick,p.weaponIndex);}
}
static int eye_family(U32 ret,bool server){static constexpr U32 a[2][5]={{0x6aa0a0,0x6aa5a0,0x6afb95,0x6b17b7,0x6bb267},{0x5a7a1a,0x5a801d,0x5b032d,0x5b379f,0x5bc5ff}};for(int i=0;i<5;i++)if(ret==plus(modules[server?1:0],a[server][i]))return i;return -1;}
static int origin_family(U32 ret,bool server){static constexpr U32 a[2][5]={{0x6aa189,0x6aa686,0x6afc88,0x6b18aa,0x6bb35a},{0x5a7afd,0x5a8128,0x5b0431,0x5b38a3,0x5bc703}};for(int i=0;i<5;i++)if(ret==plus(modules[server?1:0],a[server][i]))return i;return -1;}
static bool command_now(U32 player,bool server,Command&c,Key&k,Peer&p){
 U32 cmd=0,global=0;U8 bytes[0x44]{};U64 now=GetTickCount64(),epoch=0;
 if(!rd(plus(player,server?0x1034:0x3348),cmd)||!rd(plus(modules[server?1:0],server?0xba18ac:0x53323c0),global)||global!=player||!mem->read(cmd,bytes,sizeof(bytes))||!parse(bytes,sizeof(bytes),k)||!history.get(k.number,c)||!current_local(c.source)||!trackingEpoch.read(epoch)||epoch!=c.source.epoch)return false;
 if(server){if(!server_peer(player,c.source,p))return false;}else{if(player!=c.source.id.player||!peerCell.read(p)||now<p.atMs||now-p.atMs>350||!pointer_free(false,player))return false;}
 return command_eligible(c,k,now,server,p);
}
static const Angles*eye(U32 self,U32 ret,bool server,const Angles*original){
 if(!load32(&live)||load32(&stopped)||!load32(&handOn))return original;
 int family=eye_family(ret,server);if(family<0)return original;Context*x=context(server);if(!x)return original;x->angles=false;x->origin=false;
 Command c{};Key k{};Peer peer{};if(!command_now(self,server,c,k,peer)){U32 n=inc32(&shotReject);if(n<10||n%120==0)note("SHOT_NOT_BOUND_STOCK_PATH",server,n,ret-modules[server?1:0]);return original;}
 x->shot=c;x->self=self;x->family=U32(family);x->cmdNumber=U32(k.number);x->returned=c.source.barrel.a;x->angles=true;x->angleMs=GetTickCount64();inc32(server?&angleServer:&angleClient);
 return &x->returned; // Immediate original caller copies all three scalars, then applies stock AimPunch.
}
static const Angles*__fastcall client_eye(void*self,void*){U32 ret=U32((uintptr_t)__builtin_return_address(0));const Angles*p=originalEye[0](self);return eye(addr(self),ret,false,p);}
static const Angles*__fastcall server_eye(void*self,void*){U32 ret=U32((uintptr_t)__builtin_return_address(0));const Angles*p=originalEye[1](self);return eye(addr(self),ret,true,p);}
static void origin(U32 self,U32 ret,bool server,Vec3*out){
 if(!load32(&live)||load32(&stopped)||!load32(&handOn)||!writeable(out,12))return;int family=origin_family(ret,server);if(family<0)return;
 Context*x=context(server);Command current{};Key key{};Peer peer{};
 if(!x||!x->angles||x->self!=self||x->family!=U32(family)||GetTickCount64()<x->angleMs||GetTickCount64()-x->angleMs>100||!command_now(self,server,current,key,peer)||x->cmdNumber!=U32(key.number)||!key_same(x->shot.key,current.key,server)){inc32(&shotReject);return;}
 x->angles=false;Vec3 stock=*out;TraceResult tr{};bool traced=trace_segment(server,self,stock,x->shot.source.barrel.p,tr),clamped=false;
 Vec3 target=safe_origin(stock,x->shot.source.barrel.p,tr,clamped);if(!traced)clamped=true;
 if(!finite(target)){inc32(&shotReject);return;}*out=target;x->origin=true;x->finalOrigin=target;x->originMs=GetTickCount64();if(clamped)inc32(&clamps);inc32(server?&originServer:&originClient);
#ifdef NOVA_LAB_UNIVERSAL
 if(server)native_game_control::feedback(controller_input::Shot,key.number,x->shot.source.epoch,x->shot.source.weaponClass);
#endif
 char b[950];int n=_snprintf(b,sizeof(b),"NOVA_H2_NATIVE_SHOT side=%s cmd=%d tick=%d player=%u weaponNet=%u sample=%llu pair=%llu poseDomain=%u mount=%llu stock=(%.6f,%.6f,%.6f) nativeOrigin=(%.6f,%.6f,%.6f) sampledMuzzle=(%.6f,%.6f,%.6f) baseAngles=(%.6f,%.6f,%.6f) clamped=%u traceValid=%u fraction=%.7f recoilSpreadAmmoFX=STOCK cameraAngles=UNCHANGED\r\n",server?"SERVER":"CLIENT",key.number,key.tick,x->shot.source.playerIndex,x->shot.source.weaponIndex,x->shot.source.sample,x->shot.source.pair,x->shot.source.poseDomain,x->shot.source.mountRevision,double(stock.x),double(stock.y),double(stock.z),double(target.x),double(target.y),double(target.z),double(x->shot.source.barrel.p.x),double(x->shot.source.barrel.p.y),double(x->shot.source.barrel.p.z),double(x->returned.p),double(x->returned.y),double(x->returned.r),clamped,traced,double(tr.fraction));line(b,n>949?949:n);
}
static Vec3*__fastcall client_shoot(void*self,void*,Vec3*out){U32 ret=U32((uintptr_t)__builtin_return_address(0));Vec3*r=originalShoot[0](self,out);if(r==out)origin(addr(self),ret,false,out);return r;}
static Vec3*__fastcall server_shoot(void*self,void*,Vec3*out){U32 ret=U32((uintptr_t)__builtin_return_address(0));Vec3*r=originalShoot[1](self,out);if(r==out)origin(addr(self),ret,true,out);return r;}
static bool __fastcall bones_hook(void*self,void*,Matrix*out,int count,int mask,float time){
 bool selected=load32(&live)&&!load32(&stopped)&&GetCurrentThreadId()==renderThread&&driver&&driver->leased&&addr(self)==plus(driver->lease.vm,4)&&((count>=0&&count<=256)||(!out&&count==-1));
 if(selected){inc32(&boneCalls);if((mask&0x200)==0){mask|=0x200;inc32(&boneExpanded);}}
 // One naturally requested call; adapt only its admitted draw output.
 bool okay=originalBones(self,out,count,mask,time);
 if(selected&&(mask&0x400)){body_authority::output(out,count,okay);if(!body_authority::is_rendering())visual_recoil::output(out,count,okay);}
 return okay;
}
static void observe_trace(void*self,const Ray*ray,U32 mask,void*filter,Trace*out,U32 ret,U32 helperFrame){
 if(!load32(&live)||load32(&stopped)||self!=traces[1]||ret!=plus(modules[1],0x2589c9)||mask!=0x4600400b)return;
 U32 ancestor=0,skip=0,filterVt=0;if(!rd(plus(helperFrame,4),ancestor)||ancestor!=plus(modules[1],0x49b817)||!rd(addr(filter),filterVt)||filterVt!=plus(modules[1],0x8dce7c)||!rd(plus(addr(filter),4),skip))return;
 Context*x=context(true);if(!x||!x->origin||x->self!=skip||GetTickCount64()<x->originMs||GetTickCount64()-x->originMs>100)return;
 Command current{};Key key{};Peer peer{};if(!command_now(skip,true,current,key,peer)||x->cmdNumber!=U32(key.number)||!key_same(x->shot.key,current.key,true))return;
 Ray actual{};Trace result{};TraceResult tr{};if(!mem->read(addr(ray),&actual,sizeof(actual))||!mem->read(addr(out),&result,sizeof(result))||!decode_trace(result.b,sizeof(result),tr))return;
 Vec3 from{actual.start[0],actual.start[1],actual.start[2]},delta{actual.delta[0],actual.delta[1],actual.delta[2]};float len=length(delta);
 if(!actual.isRay||!actual.isSwept||length(sub(from,x->finalOrigin))>.02f||!finite_in_range(len,100000)||len<.01f){inc32(&observerRejected);return;}
 // A native shotgun invokes this initial trace for every pellet. Keep the
 // same command-bound origin witness until the next shot or the 100ms expiry.
 inc32(&serverTraces);Vec3 dir=scale(delta,1.f/len);char b[850];int n=_snprintf(b,sizeof(b),"NOVA_H2_SERVER_INITIAL_BULLET_TRACE cmd=%d tick=%d sample=%llu origin=(%.6f,%.6f,%.6f) directionAfterNativeSpread=(%.6f,%.6f,%.6f) fraction=%.7f end=(%.6f,%.6f,%.6f) entity=%08x allsolid=%u startsolid=%u nativeAncestor=49B817 helper=2589C9 notFinalDamageCertificate=1\r\n",x->shot.key.number,x->shot.key.tick,x->shot.source.sample,double(from.x),double(from.y),double(from.z),double(dir.x),double(dir.y),double(dir.z),double(tr.fraction),double(tr.end.x),double(tr.end.y),double(tr.end.z),tr.entity,tr.allsolid,tr.startsolid);line(b,n>849?849:n);
}static void __declspec(noinline) __fastcall trace_hook(void*self,void*,const Ray*ray,U32 mask,void*filter,Trace*out){
 U32 ret=U32((uintptr_t)__builtin_return_address(0));U32 helperFrame=0;
#if defined(__i386__) || defined(_M_IX86)
 // Frame pointer forced for this function. Own EBP[0] is the helper's EBP;
 // original helper explicitly saved its caller's return at helper EBP+4.
 helperFrame=*(U32*)__builtin_frame_address(0);
#endif
 originalTrace(self,ray,mask,filter,out);
 observe_trace(self,ray,mask,filter,out,ret,helperFrame);
}

struct Hook {U32 cell{},old{},replacement{};bool installed{};};static Hook hooks[32];static unsigned hookCount{};
static bool add_hook(U32 cell,U32 old,U32 replacement){if(hookCount==32)return false;for(unsigned i=0;i<hookCount;i++)if(hooks[i].cell==cell)return hooks[i].old==old&&hooks[i].replacement==replacement;U32 got=0;if(!rd(cell,got)||got!=old)return false;hooks[hookCount++]={cell,old,replacement,false};return true;}
static bool cell_exchange(Hook&h,bool install){U32 protect=0,ignored=0;void*p=(void*)(uintptr_t)h.cell;if(!VirtualProtect(p,4,4,&protect))return false;
 bool done=compare32((volatile I32*)p,I32(install?h.old:h.replacement),I32(install?h.replacement:h.old));
 if(done)h.installed=install;bool restored=VirtualProtect(p,4,protect,&ignored)!=0;U32 readback=0;bool verified=rd(h.cell,readback)&&readback==(install?h.replacement:h.old);return done&&restored&&verified;
}
static void shutdown(){store32(&stopped,1);store32(&live,0);store32(&handOn,0);for(unsigned i=hookCount;i>0;--i)if(hooks[i-1].installed&&!cell_exchange(hooks[i-1],false))note("HOOK_RESTORE_NOT_CERTIFIED",hooks[i-1].cell);note("STOP_CODE_REMAINS_RESIDENT",load32(&originClient),load32(&originServer),load32(&serverTraces));}
static bool install(){
 U32 c=modules[0],s=modules[1],e=modules[2];if(!exact())return false;
 originalCreate=(CreateFn)(uintptr_t)(c+0x305740);originalMimic=(MimicFn)(uintptr_t)(c+0x305d10);originalStart=(StartFn)(uintptr_t)(s+0x339210);originalBones=(BonesFn)(uintptr_t)(c+0x1d3140);originalEye[0]=(EyeFn)(uintptr_t)(c+0x3e7e30);originalEye[1]=(EyeFn)(uintptr_t)(s+0x214fb0);originalShoot[0]=(ShootFn)(uintptr_t)(c+0x42b900);originalShoot[1]=(ShootFn)(uintptr_t)(s+0x497a70);originalTrace=(TraceFn)(uintptr_t)(e+0x20dea0);
 originalAttachmentMatrix=(AttachmentMatrixFn)(uintptr_t)(c+0x1b8d70);originalAttachmentAngles=(AttachmentAnglesFn)(uintptr_t)(c+0x1b8e10);
 using Factory=void*(__cdecl*)(const char*,int*);auto f=(Factory)GetProcAddress((HMODULE)(uintptr_t)e,"CreateInterface");if(!f)return false;traces[0]=f("EngineTraceClient004",nullptr);traces[1]=f("EngineTraceServer004",nullptr);if(!traces[0]||!traces[1]||traces[0]==traces[1])return false;
 U32 tv[2]{};for(int i=0;i<2;i++){U32 fn=0;if(!rd(addr(traces[i]),tv[i])||(tv[i]!=e+0x4b5a60&&tv[i]!=e+0x4b5afc&&tv[i]!=e+0x4b5bd8)||!rd(plus(tv[i],20),fn)||fn!=e+0x20dea0)return false;}
 hookCount=0;const U32 it[]={0xbd8d60u,0xc14cecu};for(U32 t:it){if(!add_hook(c+t+12,addr((void*)originalCreate),addr((void*)create_hook))||!add_hook(c+t+212,addr((void*)originalMimic),addr((void*)mimic_hook)))return false;}
#ifdef NOVA_LAB_UNIVERSAL
 originalMouse=(MouseFn)(uintptr_t)(c+0x309570);for(U32 t:it)if(!add_hook(c+t+55*4,addr((void*)originalMouse),addr((void*)mouse_hook)))return false;
#endif
 const U32 st[]={0x8b26e0u,0x904b7cu};for(U32 t:st)if(!add_hook(s+t+16,addr((void*)originalStart),addr((void*)start_hook)))return false;
 if(!add_hook(c+0xbe0938+52,addr((void*)originalBones),addr((void*)bones_hook))||!add_hook(c+0xbfcbcc+0x2a8,addr((void*)originalEye[0]),addr((void*)client_eye))||!add_hook(c+0xbfcbcc+0x474,addr((void*)originalShoot[0]),addr((void*)client_shoot)))return false;
 const U32 pt[]={0x8fcc20u,0x919ec0u,0x91a964u};for(U32 t:pt)if(!add_hook(s+t+0x20c,addr((void*)originalEye[1]),addr((void*)server_eye))||!add_hook(s+t+0x488,addr((void*)originalShoot[1]),addr((void*)server_shoot)))return false;
 if(!add_hook(tv[1]+20,addr((void*)originalTrace),addr((void*)trace_hook)))return false;
 if(universalMuzzle&&(!add_hook(c+0xbe0938+34*4,addr((void*)originalAttachmentMatrix),addr((void*)attachment_matrix_hook))||!add_hook(c+0xbe0938+35*4,addr((void*)originalAttachmentAngles),addr((void*)attachment_angles_hook))))return false;
#ifdef NOVA_LAB_UNIVERSAL
 if(!grenade_exact())return false;
 const U32 replacements[]={addr((void*)grenade_emit_hook<0>),addr((void*)grenade_emit_hook<1>),addr((void*)grenade_emit_hook<2>),addr((void*)grenade_emit_hook<3>),addr((void*)grenade_emit_hook<4>),addr((void*)grenade_emit_hook<5>)};
 for(U32 i=0;i<6;i++){const auto&g=grenade_control::emitGuards[i];originalGrenadeEmit[i]=(GrenadeEmitFn)(uintptr_t)(s+g.rva);if(!add_hook(s+g.vtable+464*4,s+g.rva,replacements[i]))return false;}
#endif
 for(unsigned i=0;i<hookCount;i++)if(!cell_exchange(hooks[i],true)){note("HOOK_INSTALL_FAILED",i,hooks[i].cell);for(unsigned j=i+1;j>0;--j)if(hooks[j-1].installed&&!cell_exchange(hooks[j-1],false))note("HOOK_PARTIAL_RESTORE_NOT_CERTIFIED",hooks[j-1].cell);return false;}
 store32(&live,1);note("NATIVE_MUZZLE_BOUNDARIES_READY",hookCount,sizeof(muzzle_exact::guards)/sizeof(muzzle_exact::guards[0]));return true;
}
static void initialize(Mem&m,Driver<Mem>&d,U32 thread){
 if(!initialized){mem=&m;driver=&d;renderThread=thread;universalMuzzle=d.universal;initialized=true;log.open(L"nova_muzzle_h2.log");wchar_t v[8]{};muzzleOff=GetEnvironmentVariableW(L"NOVA_MUZZLE_OFF",v,8)>0&&v[0]==L'1';note("BUILD_BORE_MOUNT_NATIVE_MUZZLE",muzzleOff,4,universalMuzzle);}
 if(load32(&live)||load32(&stopped)||load32(&faulted)||muzzleOff||permanentFailure||GetTickCount64()<nextTry)return;nextTry=GetTickCount64()+2000;
 modules[0]=m.base;modules[1]=addr(GetModuleHandleW(L"server.dll"));modules[2]=addr(GetModuleHandleW(L"engine.dll"));modules[3]=addr(GetModuleHandleW(L"inputsystem.dll"));
 if(!modules[1]||!modules[2]||!modules[3]){note("AWAIT_LOCAL_MODULES",modules[1],modules[2],modules[3]);return;}
 if(!d.exact||!install()){permanentFailure=true;note("ADMISSION_FAILED_ORIENTATION_ONLY_NOT_GAMEPLAY_READY");}
}
static void begin(bool active,const Tracking&t,Vec3 body,Angles angles,Vec3 anchor,Quat anchorRot){
 visual_recoil::reset();
 store32(&handOn,active?1:0);trackingEpoch.write(t.epoch);building={};firstOkay=secondOkay=false;firstFxOkay=secondFxOkay=false;expandedAtBegin=load32(&boneCalls);
 if(!active||!load32(&live)||!driver)return;building.id=driver->lease;building.pair=driver->pair;building.sample=t.sampleId;building.epoch=t.epoch;building.capturedMs=GetTickCount64();building.bodyEye=body;
 if(universalMuzzle){
  Identity checked{};U32 pi=0,wi=0;
  if(!local_identity(checked,pi,wi,&building.weaponClass,&building.modelChecksum)||!same_identity(checked,building.id)){store32(&handOn,0);return;}
 }
 int pi=entity_index(building.id.player,false),wi=entity_index(building.id.weapon,false);if(pi>0&&pi<=64&&wi>0&&wi!=pi){building.playerIndex=U32(pi);building.weaponIndex=U32(wi);}else note("SOURCE_NETWORK_ID_NOT_ADMITTED",pi,wi);
 visual_recoil::begin(building.id.player);
 body_authority::begin(t,body,angles,anchor,anchorRot);
}
static int infinite_ammo_mode(){
 static U32 variable=0;
 if(!variable){
  U32 lib=addr(GetModuleHandleW(L"vstdlib.dll"));if(!lib)return -1;
  using Factory=void*(__cdecl*)(const char*,int*);auto factory=(Factory)GetProcAddress((HMODULE)(uintptr_t)lib,"CreateInterface");if(!factory)return -1;
  U32 object=addr(factory("VEngineCvar007",nullptr)),table=0,method=0;
  const U8 expected[]={0x55,0x8b,0xec,0x8b,0x01,0x56,0xff,0x75,0x08,0xff,0x50,0x38,0x8b,0xf0,0x85,0xf6};U8 code[16]{};
  if(!rd(object,table)||!rd(table+16*4,method)||method!=lib+0x4430||!mem->read(method,code,sizeof(code)))return -1;
  for(U32 i=0;i<16;i++)if(code[i]!=expected[i])return -1;
  using Find=void*(__thiscall*)(void*,const char*);U32 candidate=addr(((Find)(uintptr_t)method)((void*)(uintptr_t)object,"sv_infinite_ammo")),name=0;char text[17]{};
  if(!rd(candidate+12,name)||!mem->read(name,text,sizeof(text))||!streq(text,"sv_infinite_ammo"))return -1;variable=candidate;
 }
 U32 parent=0,integer=0,floatBits=0;if(!rd(variable+28,parent)||!rd(parent+48,integer)||!rd(parent+44,floatBits))return -1;
 I32 mode=I32(integer^parent);float value=0;floatBits^=parent;memcpy(&value,&floatBits,4);
 return mode>=0&&mode<=2&&value==float(mode)?mode:-1;
}
// The sole caller runs immediately after Scope::after_eye's full checkpoint.
// read_model_muzzle checks the live identity again after this native selector,
// before reading the palette, and once more before accepting the result.
// Avoid adding two complete identity/hierarchy walks to each eye here.
static U32 selected_attachment_after_checkpoint(){
 if(!driver||!driver->leased||GetCurrentThreadId()!=renderThread)return 0;
 U32 table=0,method=0,renderable=0,lookup=0,data=0,silenced=0,c=modules[0];
 if(!rd(driver->lease.weapon,table)||!rd(table+468*4,method)||(method!=c+0x6a3130&&method!=c+0x6ab4b0)||!rd(driver->lease.vm+4,renderable)||!rd(renderable+33*4,lookup)||lookup!=c+0x1b8d20)return 0;
 if(method==c+0x6a3130&&(!rd(table+461*4,data)||(data!=c+0x6aad50&&data!=c+0x6a0fb0)||!rd(table+463*4,silenced)||silenced!=c+0x6996f0))return 0;
 // The game's own viewmodel muzzle selector handles normal, suppressed and
 // alternating pistols. It only looks up metadata on this already loaded model.
 using Select=int(__thiscall*)(void*,void*);int index=((Select)(uintptr_t)method)((void*)(uintptr_t)driver->lease.weapon,(void*)(uintptr_t)driver->lease.vm);
 if(method==c+0x6ab4b0){
  int mode=infinite_ammo_mode();I32 clip=0;if(mode<0||!rd(driver->lease.weapon+0x3274,clip))return 0;
  // Elite's native visual selector runs after the shot consumes a round.
  // Select that same next-shot attachment without changing the real magazine.
  if(mode!=1){using Lookup=int(__thiscall*)(void*,const char*);index=((Lookup)(uintptr_t)lookup)((void*)(uintptr_t)(driver->lease.vm+4),(clip&1)?"1":"2");}
 }
 return index>0&&index<=64?U32(index):0;
}
#include "weapon_body_native.hpp"
static void after_eye(int eye,bool active){
 if(!active||!load32(&live)||!driver||!building.playerIndex)return;
 Pose cachedBase{};U32 cachedCounter=0;weapon_fx::Attachments cachedFx{};
 if(body_authority::intake(eye,cachedBase,cachedCounter,cachedFx)){
  U32 selected=body_authority::renderMount.attachment;
  building.poseDomain=1;building.mountRevision=body_authority::baseDisplay.mountRevision;building.anchorRevision=body_authority::baseDisplay.anchorRevision;
  building.poseMs=body_authority::baseDisplay.poseMs;building.predictedTime=body_authority::baseDisplay.predictedTime;
  if(eye==0){firstMuzzle=cachedBase;firstCounter=cachedCounter;firstOkay=true;building.muzzleAttachment=selected;firstFx=cachedFx;firstFxOkay=true;}
  else{secondOkay=firstOkay&&same_muzzle(firstMuzzle,cachedBase)&&building.muzzleAttachment==selected;if(secondOkay){building.barrel=cachedBase;building.boneCounter=cachedCounter;}secondFxOkay=firstFxOkay&&weapon_fx::same(firstFx,cachedFx);if(secondFxOkay)buildingFx=cachedFx;else inc32(&fxRejected);}
  return;
 }
 Pose p{};U32 counter=0;MuzzleReason reason=MuzzleReason::Attachment;U32 selected=universalMuzzle?selected_attachment_after_checkpoint():0;bool good=false;
 {AdmissionBatch batch;good=universalMuzzle?(selected&&read_model_muzzle(*driver,modelMuzzle,p,counter,reason,selected)):read_natural_muzzle(*driver,p,counter,reason);}
 if(good&&universalMuzzle){static U32 reportedSum=0;if(reportedSum!=modelMuzzle.checksum){reportedSum=modelMuzzle.checksum;note("MODEL_FLASH_BONE",modelMuzzle.checksum,modelMuzzle.bone,modelMuzzle.count);note("MODEL_NATIVE_CLASS",building.weaponClass,building.weaponIndex,building.playerIndex);}}
 if(!good){U32 n=inc32(&frameReject);if(reason!=lastMuzzleReason||n<8||n%180==0)note(muzzle::reason_name(reason),n,eye,load32(&boneCalls));lastMuzzleReason=reason;return;}
 if(eye==1)body_authority::observe();
 if(body_authority::rendering){
  if(!body_authority::drawSeen[eye]){note("BODY_DRAW_OUTPUT_NOT_OBSERVED",building.pair,eye);return;}
  p=body_authority::firing.barrel;
  building.poseDomain=1;building.mountRevision=body_authority::display.mountRevision;building.anchorRevision=body_authority::display.anchorRevision;
  building.poseMs=body_authority::display.poseMs;building.predictedTime=body_authority::display.predictedTime;
 }
 if(!body_authority::rendering&&visual_recoil::drawn[eye]){Matrix d{};if(visual_recoil::delta(p,d))p.p=position(compose(d,weapon_body::matrix(p)));}
 if(eye==0){firstMuzzle=p;firstCounter=counter;firstOkay=true;building.muzzleAttachment=selected;}else if(eye==1){secondOkay=firstOkay&&same_muzzle(firstMuzzle,p)&&building.muzzleAttachment==selected;if(secondOkay){building.barrel=p;building.boneCounter=counter;}else note("EYES_MUZZLE_DIVERGE",building.pair,firstCounter,counter);}
 if(universalMuzzle){weapon_fx::Attachments set{};U32 fxCounter=0;bool ready=false;{AdmissionBatch batch;ready=weapon_fx::read(*driver,set,fxCounter)&&fxCounter==counter;}
  if(ready&&body_authority::rendering){Matrix delta{};ready=body_authority::read_body_delta(delta);if(ready)for(U32 i=0;i<set.count;i++)set.matrices[i]=compose(delta,set.matrices[i]);}
  if(ready&&!body_authority::rendering&&visual_recoil::drawn[eye]){Matrix d{};ready=visual_recoil::delta(p,d);if(ready)for(U32 i=0;i<set.count;i++)set.matrices[i]=compose(d,set.matrices[i]);}
  if(eye==0){firstFxOkay=ready;if(ready)firstFx=set;}
  else if(eye==1){secondFxOkay=ready&&firstFxOkay&&weapon_fx::same(firstFx,set);if(secondFxOkay)buildingFx=set;else inc32(&fxRejected);}
 }
}
static void finish(bool renderOkay,bool restored){
 if(!initialized)return;
 if(load32(&live)&&building.playerIndex&&firstOkay&&secondOkay&&renderOkay&&restored&&load32(&boneCalls)>expandedAtBegin){
  building.valid=1;if(frameCell.write(building)){U32 n=inc32(&frameOkay);if(n<=4||n%120==0){Vec3 f=forward(building.barrel);char b[650];int z=_snprintf(b,sizeof(b),"NOVA_H2_NATURAL_MUZZLE frame=%llu sample=%llu epoch=%llu nativeCounter=%u origin=(%.6f,%.6f,%.6f) forward=(%.6f,%.6f,%.6f) bothEyes=1 restored=1 forcedSetupBonesCalls=0 nativeBoneCacheWrites=0 naturalDrawOutputMayBeAdapted=1\r\n",building.pair,building.sample,building.epoch,building.boneCounter,double(building.barrel.p.x),double(building.barrel.p.y),double(building.barrel.p.z),double(f.x),double(f.y),double(f.z));line(b,z>649?649:z);}}
 }
 if(universalMuzzle){if(building.valid&&firstFxOkay&&secondFxOkay&&renderOkay&&restored&&load32(&boneCalls)>expandedAtBegin){FxFrame f{};f.source=building;f.attachments=buildingFx;if(fxCell.write(f))inc32(&fxPublished);}else{FxFrame empty{};fxCell.write(empty);}}
 building={};firstOkay=secondOkay=false;body_authority::end();
 U64 now=GetTickCount64();if(now-lastStatus>=2000){lastStatus=now;note("COUNTS_GEOMETRY",load32(&boneCalls),load32(&frameOkay),load32(&frameReject));note("COUNTS_COMMAND",load32(&commands),load32(&commandsAllowed),load32(&serverBound));note("COUNTS_SHOT",load32(&originClient),load32(&originServer),load32(&serverTraces));note("COUNTS_GUARDS",load32(&commandsBlocked),load32(&shotReject),load32(&clamps));note("FX_COUNTS",load32(&fxPublished),load32(&fxMatrixCalls),load32(&fxRejected));body_authority::report();}
}
}
