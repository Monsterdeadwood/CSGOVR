#pragma once
#include "handcore.hpp"
#include "bore_profile.hpp"
#include "weapon_identity.hpp"
// H2: data/decision core only. Render, command generation and server simulation
// have distinct owners. A command receives a copied muzzle, never a live pointer.
namespace muzzle {
using namespace nova;using namespace handcore;
struct Matrix {float a[12];};
inline Rotation rotation(const Matrix&m){return {{m.a[0],m.a[1],m.a[2],m.a[4],m.a[5],m.a[6],m.a[8],m.a[9],m.a[10]}};}
inline Vec3 position(const Matrix&m){return {m.a[3],m.a[7],m.a[11]};}
inline Matrix compose(const Matrix&a,const Matrix&b){
 Matrix c{};for(int i=0;i<3;i++){for(int j=0;j<3;j++)for(int k=0;k<3;k++)c.a[i*4+j]+=a.a[i*4+k]*b.a[k*4+j];c.a[i*4+3]=a.a[i*4+3];for(int k=0;k<3;k++)c.a[i*4+3]+=a.a[i*4+k]*b.a[k*4+3];}return c;
}
inline bool matrix_valid(const Matrix&m){return rigid(rotation(m))&&finite(position(m));}
inline Pose matrix_pose(const Matrix&m){auto r=rotation(m);return {position(m),basis_angles({column(r,0),scale(column(r,1),-1),column(r,2)})};}
inline Vec3 forward(const Pose&p){return source_basis(p.a).forward;}
inline bool same_muzzle(const Pose&a,const Pose&b){return length(sub(a.p,b.p))<=.02f&&dot(forward(a),forward(b))>=.99999f;}
// Bounded copies under a real acquire/release lock. No C++ data-racy seqlock,
// no unbounded spin on the game's render/simulation thread.
template<class T> struct Cell {
 volatile I32 busy{};T value{};
 bool read(T&out){if(!compare32(&busy,0,1))return false;memcpy(&out,&value,sizeof(T));store32(&busy,0);return true;}
 bool write(const T&in){if(!compare32(&busy,0,1))return false;memcpy(&value,&in,sizeof(T));store32(&busy,0);return true;}
};
struct Frame {
 Identity id{};U32 playerIndex{},weaponIndex{},boneCounter{},valid{},weaponClass{},modelChecksum{},muzzleAttachment{};
 U64 pair{},sample{},epoch{},capturedMs{};Pose barrel{};Vec3 bodyEye{};
 U32 poseDomain{};U64 mountRevision{},anchorRevision{},poseMs{};I64 predictedTime{};
};
struct Peer {
 U32 player{},ownerHandle{},weapon{},weaponHandle{},playerIndex{},weaponIndex{},clientOwner{},clientWeapon{};U64 atMs{};U32 weaponClass{};
};
inline bool peer_same(const Peer&a,const Peer&b){return a.player&&a.player==b.player&&a.ownerHandle==b.ownerHandle&&a.weapon==b.weapon&&a.weaponHandle==b.weaponHandle&&a.playerIndex==b.playerIndex&&a.weaponIndex==b.weaponIndex&&a.clientOwner==b.clientOwner&&a.clientWeapon==b.clientWeapon&&a.weaponClass==b.weaponClass;}
// Named shared prefix, NOT one assumed 100-byte client/server ABI. Server has
// its own extra random-seed field after +0x40; tail bytes are never cross-joined.
struct Key {
 I32 number{},tick{};Angles view{};Vec3 pointerAim{},move{};U32 buttons{},select{},subtype{},seed{};U8 impulse{};
};
inline bool parse(const U8*bytes,size_t n,Key&k){
 if(!bytes||n<0x44)return false;
 memcpy(&k.number,bytes+4,4);memcpy(&k.tick,bytes+8,4);memcpy(&k.view,bytes+12,12);memcpy(&k.pointerAim,bytes+24,12);memcpy(&k.move,bytes+36,12);
 memcpy(&k.buttons,bytes+48,4);k.impulse=bytes[52];memcpy(&k.select,bytes+56,4);memcpy(&k.subtype,bytes+60,4);memcpy(&k.seed,bytes+64,4);
 return k.number>0&&k.tick>=0&&finite(k.view)&&finite(k.pointerAim,10)&&finite(k.move,10000);
}
inline float angle_delta(float a,float b){float d=a-b;while(d>180)d-=360;while(d<-180)d+=360;return absf(d);}
inline bool key_same(const Key&a,const Key&b,bool remote=false){
 // Typed semantic agreement. The bounded tolerance is explicit, not a claim
 // that two entire client/server CUserCmd structures are byte-identical.
 float ar=remote?.05f:0.f,mr=remote?.001f:0.f;
 return a.number==b.number&&a.tick==b.tick&&a.buttons==b.buttons&&a.select==b.select&&a.subtype==b.subtype&&a.seed==b.seed&&a.impulse==b.impulse&&
 angle_delta(a.view.p,b.view.p)<=ar&&angle_delta(a.view.y,b.view.y)<=ar&&angle_delta(a.view.r,b.view.r)<=ar&&
 length(sub(a.pointerAim,b.pointerAim))<=1e-8f&&absf(a.move.x-b.move.x)<=mr&&absf(a.move.y-b.move.y)<=mr&&absf(a.move.z-b.move.z)<=mr;
}
inline bool no_pointer(const Key&k){return dot(k.pointerAim,k.pointerAim)<1e-10f;}
inline bool fresh_frame(const Frame&f,U64 now){return f.valid&&f.pair&&f.sample&&f.epoch&&now>=f.capturedMs&&now-f.capturedMs<=180&&finite(f.barrel.p)&&finite(f.barrel.a)&&length(sub(f.barrel.p,f.bodyEye))<120.f;}
inline bool peer_fresh(const Peer&p,const Frame&f,U64 now){return p.player&&p.playerIndex==f.playerIndex&&p.weaponIndex==f.weaponIndex&&p.clientOwner==f.id.ownerHandle&&p.clientWeapon==f.id.weaponHandle&&p.weaponClass==f.weaponClass&&now>=p.atMs&&now-p.atMs<=350;}
struct Command {
 Key key{};Frame source{};Peer expectedPeer{},boundPeer{};U64 madeMs{};U32 crc{},allowed{},bound{};
};
struct History {
 Cell<Command> entries[256];
 bool put(const Command&c){if(c.key.number<=0)return false;return entries[U32(c.key.number)%256].write(c);}
 bool get(I32 number,Command&c){return number>0&&entries[U32(number)%256].read(c)&&c.key.number==number;}
 bool bind(I32 number,const Key&actual,const Peer&p){
  if(number<=0)return false;auto&x=entries[U32(number)%256];if(!compare32(&x.busy,0,1))return false;
  bool ok=x.value.key.number==number&&key_same(x.value.key,actual,true);
  if(ok){x.value.boundPeer=p;x.value.bound=1;}store32(&x.busy,0);return ok;
 }
};
inline bool command_eligible(const Command&c,const Key&k,U64 now,bool server,const Peer&current){
 // Invoked only at the exact native firearm callers. The native gun may emit
 // queued burst rounds after IN_ATTACK is released; it owns the fire decision.
 if(!c.allowed||!c.source.valid||!c.source.sample||!c.source.epoch||!no_pointer(k)||k.select||!key_same(c.key,k,server)||now<c.madeMs||now-c.madeMs>2000)return false;
 if(server)return c.bound&&peer_same(c.boundPeer,current)&&peer_same(c.expectedPeer,current);
 return peer_same(c.expectedPeer,current);
}
enum class MuzzleReason:U32 {Ready,NoLease,Header,Attachment,Palette,Mask,Busy,Counter,Matrix,Axis,Distance,Changed,Parent};
inline const char*reason_name(MuzzleReason r){switch(r){
#define M(x) case MuzzleReason::x:return #x;
 M(Ready) M(NoLease) M(Header) M(Attachment) M(Palette) M(Mask) M(Busy) M(Counter) M(Matrix) M(Axis) M(Distance) M(Changed) M(Parent)
#undef M
 }return "Unknown";}
struct ModelMuzzle {
 U32 header{},checksum{},count{},bone{},parent{},attachment{};float parentDistance{};Matrix local{};bool valid{},rootBone{};
 template<class Memory> bool admit(Driver<Memory>&d,U32 selected=0){
  U32 size=0,sum=0,bones=0,ao=0,ac=0,bo=0;const U32 h=d.lease.header;
  if(!d.rd(plus(h,8),sum)||!d.rd(plus(h,76),size)||!d.rd(plus(h,0x9c),bones)||size<0x100||size>32u*1024u*1024u||bones<1||bones>256)return false;
  if(valid&&header==h&&checksum==sum&&count==bones&&attachment==selected)return true;
  valid=false;
  if(!d.rd(plus(h,0xf0),ac)||!d.rd(plus(h,0xf4),ao)||ac<1||ac>64||ao>size||ac*92>size-ao||!d.rd(plus(h,0xa0),bo)||bo>size||bones*216>size-bo)return false;
  U32 found=0;Matrix candidate{};
  for(U32 i=0;i<ac;i++){
   if(selected&&selected!=i+1)continue;
   U32 a=plus(h,ao+i*92),bi=0,flags=0,no=0;
   if(!d.rd(plus(a,8),bi)||bi>=bones||!d.rd(plus(a,4),flags))return false;
   U32 record=plus(h,bo+bi*216);if(!d.rd(record,no))return false;
   U64 rel=U64(bo)+bi*216+no;if(rel>=size)return false;
   char name[96]{};U32 len=U32(size-rel);if(len>95)len=95;
   if(!d.m->read(plus(h,U32(rel)),name,len))return false;
   U32 n=0;while(n<len&&name[n])++n;if(n==len)return false;
   // Original three models use attachment name "1", but a unique flash bone.
   bool flash=n>=6&&(streq(name+n-6,".flash")||streq(name+n-6,"_flash"));
   if(!selected&&!flash)continue;
   U32 boneFlags=0;
   if(++found!=1||(flags&0x10000)||!d.rd(plus(record,160),boneFlags)||!(boneFlags&0x200)||!d.m->read(plus(a,12),&candidate,sizeof(candidate))||!matrix_valid(candidate))return false;
   I32 nativeParent=-1;Vec3 localPosition{};
   if(!d.rd(plus(record,4),nativeParent)||nativeParent< -1||(nativeParent>=0&&(U32(nativeParent)>=bones||U32(nativeParent)==bi))||!d.rd(plus(record,32),localPosition)||!finite(localPosition))return false;
   rootBone=nativeParent==-1;if(rootBone&&!selected)return false;
   parent=rootBone?0:U32(nativeParent);parentDistance=length(localPosition);if(!rootBone&&(!(parentDistance<100.f)||(!selected&&parentDistance<=.01f)))return false;
   bone=bi;
  }
  if(found!=1)return false;
  header=h;checksum=sum;count=bones;attachment=selected;local=candidate;valid=true;return true;
 }
};
template<class Memory> bool read_model_muzzle(Driver<Memory>&d,ModelMuzzle&rig,Pose&out,U32&counter,MuzzleReason&reason,U32 selected=0){
 auto fail=[&](MuzzleReason r){reason=r;return false;};
 if(!d.leased||!d.still_ours())return fail(MuzzleReason::NoLease);
 if(!rig.admit(d,selected))return fail(MuzzleReason::Attachment);
 const U32 vm=d.lease.vm;U32 pointer=0,accessor=0,count=0,mask=0,owner=1,depth=1,after=0;
 if(!d.rd(plus(vm,0x2914),pointer)||!pointer||!d.rd(plus(vm,0x26a8),accessor)||accessor!=pointer||!d.rd(plus(vm,0x2920),count)||count!=rig.count)return fail(MuzzleReason::Palette);
 if(!d.rd(plus(vm,0x26ac),mask)||!(mask&0x200))return fail(MuzzleReason::Mask);
 if(!d.rd(plus(vm,0x26b8),owner)||owner||!d.rd(plus(vm,0x26bc),depth)||depth)return fail(MuzzleReason::Busy);
 if(!d.rd(plus(vm,0x2690),counter)||!counter)return fail(MuzzleReason::Counter);
 Matrix bone{};if(!d.m->read(plus(pointer,rig.bone*48),&bone,sizeof(bone))||!matrix_valid(bone))return fail(MuzzleReason::Matrix);
 // Native attachment bones may translate relative to their parent in the
 // current animation (P90 does). Bind-pose distance is not an animation rule.
 // Keep that old diagnostic only for the legacy named-flash fallback.
 if(!rig.attachment&&!rig.rootBone){Matrix parent{};if(!d.m->read(plus(pointer,rig.parent*48),&parent,sizeof(parent))||!matrix_valid(parent))return fail(MuzzleReason::Parent);
  float localDistance=length(sub(position(bone),position(parent))),tolerance=rig.parentDistance*.25f;if(tolerance<2.f)tolerance=2.f;
  if(absf(localDistance-rig.parentDistance)>tolerance)return fail(MuzzleReason::Parent);
 }
 Matrix full=compose(bone,rig.local);if(!matrix_valid(full))return fail(MuzzleReason::Matrix);out=matrix_pose(full);
 // A bind pose maps mesh space, not this sequence's animated root space.
 // Direction comes from the current naturally computed attachment, not bind data.
 if(length(sub(out.p,d.gripWorld))>80.f)return fail(MuzzleReason::Distance);
 if(!d.rd(plus(vm,0x2690),after)||counter!=after||!d.still_ours())return fail(MuzzleReason::Changed);
 reason=MuzzleReason::Ready;return true;
}
template<class Memory> bool read_natural_muzzle(Driver<Memory>&d,Pose&out,U32&counter,MuzzleReason&reason){
 auto fail=[&](MuzzleReason r){reason=r;return false;};
 if(!d.leased||!d.still_ours())return fail(MuzzleReason::NoLease);
 const U32 vm=d.lease.vm,h=d.lease.header;U32 size=0,count=0,off=0,bone=0,flags=0,boff=0;Matrix local{},world{};
 if(!d.rd(plus(h,76),size)||!d.rd(plus(h,0xf0),count)||!d.rd(plus(h,0xf4),off)||size>32u*1024u*1024u||count<1||count>64||off>size||92>size-off)return fail(MuzzleReason::Header);
 U32 a=plus(h,off);if(!d.rd(plus(a,4),flags)||!d.rd(plus(a,8),bone)||bone!=7||(flags&0x10000)||!d.m->read(plus(a,12),&local,sizeof(local)))return fail(MuzzleReason::Attachment);
 for(unsigned i=0;i<12;i++)if(absf(local.a[i]-BORE_ATTACH_LOCAL[i])>1e-5f)return fail(MuzzleReason::Attachment);
 if(!matrix_valid(local)||!d.rd(plus(h,0xa0),boff)||boff>size||47*216>size-boff)return fail(MuzzleReason::Attachment);
 // Exact flash-bone bind identity prevents assigning the first attachment of
 // another P2000 header to the known profile merely because its count is 47.
 float bind[12]{};I32 parent=0;U32 boneFlags=0;
 if(!d.rd(plus(h,boff+7*216+4),parent)||parent!=3||!d.rd(plus(h,boff+7*216+160),boneFlags)||!(boneFlags&0x200)||!d.m->read(plus(h,boff+7*216+96),bind,48))return fail(MuzzleReason::Attachment);
 for(unsigned i=0;i<12;i++)if(bind[i]!=BORE_FLASH_P2B[i])return fail(MuzzleReason::Attachment);
 U32 ptr=0,num=0,owner=0,depth=0,mask=0,accessor=0,c2=0;
 if(!d.rd(plus(vm,0x2914),ptr)||!d.rd(plus(vm,0x2920),num)||num!=47||!d.rd(plus(vm,0x26a8),accessor)||accessor!=ptr)return fail(MuzzleReason::Palette);
 if(!d.rd(plus(vm,0x26ac),mask)||!(mask&0x200))return fail(MuzzleReason::Mask);
 if(!d.rd(plus(vm,0x26b8),owner)||owner||!d.rd(plus(vm,0x26bc),depth)||depth)return fail(MuzzleReason::Busy);
 if(!d.rd(plus(vm,0x2690),counter)||!counter)return fail(MuzzleReason::Counter);
 if(!d.m->read(plus(ptr,7*48),&world,sizeof(world))||!matrix_valid(world))return fail(MuzzleReason::Matrix);
 Matrix full=compose(world,local);if(!matrix_valid(full))return fail(MuzzleReason::Matrix);out=matrix_pose(full);
 Rotation b{};memcpy(b.a,BORE_ROOT_ROTATION,sizeof(b.a));Vec3 expected=apply(pose_rotation(d.desired.a),column(b,0));
 if(dot(expected,forward(out))<.65f)return fail(MuzzleReason::Axis);
 if(length(sub(out.p,d.gripWorld))>35.f)return fail(MuzzleReason::Distance);
 if(!d.rd(plus(vm,0x2690),c2)||c2!=counter||!d.still_ours())return fail(MuzzleReason::Changed);
 reason=MuzzleReason::Ready;return true;
}
struct TraceResult {float fraction{};Vec3 end{};U32 entity{};bool allsolid{},startsolid{},valid{};};
inline bool decode_trace(const U8*p,size_t n,TraceResult&t){
 if(!p||n<84)return false;memcpy(&t.fraction,p+44,4);memcpy(&t.end,p+12,12);memcpy(&t.entity,p+76,4);t.allsolid=p[54]!=0;t.startsolid=p[55]!=0;
 t.valid=finite_in_range(t.fraction,1.00001f)&&t.fraction>=0&&t.fraction<=1&&finite(t.end);return t.valid;
}
// Do not spawn a ray beyond a wall between stock body-eye and the sampled muzzle.
// A solid/invalid trace retains the stock safe origin; a hit clamps before it.
inline Vec3 safe_origin(Vec3 stock,Vec3 desired,const TraceResult&t,bool&clamped){
 clamped=true;Vec3 delta=sub(desired,stock);float n=length(delta);
 if(!finite(stock)||!finite(desired)||!t.valid||t.allsolid||t.startsolid||!finite_in_range(n,120)||n<.0001f)return stock;
 if(t.fraction>=.99999f){clamped=false;return desired;}
 float a=t.fraction-.5f/n;if(a<0)a=0;return add(stock,scale(delta,a));
}
}
