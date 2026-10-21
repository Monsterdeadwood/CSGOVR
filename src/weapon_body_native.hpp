#pragma once
// Included inside muzzle_native after the authenticated native selector.
namespace body_authority {
using weapon_body::Mount;using weapon_body::Sample;using weapon_body::Reference;
static const Shared*tracking{};
static Cell<Mount> mountCell;static Cell<Reference> referenceCell;
static Mount learning{},renderMount{};static Matrix candidate{},scratch[256]{};
static U64 seenMs{},quietUntil{},candidateMs{},mountSerial{},anchorSerial{};static U32 stableSamples{};static I32 previousClip{-1};
static Sample display{},baseDisplay{};static bool rendering{},drawSeen[2]{};static int eye{-1};
static Frame firing{};
static weapon_fx::Attachments outputFx[2]{};static U32 outputCounter[2]{};static bool outputFxReady[2]{};
static U64 fastIntakes{},slowIntakes{};
static bool is_rendering(){return rendering;}
static Vec3 drawnPalm{};static Rotation drawnPalmAxes{};static bool drawnPalmValid{};
static U64 adapted{},aliasRejects{},badOutput{},learned{};static volatile I32 commandCopies{},commandMisses{};
static volatile I32 retired{};
static bool supported(U32 sum){return sum==1095087377u||sum==1273104850u;}
static bool owns_commands(const Identity&id){
 // Preserve G5 during passive learning. Once a mount is published, the
 // command bridge owns the transition and never substitutes a stale image.
 if(load32(&retired))return true;Mount m{};if(!mountCell.read(m))return true;
 return m.valid&&same_identity(m.id,id);
}
static bool same_anchor(const Reference&a,const Reference&b){return a.epoch==b.epoch&&same_identity(a.id,b.id)&&eq(a.anchor,b.anchor)&&a.anchorRot.x==b.anchorRot.x&&a.anchorRot.y==b.anchorRot.y&&a.anchorRot.z==b.anchorRot.z&&a.anchorRot.w==b.anchorRot.w;}
static bool valid_source(const Frame&f){
 if(!f.poseDomain)return true;if(load32(&retired))return false;
 Mount m{};Reference r{};return mountCell.read(m)&&referenceCell.read(r)&&m.valid&&same_identity(m.id,f.id)&&same_identity(r.id,f.id)&&m.revision==f.mountRevision&&r.anchorRevision==f.anchorRevision&&r.epoch==f.epoch;
}
static void bind_tracking(const Shared*p){tracking=p;}
static void begin(const Tracking&t,Vec3 origin,Angles angles,Vec3 anchor,Quat anchorRot){
 nova_perf::Scope timing(nova_perf::BodyBegin);
 rendering=false;display={};baseDisplay={};outputFxReady[0]=outputFxReady[1]=false;drawSeen[0]=drawSeen[1]=false;drawnPalmValid=false;eye=-1;
 if(load32(&retired)||!driver||!driver->leased||!supported(building.modelChecksum))return;
 Vec3 playerOrigin{};if(!rd(driver->lease.player+0x138,playerOrigin)||!finite(playerOrigin))return;
 Reference next{};next.id=driver->lease;next.epoch=t.epoch;next.pair=driver->pair;next.displaySample=t.sampleId;next.atMs=GetTickCount64();next.eyeOffset=sub(origin,playerOrigin);next.anchor=anchor;next.anchorRot=anchorRot;
 Reference old{};if(!referenceCell.read(old)||!same_anchor(old,next))++anchorSerial;
 next.anchorRevision=anchorSerial;if(!referenceCell.write(next))return;
 Mount m{};if(!mountCell.read(m)||!m.valid||!same_identity(m.id,driver->lease)||m.checksum!=building.modelChecksum)return;
 if(!weapon_body::solve(m,t,GetTickCount64(),origin,angles,anchor,anchorRot,building.pair,next.anchorRevision,display))return;
 display.frame.playerIndex=building.playerIndex;display.frame.weaponIndex=building.weaponIndex;display.frame.weaponClass=building.weaponClass;
 display.frame.poseDomain=1;display.frame.mountRevision=m.revision;display.frame.anchorRevision=next.anchorRevision;display.frame.poseMs=t.sampledMs;display.frame.predictedTime=t.predictedTime;
 weapon_body::State state{};
 Vec3 palm=position(compose(display.body,m.bodyFromPalm));
 if(!weapon_body::state(display,palm,visual_recoil::punch,visual_recoil::scale,state))return;
 baseDisplay=state.base;display=state.display;firing=state.firing;
 if(visual_recoil::active()){
  char b[750];int n=_snprintf(b,sizeof(b),"NOVA_R1_WEAPON_STATE pair=%llu sample=%llu checksum=%u scale=%.6f punch=(%.6f,%.6f,%.6f) baseAngles=(%.6f,%.6f,%.6f) displayAngles=(%.6f,%.6f,%.6f) fireAngles=(%.6f,%.6f,%.6f) displayMuzzle=(%.6f,%.6f,%.6f) fireMuzzle=(%.6f,%.6f,%.6f) cameraPunch=0\r\n",firing.pair,firing.sample,m.checksum,double(visual_recoil::scale),double(visual_recoil::punch.p),double(visual_recoil::punch.y),double(visual_recoil::punch.r),double(baseDisplay.frame.barrel.a.p),double(baseDisplay.frame.barrel.a.y),double(baseDisplay.frame.barrel.a.r),double(display.frame.barrel.a.p),double(display.frame.barrel.a.y),double(display.frame.barrel.a.r),double(firing.barrel.a.p),double(firing.barrel.a.y),double(firing.barrel.a.r),double(display.frame.barrel.p.x),double(display.frame.barrel.p.y),double(display.frame.barrel.p.z),double(firing.barrel.p.x),double(firing.barrel.p.y),double(firing.barrel.p.z));line(b,n>749?749:n);
 }
 renderMount=m;rendering=true;
}
static void observe(){
 if(!driver||!driver->leased||!supported(building.modelChecksum)||!driver->rig.valid)return;
 U32 selected=selected_attachment_after_checkpoint();if(!selected||!modelMuzzle.admit(*driver,selected)||modelMuzzle.rootBone)return;
 Mount current{};if(mountCell.read(current)&&current.valid&&same_identity(current.id,driver->lease)&&current.checksum==building.modelChecksum)return;
 U64 now=GetTickCount64();I32 clip=-1;if(!rd(driver->lease.weapon+0x3274,clip))return;
 if(!same_identity(learning.id,driver->lease)||learning.checksum!=building.modelChecksum){
  learning={};learning.id=driver->lease;learning.checksum=building.modelChecksum;seenMs=now;quietUntil=now+1600;stableSamples=0;candidateMs=0;previousClip=clip;
  mountCell.write(Mount{});
 }
 if(clip!=previousClip){previousClip=clip;quietUntil=now+1600;stableSamples=0;candidateMs=0;}
 U32 palette=0,alias=0,count=0,counter=0,after=0,mask=0,owner=1,depth=1;
 if(!rd(driver->lease.vm+0x2914,palette)||!palette||!rd(driver->lease.vm+0x26a8,alias)||palette!=alias||!rd(driver->lease.vm+0x2920,count)||count!=driver->rig.count||!rd(driver->lease.vm+0x2690,counter)||!rd(driver->lease.vm+0x26ac,mask)||(mask&0x600)!=0x600||!rd(driver->lease.vm+0x26b8,owner)||owner||!rd(driver->lease.vm+0x26bc,depth)||depth)return;
 Matrix body{},flash{};Vec3 points[5]{};U32 bodyBone=modelMuzzle.parent;
 if(bodyBone>=count||!mem->read(palette+bodyBone*48,&body,48)||!mem->read(palette+modelMuzzle.bone*48,&flash,48)||!matrix_valid(body)||!matrix_valid(flash))return;
 for(U32 k=0;k<5;k++){Matrix p{};if(!mem->read(palette+driver->rig.indices[k]*48,&p,48))return;points[k]=position(p);}
 Rotation palm{};Vec3 center{};if(!palm_frame(points,palm,center)||!rd(driver->lease.vm+0x2690,after)||after!=counter||!driver->still_ours())return;
 Matrix relation=compose(weapon_body::inverse(body),weapon_body::matrix(palm,center));
 if(now<quietUntil||now-seenMs<1600){candidate=relation;candidateMs=now;stableSamples=0;return;}
 if(!candidateMs||!weapon_body::close(candidate,relation)){candidate=relation;candidateMs=now;stableSamples=0;return;}
 if(++stableSamples<12||now-candidateMs<240)return;
 learning.count=count;learning.bodyBone=bodyBone;learning.attachment=modelMuzzle.attachment;learning.bodyFromPalm=relation;
 learning.bodyFromBarrel=compose(weapon_body::inverse(body),compose(flash,modelMuzzle.local));
 U32 offset=0;if(!rd(driver->lease.header+0xa0,offset))return;
 for(U32 i=0;i<count;i++)if(!rd(driver->lease.header+offset+i*216+160,learning.boneFlags[i]))return;
 U32 ac=0,ao=0,size=0;
 if(!rd(driver->lease.header+76,size)||!rd(driver->lease.header+0xf0,ac)||!ac||ac>64||!rd(driver->lease.header+0xf4,ao)||ao>size||ac*92>size-ao)return;
 learning.attachmentCount=ac;
 for(U32 i=0;i<ac;i++){
  U32 at=driver->lease.header+ao+i*92,flags=0,bone=0;
  if(!rd(at+4,flags)||(flags&0x10000)||!rd(at+8,bone)||bone>=count||!(learning.boneFlags[bone]&0x200)||!mem->read(at+12,&learning.attachmentLocal[i],48)||!matrix_valid(learning.attachmentLocal[i]))return;
  learning.attachmentBones[i]=bone;
 }
 learning.revision=++mountSerial;learning.learnedMs=now;learning.valid=true;
 if(mountCell.write(learning)){++learned;note("BODY_IDLE_MOUNT_FROZEN",learning.checksum,learning.revision,now-seenMs);}
}
static void enter_eye(int value){eye=value;visual_recoil::eye=value;}
static void leave_eye(){eye=-1;visual_recoil::eye=-1;}
static void output(Matrix*out,int count,bool success){
 nova_perf::Scope timing(nova_perf::BodyOutput);
 if(!success||!rendering||eye<0||eye>1||!out||count<I32(renderMount.count)||!driver->checkpoint())return;
 U32 palette=0,alias=0,number=0;if(!rd(driver->lease.vm+0x2914,palette)||!rd(driver->lease.vm+0x26a8,alias)||!rd(driver->lease.vm+0x2920,number)||number!=renderMount.count)return;
 U64 a=addr(out),end=a+renderMount.count*48,p=palette,ae=alias;
 if((a<p+number*48&&end>p)||(a<ae+number*48&&end>ae)){++aliasRejects;store32(&retired,1);note("BODY_OUTPUT_CACHE_ALIAS_RETIRED",a,p,number);return;}
 if(!writeable(out,renderMount.count*48)){++badOutput;store32(&retired,1);note("BODY_OUTPUT_RANGE_RETIRED",a,count);return;}
 Matrix delta{};if(!weapon_body::adapt(out,scratch,U32(count),renderMount,display,delta)){++badOutput;store32(&retired,1);note("BODY_OUTPUT_MATRIX_RETIRED",a,count);return;}
 memcpy(out,scratch,renderMount.count*48);drawSeen[eye]=true;++adapted;
 weapon_fx::Attachments fx{};fx.count=renderMount.attachmentCount;fx.checksum=renderMount.checksum;
 bool fxGood=fx.count&&fx.count<=64&&rd(driver->lease.vm+0x2690,outputCounter[eye]);
 for(U32 i=0;fxGood&&i<fx.count;i++){
  fx.matrices[i]=compose(scratch[renderMount.attachmentBones[i]],renderMount.attachmentLocal[i]);
  fxGood=matrix_valid(fx.matrices[i])&&length(sub(position(fx.matrices[i]),driver->gripWorld))<120;
 }
 outputFxReady[eye]=fxGood;if(fxGood)outputFx[eye]=fx;
 if(eye==1){Vec3 points[5]{};for(U32 k=0;k<5;k++)points[k]=position(scratch[driver->rig.indices[k]]);drawnPalmValid=palm_frame(points,drawnPalmAxes,drawnPalm);}
 if(adapted<=4||adapted%240==0){
  auto p0=position(scratch[renderMount.bodyBone]);char b[550];int n=_snprintf(b,sizeof(b),"NOVA_BODY_DRAW pair=%llu sample=%llu eye=%d checksum=%u mount=%llu errorSU=%.9f output=%08x cache=%08x boneCacheWrites=0 forcedSetupBonesCalls=0\r\n",display.frame.pair,display.frame.sample,eye,renderMount.checksum,renderMount.revision,double(length(sub(p0,position(display.body)))),addr(out),palette);line(b,n>549?549:n);
 }
}
static bool command_source(const Identity&id,U32 pi,U32 wi,const Key&key,Frame&out){
 Mount m{};Reference r{};Tracking t{};Frame capability{};U64 now=GetTickCount64();
 // One successfully restored two-eye draw admits this mount/anchor. Its pose
 // is never reused here, and its age does not constrain subsequent commands.
 if(load32(&retired)||!tracking||!mountCell.read(m)||!m.valid||!same_identity(m.id,id)||!referenceCell.read(r)||!same_identity(r.id,id)||!frameCell.read(capability)||!capability.valid||capability.poseDomain!=1||!same_identity(capability.id,id)||capability.mountRevision!=m.revision||capability.anchorRevision!=r.anchorRevision||!read_tracking(tracking,t)||t.epoch!=r.epoch){inc32(&commandMisses);return false;}
 Vec3 nativeEye{};Sample sample{};
 if(!originalShoot[0]||originalShoot[0]((void*)(uintptr_t)id.player,&nativeEye)!=&nativeEye||!finite(nativeEye)||!weapon_body::solve(m,t,now,nativeEye,key.view,r.anchor,r.anchorRot,r.pair,r.anchorRevision,sample)){inc32(&commandMisses);return false;}
 sample.frame.playerIndex=pi;sample.frame.weaponIndex=wi;U32 tag=0,vt=0;if(!rd(id.weapon,vt)||!native_gun_class(vt,false,tag))return false;
 sample.frame.weaponClass=tag;sample.frame.poseDomain=2;sample.frame.mountRevision=m.revision;sample.frame.anchorRevision=r.anchorRevision;sample.frame.poseMs=t.sampledMs;sample.frame.predictedTime=t.predictedTime;
 Angles punch{};float scale=0;weapon_body::State state{};
 if(!visual_recoil::command_punch(id,t.epoch,punch,scale)||!weapon_body::state(sample,position(compose(sample.body,m.bodyFromPalm)),punch,scale,state)){inc32(&commandMisses);return false;}
 out=state.firing;U32 copies=inc32(&commandCopies);
 if(copies<=4||(key.buttons&1)){char b[650];int n=_snprintf(b,sizeof(b),"NOVA_BODY_COMMAND cmd=%d tick=%d sample=%llu displaySample=%llu poseAgeMs=%llu mount=%llu anchor=%llu origin=(%.6f,%.6f,%.6f) angles=(%.6f,%.6f,%.6f) source=XR_COMMAND_COPY\r\n",key.number,key.tick,t.sampleId,r.displaySample,now-t.sampledMs,m.revision,r.anchorRevision,double(out.barrel.p.x),double(out.barrel.p.y),double(out.barrel.p.z),double(out.barrel.a.p),double(out.barrel.a.y),double(out.barrel.a.r));line(b,n>649?649:n);}
 return true;
}
static bool read_body_delta(Matrix&delta){
 Matrix body{};U32 pointer=0;
 return rendering&&rd(driver->lease.vm+0x2914,pointer)&&pointer&&mem->read(pointer+renderMount.bodyBone*48,&body,48)&&matrix_valid(body)&&matrix_valid(delta=compose(display.body,weapon_body::inverse(body)));
}
static bool fx_target(const Frame&source,Frame&out){
 if(load32(&retired)||!valid_source(source))return false;
 if(GetCurrentThreadId()==renderThread&&rendering&&display.valid&&same_identity(source.id,display.frame.id)&&source.mountRevision==display.mountRevision&&source.anchorRevision==display.anchorRevision){out=firing;return true;}
 Command command{};Key key{};Peer peer{};
 if(!command_now(source.id.player,false,command,key,peer)||command.source.poseDomain!=2||!same_identity(command.source.id,source.id)||command.source.mountRevision!=source.mountRevision||command.source.anchorRevision!=source.anchorRevision)return false;
 out=command.source;return true;
}
static bool intake(int which,Pose&base,U32&counter,weapon_fx::Attachments&fx){
 nova_perf::Scope timing(nova_perf::BodyIntake);
 if(!load32(&native_game_control::outputIntake)||!rendering||!baseDisplay.valid||which<0||which>1||!drawSeen[which]||!outputFxReady[which]){++slowIntakes;return false;}
 U32 current=0;if(!rd(driver->lease.vm+0x2690,current)||current!=outputCounter[which]){++slowIntakes;return false;}
 base=firing.barrel;counter=current;fx=outputFx[which];++fastIntakes;return true;
}
static void end(){rendering=false;eye=-1;}
static void report(){note("R1_OUTPUT_INTAKE_COUNTS",fastIntakes,slowIntakes);note("BODY_COUNTS",adapted,aliasRejects,badOutput);note("BODY_COMMAND_COUNTS",load32(&commandCopies),load32(&commandMisses),learned);}
}
