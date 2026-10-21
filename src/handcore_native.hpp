#pragma once
#include "native_memory.hpp"
#include "game_control.hpp"
#include "muzzle_native.hpp"
#include "perf.hpp"
namespace handcore_native {
using namespace nova;
static NativeMemory memory;
static handcore::Driver<NativeMemory> driver;
static Log logger;
static bool initialized=false,previousF6=false,offByLaunch=false;
static U64 lastReport=0,lastSummary=0;
static Tracking lastInput{};static U64 inputNow=0;
static handcore::Reason lastReason=handcore::Reason::Ready;
using KeyFn=short(WINCALL*)(int);
using ForegroundFn=HWND(WINCALL*)();
using WindowPidFn=U32(WINCALL*)(HWND,U32*);
static KeyFn keyFn=nullptr;static ForegroundFn foregroundFn=nullptr;static WindowPidFn windowPidFn=nullptr;
static void initialize(U8*base,U32 thread){
 if(initialized)return;initialized=true;memory.base=U32((uintptr_t)base);memory.thread=thread;
 wchar_t off[8]{};offByLaunch=GetEnvironmentVariableW(L"NOVA_HANDCORE_OFF",off,8)>0&&off[0]==L'1';
 logger.open(L"nova_handcore_h1.log");driver.initialize(memory,memory.base,!offByLaunch);
#ifdef NOVA_LAB_UNIVERSAL
 driver.universal=true;memory.fastRead=true;logger.msg("UNIVERSAL_PALM_NAMED_BONES_MODE");
#endif
 HMODULE u=GetModuleHandleW(L"user32.dll");
 if(u){keyFn=(KeyFn)GetProcAddress(u,"GetAsyncKeyState");foregroundFn=(ForegroundFn)GetProcAddress(u,"GetForegroundWindow");windowPidFn=(WindowPidFn)GetProcAddress(u,"GetWindowThreadProcessId");}
 logger.msg("HANDCORE_H2_BORE_BUILD",1475988351u,driver.exact,offByLaunch);
 logger.msg("H2_R4_HOST_UNCHANGED_BORE_MOUNT_GAMEPLAY_IN_SEPARATE_MODULE",thread);
}
static void report_gate(){
 const auto&g=driver.observation;const auto&t=lastInput;
 char b[1500];int n=_snprintf(b,sizeof(b),
  "NOVA_H1F1_GATE reason=%s sample=%llu epoch=%llu inputAgeMs=%llu trackingValid=%u focused=%u rightActive=%u rightValid=%u grip=(%.5f,%.5f,%.5f) readMask=0x%02x vm=%08x firstChild=%08x boneOwner=%u boneDepth=%u child=%08x childHandle=%08x childOrdinal=%u childVt=%08x childRenderable=%08x childParent=%08x childNext=%08x childFirst=%08x childBoneOwner=%u childBoneDepth=%u checksum=%u bones=%u model=%s childPairs=%llu scope=SEQUENTIAL_CHECKED_READS_NOT_ATOMIC_SNAPSHOT\r\n",
  handcore::reason_name(driver.reason),t.sampleId,t.epoch,inputNow>=t.sampledMs?inputNow-t.sampledMs:~U64(0),t.valid,t.focused,t.hand[1].active,t.hand[1].valid,
  double(t.hand[1].pos.x),double(t.hand[1].pos.y),double(t.hand[1].pos.z),g.readMask,g.vm,g.child,g.boneOwner,g.boneDepth,g.childEntity,g.childHandle,g.childOrdinal,g.childVtable,g.childRenderable,g.childParent,g.childNext,g.childChildren,g.childBoneOwner,g.childBoneDepth,g.checksum,g.boneCount,(g.readMask&2)?g.modelName:"UNREAD",driver.counts.childPairs);
 if(n>0&&logger.file!=BAD_HANDLE){U32 done=0;WriteFile(logger.file,b,U32(n<1499?n:1499),&done,nullptr);}
}
static void report_reason(){
 U64 now=GetTickCount64();if(driver.reason==lastReason&&now-lastReport<2500)return;
 lastReason=driver.reason;lastReport=now;
 char line[512];int n=_snprintf(line,sizeof(line),"NOVA_H1_STATUS reason=%s enabled=%u retired=%u leased=%u attempts=%llu applied=%llu restored=%llu conflicts=%llu\r\n",
 handcore::reason_name(driver.reason),driver.enabled?1:0,driver.retired?1:0,driver.leased?1:0,driver.counts.attempts,driver.counts.applied,driver.counts.restored,driver.counts.conflicts);
 if(n>0&&logger.file!=BAD_HANDLE){U32 done=0;WriteFile(logger.file,line,U32(n<511?n:511),&done,nullptr);}
 report_gate();
}
static void toggle_at_boundary(){
 if(offByLaunch||!keyFn||!foregroundFn||!windowPidFn)return;
 U32 pid=0;HWND h=foregroundFn();if(!h||!windowPidFn(h,&pid)||pid!=GetCurrentProcessId()){previousF6=false;return;}
 bool down=(keyFn(0x75)&0x8000)!=0;
 if(down&&!previousF6&&!driver.leased&&!driver.retired){driver.enabled=!driver.enabled;logger.msg("H1_F6_BOUNDARY_ENABLED",driver.enabled);}
 previousF6=down;
}
struct Scope {
 bool active{},logged{},finished{};U64 frame{};
 Scope(U8*base,U32 thread,const Tracking&t,U64 frameId,const void*view,Vec3 anchor,Quat anchorRot,int drawFlags){
  nova_perf::Scope timing(nova_perf::HandBegin);
  initialize(base,thread);toggle_at_boundary();muzzle_native::initialize(memory,driver,thread);frame=frameId;
  Vec3 origin{};Angles angles{};memcpy(&origin,(const U8*)view+0xb8,12);memcpy(&angles,(const U8*)view+0xc4,12);
  lastInput=t;inputNow=GetTickCount64();
  active=driver.begin(t,inputNow,frameId,origin,angles,anchor,anchorRot,drawFlags);
  muzzle_native::begin(active,t,origin,angles,anchor,anchorRot);
  if(!active){report_reason();return;}
  logged=driver.counts.applied<=3||driver.counts.applied%120==0;
  if(logged){report_gate();char s[1152];int n=_snprintf(s,sizeof(s),
   "NOVA_H1_ROOT frame=%llu sample=%llu epoch=%llu vm=%08x handle=%08x owner=%08x weapon=%08x model=%u mask=RIGHT_FULL_GRIP mount=%s local=(%.6f,%.6f,%.6f) angles=(%.6f,%.6f,%.6f) targetPalm=(%.6f,%.6f,%.6f) previous=(%.6f,%.6f,%.6f) drawFlags=%d\r\n",
   frame,driver.sample,driver.epoch,driver.lease.vm,driver.lease.vmHandle,driver.lease.ownerHandle,driver.lease.weaponHandle,unsigned(driver.lease.model),driver.universal?"LIVE_NAMED_PALM":"P2000_BORE_ALIGNED_AROUND_PALM",
   double(driver.desired.p.x),double(driver.desired.p.y),double(driver.desired.p.z),double(driver.desired.a.p),double(driver.desired.a.y),double(driver.desired.a.r),double(driver.gripWorld.x),double(driver.gripWorld.y),double(driver.gripWorld.z),double(driver.before.p.x),double(driver.before.p.y),double(driver.before.p.z),drawFlags);
   if(n>0&&logger.file!=BAD_HANDLE){U32 done=0;WriteFile(logger.file,s,U32(n<1151?n:1151),&done,nullptr);}
  }
 }
 void after_eye(int eye){
  bool ownPalm=nova_fps::on(nova_fps::PalmIntake)&&active&&driver.universal&&eye==1&&muzzle_native::body_authority::rendering&&muzzle_native::body_authority::drawSeen[1]&&muzzle_native::body_authority::drawnPalmValid;
  if(ownPalm)++nova_fps::skippedPalm;
  if(driver.universal&&eye==1&&!ownPalm&&driver.observe_palm()){
   static U32 samples=0;if(++samples<=3||samples%120==0)logger.msg("UNIVERSAL_PALM_MOUNT",driver.rig.checksum,driver.rig.count,driver.rig.indices[0]);
  }
  if(driver.universal&&eye==1){
   Vec3 observed{};bool observedOkay=active&&driver.rig.pending;
   if(observedOkay)observed=add(driver.desired.p,handcore::apply(handcore::pose_rotation(driver.desired.a),driver.rig.nextCenter));
   auto axes=handcore::multiply(handcore::pose_rotation(driver.desired.a),driver.rig.nextRotation);
   if(muzzle_native::body_authority::rendering&&muzzle_native::body_authority::drawSeen[1]){
    observedOkay=muzzle_native::body_authority::drawnPalmValid;
    observed=muzzle_native::body_authority::drawnPalm;axes=muzzle_native::body_authority::drawnPalmAxes;
   }
   native_game_control::hand(frame,observedOkay,U32(driver.reason),observed,driver.gripWorld,axes);
  }
  if(!active)return;
  if(!driver.checkpoint()){active=false;report_reason();return;}
  muzzle_native::after_eye(eye,active);
  if(logged){Vec3 palm{};U32 c=0;
   if(driver.read_natural_palm(palm,c)){
    ++driver.counts.paletteReads;char s[512];int n=_snprintf(s,sizeof(s),"NOVA_H1_NATURAL_PALM frame=%llu sample=%llu eye=%d nativeCounter=%u observed=(%.6f,%.6f,%.6f) target=(%.6f,%.6f,%.6f) distanceSU=%.6f horizon=READ_ONLY_CACHE_NOT_PIXEL_PROOF\r\n",frame,driver.sample,eye,c,double(palm.x),double(palm.y),double(palm.z),double(driver.gripWorld.x),double(driver.gripWorld.y),double(driver.gripWorld.z),double(length(sub(palm,driver.gripWorld))));
    if(n>0&&logger.file!=BAD_HANDLE){U32 done=0;WriteFile(logger.file,s,U32(n<511?n:511),&done,nullptr);}
   }else{++driver.counts.paletteMissing;logger.msg("H1_NATURAL_PALETTE_NOT_ADMITTED",frame,eye);}
  }
 }
 void finish(bool renderOkay=false){
  if(finished)return;finished=true;bool hadLease=driver.leased;bool restored=driver.leased&&driver.end();
  if(driver.universal)driver.rig.commit(renderOkay&&!driver.retired&&(!hadLease||restored));
  if(!restored&&active)report_reason();muzzle_native::finish(renderOkay,restored);active=false;
  if(GetTickCount64()-lastSummary>2000){lastSummary=GetTickCount64();logger.msg("H1_COUNTS",driver.counts.applied,driver.counts.restored,driver.counts.conflicts);logger.msg("H1F1_ADMISSION",driver.counts.attempts,driver.counts.rejected,driver.counts.childPairs);
#ifdef NOVA_LAB_UNIVERSAL
   logger.msg("G5_PERMISSION_READ_COUNTS",memory.permissionQueries,memory.permissionCacheHits,load32(&native_game_control::additionalReadBatch));
   logger.msg("F4_RESIDENT_PERMISSION_COUNTS",load32(&native_memory_access::attempts),load32(&native_memory_access::approved),load32(&native_memory_access::fallbacks));
#endif
  }
 }
 ~Scope(){finish();}
};
}
