extern "C" {int _fltused = 0;}
#include "platform.hpp"
#include "gfx.hpp"
#include "wire.hpp"
#include "frame_transport.hpp"
#include "math.hpp"
#ifdef NOVA_LAB_UNIVERSAL
#include "stable_view.hpp"
#endif
#include "handcore_native.hpp"
#include "perf.hpp"
#include "gpu_perf.hpp"
#include "ray_optics.hpp"
#include "optic_surface_profiles.hpp"
#include "view_region.hpp"
extern "C" __declspec(dllimport) BOOL WINCALL FlushInstructionCache(HANDLE,const void*,SIZE_T);
// NOVA SCENE 32: independent native-game producer. No HLAE, no CPU pixels,
// no externally reconstructed scene, no replayed draws, no C1-C5 frame protocol.
using namespace nova;
namespace {
static Shared*bus=nullptr;
static nova_frame_transport::Endpoint frameExtra;
static Image& source_image(U32 slot){return slot<3?bus->images[slot]:(slot==3?frameExtra.page->image:bus->images[3]);}
static CapturedStereo& source_capture(U32 slot){return slot<3?bus->captured[slot]:frameExtra.page->captured;}
static U32 source_texture(U32 slot){return slot<3?slot*2:8;}
static Log logg;
static HANDLE map=nullptr;
static U8 *client=nullptr,*mat=nullptr,*shaderDll=nullptr,*engine=nullptr;
static D9Device*gpu=nullptr;static void*material=nullptr,*eyeTex[2]{};
static D9Texture*shareTex[10]{};static D9Surface*shareRT[10]{};
static D9Query*queries[5]{};static U32 pending[5]{},fenceReported[5]{};
static D9Surface*menuResolve=nullptr;static U32 menuResolveW=0,menuResolveH=0,menuResolveFmt=0;
static volatile I32 active=0,hookRendering=0,hookPresenting=0,installed=0,gpuPhase=0,worldPhase=0,worldHookInstalled=0;
static U32 graphicsThread=0;
#ifdef NOVA_LAB_UNIVERSAL
static HMODULE graphicsModuleLease=nullptr;static volatile I32 gpuRetired=0;
static D9Surface*windowMirror=nullptr;static U64 windowMirrorMs{},windowMirrorPair{},windowMirrorSample{};
static bool eyeExceedsWindow=false;static U32 verifiedEyeExtents=0;
static bool stop_on_graphics_thread();
static bool change_scope_calls(bool);
namespace lens_material {static void shutdown();}
namespace pixel_budget {static void shutdown();}
static void retire_owned_gpu(){
 if(load32(&gpuRetired)|| (graphicsThread&&GetCurrentThreadId()!=graphicsThread))return;
 change_scope_calls(false);lens_material::shutdown();pixel_budget::shutdown();nova_gpu_perf::retire();
 native_game_control::release_buttons();
 for(auto*&query:queries)if(query){query->release();query=nullptr;}
 for(auto*&surface:shareRT)if(surface){surface->release();surface=nullptr;}
 for(auto*&texture:shareTex)if(texture){texture->release();texture=nullptr;}
 if(menuResolve){menuResolve->release();menuResolve=nullptr;}
 if(windowMirror){windowMirror->release();windowMirror=nullptr;}windowMirrorMs=0;
 // eyeTex and gpu are borrowed engine objects. No foreign reference is released.
 store32(&gpuRetired,1);logg.msg("NATIVE_GPU_RESOURCES_RETIRED_ON_GRAPHICS_THREAD",GetCurrentThreadId());
}
#endif
static U64 serial=0,epoch=1,anchorEpoch=0;
static Vec3 anchorCenter{};static Quat anchorRotation{};
using Render=void(__thiscall*)(void*,const void*,const void*,int,int);
using Present=HRESULT(WINCALL*)(D9Device*,const RECT*,const RECT*,HWND,const void*);
using PresentEx=HRESULT(WINCALL*)(D9Device*,const RECT*,const RECT*,HWND,const void*,U32);
static Render nativeRender=nullptr;static Present originalPresent=nullptr;static PresentEx originalPresentEx=nullptr;
static void**renderSlot=nullptr,**presentSlot=nullptr,**presentExSlot=nullptr;
static bool readable(const void*addr,SIZE_T bytes){int resident=native_memory_access::probe(addr,bytes);if(resident)return resident>0;MEMORY_BASIC_INFORMATION m{};if(!addr||VirtualQuery(addr,&m,sizeof(m))<sizeof(m)||m.State!=0x1000)return false;
 U32 prot=m.Protect&0xff;if(prot==0x01||prot==0x10)return false;
 uintptr_t p=(uintptr_t)addr,end=p+bytes,limit=(uintptr_t)m.BaseAddress+m.RegionSize;
 return end>=p&&end<=limit;}
static bool bytes(U8*base,U32 off,const U8*expected,size_t n){if(!readable(base+off,n))return false;for(size_t i=0;i<n;++i)if(base[off+i]!=expected[i])return false;return true;}
static bool hook(void**cell,void*expected,void*replacement){if(!readable(cell,4)||*cell!=expected)return false;U32 old=0;if(!VirtualProtect(cell,4,0x40,&old))return false;
 bool done=__atomic_compare_exchange_n((uintptr_t*)cell,(uintptr_t*)&expected,(uintptr_t)replacement,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE);
 U32 ignored=0;bool restored=VirtualProtect(cell,4,old,&ignored)!=0;
 return done&&restored;}
static void undo(void**cell,void*replacement,void*original){if(!cell||!original)return;U32 old;if(!VirtualProtect(cell,4,0x40,&old)){logg.msg("UNHOOK_PROTECT_FAILED");return;}
 uintptr_t expected=(uintptr_t)replacement;bool success=__atomic_compare_exchange_n((uintptr_t*)cell,&expected,(uintptr_t)original,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE);
 U32 ignored;bool r=VirtualProtect(cell,4,old,&ignored)!=0;if(!success||!r)logg.msg("UNHOOK_UNCERTAIN",success,r);}
static HRESULT WINCALL present_hook(D9Device*,const RECT*,const RECT*,HWND,const void*);
static HRESULT WINCALL presentex_hook(D9Device*,const RECT*,const RECT*,HWND,const void*,U32);
#include "native_pixel_budget_state.hpp"
#include "native_lens_material.hpp"
static bool repair_stateblock_device(){
 if(GetCurrentThreadId()!=graphicsThread||!gpu||presentSlot!=gpu->v+17||presentExSlot!=gpu->v+121)return false;
 bool good=pixel_budget::rearm_cell(presentSlot,(void*)originalPresent,(void*)present_hook);
 good=pixel_budget::rearm_cell(presentExSlot,(void*)originalPresentEx,(void*)presentex_hook)&&good;
 good=pixel_budget::rearm()&&good;
 if(lens_material::installed)good=pixel_budget::rearm_cell(lens_material::drawCell,(void*)lens_material::originalDraw,(void*)lens_material::draw_hook)&&good;
 logg.msg(good?"F2_NATIVE_RECORDING_DEVICE_LEASES_REPAIRED":"F2_NATIVE_RECORDING_FOREIGN_DEVICE_TABLE",GetCurrentThreadId());return good;
}
// Separate present-device identity from world-material admission.
// The live menu must continue even if native WORLD view ownership is unavailable.
static bool bind_device(){
 if(!mat||!shaderDll||!readable(mat+0xfd6dc,4))return false;
 void*shader=*(void**)(mat+0xfd6dc);
 if(!readable(shader,0x5c0))return false;
 void**sv=*(void***)shader;
 if(!readable(sv,181*4)||sv[180]!=shaderDll+0x22630)return false;
 D9Device*found=*(D9Device**)((U8*)shader+0x5a4);
 if(!readable(found,4)||!readable(found->v,122*4))return false;
 gpu=found;return true;
}
static bool bind_world(){
 if(!client||!mat||!shaderDll||!readable(client+0x534d6c4,4))return false;
 material=*(void**)(client+0x534d6c4);
 if(!readable(material,4))return false;
 void**mv=*(void***)material;
 if(!readable(mv,116*4)||mv[115]!=mat+0x181b0)return false;
 void*shader=*(void**)(mat+0xfd6dc);
 if(!readable(shader,0x5c0))return false;
 if(*(U32*)((U8*)shader+0x5bc)!=0){logg.msg("WORLD_BUFFERED_QUEUE_NOT_ADMITTED");return false;}
 return true;
}
static bool make_eye_targets(){auto**vt=*(void***)material;
#ifdef NOVA_LAB_UNIVERSAL
 // A window is clamped to the desktop by this engine. The owned eye targets
 // have separate depth surfaces and use literal dimensions instead.
 D9Surface*back=nullptr;D9SurfaceDesc actual{};
 if(ok(gpu->back(&back))&&back){back->desc(&actual);back->release();}
 logg.msg("UNIVERSAL_EYE_EXTENT",bus->width,actual.width,actual.height);
 eyeExceedsWindow=bus->width>actual.width||bus->height>actual.height;
 const int sizeMode=8; // RT_SIZE_LITERAL; verified against actual bound RT below.
#else
 const int sizeMode=0;
#endif
 using Create=void*(__thiscall*)(void*,const char*,int,int,int,int,int,U32,U32);
 Create create=(Create)vt[97];if(!readable((void*)create,5))return false;
 for(int e=0;e<2;e++){char name[72];_snprintf(name,sizeof(name),"_nova_scene_%u_%c",GetCurrentProcessId(),e?'R':'L');
 eyeTex[e]=create(material,name,bus->width,bus->height,sizeMode,12,1,0xc,0);if(!eyeTex[e])return false;}
 return true;}
static bool open_shared(){if(!gpu||!bus||!load32(&bus->resourceReady))return false;
 for(int i=0;i<10;i++){
  U64 hv=i<8?bus->sharedHandles[i]:frameExtra.page->handles[i-8];if(!hv||hv>0xffffffffu)return false;
  HANDLE h=(HANDLE)(uintptr_t)hv;HRESULT r=gpu->texture(bus->width,bus->height,21,&shareTex[i],&h);
  if(failed(r)||!shareTex[i]||failed(shareTex[i]->surface(&shareRT[i]))||!shareRT[i]){logg.msg("SHARE_OPEN_FAIL",i,(U32)r);return false;}
 }
 for(int i=0;i<5;i++){HRESULT r=gpu->query(&queries[i]);if(failed(r)||!queries[i]){logg.msg("QUERY_CREATE_FAILED",i,(U32)r);return false;}}
 store32(&bus->openMask,0xff);return true;}
static void check_gpu(){nova_perf::Scope timing(nova_perf::SourcePoll);if(!graphicsThread||GetCurrentThreadId()!=graphicsThread)return;for(int i=0;i<5;i++){
 if(!pending[i]||load32(&source_image(i).phase)!=WAITING_GPU)continue;
 HRESULT r=queries[i]->poll();if(r==0){pending[i]=0;nova_frame_transport::ready(source_image(i));if(i==4)inc32(&bus->menuPixels);else inc32(&bus->stereoPixels);}
 else if((r<0||GetTickCount64()-source_image(i).writeMs>800)&&!fenceReported[i]){
  // A timeout is not a last-writer completion. Keep the slot leased; never recycle uncertain GPU bytes.
  fenceReported[i]=1;inc32(&bus->sourceFenceFaults);inc32(&bus->graphicsErrors);
  logg.msg("GPU_QUERY_NOT_COMPLETED_SLOT_QUARANTINED",i,(U32)r);
 }
 }nova_gpu_perf::poll();}
static bool issue(int idx,U64 frame){if(failed(queries[idx]->end())){
 // Copies may already be in flight. WRITER_RESERVED intentionally quarantines this slot.
 inc32(&bus->graphicsErrors);inc32(&bus->sourceFenceFaults);logg.msg("SOURCE_QUERY_ISSUE_FAILED_SLOT_QUARANTINED",idx);return false;}
 if(!nova_frame_transport::commit(source_image(idx),frame,epoch,bus->width,bus->height))return false;pending[idx]=1;fenceReported[idx]=0;return true;}
static int acquire_scene_slot(){for(int i=0;i<(nova_fps::on(nova_fps::FourthSourcePair)?4:3);i++)if(nova_frame_transport::reserve(source_image(i)))return i;return -1;}
using ScopePredicate=bool(__thiscall*)(void*);
static ScopePredicate nativeScopePredicate=nullptr;static bool suppressNativeScope=false;static bool scopeCandidate=false;
static bool scopeCallInstalled[2]{};static U32 scopeOld[2]{},scopeNew[2]{};
static bool __fastcall scope_predicate_hook(void*self,void*){
 if(suppressNativeScope&&GetCurrentThreadId()==graphicsThread)return false;
 return nativeScopePredicate(self);
}
static bool change_scope_calls(bool install){
 // Exact, direct scope-render CALLs only. No shared predicate body/trampoline.
 // Installation and retirement run on the admitted unbuffered graphics thread.
 if(GetCurrentThreadId()!=graphicsThread)return false;
 const U32 sites[2]={0x6ad239,0x6ad51e};
 if(install){const U8 prefix[]={0x55,0x8b,0xec,0x53,0x56,0x8b,0xf1,0x57,0x8b,0x4e,0x3c};if(!bytes(client,0x6ad880,prefix,sizeof(prefix)))return false;nativeScopePredicate=(ScopePredicate)(client+0x6ad880);}
 for(int i=0;i<2;i++){
  if(scopeCallInstalled[i]==install)continue;
  U8*call=client+sites[i];if(!readable(call,5)||call[0]!=0xe8)return false;
  U32 value=0;memcpy(&value,call+1,4);
  if(install){scopeOld[i]=U32((uintptr_t)(client+0x6ad880))-U32((uintptr_t)(call+5));scopeNew[i]=U32((uintptr_t)scope_predicate_hook)-U32((uintptr_t)(call+5));}
  U32 expected=install?scopeOld[i]:scopeNew[i],replacement=install?scopeNew[i]:scopeOld[i];
  if(value!=expected){logg.msg("OPTIC_SCOPE_CALL_FOREIGN",i,value,expected);return false;}
  U32 protect=0,ignored=0;if(!VirtualProtect(call+1,4,0x40,&protect))return false;
  memcpy(call+1,&replacement,4);bool flushed=FlushInstructionCache((HANDLE)(intptr_t)-1,call,5)!=0;
  bool protectedAgain=VirtualProtect(call+1,4,protect,&ignored)!=0;memcpy(&value,call+1,4);
  scopeCallInstalled[i]=install;
  if(!flushed||!protectedAgain||value!=replacement)return false;
  logg.msg(install?"OPTIC_SCOPE_CALL_INSTALLED":"OPTIC_SCOPE_CALL_RESTORED",i,sites[i]);
 }
 return true;
}
static void legacy_render(void*self,const void*view,const void*hud,int clear,int flags){nova_perf::Scope timing(nova_perf::NativeRender);bool previous=suppressNativeScope;suppressNativeScope=scopeCandidate;nativeRender(self,view,hud,clear,flags);suppressNativeScope=previous;}
// Native material RT ownership, separate from the shared host staging textures.
static void*scene_context(){auto**vt=*(void***)material;return ((void*(__thiscall*)(void*))vt[115])(material);}
static void context_push(void*ctx,void*rt,U32 w=0,U32 h=0){void**v=*(void***)ctx;((void(__thiscall*)(void*,void*,void*,int,int,int,int))v[116])(ctx,rt,nullptr,0,0,(int)(w?w:bus->width),(int)(h?h:bus->height));}
static void context_pop(void*ctx){void**v=*(void***)ctx;((void(__thiscall*)(void*))v[120])(ctx);}
static void context_release(void*ctx){void**v=*(void***)ctx;((void(__thiscall*)(void*))v[1])(ctx);}
static void log_eye_packet(const CapturedStereo& c,int slot){
 static U32 logged=0;if(++logged>3 && logged%120!=0)return;
 for(int e=0;e<2;e++){
  char line[768];int n=_snprintf(line,sizeof(line),
   "NOVA_R3_EYE custom=0 lens=NATIVE_CENTERED frame=%llu sample=%llu epoch=%llu slot=%d eye=%d time=%lld source=(%.6f,%.6f,%.6f) angles=(%.6f,%.6f,%.6f) fov=(%.7f,%.7f,%.7f,%.7f) P=(%.7f,%.7f,%.7f,%.7f) nativeEyeGapM=%.7f\r\n",
   c.pairId,c.sampleId,c.trackingEpoch,slot,e,c.predictedTime,
   double(c.sourceEye[e].x),double(c.sourceEye[e].y),double(c.sourceEye[e].z),
   double(c.sourceAngles[e][0]),double(c.sourceAngles[e][1]),double(c.sourceAngles[e][2]),
   double(c.eye[e].fov.l),double(c.eye[e].fov.r),double(c.eye[e].fov.u),double(c.eye[e].fov.d),
   double(c.projection[e][0]),double(c.projection[e][2]),double(c.projection[e][5]),double(c.projection[e][6]),
   double(length(sub(c.sourceEye[1],c.sourceEye[0]))/UNITS_PER_METRE));
  if(n>0&&logg.file!=BAD_HANDLE){U32 done=0;WriteFile(logg.file,line,U32(n<767?n:767),&done,nullptr);}
 }
}
#include "body_optic_view.hpp"
static void stereo(void*self,const void*view,const void*hud,int clear,int flags,const Tracking&t){
 nova_perf::Scope timing(nova_perf::Stereo);
 // Backpressure is a skipped opportunity, not a graphics API error.
 int slot=acquire_scene_slot();if(slot<0){inc32(&bus->slotBusy);
  // A mono render would be immediately covered by the last complete XR mirror.
  // Retain that same mirror on backpressure; first/stale/foreign-epoch frames
  // retain the original native fallback. Present still flushes native commands.
  if(nova_fps::on(nova_fps::AuxiliaryCopies)&&windowMirror&&windowMirrorMs&&anchorEpoch==t.epoch&&t.sampleId>=windowMirrorSample&&GetTickCount64()-windowMirrorMs<250){++nova_fps::skippedMirror;return;}
  legacy_render(self,view,hud,clear,flags);return;}
 if(!t.sampleId||GetTickCount64()-t.sampledMs>250||!valid_eye(t.eye[0])||!valid_eye(t.eye[1])){
  inc32(&bus->poseRejects);nova_frame_transport::fail(source_image(slot));legacy_render(self,view,hud,clear,flags);return;
 }
 bool manualRecenter=false;
#ifdef NOVA_LAB_UNIVERSAL
 manualRecenter=native_game_control::take_recenter(t);
#endif
 Vec3 oldAnchor=anchorCenter;
 if(anchorEpoch!=t.epoch||manualRecenter){
  anchorEpoch=t.epoch;anchorCenter=scale(add(t.eye[0].pos,t.eye[1].pos),0.5f);
  anchorRotation=anchor_yaw(t.eye[0].rot,t.eye[1].rot);logg.msg("R3_TRACKING_ANCHOR",anchorEpoch,t.sampleId);
  if(manualRecenter){
   char text[384];int n=_snprintf(text,sizeof(text),"NOVA_R3_MANUAL_RECENTER epoch=%llu sample=%llu old=(%.5f,%.5f,%.5f) center=(%.5f,%.5f,%.5f) yawQuat=(%.6f,%.6f)\r\n",t.epoch,t.sampleId,double(oldAnchor.x),double(oldAnchor.y),double(oldAnchor.z),double(anchorCenter.x),double(anchorCenter.y),double(anchorCenter.z),double(anchorRotation.y),double(anchorRotation.w));U32 done=0;if(n>0&&logg.file!=BAD_HANDLE)WriteFile(logg.file,text,U32(n<383?n:383),&done,nullptr);
  }
 }
 U8 eyeBytes[2][0x14c];CapturedStereo capture{};
 capture.pairId=serial+1;capture.trackingEpoch=t.epoch;capture.sampleId=t.sampleId;
 capture.predictedTime=t.predictedTime;capture.capturedMs=GetTickCount64();capture.projectionKind=2;
 float cullFov=0,cullAspect=0;Fov rendered{};
 if(!native_lens(t.eye[0].fov,t.eye[1].fov,cullFov,cullAspect,rendered)){
  inc32(&bus->poseRejects);nova_frame_transport::fail(source_image(slot));legacy_render(self,view,hud,clear,flags);return;
 }
#ifdef NOVA_LAB_UNIVERSAL
 U8 stableView[0x14c];
 if(!((const U8*)view)[0x20]&&!((const U8*)view)[0x34])view=stable_camera::prepare(view,stableView,client,engine,capture.pairId,t.sampleId);
 native_game_control::reference(t,view,anchorCenter,anchorRotation);
#endif
 for(int eye=0;eye<2;eye++){
  capture.eye[eye]=t.eye[eye];capture.eye[eye].fov=rendered;capture.eye[eye].rot=normalized(capture.eye[eye].rot);Angles a{};
  if(!write_eye_view((const U8*)view,eyeBytes[eye],capture.eye[eye],anchorCenter,anchorRotation,cullFov,cullAspect,
     bus->width,bus->height,capture.sourceEye[eye],a,capture.projection[eye])){
   inc32(&bus->poseRejects);if(bus->poseRejects<4)logg.msg("R3_NONSTANDARD_VIEW_OR_INVALID_NATIVE_LENS",((const U8*)view)[0x20],((const U8*)view)[0x34]);
   nova_frame_transport::fail(source_image(slot));legacy_render(self,view,hud,clear,flags);return;
  }
  capture.sourceAngles[eye][0]=a.p;capture.sourceAngles[eye][1]=a.y;capture.sourceAngles[eye][2]=a.r;
  nova_fps::distance(eyeBytes[eye]);float nearZ=0,farZ=0;memcpy(&nearZ,eyeBytes[eye]+0xd0,4);memcpy(&farZ,eyeBytes[eye]+0xd4,4);
  projection(capture.eye[eye].fov,nearZ,farZ,capture.projection[eye]);
 }
 void*ctx=scene_context();if(!readable(ctx,4)){nova_frame_transport::fail(source_image(slot));legacy_render(self,view,hud,clear,flags);return;}
 // H1 is a scoped post-stock native viewmodel-root operation. R4 WORLD view,
 // original render schedule and shared GPU publication stay unchanged.

 U32 opticVm=0,opticModel=body_optic_equipped(opticVm);bool optic=opticModel!=0;scopeCandidate=optic;
 int vrFlags=optic?(flags|3):flags;
 handcore_native::Scope hand(client,graphicsThread,t,capture.pairId,view,anchorCenter,anchorRotation,vrFlags);
 if(hand.active){for(int eye=0;eye<2;eye++)memcpy(eyeBytes[eye]+0xb4,&cullFov,4);} // viewmodel lens only
 bool success=true,croppedPair=false;
 bool opticsInstalled=optic&&lens_material::install(ctx);
 float opticFarZ=0;memcpy(&opticFarZ,(const U8*)view+0xd4,4);
 if(opticsInstalled)body_optic_image(self,eyeBytes[0],hud,ctx,capture,opticFarZ);
#ifdef NOVA_LAB_UNIVERSAL
 // This retail postprocess samples window-sized intermediate buffers. For an
 // oversized eye it stretches the last row/column over the rest of the view.
 // Ordinary native view flags bypass bloom/DOF for these two views only;
 // the owned XR host supplies AA/sharpening at the actual eye resolution.
 if(eyeExceedsWindow||optic)for(int eye=0;eye<2;eye++)eyeBytes[eye][0x148]&=U8(~0x0cu);
 D9Surface*leftMirror=nullptr;
#endif
 for(int eye=0;eye<2;eye++){
  context_push(ctx,eyeTex[eye]);D9Surface*color=nullptr;
  HRESULT rt=gpu->getrt(&color);
  if(failed(rt)||!color){success=false;context_pop(ctx);break;}
#ifdef NOVA_LAB_UNIVERSAL
  D9SurfaceDesc actual{};HRESULT descriptor=color->desc(&actual);
  if(failed(descriptor)||actual.width!=bus->width||actual.height!=bus->height){
   logg.msg("NATIVE_EYE_EXTENT_MISMATCH",eye,actual.width,actual.height);
   color->release();context_pop(ctx);success=false;store32(&worldPhase,-1);break;
  }
  if(!(verifiedEyeExtents&(1u<<eye))){verifiedEyeExtents|=1u<<eye;logg.msg("NATIVE_LITERAL_EYE_READY",eye,actual.width,actual.height);}
#endif
  lens_material::begin_view(opticsInstalled,opticVm,opticModel,capture.pairId,eye);
  muzzle_native::body_authority::enter_eye(eye);
  {pixel_budget::Scope pixels(color,capture.eye[eye].fov,t.eye[eye].fov);croppedPair|=pixel_budget::active;nova_gpu_perf::Scope gpuTiming(eye?nova_gpu_perf::RightEye:nova_gpu_perf::LeftEye,capture.pairId);legacy_render(self,eyeBytes[eye],hud,clear,vrFlags);}
  lens_material::end_view();
  muzzle_native::body_authority::leave_eye();
  {nova_perf::Scope handTiming(nova_perf::HandEye);hand.after_eye(eye);}
  // Same native render/material/RT route as the user-visible R1, no draw replay.
  HRESULT cp=0;{nova_gpu_perf::Scope gpuTiming(nova_gpu_perf::EyeCopy,capture.pairId);cp=gpu->stretch(color,nullptr,shareRT[source_texture(slot)+eye],nullptr);}
 #ifdef NOVA_LAB_UNIVERSAL
  if(eye==0&&!failed(cp))leftMirror=color;else color->release();
 #else
  color->release();
 #endif
  context_pop(ctx);
  if(failed(cp)){success=false;logg.msg("SCENE_STRETCH_FAILED",eye,(U32)cp);break;}
 }
 {nova_perf::Scope handTiming(nova_perf::HandEnd);hand.finish(success);}
 scopeCandidate=false;
 context_release(ctx);
#ifdef NOVA_LAB_UNIVERSAL
 // Retain our own reference to the native LEFT target. Reuse the last complete
 // pair on backpressure frames, so the window cannot alternate XR/stock hands.
 if(leftMirror){
  if(success){if(windowMirror)windowMirror->release();windowMirror=leftMirror;windowMirrorMs=GetTickCount64();windowMirrorPair=capture.pairId;windowMirrorSample=capture.sampleId;}
  else{windowMirrorMs=0;leftMirror->release();}
 }
#endif
 if(success){
  capture.valid=1;capture.projectionKind=croppedPair?3:2;capture.pairId=++serial;source_capture(slot)=capture;barrier();
  if(issue(slot,capture.pairId)){
   inc32(&bus->publishStereo);inc32(&bus->capturedPackets);inc32(&bus->projectionPairs);
   store32(&bus->worldEvidence,1);log_eye_packet(capture,slot);
  }
 }else{
  // A partial LEFT copy might still be executing. Quarantine; don't grant a new writer.
  inc32(&bus->graphicsErrors);logg.msg("PARTIAL_PAIR_SLOT_QUARANTINED",slot);
 }
}
// WORLD is an optional subsystem from the perspective of an always-live XR menu.
// No CreateNamedRenderTargetTextureEx call runs until the actual game render thread.
static bool ensure_world(){
 if(load32(&worldPhase)==2)return true;
 if(load32(&worldPhase)!=0||!load32(&gpuPhase))return false;
 if(!compare32(&worldPhase,0,1))return false;
 if(!make_eye_targets()){
  store32(&worldPhase,-1);logg.msg("NATIVE_WORLD_TARGETS_UNAVAILABLE_MENU_CONTINUES");return false;
 }
 if(!lens_material::make_target())logg.msg("O5_NATIVE_LENS_TARGET_UNAVAILABLE");
 pixel_budget::repairDeviceTable=repair_stateblock_device;
 if(!pixel_budget::install())logg.msg("F1_PIXEL_HOOKS_UNAVAILABLE");
 if(!repair_stateblock_device()){store32(&worldPhase,-1);native_game_control::stopGameRequested=true;return false;}
 if(!change_scope_calls(true)){change_scope_calls(false);store32(&worldPhase,-1);return false;}
 store32(&worldPhase,2);logg.msg("NATIVE_WORLD_TARGETS_READY");return true;
}
static bool ensure_gpu(){
 I32 phase=(I32)load32(&gpuPhase);
 if(phase==2)return GetCurrentThreadId()==graphicsThread;
 if(phase==-1||phase==1)return false;
 if(!compare32(&gpuPhase,0,1))return false;
 graphicsThread=GetCurrentThreadId();
#ifdef NOVA_LAB_UNIVERSAL
 // Shared D3D9 objects can otherwise outlive the engine's own FreeLibrary.
 // Keep the already loaded graphics module alive for this process lifetime.
 if(!graphicsModuleLease){graphicsModuleLease=LoadLibraryW(L"d3d9.dll");if(!graphicsModuleLease){store32(&gpuPhase,-1);return false;}logg.msg("D3D9_MODULE_LIFETIME_LEASE",1);}
#endif
 nova_perf::initialize(L"nova_perf_game.log",bus->logDir);
 nova_gpu_perf::initialize(gpu,bus->logDir);
 if(!open_shared()){
  logg.msg("GAME_GPU_INIT_FAIL",graphicsThread);
  inc32(&bus->graphicsErrors);store32(&bus->rendererState,-2);store32(&gpuPhase,-1);
  return false;
 }
 store32(&bus->gameReady,1);store32(&bus->rendererState,2);store32(&gpuPhase,2);
 logg.msg("GAME_GPU_INIT_NATIVE_THREAD",graphicsThread);
 const GUID on12={0xe7fda234,0xb589,0x4049,{0x94,0x0d,0x88,0x78,0x97,0x75,0x31,0xc8}};Com*backend=nullptr;
 HRESULT backendHr=gpu->qi(on12,(void**)&backend);
 logg.msg(backend?"GRAPHICS_API_D3D9_ON12":"GRAPHICS_API_NATIVE_D3D9",U32(backendHr));if(backend)backend->release();
 return true;
}
static void __fastcall render_hook(void*self,void*edx,const void*view,const void*hud,int clear,int flags){
 (void)edx;
 if(load32(&active)!=1||!bus||load32(&bus->stop)||!readable(view,0x14c)||!readable(hud,0x14c)||!compare32(&hookRendering,0,1)){legacy_render(self,view,hud,clear,flags);return;}
 scopeCandidate=false;
 inc32(&bus->sceneCalls);if(!ensure_gpu()){legacy_render(self,view,hud,clear,flags);store32(&hookRendering,0);return;}nova_perf::frame_boundary();native_game_control::poll(graphicsThread);
 {nova_perf::Scope rootTiming(nova_perf::FrameRoot);check_gpu();
 if(!ensure_world()){legacy_render(self,view,hud,clear,flags);store32(&hookRendering,0);return;}
 bool can=load32(&bus->gameReady)&&load32(&bus->resourceReady);
 Tracking t{};bool has=can&&read_tracking(bus,t)&&t.valid&&t.epoch!=0;
#ifdef NOVA_LAB_UNIVERSAL
 bool flat=native_game_control::flat_frame();
 if(flat)has=false;
#endif
 if(has)stereo(self,view,hud,clear,flags,t);else legacy_render(self,view,hud,clear,flags);
#ifdef NOVA_LAB_UNIVERSAL
 if(has&&t.focused&&windowMirror&&windowMirrorMs&&GetTickCount64()-windowMirrorMs<250){
  nova_gpu_perf::Scope gpuTiming(nova_gpu_perf::Mirror,windowMirrorPair);
  D9Surface*back=nullptr;HRESULT mr=gpu->back(&back);if(!failed(mr)&&back){mr=gpu->stretch(windowMirror,nullptr,back,nullptr);back->release();}else if(!back)mr=HRESULT(0x80004005u);
  static U32 mirrors=0;if(failed(mr)||++mirrors<=3||mirrors%120==0)logg.msg("NATIVE_XR_LEFT_MIRROR",windowMirrorPair,windowMirrorSample,U32(mr));
 }
#endif
 store32(&hookRendering,0);
 }nova_perf::report(); // World work can run on a different thread from Present.
#ifdef NOVA_LAB_UNIVERSAL
 // Commands may arrive through the world callback before Present. Retire at
 // this completed native root boundary as well, always on the GPU owner.
 if(native_game_control::stop_requested())stop_on_graphics_thread();
#endif
}
static void menu_capture(){nova_perf::Scope timing(nova_perf::MenuCopy);if(!bus||!gpu||load32(&bus->stop)||!load32(&bus->gameReady))return;
 if(nova_fps::on(nova_fps::AuxiliaryCopies)&&!native_game_control::flat_frame()){++nova_fps::skippedMenu;return;}
 if(!nova_frame_transport::reserve(source_image(4)))return;D9Surface*back=nullptr;HRESULT hr=gpu->back(&back);
 nova_gpu_perf::Scope gpuTiming(nova_gpu_perf::Menu,serial);
 if(failed(hr)||!back){nova_frame_transport::fail(source_image(4));return;}
 D9SurfaceDesc d{};if(failed(back->desc(&d))){back->release();nova_frame_transport::fail(source_image(4));return;}
#ifdef NOVA_LAB_UNIVERSAL
 native_game_control::screen_extent(d.width,d.height);
#endif
 // Render the ACTUAL live CS:GO monitor image into a native GPU texture; never synthesize a world.
 HRESULT fill=gpu->color(shareRT[6],0xff000000); // optional letterbox clear only; actual game pixels mandatory
 RECT area{0,0,(LONG)bus->width,(LONG)bus->height};
 if(d.width&&d.height){double a=(double)d.width/d.height; if(a>1){I32 h=(I32)(bus->height/a);area.top=(I32(bus->height)-h)/2;area.bottom=area.top+h;}else{I32 w=(I32)(bus->width*a);area.left=(I32(bus->width)-w)/2;area.right=area.left+w;}}
 if(failed(fill))logg.msg("LETTERBOX_CLEAR_UNAVAILABLE",(U32)fill);
 D9Surface*source=back;
 if(d.multisampleType){
  if(menuResolve&&(menuResolveW!=d.width||menuResolveH!=d.height||menuResolveFmt!=d.fmt)){
   menuResolve->release();menuResolve=nullptr;
  }
  if(!menuResolve){
   HRESULT mr=gpu->target(d.width,d.height,d.fmt,&menuResolve);
   if(failed(mr)||!menuResolve){logg.msg("MENU_MSAA_INTERMEDIATE_FAIL",(U32)mr);back->release();nova_frame_transport::fail(source_image(4));return;}
   menuResolveW=d.width;menuResolveH=d.height;menuResolveFmt=d.fmt;
  }
  HRESULT resolve=gpu->stretch(back,nullptr,menuResolve,nullptr);
  if(failed(resolve)){logg.msg("MENU_MSAA_RESOLVE_FAIL",(U32)resolve);back->release();nova_frame_transport::fail(source_image(4));return;}
  source=menuResolve;
 }
 hr=gpu->stretch(source,nullptr,shareRT[6],&area,1);back->release();
 if(failed(hr)){nova_frame_transport::fail(source_image(4));inc32(&bus->graphicsErrors);return;}
 if(issue(4,++serial))inc32(&bus->publishMenu);
}
static HRESULT WINCALL present_hook(D9Device*d,const RECT*a,const RECT*b,HWND w,const void*f){
 if(d==gpu&&load32(&active)==1&&compare32(&hookPresenting,0,1)){
#ifdef NOVA_LAB_UNIVERSAL
 if(bus&&load32(&bus->stop)){retire_owned_gpu();store32(&hookPresenting,0);return originalPresent(d,a,b,w,f);}
#endif
 if(!ensure_gpu()){store32(&hookPresenting,0);return originalPresent(d,a,b,w,f);}
 native_game_control::poll(graphicsThread);
#ifdef NOVA_LAB_UNIVERSAL
 if(native_game_control::stop_requested()&&stop_on_graphics_thread()){store32(&hookPresenting,0);return originalPresent(d,a,b,w,f);}
#endif
 if(!ensure_gpu()){store32(&hookPresenting,0);return originalPresent(d,a,b,w,f);}
 inc32(&bus->presentCalls);check_gpu();menu_capture();store32(&hookPresenting,0);
 }nova_perf::report();nova_perf::Scope timing(nova_perf::Present);HRESULT result=originalPresent(d,a,b,w,f);if(d==gpu&&load32(&active)==1&&nova_fps::on(nova_fps::SourcePostPresent))check_gpu();return result;
}
static HRESULT WINCALL presentex_hook(D9Device*d,const RECT*a,const RECT*b,HWND w,const void*f,U32 flags){
 if(d==gpu&&load32(&active)==1&&compare32(&hookPresenting,0,1)){
#ifdef NOVA_LAB_UNIVERSAL
 if(bus&&load32(&bus->stop)){retire_owned_gpu();store32(&hookPresenting,0);return originalPresentEx(d,a,b,w,f,flags);}
#endif
 if(!ensure_gpu()){store32(&hookPresenting,0);return originalPresentEx(d,a,b,w,f,flags);}
 native_game_control::poll(graphicsThread);
#ifdef NOVA_LAB_UNIVERSAL
 if(native_game_control::stop_requested()&&stop_on_graphics_thread()){store32(&hookPresenting,0);return originalPresentEx(d,a,b,w,f,flags);}
#endif
 if(!ensure_gpu()){store32(&hookPresenting,0);return originalPresentEx(d,a,b,w,f,flags);}
 inc32(&bus->presentCalls);check_gpu();menu_capture();store32(&hookPresenting,0);
 }nova_perf::report();nova_perf::Scope timing(nova_perf::Present);HRESULT result=originalPresentEx(d,a,b,w,f,flags);if(d==gpu&&load32(&active)==1&&nova_fps::on(nova_fps::SourcePostPresent))check_gpu();return result;
}
#ifdef NOVA_LAB_UNIVERSAL
static bool stop_on_graphics_thread(){
 if(load32(&hookRendering)||(graphicsThread&&GetCurrentThreadId()!=graphicsThread))return false;
 retire_owned_gpu();if(!load32(&gpuRetired))return false;
 native_menu::shutdown();muzzle_native::shutdown();store32(&active,0);
 if(load32(&installed)){
  if(load32(&worldHookInstalled))undo(renderSlot,(void*)render_hook,(void*)nativeRender);
  undo(presentExSlot,(void*)presentex_hook,(void*)originalPresentEx);
  undo(presentSlot,(void*)present_hook,(void*)originalPresent);
  store32(&installed,0);store32(&worldHookInstalled,0);
 }
 store32(&bus->stop,1);logg.msg("NATIVE_CONTROL_STOP_BEFORE_ENGINE_QUIT",GetCurrentThreadId());
 native_game_control::queue_quit();return true;
}
#endif
static bool install(){
 presentSlot=(void**)&gpu->v[17];presentExSlot=(void**)&gpu->v[121];
 originalPresent=(Present)*presentSlot;originalPresentEx=(PresentEx)*presentExSlot;
 if(!readable((void*)originalPresent,8)||!readable((void*)originalPresentEx,8))return false;
 if(!hook(presentSlot,(void*)originalPresent,(void*)present_hook))return false;
 if(!hook(presentExSlot,(void*)originalPresentEx,(void*)presentex_hook)){
  undo(presentSlot,(void*)present_hook,(void*)originalPresent);return false;
 }
 store32(&installed,1);store32(&active,1);
 logg.msg("PRESENT_HOOK_INDEPENDENT_OF_WORLD");
 const U8 prologue[]={0x55,0x8b,0xec,0x83,0xe4,0xc0};
 if(!bytes(client,0x3a5280,prologue,sizeof(prologue))||!bind_world()){
  logg.msg("WORLD_NOT_ADMITTED_PRESENT_REMAINS_ACTIVE");return true;
 }
 renderSlot=(void**)(client+0xc1abb4+24);nativeRender=(Render)(client+0x3a5280);
 if(!readable(renderSlot,4)||*renderSlot!=(void*)nativeRender){
  logg.msg("WORLD_VTABLE_DIFFERENT_PRESENT_REMAINS_ACTIVE");return true;
 }
 if(hook(renderSlot,(void*)nativeRender,(void*)render_hook)){
  store32(&worldHookInstalled,1);logg.msg("WORLD_HOOK_INSTALLED");
 }else logg.msg("WORLD_HOOK_NOT_INSTALLED_PRESENT_REMAINS_ACTIVE");
 return true;
}
static DWORD WINCALL worker(void*){
 U32 pid=GetCurrentProcessId();wchar_t name[64]{};mapping_name(name,pid);
 for(int n=0;n<600&&!bus;n++){map=OpenFileMappingW(0xf001f,0,name);if(map!=nullptr){bus=(Shared*)MapViewOfFile(map,0xf001f,0,0,sizeof(Shared));break;}Sleep(100);}
 if(!bus||bus->magic!=MAGIC||bus->version!=VERSION||bus->bytes!=sizeof(Shared)||bus->gamePid!=pid){logg.msg("IPC_BOOT_FAILED",pid);return 1;}
 muzzle_native::body_authority::bind_tracking(bus);
 logg.open(L"nova_game32.log",bus->logDir);logg.msg("R3_BUILD_NATIVE_LENS",VERSION,sizeof(Shared),sizeof(CapturedStereo));
 for(int n=0;n<1200&&(!client||!mat||!shaderDll||!engine);n++){
  client=(U8*)GetModuleHandleW(L"client.dll");mat=(U8*)GetModuleHandleW(L"materialsystem.dll");shaderDll=(U8*)GetModuleHandleW(L"shaderapidx9.dll");engine=(U8*)GetModuleHandleW(L"engine.dll");Sleep(100);
 }
 for(int n=0;n<600&&!load32(&bus->resourceReady)&&!load32(&bus->stop);n++)Sleep(100);
 bool deviceReady=false;
 for(int n=0;n<600&&!deviceReady&&!load32(&bus->stop);n++){deviceReady=bind_device();if(!deviceReady)Sleep(100);}
 nova_fps::initialize(pid,bus->hostPid);
 if(!frameExtra.connect(pid,bus->hostPid)){logg.msg("F6_TRANSPORT_CONNECT_FAILED");inc32(&bus->graphicsErrors);store32(&bus->stop,1);return 2;}
 native_game_control::initialize(client,engine,bus->hostPid);
 if(!client||!mat||!shaderDll||!engine||!load32(&bus->resourceReady)||!deviceReady||!install()){
  inc32(&bus->graphicsErrors);store32(&bus->rendererState,-1);logg.msg("NATIVE_SOURCE_ADMISSION_FAILED");return 2;
 }
 store32(&bus->rendererState,1);logg.msg("NATIVE_MENU_PRESENT_READY_NO_COM_ON_WORKER",pid,load32(&worldHookInstalled));
 while(!load32(&bus->stop)){Sleep(10);}
#ifdef NOVA_LAB_UNIVERSAL
 for(int n=0;n<400&&!load32(&gpuRetired);n++)Sleep(5);
 if(!load32(&gpuRetired))logg.msg("GPU_RETIRE_NOT_OBSERVED_MODULE_LEASE_RETAINED");
#endif
 store32(&active,0);
#ifdef NOVA_LAB_UNIVERSAL
 // Let the complete two-eye root transaction restore before retiring hooks.
 for(int n=0;n<200&&(load32(&hookRendering)||load32(&hookPresenting));n++)Sleep(5);
 if(load32(&hookRendering)||load32(&hookPresenting)){logg.msg("NATIVE_SHUTDOWN_DEFERRED_ACTIVE_CALLBACK");return 3;}
#endif
#ifdef NOVA_LAB_UNIVERSAL
 native_menu::shutdown();if(load32(&installed))muzzle_native::shutdown();
#else
 muzzle_native::shutdown();
#endif
 // Non-forced shutdown: restore only still-owned vtables, preserve code memory and game files.
 if(load32(&installed)){
  if(load32(&worldHookInstalled))undo(renderSlot,(void*)render_hook,(void*)nativeRender);
  undo(presentExSlot,(void*)presentex_hook,(void*)originalPresentEx);
  undo(presentSlot,(void*)present_hook,(void*)originalPresent);
 }
 frameExtra.close();nova_fps::close();
 logg.msg("NATIVE_SHUTDOWN",bus->sceneCalls,bus->presentCalls,bus->stereoPixels);
 return 0;
}
}
NOVA_EXPORT BOOL WINCALL DllMain(HMODULE h,U32 event,void*){
 if(event==1){DisableThreadLibraryCalls(h);HANDLE t=CreateThread(nullptr,0,worker,nullptr,0,nullptr);if(t)CloseHandle(t);}
 return 1;
}
