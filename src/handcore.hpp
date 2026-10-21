#pragma once
#include "math.hpp"
#include "handcore_profile.hpp"
#include "bore_profile.hpp"
#include "handcore_exact.hpp"
// H1: a bounded native viewmodel-root transaction. No frame transport, hook,
// bone-palette write, game command, aim redirect or synthetic hand renderer.
namespace handcore {
using namespace nova;
struct Rotation {float a[9];};
struct Pose {Vec3 p; Angles a;};
static inline bool eq(Vec3 a,Vec3 b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
static inline bool eq(Angles a,Angles b){return a.p==b.p&&a.y==b.y&&a.r==b.r;}
static inline Vec3 column(const Rotation&r,unsigned c){return {r.a[c],r.a[3+c],r.a[6+c]};}
static inline Rotation columns(Vec3 x,Vec3 y,Vec3 z){return {{x.x,y.x,z.x,x.y,y.y,z.y,x.z,y.z,z.z}};}
static inline Vec3 apply(const Rotation&r,Vec3 v){return add(add(scale(column(r,0),v.x),scale(column(r,1),v.y)),scale(column(r,2),v.z));}
static inline Rotation transpose(const Rotation&r){return {{r.a[0],r.a[3],r.a[6],r.a[1],r.a[4],r.a[7],r.a[2],r.a[5],r.a[8]}};}
static inline Rotation multiply(const Rotation&a,const Rotation&b){return columns(apply(a,column(b,0)),apply(a,column(b,1)),apply(a,column(b,2)));}
static inline Vec3 cross(Vec3 a,Vec3 b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
static inline bool finite(Vec3 a,float lim=10000000.f){return finite_in_range(a.x,lim)&&finite_in_range(a.y,lim)&&finite_in_range(a.z,lim);}
static inline bool finite(Angles a){return finite_in_range(a.p,10000)&&finite_in_range(a.y,10000)&&finite_in_range(a.r,10000);}
static inline bool rigid(const Rotation&r){
 for(unsigned i=0;i<9;i++)if(!finite_in_range(r.a[i],2.f))return false;
 Vec3 x=column(r,0),y=column(r,1),z=column(r,2);
 return absf(dot(x,x)-1)<.001f&&absf(dot(y,y)-1)<.001f&&absf(dot(z,z)-1)<.001f&&absf(dot(x,y))<.001f&&absf(dot(x,z))<.001f&&absf(dot(y,z))<.001f&&dot(cross(x,y),z)>.999f;
}
static inline Rotation pose_rotation(Angles a){Basis b=source_basis(a);return columns(b.forward,scale(b.right,-1),b.up);}
static inline bool same_pose(const Pose&a,const Pose&b){return a.p.x==b.p.x&&a.p.y==b.p.y&&a.p.z==b.p.z&&a.a.p==b.a.p&&a.a.y==b.a.y&&a.a.r==b.a.r;}
static inline bool valid_quat(Quat q){float n=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;return n>.8f&&n<1.2f;}
static inline Vec3 palm_position(const Pose&p){Rotation r=pose_rotation(p.a);return add(p.p,apply(r,{PALM_POSITION[0],PALM_POSITION[1],PALM_POSITION[2]}));}
static inline bool palm_frame(const Vec3 p[5],Rotation&r,Vec3&pos){
 Vec3 across=sub(p[1],p[4]);Vec3 mid=scale(add(add(p[1],p[2]),add(p[3],p[4])),.25f);
 Vec3 lng=sub(mid,p[0]);float n=length(across);if(!(n>.1f&&n<20))return false;
 Vec3 z=scale(across,-1/n),x=cross(across,lng);n=length(x);if(!(n>.01f&&n<1000))return false;
 x=scale(x,1/n);Vec3 y=cross(z,x);r=columns(x,y,z);pos=scale(add(p[0],mid),.5f);return rigid(r)&&finite(pos);
}
static inline bool solve(const Tracking&t,U64 now,Vec3 bodyOrigin,Angles bodyAngles,Vec3 anchor,Quat anchorRot,Pose&out,Vec3&gripWorld){
 // Exactly the frame's two-eye tracking sample; do not independently read "latest".
 if(!t.valid||!t.focused||!t.sampleId||!t.epoch||t.predictedTime<=0||now<t.sampledMs||now-t.sampledMs>250)return false;
 const HandPose&h=t.hand[1];if(!h.valid||!h.active||!finite(h.pos,30)||!valid_quat(h.rot)||!valid_quat(anchorRot)||!finite(anchor,30)||!finite(bodyOrigin)||!finite(bodyAngles))return false;
 Vec3 center=scale(add(t.eye[0].pos,t.eye[1].pos),.5f);
 if(!valid_eye(t.eye[0])||!valid_eye(t.eye[1])||length(sub(h.pos,center))>2.0f)return false;
 Quat q=qmul(conjugate(normalized(anchorRot)),normalized(h.rot));Basis parent=source_basis(bodyAngles);
 // The host publishes an explicit weapon target: grip point + aim frame (or
 // the named grip-only control). The raw XR observations remain separate.
 Rotation target=columns(basis_apply(parent,to_source(rotate(q,{1,0,0}))),basis_apply(parent,to_source(rotate(q,{0,1,0}))),basis_apply(parent,to_source(rotate(q,{0,0,1}))));
 // H2 mounts the actual model bore, not a guessed anatomical forward.
 // Model muzzle +X -> weapon target -Z; muzzle +Z -> target +Y. Rotate ABOUT the same
 // palm cradle so the wrist does not jump to the weapon model origin.
 Rotation bore{};memcpy(bore.a,BORE_ROOT_ROTATION,sizeof(bore.a));
 Rotation targetBore=columns(scale(column(target,2),-1),scale(column(target,0),-1),column(target,1));
 Rotation model=multiply(targetBore,transpose(bore));if(!rigid(model))return false;
 gripWorld=add(bodyOrigin,scale(basis_apply(parent,to_source(rotate(conjugate(normalized(anchorRot)),sub(h.pos,anchor)))),UNITS_PER_METRE));
 out.p=sub(gripWorld,apply(model,{PALM_POSITION[0],PALM_POSITION[1],PALM_POSITION[2]}));
 out.a=basis_angles({column(model,0),scale(column(model,1),-1),column(model,2)});
 // No first-live-pose grab offset and no game/head mutation.
 return finite(out.p)&&finite(out.a)&&length(sub(out.p,bodyOrigin))<180.f&&length(sub(palm_position(out),gripWorld))<.005f;
}
enum class Reason:U32 { Ready=0, Disabled, Tracking, ExactBytes, LocalPlayer, DeadOrObserver, ViewmodelHandle, ClassIdentity, Parent, WeaponOwner, Header, ModelNotP2000, BoneIdentity, ModelScale, ReadPose, PoseRange, ForeignIdentity, ForeignPose, SetterRejected, RestoreRejected, PaletteUnavailable, DrawFlags, NativeBusyOrChildren, GateRead, NativeBoneBusy, ChildClass, ChildParent, ChildNested, ChildCycleOrLimit, ChildIdentity, ChildChanged };
static inline const char*reason_name(Reason r){switch(r){
#define H1_REASON(x) case Reason::x:return #x;
 H1_REASON(Ready) H1_REASON(Disabled) H1_REASON(Tracking) H1_REASON(ExactBytes) H1_REASON(LocalPlayer) H1_REASON(DeadOrObserver) H1_REASON(ViewmodelHandle) H1_REASON(ClassIdentity) H1_REASON(Parent) H1_REASON(WeaponOwner) H1_REASON(Header) H1_REASON(ModelNotP2000) H1_REASON(BoneIdentity) H1_REASON(ModelScale) H1_REASON(ReadPose) H1_REASON(PoseRange) H1_REASON(ForeignIdentity) H1_REASON(ForeignPose) H1_REASON(SetterRejected) H1_REASON(RestoreRejected) H1_REASON(PaletteUnavailable) H1_REASON(DrawFlags) H1_REASON(NativeBusyOrChildren) H1_REASON(GateRead) H1_REASON(NativeBoneBusy) H1_REASON(ChildClass) H1_REASON(ChildParent) H1_REASON(ChildNested) H1_REASON(ChildCycleOrLimit) H1_REASON(ChildIdentity) H1_REASON(ChildChanged)
#undef H1_REASON
 }return "Unknown";}
struct Identity {U32 player{},ownerHandle{},vm{},vmHandle{},weapon{},weaponHandle{},wrapper{},header{};U16 model{};};
static inline bool same_identity(const Identity&a,const Identity&b){return a.player==b.player&&a.ownerHandle==b.ownerHandle&&a.vm==b.vm&&a.vmHandle==b.vmHandle&&a.weapon==b.weapon&&a.weaponHandle==b.weaponHandle&&a.wrapper==b.wrapper&&a.header==b.header&&a.model==b.model;}
static inline U32 plus(U32 a,U32 b){return U64(a)+b<=0xffffffffull?a+b:0;}
// A native first-person model may have a separate attachment-model child.
// This bounded contract admits only direct LEAF children of one exact native class.
// No arbitrary descendant tree, reparent, child transform, mutex write or extra draw.
struct Attachment {U32 entity{},handle{},parent{},next{},vtable{};U16 model{};};
struct Hierarchy {U32 count{};Attachment nodes[4]{};};
static inline bool same_hierarchy(const Hierarchy&a,const Hierarchy&b){
 if(a.count!=b.count)return false;
 for(U32 k=0;k<a.count;k++){const auto&x=a.nodes[k];const auto&y=b.nodes[k];
  if(x.entity!=y.entity||x.handle!=y.handle||x.parent!=y.parent||x.next!=y.next||x.vtable!=y.vtable||x.model!=y.model)return false;
 }return true;
}
struct GateObservation {
 // bits indicate which fields were actually read; zero is not "unknown".
 U32 readMask{},vm{},child{},boneOwner{},boneDepth{},childEntity{},childHandle{},childOrdinal{},childVtable{},childRenderable{},childParent{},childNext{},childChildren{},childBoneOwner{},childBoneDepth{},checksum{},boneCount{};
 char modelName[64]{};
};
struct Counters {U64 attempts{},applied{},eyeChecks{},restored{},rejected{},conflicts{},paletteReads{},paletteMissing{},childPairs{};};
template<class M> auto begin_batch(M&m,int)->decltype(m.begin_read_batch(),void()){m.begin_read_batch();}
template<class M> void begin_batch(M&,long){}
template<class M> auto end_batch(M&m,int)->decltype(m.end_read_batch(),void()){m.end_read_batch();}
template<class M> void end_batch(M&,long){}
template<class M> struct ReadBatch {M&memory;explicit ReadBatch(M&m):memory(m){begin_batch(memory,0);}~ReadBatch(){end_batch(memory,0);}};
// One anatomical contract, discovered by bone names in each current model.
// Indices and the mount come from the model and its natural animated palette.
struct PalmRig {
 U32 header{},checksum{},count{},indices[5]{};bool valid{},mounted{},pending{};
 Rotation rootFromPalm{},nextRotation{};Vec3 center{},nextCenter{};
 template<class Memory> bool admit(Memory&m,const Identity&id,U32 sum,U32 bones,U32 size,bool full){
  if(valid&&header==id.header&&checksum==sum&&count==bones)return true;
  valid=mounted=pending=false;if(!full||bones<5||bones>256)return false;
  U32 offset=0;if(!m.read(plus(id.header,0xa0),&offset,4)||offset>size||bones*216>size-offset)return false;
  const char*ends[5]={"Bip01_R_Hand","Bip01_R_Finger1","Bip01_R_Finger2","Bip01_R_Finger3","Bip01_R_Finger4"};
  U32 found=0;
  for(U32 i=0;i<bones;i++){
   U32 record=plus(id.header,offset+i*216),relative=0;if(!m.read(record,&relative,4))return false;
   U64 position=U64(offset)+i*216+relative;if(position>=size)continue;
   char name[96]{};U32 bytes=U32(size-position);if(bytes>95)bytes=95;
   if(!m.read(plus(id.header,U32(position)),name,bytes))return false;
   U32 length=0;while(length<bytes&&name[length])++length;if(length==bytes)continue;
   for(U32 k=0;k<5;k++){U32 suffix=U32(strnlen_(ends[k],64));
    if(length>=suffix&&streq(name+length-suffix,ends[k])){if(found&(1u<<k))return false;indices[k]=i;found|=1u<<k;}
   }
  }
  if(found!=31)return false;
  for(U32 k=1;k<5;k++){I32 parent=-1;if(!m.read(plus(id.header,offset+indices[k]*216+4),&parent,4)||parent!=I32(indices[0]))return false;}
  header=id.header;checksum=sum;count=bones;valid=true;return true;
 }
 template<class Memory> bool sample(Memory&m,const Identity&id,const Pose&root){
  if(!valid||header!=id.header)return false;
  U32 pointer=0,accessor=0,bones=0,counter=0,after=0,mask=0,owner=1,depth=1;Vec3 points[5]{};
  auto rd=[&](U32 off,U32&v){return m.read(plus(id.vm,off),&v,4);};
  if(!rd(0x2914,pointer)||!pointer||!rd(0x26a8,accessor)||accessor!=pointer||!rd(0x2920,bones)||bones!=count||!rd(0x2690,counter)||!rd(0x26ac,mask)||!(mask&0x400)||!rd(0x26b8,owner)||!rd(0x26bc,depth)||owner||depth)return false;
  for(U32 k=0;k<5;k++){float matrix[12]{};if(!m.read(plus(pointer,indices[k]*48),matrix,48))return false;points[k]={matrix[3],matrix[7],matrix[11]};if(!finite(points[k]))return false;}
  if(!rd(0x2690,after)||after!=counter)return false;
  Rotation worldPalm{};Vec3 worldCenter{};if(!palm_frame(points,worldPalm,worldCenter))return false;
  Rotation inverse=transpose(pose_rotation(root.a));nextRotation=multiply(inverse,worldPalm);nextCenter=apply(inverse,sub(worldCenter,root.p));
  pending=rigid(nextRotation)&&finite(nextCenter)&&length(nextCenter)<100;return pending;
 }
 void commit(bool okay){if(pending&&okay){rootFromPalm=nextRotation;center=nextCenter;mounted=true;}pending=false;}
};
static inline bool solve_palm(const Tracking&t,U64 now,Vec3 origin,Angles body,Vec3 anchor,Quat anchorRot,const PalmRig&rig,Pose&out,Vec3&gripWorld){
 if(!rig.mounted||!t.valid||!t.focused||!t.sampleId||!t.epoch||t.predictedTime<=0||now<t.sampledMs||now-t.sampledMs>250)return false;
 const HandPose&h=t.hand[1];if(!h.valid||!h.active||!finite(h.pos,30)||!valid_quat(h.rot)||!valid_quat(anchorRot)||!finite(anchor,30)||!finite(origin)||!finite(body))return false;
 if(!valid_eye(t.eye[0])||!valid_eye(t.eye[1])||length(sub(h.pos,scale(add(t.eye[0].pos,t.eye[1].pos),.5f)))>2)return false;
 Quat inverse=conjugate(normalized(anchorRot)),q=qmul(inverse,normalized(h.rot));Basis parent=source_basis(body);
 Rotation grip=columns(basis_apply(parent,to_source(rotate(q,{1,0,0}))),basis_apply(parent,to_source(rotate(q,{0,1,0}))),basis_apply(parent,to_source(rotate(q,{0,0,1}))));
 // OpenXR right grip +X is INTO the palm; -Z runs pinky->thumb.
 // Our named-bone frame has +X out of the palm and +Z thumb->pinky.
 Rotation palm=columns(scale(column(grip,0),-1),scale(column(grip,1),-1),column(grip,2));
 Rotation model=multiply(palm,transpose(rig.rootFromPalm));if(!rigid(model))return false;
 gripWorld=add(origin,scale(basis_apply(parent,to_source(rotate(inverse,sub(h.pos,anchor)))),UNITS_PER_METRE));
 out.p=sub(gripWorld,apply(model,rig.center));out.a=basis_angles({column(model,0),scale(column(model,1),-1),column(model,2)});
 return finite(out.p)&&finite(out.a)&&length(sub(out.p,origin))<180&&length(sub(add(out.p,apply(pose_rotation(out.a),rig.center)),gripWorld))<.005f;
}
// Memory is a membrane: production does checked native reads and SetLocal* calls;
// tests execute THIS intake/transaction code against an adversarial x86 memory map.
template<class Memory> struct Driver {
 Memory*m{};U32 base{};bool enabled{true},retired{},leased{},exact{};Reason reason{Reason::Disabled};Counters counts{};
 Identity lease{};Hierarchy leasedChildren{},currentChildren{};GateObservation observation{};Pose before{},desired{};Vec3 gripWorld{};U64 pair{},sample{},epoch{};
 bool universal{};PalmRig rig;
 template<class T> bool rd(U32 a,T&v){return a&&m->read(a,&v,sizeof(v));}
 bool resolve(U32 h,U32&entity){
  if(h==0xffffffffu)return false;U32 e=plus(base,0x4e051ccu+((h&0xffffu)*16));U32 serial=0;
  return rd(e,entity)&&rd(plus(e,4),serial)&&serial==(h>>16)&&entity!=0;
 }
 bool read_pose(U32 e,Pose&p){return rd(plus(e,0xac),p.p)&&rd(plus(e,0xd0),p.a)&&finite(p.p)&&finite(p.a);}
 bool exact_bytes(){
  for(const Guard&g:guards){U8 actual[104]{},expected[104]{};if(!m->read(plus(base,g.rva),actual,g.n))return false;memcpy(expected,g.b,g.n);
   for(unsigned k=0;k<g.nrel;k++){U32 v=0;memcpy(&v,expected+g.rel[k],4);v=U32(base+(v-0x10000000u));memcpy(expected+g.rel[k],&v,4);}
   for(unsigned k=0;k<g.n;k++)if(actual[k]!=expected[k])return false;
  }return true;
 }
 void initialize(Memory&mem,U32 moduleBase,bool on){m=&mem;base=moduleBase;enabled=on;exact=exact_bytes();reason=exact?Reason::Disabled:Reason::ExactBytes;}
 bool identity(Identity&i,bool full){
  ReadBatch<Memory> batch(*m);
  observation={};
  U8 life=255;I32 observer=-1,health=0;U32 vt=0,calc=0,parent=0,owner=0,w=0;float scaleValue=0;
  if(!rd(plus(base,0x52bf6d8),i.player)||!i.player){reason=Reason::LocalPlayer;return false;}
  if(!rd(plus(i.player,0x25f),life)||!rd(plus(i.player,0x3388),observer)||!rd(plus(i.player,0x100),health)||life!=0||observer!=0||health<=0){reason=Reason::DeadOrObserver;return false;}
  if(!rd(plus(i.player,0x3308),i.vmHandle)||!resolve(i.vmHandle,i.vm)){reason=Reason::ViewmodelHandle;return false;}
  if(!rd(i.vm,vt)||vt!=plus(base,0xbe09f4)||!rd(plus(vt,250*4),calc)||calc!=plus(base,0x3420e0)){reason=Reason::ClassIdentity;return false;}
  if(!rd(plus(i.vm,0x308),parent)||parent!=0xffffffffu){reason=Reason::Parent;return false;}
  if(!rd(plus(i.vm,0x29dc),i.ownerHandle)||!resolve(i.ownerHandle,owner)||owner!=i.player||!rd(plus(i.vm,0x29d8),i.weaponHandle)||!resolve(i.weaponHandle,i.weapon)||!rd(plus(i.player,0x2f08),w)||w!=i.weaponHandle){reason=Reason::WeaponOwner;return false;}
  if(!rd(plus(i.vm,0x274c),scaleValue)||absf(scaleValue-1.f)>.00001f||!(scaleValue>.99f)){reason=Reason::ModelScale;return false;}
  if(!rd(plus(i.vm,0x258),i.model)||!rd(plus(i.vm,0x2950),i.wrapper)||!rd(i.wrapper,i.header)||!i.header){reason=Reason::Header;return false;}
  U32 magic=0,checksum=0,count=0,version=0,size=0,virt=0,boff=0;
  if(!rd(i.header,magic)||!rd(plus(i.header,4),version)||!rd(plus(i.header,8),checksum)||!rd(plus(i.header,76),size)||!rd(plus(i.header,0x9c),count)||!rd(plus(i.wrapper,4),virt)||magic!=0x54534449u||version<44||version>49||size<0x100||size>32u*1024u*1024u||virt){reason=Reason::Header;return false;}
  observation.vm=i.vm;observation.checksum=checksum;observation.boneCount=count;observation.readMask|=1;
  if(m->read(plus(i.header,12),observation.modelName,sizeof(observation.modelName))){observation.modelName[63]=0;observation.readMask|=2;}
  if(universal){if(!rig.admit(*m,i,checksum,count,size,full)){reason=Reason::BoneIdentity;return false;}return hierarchy(i,currentChildren);}
  if(checksum!=MODEL_CHECKSUM||count!=MODEL_BONES){reason=Reason::ModelNotP2000;return false;}
  if(!full)return hierarchy(i,currentChildren);
  char name[64]{};if(!m->read(plus(i.header,12),name,sizeof(name))){reason=Reason::Header;return false;}
  bool term=false;for(unsigned n=0;n<64;n++){if(name[n]=='\0'){term=true;break;}if(name[n]=='\\')name[n]='/';if(name[n]>='A'&&name[n]<='Z')name[n]=char(name[n]+32);}
  if(!term||!streq(name,"weapons/v_pist_hkp2000.mdl")){reason=Reason::ModelNotP2000;return false;}
  if(!rd(plus(i.header,0xa0),boff)||boff>size||MODEL_BONES*216>size-boff){reason=Reason::BoneIdentity;return false;}
  for(unsigned k=0;k<5;k++){
   U32 rec=plus(i.header,boff+PALM_BONES[k]*216);U32 noff=0;I32 par=0;float p2b[12]{};
   if(!rd(rec,noff)||!rd(plus(rec,4),par)||par!=(k==0?1:2)||!m->read(plus(rec,96),p2b,sizeof(p2b))){reason=Reason::BoneIdentity;return false;}
   for(unsigned n=0;n<12;n++)if(p2b[n]!=PALM_POSE_TO_BONE[k][n]){reason=Reason::BoneIdentity;return false;}
   const char*expected=PALM_BONE_NAMES[k];unsigned n=0;while(n<64&&expected[n])++n;
   U64 rel=U64(boff)+U64(PALM_BONES[k])*216+noff;char actual[65]{};
   if(n==64||rel>=size||n+1>size-rel||!m->read(plus(i.header,U32(rel)),actual,n+1)){reason=Reason::BoneIdentity;return false;}
   for(unsigned j=0;j<=n;j++)if(actual[j]!=expected[j]){reason=Reason::BoneIdentity;return false;}
  }
  return hierarchy(i,currentChildren);
 }
 bool hierarchy(const Identity&i,Hierarchy&out){
  out={};GateObservation&g=observation;g.vm=i.vm;
  U32 h=0,owner=0,depth=0;
  if(!rd(plus(i.vm,0x30c),h)){reason=Reason::GateRead;return false;}g.child=h;g.readMask|=4;
  if(!rd(plus(i.vm,0x26b8),owner)||!rd(plus(i.vm,0x26bc),depth)){reason=Reason::GateRead;return false;}
  g.boneOwner=owner;g.boneDepth=depth;g.readMask|=8;
  if(owner!=0||depth!=0){reason=Reason::NativeBoneBusy;return false;}
  while(h!=0xffffffffu){
   // Diagnostic data for node k must never be combined with node k-1.
   g.readMask&=15;g.childHandle=h;g.childOrdinal=out.count;
   g.childEntity=g.childVtable=g.childRenderable=g.childParent=g.childNext=g.childChildren=g.childBoneOwner=g.childBoneDepth=0;
   if(out.count>=4){reason=Reason::ChildCycleOrLimit;return false;}
   Attachment n{};n.handle=h;U32 self=0,children=0,renderable=0;float size=0;
   if(!resolve(h,n.entity)||n.entity==i.vm||n.entity==i.player||n.entity==i.weapon){reason=Reason::ChildIdentity;return false;}
   for(U32 k=0;k<out.count;k++)if(out.nodes[k].entity==n.entity||out.nodes[k].handle==h){reason=Reason::ChildCycleOrLimit;return false;}
   g.childEntity=n.entity;g.readMask|=16;
   if(!rd(n.entity,n.vtable)||!rd(plus(n.entity,4),renderable)){reason=Reason::GateRead;return false;}
   g.childVtable=n.vtable;g.childRenderable=renderable;g.readMask|=32;
   // RTTI and original constructor/vtable bytes identify this exact native class.
   if(n.vtable!=plus(base,0xb9f004)||renderable!=plus(base,0xb9f3e8)){reason=Reason::ChildClass;return false;}
   if(!rd(plus(n.entity,0x28c),self)||self!=h){reason=Reason::ChildIdentity;return false;}
   if(!rd(plus(n.entity,0x308),n.parent)||!rd(plus(n.entity,0x310),n.next)||!rd(plus(n.entity,0x30c),children)){reason=Reason::GateRead;return false;}
   g.childParent=n.parent;g.childNext=n.next;g.childChildren=children;g.readMask|=64;
   if(n.parent!=i.vmHandle){reason=Reason::ChildParent;return false;}
   if(children!=0xffffffffu){reason=Reason::ChildNested;return false;}
   if(!rd(plus(n.entity,0x26b8),owner)||!rd(plus(n.entity,0x26bc),depth)){reason=Reason::GateRead;return false;}
   g.childBoneOwner=owner;g.childBoneDepth=depth;g.readMask|=128;
   if(owner!=0||depth!=0){reason=Reason::NativeBoneBusy;return false;}
   if(!rd(plus(n.entity,0x258),n.model)||!rd(plus(n.entity,0x274c),size)||absf(size-1.f)>.00001f||!(size>.99f)){reason=Reason::ModelScale;return false;}
   out.nodes[out.count++]=n;h=n.next;
  }
  return true;
 }
 bool still_ours(){Identity cur{};Pose p{};if(!identity(cur,false)||!same_identity(cur,lease)){reason=Reason::ForeignIdentity;return false;}
  if(!same_hierarchy(currentChildren,leasedChildren)){reason=Reason::ChildChanged;return false;}
  if(!read_pose(lease.vm,p)||!same_pose(p,desired)){reason=Reason::ForeignPose;return false;}return true;
 }
 bool begin(const Tracking&t,U64 now,U64 pairId,Vec3 origin,Angles body,Vec3 anchor,Quat anchorRot,int drawFlags){
  observation={};++counts.attempts;if(leased){retired=true;reason=Reason::ForeignIdentity;return false;}
  if(retired||!enabled){reason=Reason::Disabled;++counts.rejected;return false;}
  if(!exact){reason=Reason::ExactBytes;++counts.rejected;return false;}
  if(!(drawFlags&1)){reason=Reason::DrawFlags;++counts.rejected;return false;}
  Identity id{};
  if(universal){
   if(!identity(id,true)){++counts.rejected;return false;}
   if(!rig.mounted){reason=Reason::PaletteUnavailable;++counts.rejected;return false;}
   if(!solve_palm(t,now,origin,body,anchor,anchorRot,rig,desired,gripWorld)){reason=Reason::Tracking;++counts.rejected;return false;}
  }else{
   if(!solve(t,now,origin,body,anchor,anchorRot,desired,gripWorld)){reason=Reason::Tracking;++counts.rejected;return false;}
   if(!identity(id,true)){++counts.rejected;return false;}
  }
  if(!read_pose(id.vm,before)){reason=Reason::ReadPose;++counts.rejected;return false;}
  // Reject wildly separated viewmodel entities instead of repairing a foreign camera.
  if(length(sub(before.p,origin))>256.f){reason=Reason::PoseRange;++counts.rejected;return false;}
  lease=id;leasedChildren=currentChildren;pair=pairId;sample=t.sampleId;epoch=t.epoch;
  Identity again{};Pose againPose{};if(!identity(again,false)||!same_identity(again,id)||!same_hierarchy(currentChildren,leasedChildren)||!read_pose(id.vm,againPose)||!same_pose(before,againPose)){reason=Reason::ForeignIdentity;++counts.rejected;return false;}
  m->set_position(id.vm,desired.p);m->set_angles(id.vm,desired.a);leased=true;
  if(!still_ours()){
   // A simulated/real partial setter outcome is not automatically a foreign write.
   // Roll back only the precise old/new combinations we can account for.
   Identity current{};Pose p{};
   if(identity(current,false)&&same_identity(current,id)&&same_hierarchy(currentChildren,leasedChildren)&&read_pose(id.vm,p)&&
      (eq(p.p,before.p)||eq(p.p,desired.p))&&(eq(p.a,before.a)||eq(p.a,desired.a))){
    m->set_position(id.vm,before.p);m->set_angles(id.vm,before.a);
    Identity recovered{};Pose finalPose{};
    if(identity(recovered,false)&&same_identity(recovered,id)&&same_hierarchy(currentChildren,leasedChildren)&&read_pose(id.vm,finalPose)&&same_pose(finalPose,before))++counts.restored;
   }
   leased=false;reason=Reason::SetterRejected;retired=true;++counts.conflicts;return false;
  }
  ++counts.applied;if(leasedChildren.count)++counts.childPairs;reason=Reason::Ready;return true;
 }
 bool checkpoint(){if(!leased)return false;++counts.eyeChecks;if(!still_ours()){retired=true;++counts.conflicts;return false;}return true;}
 bool observe_palm(){
  if(!universal||retired||!enabled)return false;
  Identity id{},after{};Pose root{},again{};
  if(leased&&!still_ours())return false;
  if(!identity(id,true)||!read_pose(id.vm,root)||!rig.sample(*m,id,root))return false;
  if(!identity(after,false)||!same_identity(id,after)||!read_pose(id.vm,again)||!same_pose(root,again)){rig.pending=false;return false;}
  return true;
 }
 bool end(){
  if(!leased)return true;
  if(!still_ours()){leased=false;retired=true;++counts.conflicts;return false;}
  m->set_position(lease.vm,before.p);m->set_angles(lease.vm,before.a);leased=false;
  Identity id{};Pose current{};
  if(!identity(id,false)||!same_identity(id,lease)||!same_hierarchy(currentChildren,leasedChildren)||!read_pose(lease.vm,current)||!same_pose(current,before)){reason=Reason::RestoreRejected;retired=true;++counts.conflicts;return false;}
  ++counts.restored;reason=Reason::Ready;return true;
 }
 // Optional passive reading of five naturally computed bone origins. Its domain
 // is logged, not promoted to rendered-palm proof. No cache lock or write.
 bool read_natural_palm(Vec3&pos,U32&counter){
  if(!leased||!still_ours())return false;
  U32 ptr=0,num=0,owner=0,depth=0,readMask=0,counter2=0,accessor=0;Vec3 points[5]{};Rotation r{};
  if(!rd(plus(lease.vm,0x2914),ptr)||!rd(plus(lease.vm,0x2920),num)||!rd(plus(lease.vm,0x2690),counter)||!rd(plus(lease.vm,0x26b8),owner)||owner!=0||!rd(plus(lease.vm,0x26bc),depth)||depth!=0||num!=(universal?rig.count:MODEL_BONES)||!rd(plus(lease.vm,0x26ac),readMask))return false;
  if(!rd(plus(lease.vm,0x26a8),accessor)||accessor!=ptr)return false;
  // Require a nonzero computed/read mask, not the unrelated muzzle-attachment bit.
  if(!(readMask&0x400u))return false;
  for(unsigned k=0;k<5;k++){float matrix[12]{};if(!m->read(plus(ptr,(universal?rig.indices[k]:PALM_BONES[k])*48),matrix,48))return false;points[k]={matrix[3],matrix[7],matrix[11]};if(!finite(points[k]))return false;}
  if(!rd(plus(lease.vm,0x2690),counter2)||counter2!=counter||!still_ours())return false;
  return palm_frame(points,r,pos);
 }
};
}
