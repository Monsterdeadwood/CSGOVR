extern "C" {int _fltused = 0;}
#include "platform.hpp"
#include "gfx.hpp"
#include "d3d11.h"
#include "wire.hpp"
#include "frame_transport.hpp"
#include "math.hpp"
#include "view_region.hpp"
#include "perf.hpp"
#include "controller_input.hpp"
#include "swapchain_format.hpp"
#include "fps_channel.hpp"
#include "xr_content_cache.hpp"
#define XR_NO_PROTOTYPES
#define XR_USE_GRAPHICS_API_D3D11
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"
using namespace nova;
namespace {
static Log logg;static Shared*bus=nullptr;
static nova_frame_transport::Endpoint frameExtra;
static Image& source_image(U32 slot){return slot<3?bus->images[slot]:(slot==3?frameExtra.page->image:bus->images[3]);}
static CapturedStereo& source_capture(U32 slot){return slot<3?bus->captured[slot]:frameExtra.page->captured;}
static U32 source_texture(U32 slot){return slot<3?slot*2:8;}
static HANDLE map=nullptr,game=nullptr;
[[maybe_unused]] static HMODULE loader=nullptr;static DXGIFactory*factory=nullptr;static DXGIAdapter*adapter=nullptr;
static D3DDevice*gpu=nullptr;static D3DContext*context=nullptr;
static D3DTexture*sharing[10]{},*worldBank[2][2]{},*menuResident=nullptr,*xrImages[5][12]{},*markerTextures[2]{};
static bool markersEnabled=false;static U32 swapchainCount=3;
#ifdef NOVA_LAB_UNIVERSAL
static D3DTexture*loadingTexture=nullptr;
#endif
static CapturedStereo worldCapture[2]{},pendingCapture{};
static ViewHistory viewHistory;
static ViewRegion worldRegion[2][2]{},pendingRegion[2]{};
static U64 regionExact=0,regionFallback=0,copyStarted=0,pendingPolls=0;
static void log_camera_map(const CapturedStereo&c,const ViewRegion v[2]){
 U64 n=regionExact+regionFallback;if(n>3&&n%120!=0)return;
 for(int e=0;e<2;e++){
  Vec3 up=captured_room_up(c,e);char line[1400];
  int bytes=_snprintf(line,sizeof(line),
   "NOVA_R4_CAMERA frame=%llu sample=%llu epoch=%llu eye=%d mode=%s rect=(%d,%d,%d,%d) nativeFov=(%.7f,%.7f,%.7f,%.7f) submittedFov=(%.7f,%.7f,%.7f,%.7f) roomUp=(%.6f,%.6f,%.6f) rawPos=(%.6f,%.6f,%.6f) rawQuat=(%.7f,%.7f,%.7f,%.7f) sourcePos=(%.5f,%.5f,%.5f) sourceAngles=(%.5f,%.5f,%.5f) nativeP=(%.7f,%.7f,%.7f,%.7f) ageMs=%llu\r\n",
   c.pairId,c.sampleId,c.trackingEpoch,e,v[e].exactSample?"EXACT_SAMPLE_REGION":"NATIVE_FULL_FALLBACK",
   v[e].x,v[e].y,v[e].width,v[e].height,
   double(c.eye[e].fov.l),double(c.eye[e].fov.r),double(c.eye[e].fov.u),double(c.eye[e].fov.d),
   double(v[e].fov.l),double(v[e].fov.r),double(v[e].fov.u),double(v[e].fov.d),
   double(up.x),double(up.y),double(up.z),
   double(c.eye[e].pos.x),double(c.eye[e].pos.y),double(c.eye[e].pos.z),
   double(c.eye[e].rot.x),double(c.eye[e].rot.y),double(c.eye[e].rot.z),double(c.eye[e].rot.w),
   double(c.sourceEye[e].x),double(c.sourceEye[e].y),double(c.sourceEye[e].z),
   double(c.sourceAngles[e][0]),double(c.sourceAngles[e][1]),double(c.sourceAngles[e][2]),
   double(c.projection[e][0]),double(c.projection[e][5]),double(c.projection[e][2]),double(c.projection[e][6]),
   GetTickCount64()-c.capturedMs);
  if(bytes>0&&logg.file!=BAD_HANDLE){U32 written=0;WriteFile(logg.file,line,U32(bytes<1399?bytes:1399),&written,nullptr);}
 }
}
static int activeWorldBank=0,pendingBank=-1;
static U64 lastConsumedFrame=0,trackingSample=0,submitSerial=0,menuGeneration=0;
static nova_fps_channel::Endpoint fpsChannel;
static xr_content_cache::Released worldReleased,quadReleased;
static U32 fpsFeatures=0;
static XrTime referenceChangeTime=0;
static U32 xrImageCount[5]{};
static D3DQuery*pendingQuery=nullptr;static I32 pendingSlot=-1,pendingKind=0,goodKind=0;
static U64 preWaitPolls{},preWaitPromotions{},postWaitPromotions{};
static U64 pendingBegin=0,lastWorld=0,lastMenu=0,lastSeen=0;
static bool haveWorld=false,haveMenu=false;
static XrInstance instance=XR_NULL_HANDLE;static XrSystemId systemID=XR_NULL_SYSTEM_ID;
static XrSession session=XR_NULL_HANDLE;static XrSpace local=XR_NULL_HANDLE,viewSpace=XR_NULL_HANDLE;
static XrSwapchain swapchains[5]{};static XrSessionState sessionState=XR_SESSION_STATE_UNKNOWN;
static I64 swapchainFormat=87;
static bool running=false,exiting=false;static U64 trackingEpoch=1;
static XrActionSet handSet=XR_NULL_HANDLE;static XrAction handAction=XR_NULL_HANDLE;static XrSpace handSpaces[2]={XR_NULL_HANDLE,XR_NULL_HANDLE};static XrPath handPaths[2]{};static bool handInput=false;
static XrAction aimAction=XR_NULL_HANDLE;static XrSpace aimSpaces[2]={XR_NULL_HANDLE,XR_NULL_HANDLE};
#ifdef NOVA_LAB_UNIVERSAL
static XrAction triggerAction=XR_NULL_HANDLE;
static HANDLE buttonsMap{};static controller_input::Channel*buttons{};
enum ExtraInput {GripValue,StickValue,StickClick,ButtonA,ButtonB,ButtonX,ButtonY,ButtonMenu,ExtraCount};
static XrAction extraActions[ExtraCount]{},hapticAction=XR_NULL_HANDLE;
static U64 hapticSerial{},lastShotHaptic{};static bool hapticWasActive{};static float hapticGain=1.f;
struct HapticTail {U64 due{};float amplitude{};U32 durationMs{};};static HapticTail hapticTails[2]{};
#endif
#define XR_ENTRIES(X) \
 X(xrCreateInstance) X(xrDestroyInstance) X(xrGetSystem) X(xrEnumerateInstanceExtensionProperties) \
 X(xrGetInstanceProperties) X(xrGetD3D11GraphicsRequirementsKHR) X(xrCreateSession) X(xrDestroySession) \
 X(xrCreateReferenceSpace) X(xrDestroySpace) X(xrEnumerateSwapchainFormats) \
 X(xrCreateSwapchain) X(xrDestroySwapchain) X(xrEnumerateSwapchainImages) \
 X(xrPollEvent) X(xrBeginSession) X(xrEndSession) X(xrWaitFrame) X(xrBeginFrame) X(xrEndFrame) \
 X(xrLocateViews) X(xrAcquireSwapchainImage) X(xrWaitSwapchainImage) X(xrReleaseSwapchainImage)
#define DECL(n) static PFN_##n n=nullptr;
XR_ENTRIES(DECL)
#undef DECL
#define XR_INPUT_ENTRIES(X) \
 X(xrCreateActionSet) X(xrDestroyActionSet) X(xrCreateAction) X(xrDestroyAction) \
 X(xrStringToPath) X(xrSuggestInteractionProfileBindings) X(xrAttachSessionActionSets) \
 X(xrCreateActionSpace) X(xrSyncActions) X(xrGetActionStatePose) X(xrLocateSpace) X(xrGetActionStateFloat) \
 X(xrGetActionStateBoolean) X(xrGetActionStateVector2f) X(xrApplyHapticFeedback) X(xrStopHapticFeedback)
#define DECL_OPTIONAL(n) static PFN_##n n=nullptr;
XR_INPUT_ENTRIES(DECL_OPTIONAL)
#undef DECL_OPTIONAL
static PFN_xrGetInstanceProcAddr getXR=nullptr;
#ifdef NOVA_LAB_SIM
#include "lab_runtime.hpp"
#endif
#include "calibration_io.hpp"
static const U32 D3D11_SDK_VERSION_=7;
#ifdef NOVA_LAB_UNIVERSAL
#include "image_quality.hpp"
static image_quality::Pass imageQuality;
#endif
static U32 envpid(){wchar_t pid[32]={};U32 size=GetEnvironmentVariableW(L"NOVA_PID",pid,32);if(!size||size>=31)return 0;U32 n=0;for(int i=0;pid[i];++i){if(pid[i]<L'0'||pid[i]>L'9')return 0;n=n*10+(U32)(pid[i]-L'0');}return n;}
static bool xrpass(XrResult r,const char*where){if(XR_SUCCEEDED(r))return true;logg.msg(where,(U64)(U32)r);inc32(&bus->xrErrors);return false;}
static bool running_game(){return game&&WaitForSingleObject(game,0)==0x102;}
static bool dynamic_openxr(){
#ifdef NOVA_LAB_SIM
 if(!lab_load_backend())return false;
#else
 loader=LoadLibraryW(L"openxr_loader.dll");if(!loader)return false;
 getXR=(PFN_xrGetInstanceProcAddr)GetProcAddress(loader,"xrGetInstanceProcAddr");if(!getXR)return false;
#endif
 if(XR_FAILED(getXR(XR_NULL_HANDLE,"xrCreateInstance",(PFN_xrVoidFunction*)&xrCreateInstance))||!xrCreateInstance)return false;
 if(XR_FAILED(getXR(XR_NULL_HANDLE,"xrEnumerateInstanceExtensionProperties",(PFN_xrVoidFunction*)&xrEnumerateInstanceExtensionProperties))||!xrEnumerateInstanceExtensionProperties)return false;
 const char*ext[]={"XR_KHR_D3D11_enable"};
 XrInstanceCreateInfo info{XR_TYPE_INSTANCE_CREATE_INFO};const char*name="CSGOVR NOVA Camera R4";
 for(int i=0;name[i]&&i<XR_MAX_APPLICATION_NAME_SIZE-1;i++)info.applicationInfo.applicationName[i]=name[i];
 info.applicationInfo.apiVersion=XR_MAKE_VERSION(1,0,0);info.enabledExtensionCount=1;info.enabledExtensionNames=ext;
 if(!xrpass(xrCreateInstance(&info,&instance),"INSTANCE_FAIL"))return false;
 #define LOAD(n) if(XR_FAILED(getXR(instance,#n,(PFN_xrVoidFunction*)&n))||!n){logg.msg("XR_SYMBOL_MISSING");return false;}
 XR_ENTRIES(LOAD)
 #undef LOAD
 XrInstanceProperties properties{XR_TYPE_INSTANCE_PROPERTIES};
 if(XR_SUCCEEDED(xrGetInstanceProperties(instance,&properties))){
  char line[512];int size=_snprintf(line,sizeof(line),"NOVA_OPENXR_RUNTIME name=%s version=%llu\r\n",properties.runtimeName,U64(properties.runtimeVersion));
  if(size>0&&logg.file!=BAD_HANDLE){U32 written=0;WriteFile(logg.file,line,U32(size<511?size:511),&written,nullptr);}
 }
 #define OPTIONAL(n) (void)getXR(instance,#n,(PFN_xrVoidFunction*)&n);
 XR_INPUT_ENTRIES(OPTIONAL)
 #undef OPTIONAL
 XrSystemGetInfo si{XR_TYPE_SYSTEM_GET_INFO};si.formFactor=XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
 while(running_game()&&!load32(&bus->stop)){
  XrResult r=xrGetSystem(instance,&si,&systemID);if(XR_SUCCEEDED(r))return true;
  if(r!=XR_ERROR_FORM_FACTOR_UNAVAILABLE){xrpass(r,"GET_SYSTEM_FAIL");return false;}
  Sleep(700);
 }
 return false;
}
static bool make_d3d11(){
 XrGraphicsRequirementsD3D11KHR req{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};
 if(!xrpass(xrGetD3D11GraphicsRequirementsKHR(instance,systemID,&req),"GPU_REQUIRES_FAIL"))return false;
 if(failed(CreateDXGIFactory1(IID_DXGI_FACTORY1,(void**)&factory)))return false;
 for(U32 i=0;i<16;i++){
  DXGIAdapter*a=nullptr;HRESULT r=factory->at(i,&a);if(failed(r)||!a)break;
  DXGI_ADAPTER_DESC1 d{};r=a->desc(&d);
  if(ok(r)&&d.AdapterLuid.LowPart==req.adapterLuid.LowPart&&d.AdapterLuid.HighPart==req.adapterLuid.HighPart){adapter=a;break;}a->release();
 }
 if(!adapter){logg.msg("XR_GPU_LUID_UNAVAILABLE");return false;}
 U32 levels[]={0xb100,0xb000,0xa100,0xa000};U32 selected=0;
 HRESULT hr=D3D11CreateDevice(adapter,0,nullptr,0x20,levels,4,D3D11_SDK_VERSION_,&gpu,&selected,&context);
 if(failed(hr)||!gpu||!context||selected<(U32)req.minFeatureLevel){logg.msg("D3D11_DEVICE_FAIL",(U32)hr,selected,(U32)req.minFeatureLevel);return false;}
 logg.msg("XR_GPU_MATCH",req.adapterLuid.LowPart,(U32)req.adapterLuid.HighPart,selected);
 return true;
}
static bool graphics_resources(){
 // Producer owns only D3D9 views. Host owns shareable GPU allocations and compositor textures.
 D3DTextureDesc d{};d.width=bus->width;d.height=bus->height;d.mips=1;d.array=1;d.format=87;d.scount=1;d.usage=0;d.bind=0x28;d.cpu=0;d.misc=2;
 U64 handles[10]{};
 for(int i=0;i<10;i++){
  HRESULT hr=gpu->create(&d,&sharing[i]);if(failed(hr)||!sharing[i]){logg.msg("SHARE_CREATE_FAIL",i,(U32)hr);return false;}
  DXGIResource*res=nullptr;hr=sharing[i]->qi(IID_DXGI_RESOURCE,(void**)&res);if(failed(hr)||!res)return false;
  HANDLE handle=nullptr;hr=res->handle(&handle);res->release();if(failed(hr)||!handle)return false;
  handles[i]=(U64)(uintptr_t)handle;
  if(!handles[i]||handles[i]>0xffffffffULL)return false;
  for(int j=0;j<i;j++)if(handles[j]==handles[i])return false;
 }
 d.misc=0;
 for(int b=0;b<2;b++)for(int e=0;e<2;e++)if(failed(gpu->create(&d,&worldBank[b][e]))||!worldBank[b][e])return false;
 if(failed(gpu->create(&d,&menuResident))||!menuResident)return false;
#ifdef NOVA_LAB_UNIVERSAL
 // A native loading panel covers the interval before the first game image,
 // and map transitions where neither a fresh WORLD nor menu image exists.
 U64 pixelBytes=U64(d.width)*d.height*4;U32*pixels=(U32*)VirtualAlloc(nullptr,SIZE_T(pixelBytes),0x3000,4);
 if(!pixels)return false;
 for(U64 i=0;i<pixelBytes/4;i++)pixels[i]=0xff101820u;
 const U8 letters[7][7]={{16,16,16,16,16,16,31},{14,17,17,17,17,17,14},{14,17,17,31,17,17,17},{30,17,17,17,17,17,30},{31,4,4,4,4,4,31},{17,25,21,19,17,17,17},{14,17,16,23,17,17,15}};
 U32 scale=d.width/80;if(scale<1)scale=1;U32 left=(d.width-41*scale)/2,top=(d.height-7*scale)/2;
 for(U32 c=0;c<7;c++)for(U32 y=0;y<7;y++)for(U32 x=0;x<5;x++)if(letters[c][y]&(16u>>x))
  for(U32 yy=0;yy<scale;yy++)for(U32 xx=0;xx<scale;xx++)pixels[(top+y*scale+yy)*d.width+left+(c*6+x)*scale+xx]=0xffdce6e9u;
 D3DInitialData loadingData{pixels,d.width*4,0};HRESULT loadingResult=gpu->create_data(&d,&loadingData,&loadingTexture);
 VirtualFree(pixels,0,0x8000);if(failed(loadingResult)||!loadingTexture)return false;
#endif
 if(markersEnabled){
  D3DTextureDesc marker=d;marker.width=marker.height=16;marker.bind=8;
  for(U32 k=0;k<2;k++){U32 pixels[256];for(auto&p:pixels)p=k?0xff00ff00u:0xffff4040u;D3DInitialData initial{pixels,16*4,0};
   if(failed(gpu->create_data(&marker,&initial,&markerTextures[k]))||!markerTextures[k])return false;
  }
 }
 // Destination is available as soon as the own D3D11 device and shared allocations exist.
 for(int i=0;i<8;i++)bus->sharedHandles[i]=handles[i];frameExtra.page->handles[0]=handles[8];frameExtra.page->handles[1]=handles[9];barrier();store32(&bus->resourceReady,1);
 logg.msg("HOST_TEXTURES_READY",10,bus->width,bus->height);
 return true;
}
#ifdef NOVA_LAB_UNIVERSAL
static XrAction create_control_action(const char*name,XrActionType type){
 XrActionCreateInfo info{XR_TYPE_ACTION_CREATE_INFO};info.actionType=type;info.countSubactionPaths=2;info.subactionPaths=handPaths;
 for(U32 i=0;name[i]&&i<XR_MAX_ACTION_NAME_SIZE-1;i++){info.actionName[i]=name[i];info.localizedActionName[i]=name[i];}
 XrAction action=XR_NULL_HANDLE;XrResult result=xrCreateAction(handSet,&info,&action);if(XR_FAILED(result))logg.msg("XR_CONTROL_ACTION_FAILED",U32(result),type);return action;
}
#endif
static void setup_optional_hands(){
 // Tracking producer is separately owned from head, render, native weapon and ballistics.
 // Controller poses are captured for identity/lease evidence; no invented game skeleton hook.
 if(!xrCreateActionSet||!xrCreateAction||!xrStringToPath||!xrCreateActionSpace||
    !xrSuggestInteractionProfileBindings||!xrAttachSessionActionSets||!xrSyncActions||!xrGetActionStatePose||!xrLocateSpace){
  logg.msg("HAND_ACTIONS_UNSUPPORTED_RENDER_CONTINUES");return;
 }
 const char*names[2]={"/user/hand/left","/user/hand/right"};
 for(int i=0;i<2;i++)if(XR_FAILED(xrStringToPath(instance,names[i],&handPaths[i])))return;
 XrActionSetCreateInfo set{XR_TYPE_ACTION_SET_CREATE_INFO};
 const char*setName="nova_input";for(int i=0;setName[i];i++){set.actionSetName[i]=setName[i];set.localizedActionSetName[i]=setName[i];}
 if(XR_FAILED(xrCreateActionSet(instance,&set,&handSet)))return;
 XrActionCreateInfo ca{XR_TYPE_ACTION_CREATE_INFO};ca.actionType=XR_ACTION_TYPE_POSE_INPUT;
 const char*action="hand_grip";for(int i=0;action[i];i++){ca.actionName[i]=action[i];ca.localizedActionName[i]=action[i];}
 ca.countSubactionPaths=2;ca.subactionPaths=handPaths;
 if(XR_FAILED(xrCreateAction(handSet,&ca,&handAction)))return;
 memset(ca.actionName,0,sizeof(ca.actionName));memset(ca.localizedActionName,0,sizeof(ca.localizedActionName));
 const char*aimName="weapon_aim";for(int i=0;aimName[i];i++){ca.actionName[i]=aimName[i];ca.localizedActionName[i]=aimName[i];}
 if(XR_FAILED(xrCreateAction(handSet,&ca,&aimAction)))return;
#ifdef NOVA_LAB_UNIVERSAL
 if(xrGetActionStateFloat){
  memset(ca.actionName,0,sizeof(ca.actionName));memset(ca.localizedActionName,0,sizeof(ca.localizedActionName));
  const char*triggerName="fire_trigger";for(U32 i=0;triggerName[i];i++){ca.actionName[i]=triggerName[i];ca.localizedActionName[i]=triggerName[i];}
  ca.actionType=XR_ACTION_TYPE_FLOAT_INPUT;
  if(XR_FAILED(xrCreateAction(handSet,&ca,&triggerAction)))triggerAction=XR_NULL_HANDLE;
 }
 if(xrGetActionStateFloat)extraActions[GripValue]=create_control_action("grip_value",XR_ACTION_TYPE_FLOAT_INPUT);
 if(xrGetActionStateVector2f)extraActions[StickValue]=create_control_action("move_turn_stick",XR_ACTION_TYPE_VECTOR2F_INPUT);
 if(xrGetActionStateBoolean){
  const char*names[]={"stick_click","button_a","button_b","button_x","button_y","application_menu"};
  for(U32 i=0;i<6;i++)extraActions[StickClick+i]=create_control_action(names[i],XR_ACTION_TYPE_BOOLEAN_INPUT);
 }
 if(xrApplyHapticFeedback&&xrStopHapticFeedback)hapticAction=create_control_action("weapon_haptics",XR_ACTION_TYPE_VIBRATION_OUTPUT);
#endif
 const char*profiles[]={"/interaction_profiles/oculus/touch_controller","/interaction_profiles/meta/touch_controller_plus"};
 const char*paths[]={"/user/hand/left/input/grip/pose","/user/hand/right/input/grip/pose","/user/hand/left/input/aim/pose","/user/hand/right/input/aim/pose"};
 XrPath bindingPaths[4]{};
 for(int i=0;i<4;i++)if(XR_FAILED(xrStringToPath(instance,paths[i],&bindingPaths[i])))return;
 XrActionSuggestedBinding binds[32]{{handAction,bindingPaths[0]},{handAction,bindingPaths[1]},{aimAction,bindingPaths[2]},{aimAction,bindingPaths[3]}};U32 bindingCount=4;
#ifdef NOVA_LAB_UNIVERSAL
 if(triggerAction!=XR_NULL_HANDLE){
  const char*triggerPaths[]={"/user/hand/left/input/trigger/value","/user/hand/right/input/trigger/value"};
  for(U32 i=0;i<2;i++){XrPath path=XR_NULL_PATH;if(XR_FAILED(xrStringToPath(instance,triggerPaths[i],&path)))return;binds[bindingCount++]={triggerAction,path};}
 }
 struct Binding {XrAction action;const char*path;};
 const Binding extraBindings[]={
  {extraActions[GripValue],"/user/hand/left/input/squeeze/value"},{extraActions[GripValue],"/user/hand/right/input/squeeze/value"},
  {extraActions[StickValue],"/user/hand/left/input/thumbstick"},{extraActions[StickValue],"/user/hand/right/input/thumbstick"},
  {extraActions[StickClick],"/user/hand/left/input/thumbstick/click"},{extraActions[StickClick],"/user/hand/right/input/thumbstick/click"},
  {extraActions[ButtonA],"/user/hand/right/input/a/click"},{extraActions[ButtonB],"/user/hand/right/input/b/click"},
  {extraActions[ButtonX],"/user/hand/left/input/x/click"},{extraActions[ButtonY],"/user/hand/left/input/y/click"},
  {extraActions[ButtonMenu],"/user/hand/left/input/menu/click"},{hapticAction,"/user/hand/left/output/haptic"},{hapticAction,"/user/hand/right/output/haptic"}};
 for(const auto&binding:extraBindings)if(binding.action!=XR_NULL_HANDLE){XrPath path=XR_NULL_PATH;if(XR_SUCCEEDED(xrStringToPath(instance,binding.path,&path)))binds[bindingCount++]={binding.action,path};}
#endif
 for(int i=0;i<2;i++){XrPath profile=XR_NULL_PATH;
  if(XR_SUCCEEDED(xrStringToPath(instance,profiles[i],&profile))){
   XrInteractionProfileSuggestedBinding sug{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};sug.interactionProfile=profile;sug.suggestedBindings=binds;sug.countSuggestedBindings=bindingCount;
   XrResult bindingResult=xrSuggestInteractionProfileBindings(instance,&sug);logg.msg("XR_CONTROLLER_BINDINGS",i,bindingCount,U32(bindingResult));
  }
 }
 XrSessionActionSetsAttachInfo attach{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};attach.countActionSets=1;attach.actionSets=&handSet;
 if(XR_FAILED(xrAttachSessionActionSets(session,&attach)))return;
 for(int i=0;i<2;i++){XrActionSpaceCreateInfo space{XR_TYPE_ACTION_SPACE_CREATE_INFO};space.action=handAction;space.subactionPath=handPaths[i];space.poseInActionSpace.orientation.w=1;
  if(XR_FAILED(xrCreateActionSpace(session,&space,&handSpaces[i])))return;
  space.action=aimAction;if(XR_FAILED(xrCreateActionSpace(session,&space,&aimSpaces[i])))return;
 }
 handInput=true;logg.msg("XR_NATIVE_HAND_INPUT_READY");logg.msg("LAB_GRIP_POINT_AIM_ORIENTATION_READY");
#ifdef NOVA_LAB_UNIVERSAL
 if(buttons){buttons->features=controller_input::Pose|(triggerAction?controller_input::TriggerInput:0)|(extraActions[ButtonA]&&extraActions[ButtonMenu]?controller_input::Buttons:0)|(extraActions[StickValue]?controller_input::Sticks:0)|(hapticAction?controller_input::Haptics:0);logg.msg("XR_CONTROLLER_FEATURES",buttons->features,sizeof(controller_input::Channel));}
#endif
}
static bool make_session(){
 XrGraphicsBindingD3D11KHR binding{XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};binding.device=(ID3D11Device*)gpu;
 XrSessionCreateInfo sc{XR_TYPE_SESSION_CREATE_INFO};sc.next=&binding;sc.systemId=systemID;
 if(!xrpass(xrCreateSession(instance,&sc,&session),"SESSION_CREATE_FAIL"))return false;
 XrReferenceSpaceCreateInfo rc{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};rc.poseInReferenceSpace.orientation.w=1;rc.referenceSpaceType=XR_REFERENCE_SPACE_TYPE_LOCAL;
 if(!xrpass(xrCreateReferenceSpace(session,&rc,&local),"LOCAL_CREATE_FAIL"))return false;
 rc.referenceSpaceType=XR_REFERENCE_SPACE_TYPE_VIEW;
 if(!xrpass(xrCreateReferenceSpace(session,&rc,&viewSpace),"VIEW_CREATE_FAIL"))return false;
 setup_optional_hands();
 U32 n=0;if(!xrpass(xrEnumerateSwapchainFormats(session,0,&n,nullptr),"FMT_COUNT_FAIL"))return false;
 if(!n||n>4096){logg.msg("XR_FORMAT_COUNT_INVALID",n);return false;}
 I64*fmts=(I64*)VirtualAlloc(nullptr,SIZE_T(n)*sizeof(I64),0x3000,4);if(!fmts)return false;
 U32 capacity=n;bool enumerated=xrpass(xrEnumerateSwapchainFormats(session,capacity,&n,fmts),"FMT_ENUM_FAIL");
 if(enumerated&&n<=capacity){
  for(U32 i=0;i<n;i++)logg.msg("XR_OFFERED_SWAPCHAIN_FORMAT",i,U64(fmts[i]));
  swapchainFormat=choose_swapchain_format(fmts,n);
#ifdef NOVA_LAB_SIM
  // Simulator-only experiment: select one actually offered format, allowing
  // the complete game pipeline to exercise the non-BGRA SteamVR code path.
  wchar_t requested[12]{};
  if(GetEnvironmentVariableW(L"NOVA_SIM_COLOR_FORMAT",requested,12)>0){
   U32 value=0;for(U32 k=0;requested[k]>=L'0'&&requested[k]<=L'9';k++)value=value*10+U32(requested[k]-L'0');
   bool offered=false;for(U32 k=0;k<n;k++)if(fmts[k]==I64(value))offered=true;
   I64 selected=I64(value);swapchainFormat=offered?choose_swapchain_format(&selected,1):0;
   logg.msg("SIM_RUNTIME_FORMAT_RESTRICTION",value,offered);
  }
#endif
 }else swapchainFormat=0;
 VirtualFree(fmts,0,0x8000);
 if(!swapchainFormat){logg.msg("XR_NO_SUPPORTED_COLOR_FORMAT",n);return false;}
#ifndef NOVA_LAB_UNIVERSAL
 if(!native_bgra_copy_compatible(U32(swapchainFormat))){logg.msg("XR_COLOR_CONVERSION_REQUIRES_UNIVERSAL_HOST");return false;}
#endif
 logg.msg("XR_SELECTED_SWAPCHAIN_FORMAT",U64(swapchainFormat),native_bgra_copy_compatible(U32(swapchainFormat))?0:1);
 for(U32 s=0;s<swapchainCount;s++){
  XrSwapchainCreateInfo si{XR_TYPE_SWAPCHAIN_CREATE_INFO};si.usageFlags=XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT|XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT;
  si.format=swapchainFormat;si.sampleCount=1;si.width=bus->width;si.height=bus->height;si.faceCount=1;si.arraySize=1;si.mipCount=1;
  if(s>=3)si.width=si.height=16;
  if(!xrpass(xrCreateSwapchain(session,&si,&swapchains[s]),"SWAP_CREATE_FAIL"))return false;
  if(!xrpass(xrEnumerateSwapchainImages(swapchains[s],0,&xrImageCount[s],nullptr),"SWAP_SIZE_FAIL"))return false;
  if(xrImageCount[s]<1||xrImageCount[s]>12)return false;
  XrSwapchainImageD3D11KHR entries[12]{};
  for(U32 i=0;i<xrImageCount[s];i++)entries[i].type=XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR;
  if(!xrpass(xrEnumerateSwapchainImages(swapchains[s],xrImageCount[s],&xrImageCount[s],(XrSwapchainImageBaseHeader*)entries),"SWAP_IMAGES_FAIL"))return false;
  for(U32 i=0;i<xrImageCount[s];i++)xrImages[s][i]=(D3DTexture*)entries[i].texture;
 }
 logg.msg("XR_SWAPCHAINS_READY",swapchainCount);
 return true;
}
static bool acquire_and_copy(int index,D3DTexture*from){
 nova_perf::Scope timing(nova_perf::SwapCopy);
 U32 image=0;XrSwapchainImageAcquireInfo ai{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
 if(!xrpass(xrAcquireSwapchainImage(swapchains[index],&ai,&image),"ACQUIRE_FAIL"))return false;
 XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};wi.timeout=100000000;
 bool waitok=false;
 for(int n=0;n<25&&running_game();n++){XrResult result=xrWaitSwapchainImage(swapchains[index],&wi);if(result==XR_SUCCESS){waitok=true;break;}if(result!=XR_TIMEOUT_EXPIRED){xrpass(result,"WAIT_IMAGE_FAIL");break;}}
 if(!waitok){
  // A timed-out wait does not grant a write/release lease. End this run; session
  // destruction retires the outstanding acquisition, rather than copying into it.
  logg.msg("SWAP_WAIT_NOT_COMPLETED_NO_COPY_NO_RELEASE",index);store32(&bus->stop,1);return false;
 }
 bool copied=image<xrImageCount[index]&&xrImages[index][image]&&from;
 if(copied){
#ifdef NOVA_LAB_UNIVERSAL
  bool drawn=index<2&&imageQuality.draw(from,xrImages[index][image],true,U32(swapchainFormat));
  if(!drawn){
   if(native_bgra_copy_compatible(U32(swapchainFormat)))context->copy(xrImages[index][image],from);
   else if(!imageQuality.draw(from,xrImages[index][image],false,U32(swapchainFormat))){
    logg.msg("XR_FORMAT_CONVERSION_FAILED",U64(swapchainFormat),index,image);inc32(&bus->graphicsErrors);copied=false;
   }
  }
#else
  context->copy(xrImages[index][image],from);
#endif
  context->flush();}
 XrSwapchainImageReleaseInfo ri{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
 bool released=xrpass(xrReleaseSwapchainImage(swapchains[index],&ri),"SWAP_RELEASE_FAIL");
 return copied&&released;
}
static void reject_ready(int slot){if(nova_frame_transport::acquire(source_image(slot))){nova_frame_transport::release(source_image(slot));++frameExtra.page->superseded;}}
static bool consume_ready(bool beforeWait=false){nova_perf::Scope timing(nova_perf::Consume);if(!bus||!gpu)return false;if(beforeWait)++preWaitPolls;
 bool completed=false;
 if(pendingSlot>=0){
  HRESULT result=((HRESULT(WINCALL*)(void*,void*,void*,U32,U32))context->v[29])(context,pendingQuery,nullptr,0,1);
  if(result==0){
   goodKind=pendingKind;lastSeen=GetTickCount64();
   if(goodKind==STEREO){
    if(capture_valid(pendingCapture,pendingCapture.pairId,trackingEpoch)){
     // Promotion is one host-thread operation: completed bank AND its own metadata.
     worldCapture[pendingBank]=pendingCapture;
     worldRegion[pendingBank][0]=pendingRegion[0];worldRegion[pendingBank][1]=pendingRegion[1];
     activeWorldBank=pendingBank;
     if(pendingRegion[0].exactSample&&pendingRegion[1].exactSample)++regionExact;else ++regionFallback;
     log_camera_map(pendingCapture,pendingRegion);
     lastWorld=pendingCapture.capturedMs;haveWorld=true;lastConsumedFrame=pendingCapture.pairId;
     if(beforeWait)++preWaitPromotions;else ++postWaitPromotions;
     U32 count=inc32(&bus->consumedPackets);
     if(count<=3||count%120==0)logg.msg("R3_PAIR_METADATA_LATCHED",pendingCapture.pairId,pendingCapture.sampleId,activeWorldBank);
    }else{inc32(&bus->epochRejects);}
   }else{lastMenu=lastSeen;haveMenu=true;++menuGeneration;}
   // Only now is the source GPU's last D3D11 reader known complete.
   nova_frame_transport::release(source_image(pendingSlot));pendingQuery->release();pendingQuery=nullptr;pendingSlot=-1;pendingBank=-1;completed=true;
  }else if(result<0||GetTickCount64()-pendingBegin>5000){
   inc32(&bus->graphicsErrors);logg.msg("HOST_GPU_CONSUMER_STALLED",pendingSlot,(U32)result);
   store32(&bus->stop,1);return false;
  }
  // Still waiting: retain every GPU lease. Completion: allow the next copy in
  // THIS call, rather than reserving a whole additional XR frame for it.
  if(pendingSlot>=0){++pendingPolls;return true;}
 }
 int found=-1,kind=0;U64 maxFrame=lastConsumedFrame;
 for(int i=0;i<4;i++){
  const Image&v=source_image(i);if(load32(&v.phase)==GPU_FINISHED&&v.frame>maxFrame){found=i;maxFrame=v.frame;kind=STEREO;}
 }
 // A completed but superseded pair has no host GPU reader: safe to discard,
 // and it must never become an older pose after a newer frame was displayed.
 for(int i=0;i<4;i++)if(i!=found&&load32(&source_image(i).phase)==GPU_FINISHED&&source_image(i).frame<=maxFrame)reject_ready(i);
 if(found<0&&load32(&source_image(4).phase)==GPU_FINISHED){found=4;kind=MENU;}
 if(found<0){++frameExtra.page->noSource;return completed;}if(!nova_frame_transport::acquire(source_image(found)))return completed;
 if(kind==STEREO){
  pendingCapture=source_capture(found);
  if(!capture_valid(pendingCapture,source_image(found).frame,trackingEpoch)){
   inc32(&bus->epochRejects);nova_frame_transport::release(source_image(found));return false;
  }
  // Both regions are bound to the captured sample before any copy is queued.
  // Missing history retains the correct full-native description; no guessed FOV.
  bool exactRegion=pair_regions(pendingCapture,viewHistory,bus->width,bus->height,pendingRegion);
  if(pendingCapture.projectionKind==3&&!exactRegion){
   // A cropped producer never receives the legacy full-image fallback label.
   // No source copy/reader is outstanding, so this lease can be released now.
   inc32(&bus->poseRejects);logg.msg("F1_CROPPED_PAIR_REJECTED_MISSING_EXACT_SAMPLE",pendingCapture.pairId,pendingCapture.sampleId);nova_frame_transport::release(source_image(found));return false;
  }
 }
 D3D11_QUERY_DESC q{};q.Query=0;
 HRESULT hr=((HRESULT(WINCALL*)(void*,const D3D11_QUERY_DESC*,D3DQuery**))gpu->v[24])(gpu,&q,&pendingQuery);
 if(failed(hr)||!pendingQuery){
  // No copy has been queued yet, therefore releasing this particular lease is safe.
  nova_frame_transport::release(source_image(found));logg.msg("GPU_CONSUMER_QUERY_CREATE_FAILED",(U32)hr);store32(&bus->stop,1);return false;
 }
 if(kind==STEREO){
  pendingBank=1-activeWorldBank;
  context->copy(worldBank[pendingBank][0],sharing[source_texture(found)]);
  context->copy(worldBank[pendingBank][1],sharing[source_texture(found)+1]);
 }else context->copy(menuResident,sharing[6]);
 ((void(WINCALL*)(void*,D3DQuery*))context->v[28])(context,pendingQuery);context->flush();
 pendingSlot=found;pendingKind=kind;pendingBegin=GetTickCount64();++copyStarted;return true;
}
static void invalidate_tracking(){worldReleased.invalidate();quadReleased.invalidate();Tracking empty{};empty.epoch=trackingEpoch;write_tracking(bus,empty);haveWorld=false;
#ifdef NOVA_LAB_UNIVERSAL
 if(buttons)controller_input::write(buttons,controller_input::Sample{});
#endif
}
static void update_tracking(XrTime predicted){
 nova_perf::Scope timing(nova_perf::Tracking);
 if(referenceChangeTime&&predicted>=referenceChangeTime){referenceChangeTime=0;trackingEpoch++;invalidate_tracking();logg.msg("R3_REFERENCE_SPACE_EPOCH",trackingEpoch);}

 XrViewLocateInfo li{XR_TYPE_VIEW_LOCATE_INFO};li.viewConfigurationType=XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;li.displayTime=predicted;li.space=local;
 XrViewState vs{XR_TYPE_VIEW_STATE};XrView views[2]={{XR_TYPE_VIEW},{XR_TYPE_VIEW}};U32 count=0;
 XrResult r=xrLocateViews(session,&li,&vs,2,&count,views);if(XR_FAILED(r)||count!=2){invalidate_tracking();return;}
 Tracking t{};t.predictedTime=predicted;t.epoch=trackingEpoch;t.sampleId=++trackingSample;t.sampledMs=GetTickCount64();t.valid=(vs.viewStateFlags&(XR_VIEW_STATE_POSITION_VALID_BIT|XR_VIEW_STATE_ORIENTATION_VALID_BIT))==(XR_VIEW_STATE_POSITION_VALID_BIT|XR_VIEW_STATE_ORIENTATION_VALID_BIT);
 t.focused=sessionState==XR_SESSION_STATE_FOCUSED;
 for(int e=0;e<2;e++){
  t.eye[e].pos={views[e].pose.position.x,views[e].pose.position.y,views[e].pose.position.z};
  t.eye[e].rot={views[e].pose.orientation.x,views[e].pose.orientation.y,views[e].pose.orientation.z,views[e].pose.orientation.w};
  t.eye[e].fov={views[e].fov.angleLeft,views[e].fov.angleRight,views[e].fov.angleUp,views[e].fov.angleDown};
 }
#ifdef NOVA_LAB_UNIVERSAL
 controller_input::Sample controls{};controls.sampledMs=t.sampledMs;controls.sample=t.sampleId;controls.epoch=t.epoch;controls.focused=t.focused;
#endif
 if(handInput){
  poll_hand_calibration();t.reserved=handCalibration.source==0?weapon_input::AIM_SCHEMA:weapon_input::GRIP_CONTROL_SCHEMA;
  XrActiveActionSet aa{handSet,XR_NULL_PATH};XrActionsSyncInfo sync{XR_TYPE_ACTIONS_SYNC_INFO};sync.countActiveActionSets=1;sync.activeActionSets=&aa;
  if(XR_SUCCEEDED(xrSyncActions(session,&sync))){
#ifdef NOVA_LAB_UNIVERSAL
   if(triggerAction!=XR_NULL_HANDLE&&xrGetActionStateFloat){
    XrActionStateGetInfo get{XR_TYPE_ACTION_STATE_GET_INFO};get.action=triggerAction;get.subactionPath=handPaths[1];XrActionStateFloat value{XR_TYPE_ACTION_STATE_FLOAT};
    if(XR_SUCCEEDED(xrGetActionStateFloat(session,&get,&value))&&value.isActive){controls.trigger=value.currentState;controls.active=1;}
    get.subactionPath=handPaths[0];value={XR_TYPE_ACTION_STATE_FLOAT};if(XR_SUCCEEDED(xrGetActionStateFloat(session,&get,&value))&&value.isActive)controls.leftTrigger=value.currentState;
   }
   for(U32 hand=0;hand<2;hand++){
    XrActionStateGetInfo get{XR_TYPE_ACTION_STATE_GET_INFO};get.subactionPath=handPaths[hand];
    if(extraActions[GripValue]!=XR_NULL_HANDLE){get.action=extraActions[GripValue];XrActionStateFloat value{XR_TYPE_ACTION_STATE_FLOAT};if(XR_SUCCEEDED(xrGetActionStateFloat(session,&get,&value))&&value.isActive){if(hand)controls.rightGrip=value.currentState;else controls.leftGrip=value.currentState;}}
    if(extraActions[StickValue]!=XR_NULL_HANDLE){get.action=extraActions[StickValue];XrActionStateVector2f value{XR_TYPE_ACTION_STATE_VECTOR2F};if(XR_SUCCEEDED(xrGetActionStateVector2f(session,&get,&value))&&value.isActive){controls.actionMask|=1u<<hand;if(hand){controls.rightX=value.currentState.x;controls.rightY=value.currentState.y;}else{controls.leftX=value.currentState.x;controls.leftY=value.currentState.y;}}}
    if(extraActions[StickClick]!=XR_NULL_HANDLE){get.action=extraActions[StickClick];XrActionStateBoolean value{XR_TYPE_ACTION_STATE_BOOLEAN};if(XR_SUCCEEDED(xrGetActionStateBoolean(session,&get,&value))&&value.isActive&&value.currentState)controls.buttons|=hand?controller_input::RightClick:controller_input::LeftClick;}
   }
   const U32 actionIndices[]={ButtonA,ButtonB,ButtonX,ButtonY,ButtonMenu};const U32 masks[]={controller_input::A,controller_input::B,controller_input::X,controller_input::Y,controller_input::Menu};
   for(U32 i=0;i<5;i++)if(extraActions[actionIndices[i]]!=XR_NULL_HANDLE){XrActionStateGetInfo get{XR_TYPE_ACTION_STATE_GET_INFO};get.action=extraActions[actionIndices[i]];get.subactionPath=handPaths[i<2?1:0];XrActionStateBoolean value{XR_TYPE_ACTION_STATE_BOOLEAN};if(XR_SUCCEEDED(xrGetActionStateBoolean(session,&get,&value))&&value.isActive&&value.currentState)controls.buttons|=masks[i];}
#endif
  for(int i=0;i<2;i++){
   XrActionStateGetInfo get{XR_TYPE_ACTION_STATE_GET_INFO};get.action=handAction;get.subactionPath=handPaths[i];
   XrActionStatePose pose{XR_TYPE_ACTION_STATE_POSE};
   if(XR_SUCCEEDED(xrGetActionStatePose(session,&get,&pose))&&pose.isActive){
    XrSpaceLocation loc{XR_TYPE_SPACE_LOCATION};
    if(XR_SUCCEEDED(xrLocateSpace(handSpaces[i],local,predicted,&loc))){
     weapon_input::Sample grip{},aim{};
     grip.position={loc.pose.position.x,loc.pose.position.y,loc.pose.position.z};grip.rotation={loc.pose.orientation.x,loc.pose.orientation.y,loc.pose.orientation.z,loc.pose.orientation.w};
     grip.active=true;grip.positionValid=(loc.locationFlags&XR_SPACE_LOCATION_POSITION_VALID_BIT)!=0;grip.orientationValid=(loc.locationFlags&XR_SPACE_LOCATION_ORIENTATION_VALID_BIT)!=0;
     get.action=aimAction;XrActionStatePose aimState{XR_TYPE_ACTION_STATE_POSE};XrSpaceLocation aimLoc{XR_TYPE_SPACE_LOCATION};
     if(XR_SUCCEEDED(xrGetActionStatePose(session,&get,&aimState))&&aimState.isActive&&XR_SUCCEEDED(xrLocateSpace(aimSpaces[i],local,predicted,&aimLoc))){
      aim.active=true;aim.positionValid=(aimLoc.locationFlags&XR_SPACE_LOCATION_POSITION_VALID_BIT)!=0;aim.orientationValid=(aimLoc.locationFlags&XR_SPACE_LOCATION_ORIENTATION_VALID_BIT)!=0;
      aim.position={aimLoc.pose.position.x,aimLoc.pose.position.y,aimLoc.pose.position.z};aim.rotation={aimLoc.pose.orientation.x,aimLoc.pose.orientation.y,aimLoc.pose.orientation.z,aimLoc.pose.orientation.w};
     }
#ifdef NOVA_LAB_UNIVERSAL
     t.reserved=weapon_input::GRIP_CONTROL_SCHEMA;
     t.hand[i]={grip.position,grip.rotation,grip.positionValid&&grip.orientationValid,grip.active};
#else
     t.hand[i]=weapon_input::target(grip,aim,handCalibration);
#endif
     if(i==1&&(t.sampleId<4||t.sampleId%120==0))logg.msg("LAB_WEAPON_POSE_SCHEMA",t.reserved,handCalibration.revision,t.hand[i].valid);
    }
   }
  }
  }
 }
 if(!valid_eye(t.eye[0])||!valid_eye(t.eye[1]))t.valid=0;
#ifdef NOVA_LAB_UNIVERSAL
 controls.valid=t.valid&&t.hand[1].valid&&t.hand[1].active;if(buttons)controller_input::write(buttons,controls);
#endif
 viewHistory.remember(t);write_tracking(bus,t);
}
#ifdef NOVA_LAB_UNIVERSAL
static void poll_controller_settings(){
 if(!buttons)return;I32 request=load32(&buttons->settingsRequest);if(request==I32(load32(&buttons->settingsAck)))return;
 float value=buttons->hapticGain;hapticGain=finite_in_range(value,1.001f)&&value>=0?value:1.f;buttons->hapticGain=hapticGain;
 U32 degrees=buttons->snapDegrees;if(degrees!=15&&degrees!=30&&degrees!=45)buttons->snapDegrees=30;
 buttons->turnMode=0;store32(&buttons->settingsAck,request);logg.msg("XR_CONTROLLER_SETTINGS",U32(hapticGain*1000),buttons->snapDegrees);
}
static bool vibration(U32 hand,float amplitude,U32 duration){
 amplitude*=hapticGain;if(amplitude<=0||!buttons||hapticAction==XR_NULL_HANDLE||!xrApplyHapticFeedback||!running||sessionState!=XR_SESSION_STATE_FOCUSED)return false;
 if(amplitude>.95f)amplitude=.95f;if(duration>65)duration=65;
 XrHapticActionInfo info{XR_TYPE_HAPTIC_ACTION_INFO};info.action=hapticAction;info.subactionPath=handPaths[hand];
 XrHapticVibration pulse{XR_TYPE_HAPTIC_VIBRATION};pulse.duration=I64(duration)*1000000;pulse.frequency=XR_FREQUENCY_UNSPECIFIED;pulse.amplitude=amplitude;
 XrResult result=xrApplyHapticFeedback(session,&info,(const XrHapticBaseHeader*)&pulse);
 if(XR_FAILED(result)){++buttons->hapticErrors;logg.msg("XR_HAPTIC_FAILED",U32(result),hand);return false;}
 ++buttons->hapticApplied;hapticWasActive=true;logg.msg("XR_HAPTIC_APPLIED",hand,U32(amplitude*1000),duration);return true;
}
static void stop_haptics(){
 for(auto&tail:hapticTails)tail={};
 if(hapticWasActive&&running&&hapticAction!=XR_NULL_HANDLE&&xrStopHapticFeedback){for(U32 i=0;i<2;i++){XrHapticActionInfo info{XR_TYPE_HAPTIC_ACTION_INFO};info.action=hapticAction;info.subactionPath=handPaths[i];(void)xrStopHapticFeedback(session,&info);}}
 hapticWasActive=false;
}
static void poll_haptics(){
 if(!buttons)return;controller_input::Sample input{};if(!controller_input::read(buttons,input)||!controller_input::live(input,GetTickCount64())){stop_haptics();return;}
 U64 now=GetTickCount64();controller_input::Feedback feedback{};
 if(controller_input::feedback_read(buttons,feedback)&&feedback.serial&&feedback.serial!=hapticSerial){
  hapticSerial=feedback.serial;
  if(feedback.epoch!=input.epoch||now<feedback.createdMs||now-feedback.createdMs>200){__atomic_fetch_add(&buttons->hapticDropped,1u,__ATOMIC_RELAXED);}
  else{
   logg.msg("XR_NATIVE_FEEDBACK",feedback.kind,U32(feedback.command),feedback.serial);
   if(feedback.kind==controller_input::Shot){bool rapid=lastShotHaptic&&now-lastShotHaptic<140;lastShotHaptic=now;hapticTails[1]={};if(vibration(1,rapid?.75f:.9f,rapid?32:50)&&!rapid)hapticTails[1]={now+62,.3f,15};}
   else if(feedback.kind==controller_input::ReloadInput){if(vibration(0,.2f,14))hapticTails[0]={now+65,.25f,16};}
   else if(feedback.kind==controller_input::WeaponInput)vibration(1,.2f,12);
   else if(feedback.kind==controller_input::MenuInput)vibration(0,.15f,12);
  }
 }
 for(U32 hand=0;hand<2;hand++)if(hapticTails[hand].due&&now>=hapticTails[hand].due){auto tail=hapticTails[hand];hapticTails[hand]={};vibration(hand,tail.amplitude,tail.durationMs);}
}
#endif
static U64 quality_stamp(){
#ifdef NOVA_LAB_UNIVERSAL
 U32 bits=0;memcpy(&bits,&imageQuality.sharpness,4);
 return (U64(bits)<<3)|U64(imageQuality.enabled)|(U64(imageQuality.aa)<<1)|(U64(imageQuality.ready)<<2);
#else
 return 0;
#endif
}
static bool copy_quad(D3DTexture*texture,U32 kind,U64 generation){
 xr_content_cache::Key key{trackingEpoch,generation,0,kind};
 if(!quadReleased.needs(key,(fpsFeatures&16)!=0)){if(fpsChannel.page)++fpsChannel.page->quadReuses;return true;}
 quadReleased.invalidate();if(!acquire_and_copy(2,texture))return false;
 quadReleased.committed(key);if(fpsChannel.page)++fpsChannel.page->quadUploads;return true;
}
static bool frame(){
 // Source copies target only our resident banks, not XR swapchain images.
 // Start/poll them before the runtime's pacing wait, then preserve the existing
 // post-wait completion check. No blocking poll and no early source release.
 if(fpsFeatures&256u)consume_ready(true);
 XrFrameWaitInfo w{XR_TYPE_FRAME_WAIT_INFO};XrFrameState f{XR_TYPE_FRAME_STATE};
 {nova_perf::Scope timing(nova_perf::WaitFrame);if(!xrpass(xrWaitFrame(session,&w,&f),"WAIT_FRAME_FAIL"))return false;}
 XrFrameBeginInfo bi{XR_TYPE_FRAME_BEGIN_INFO};XrResult br=xrBeginFrame(session,&bi);
 if(br!=XR_FRAME_DISCARDED&&!xrpass(br,"BEGIN_FRAME_FAIL"))return false;
 update_tracking(f.predictedDisplayTime);
#ifdef NOVA_LAB_UNIVERSAL
 poll_haptics();
#endif
 consume_ready();
 XrCompositionLayerProjectionView views[2]={{XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW},{XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW}};
 XrCompositionLayerProjection projection{XR_TYPE_COMPOSITION_LAYER_PROJECTION};projection.space=local;projection.viewCount=2;projection.views=views;
 XrCompositionLayerQuad quad{XR_TYPE_COMPOSITION_LAYER_QUAD};quad.space=viewSpace;quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;quad.pose.orientation.w=1;quad.pose.position.z=-2;quad.size.width=1.8f;quad.size.height=1.8f;
 XrCompositionLayerBaseHeader const*layers[3]{};U32 count=0,submittedKind=0;bool copiesOkay=true,projectionSubmitted=false;
 XrCompositionLayerQuad markers[2]={{XR_TYPE_COMPOSITION_LAYER_QUAD},{XR_TYPE_COMPOSITION_LAYER_QUAD}};
 bool world=haveWorld&&f.shouldRender&&GetTickCount64()-lastWorld<500;
#ifdef NOVA_LAB_UNIVERSAL
 if(buttons&&load32(&buttons->gameUiOpen))world=false; // Show the real native game UI as the existing view-space quad.
#endif
 bool menu=haveMenu&&f.shouldRender&&GetTickCount64()-lastMenu<1500;
 // Persistent real game MENU image, independent of whether the world 3D callback has started.
 if(world){
  const CapturedStereo&c=worldCapture[activeWorldBank];
  if(capture_valid(c,c.pairId,trackingEpoch)){
   xr_content_cache::Key key{c.trackingEpoch,c.pairId,quality_stamp(),1};
   bool copied=true;
   if(worldReleased.needs(key,(fpsFeatures&16)!=0)){
    // OpenXR uses the most recently released image. New source content still
    // uploads both eyes, atomically commits its key and retains its OWN pose.
    worldReleased.invalidate();copied=acquire_and_copy(0,worldBank[activeWorldBank][0]);
    if(copied)copied=acquire_and_copy(1,worldBank[activeWorldBank][1]);
    if(copied){worldReleased.committed(key);if(fpsChannel.page)++fpsChannel.page->worldUploads;}
   }else if(fpsChannel.page)++fpsChannel.page->worldReuses;
   copiesOkay=copied;
   if(copied){
    for(int e=0;e<2;e++){
     views[e].pose.position={c.eye[e].pos.x,c.eye[e].pos.y,c.eye[e].pos.z};
     views[e].pose.orientation={c.eye[e].rot.x,c.eye[e].rot.y,c.eye[e].rot.z,c.eye[e].rot.w};
     // Pixel-edge region and FOV belong to this exact completed image bank.
     // Pixels remain native; this neither rewrites Source projection nor warps them.
     const ViewRegion&v=worldRegion[activeWorldBank][e];
     views[e].fov={v.fov.l,v.fov.r,v.fov.u,v.fov.d};
     views[e].subImage.swapchain=swapchains[e];
     views[e].subImage.imageRect.offset={v.x,v.y};views[e].subImage.imageRect.extent={v.width,v.height};
    }
    layers[0]=(const XrCompositionLayerBaseHeader*)&projection;count=1;submittedKind=1;projectionSubmitted=true;inc32(&bus->submitPosePairs);
    if(++submitSerial<=3||submitSerial%240==0)logg.msg("R3_SUBMIT_IMAGE_POSE",c.pairId,c.sampleId,GetTickCount64()-c.capturedMs);
   }
  }else inc32(&bus->epochRejects);
 }else if(menu){
  copiesOkay=copy_quad(menuResident,2,menuGeneration);
  if(copiesOkay){quad.subImage.swapchain=swapchains[2];quad.subImage.imageRect.extent={I32(bus->width),I32(bus->height)};
#ifdef NOVA_LAB_UNIVERSAL
   U32 extent=buttons?load32(&buttons->screenExtent):0,width=extent&65535u,height=extent>>16;
   if(width&&height){
    float aspect=float(width)/float(height);I32 w=I32(bus->width),h=I32(bus->height);
    if(aspect>1){h=I32(float(bus->height)/aspect);quad.subImage.imageRect.offset.y=(I32(bus->height)-h)/2;}else{w=I32(float(bus->width)*aspect);quad.subImage.imageRect.offset.x=(I32(bus->width)-w)/2;}
    quad.subImage.imageRect.extent={w,h};float horizontal=.72f,vertical=.8f;Tracking current{};
    if(read_tracking(bus,current)&&current.valid){horizontal=1.5f;vertical=1.5f;for(const auto&eye:current.eye){if(-eye.fov.l<horizontal)horizontal=-eye.fov.l;if(eye.fov.r<horizontal)horizontal=eye.fov.r;if(eye.fov.u<vertical)vertical=eye.fov.u;if(-eye.fov.d<vertical)vertical=-eye.fov.d;}}
    float halfWidth=float(tan(horizontal))*.96f,halfHeight=float(tan(vertical))*.96f;if(halfWidth/aspect>halfHeight)halfWidth=halfHeight*aspect;
    quad.pose.position.z=-1;quad.size.width=halfWidth*2;quad.size.height=halfWidth*2/aspect;
   }
#endif
   layers[0]=(const XrCompositionLayerBaseHeader*)&quad;count=1;submittedKind=2;}
 }
#ifdef NOVA_LAB_UNIVERSAL
 if(!count&&f.shouldRender&&copiesOkay&&loadingTexture){
  copiesOkay=copy_quad(loadingTexture,3,1);
  if(copiesOkay){quad.subImage.swapchain=swapchains[2];quad.subImage.imageRect.extent={I32(bus->width),I32(bus->height)};
   layers[0]=(const XrCompositionLayerBaseHeader*)&quad;count=1;submittedKind=3;}
 }
#endif
 if(markersEnabled&&projectionSubmitted){
  Tracking current{};const Tracking*captured=viewHistory.match(worldCapture[activeWorldBank]);
  if(captured&&read_tracking(bus,current)&&current.valid){
   const Tracking*values[2]={captured,&current};
   for(U32 k=0;k<2;k++)if(values[k]->hand[1].valid){
    if(!acquire_and_copy(I32(3+k),markerTextures[k])){copiesOkay=false;continue;}
    auto&mark=markers[k];mark.space=local;mark.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
    const Vec3 p=values[k]->hand[1].pos;const Quat q=current.eye[0].rot;
    mark.pose.position={p.x,p.y,p.z};mark.pose.orientation={q.x,q.y,q.z,q.w};mark.size={.012f,.012f};
    mark.subImage.swapchain=swapchains[3+k];mark.subImage.imageRect.extent={16,16};layers[count++]=(const XrCompositionLayerBaseHeader*)&mark;
   }
  }
 }
 XrFrameEndInfo end{XR_TYPE_FRAME_END_INFO};end.displayTime=f.predictedDisplayTime;end.environmentBlendMode=XR_ENVIRONMENT_BLEND_MODE_OPAQUE;end.layerCount=count;end.layers=count?layers:nullptr;
 bool success=false;{nova_perf::Scope timing(nova_perf::EndFrame);success=xrpass(xrEndFrame(session,&end),"END_FRAME_FAIL");}
 if(success){if(fpsChannel.page)store32(&fpsChannel.page->submittedKind,submittedKind);inc32(&bus->endFrameSuccess);if(count){if(projectionSubmitted)inc32(&bus->submitStereo);else inc32(&bus->submitMenu);}else inc32(&bus->emptyFrames);}
 // A requested cooperative stop is handled by the outer loop. A failed image
 // copy remains an error even when that failure also raises the stop flag.
 return success&&copiesOkay;
}
static void events(){while(1){XrEventDataBuffer data{XR_TYPE_EVENT_DATA_BUFFER};XrResult r=xrPollEvent(instance,&data);if(r==XR_EVENT_UNAVAILABLE)break;if(XR_FAILED(r)){xrpass(r,"EVENT_FAIL");break;}
 if(data.type==XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING){auto*c=(XrEventDataReferenceSpaceChangePending*)&data;if(c->referenceSpaceType==XR_REFERENCE_SPACE_TYPE_LOCAL){referenceChangeTime=c->changeTime;logg.msg("R3_REFERENCE_SPACE_CHANGE_PENDING",(U64)c->changeTime);}}
 if(data.type==XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED){auto*s=(XrEventDataSessionStateChanged*)&data;sessionState=s->state;
  logg.msg("SESSION_STATE",(U32)sessionState,trackingEpoch);
  if(sessionState==XR_SESSION_STATE_READY&&!running){XrSessionBeginInfo b{XR_TYPE_SESSION_BEGIN_INFO};b.primaryViewConfigurationType=XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
   if(xrpass(xrBeginSession(session,&b),"SESSION_BEGIN_FAIL")){running=true;trackingEpoch++;goodKind=0;haveWorld=haveMenu=false;invalidate_tracking();}}
  if(sessionState==XR_SESSION_STATE_STOPPING&&running){
#ifdef NOVA_LAB_UNIVERSAL
   stop_haptics();
#endif
   xrEndSession(session);running=false;trackingEpoch++;goodKind=0;haveWorld=haveMenu=false;invalidate_tracking();}
  if(sessionState==XR_SESSION_STATE_EXITING||sessionState==XR_SESSION_STATE_LOSS_PENDING){exiting=true;}
 }
 }}
static void cleanup(){if(bus)store32(&bus->stop,1);
#ifdef NOVA_LAB_UNIVERSAL
 stop_haptics();
 imageQuality.cleanup();
#endif
 for(int i=0;i<2;i++)if(handSpaces[i]!=XR_NULL_HANDLE&&xrDestroySpace)xrDestroySpace(handSpaces[i]);
 for(int i=0;i<2;i++)if(aimSpaces[i]!=XR_NULL_HANDLE&&xrDestroySpace)xrDestroySpace(aimSpaces[i]);
#ifdef NOVA_LAB_UNIVERSAL
 for(auto action:extraActions)if(action!=XR_NULL_HANDLE&&xrDestroyAction)xrDestroyAction(action);
 if(hapticAction!=XR_NULL_HANDLE&&xrDestroyAction)xrDestroyAction(hapticAction);
 if(triggerAction!=XR_NULL_HANDLE&&xrDestroyAction)xrDestroyAction(triggerAction);
 if(buttons){controller_input::write(buttons,controller_input::Sample{});UnmapViewOfFile(buttons);}if(buttonsMap)CloseHandle(buttonsMap);
#endif
 if(aimAction!=XR_NULL_HANDLE&&xrDestroyAction)xrDestroyAction(aimAction);
 if(handAction!=XR_NULL_HANDLE&&xrDestroyAction)xrDestroyAction(handAction);
 if(handSet!=XR_NULL_HANDLE&&xrDestroyActionSet)xrDestroyActionSet(handSet);
 if(pendingQuery){pendingQuery->release();pendingQuery=nullptr;}
 for(U32 i=0;i<swapchainCount;i++)if(swapchains[i]!=XR_NULL_HANDLE&&xrDestroySwapchain)xrDestroySwapchain(swapchains[i]);
 if(viewSpace!=XR_NULL_HANDLE&&xrDestroySpace)xrDestroySpace(viewSpace);
 if(local!=XR_NULL_HANDLE&&xrDestroySpace)xrDestroySpace(local);
 if(session!=XR_NULL_HANDLE&&xrDestroySession)xrDestroySession(session);
 if(instance!=XR_NULL_HANDLE&&xrDestroyInstance)xrDestroyInstance(instance);
 for(int i=0;i<10;i++)if(sharing[i])sharing[i]->release();
 for(int b=0;b<2;b++)for(int e=0;e<2;e++)if(worldBank[b][e])worldBank[b][e]->release();
 if(menuResident)menuResident->release();
#ifdef NOVA_LAB_UNIVERSAL
 if(loadingTexture)loadingTexture->release();
#endif
 for(auto*texture:markerTextures)if(texture)texture->release();
 if(context)context->release();if(gpu)gpu->release();if(adapter)adapter->release();if(factory)factory->release();
 frameExtra.close();fpsChannel.close();
 if(game)CloseHandle(game);if(bus)UnmapViewOfFile(bus);if(map)CloseHandle(map);logg.close();
}
static int run(){logg.open(L"nova_xr64.log");nova_perf::initialize(L"nova_perf_host.log");U32 pid=envpid();if(!pid){logg.msg("INVALID_PID");return 1;}
 game=OpenProcess(0x100000|0x1000,0,pid);if(!game){logg.msg("GAME_NOT_RUNNING");return 2;}
 wchar_t name[64]={};mapping_name(name,pid);map=CreateFileMappingW(BAD_HANDLE,nullptr,0x04,0,sizeof(Shared),name);
 if(!map)return 3;bus=(Shared*)MapViewOfFile(map,0xf001f,0,0,sizeof(Shared));if(!bus)return 4;
 if(bus->magic&&bus->magic!=MAGIC)return 5;memset(bus,0,sizeof(*bus));
 bus->magic=MAGIC;bus->version=VERSION;bus->bytes=sizeof(*bus);bus->gamePid=pid;bus->hostPid=GetCurrentProcessId();bus->width=1024;bus->height=1024;bus->textureCount=8;GetEnvironmentVariableW(L"NOVA_LOGDIR",bus->logDir,512);
#ifdef NOVA_LAB_UNIVERSAL
 wchar_t buttonsName[64]{};controller_input::name(buttonsName,pid);buttonsMap=CreateFileMappingW(BAD_HANDLE,nullptr,4,0,sizeof(controller_input::Channel),buttonsName);
 if(buttonsMap&&GetLastError()!=183){buttons=(controller_input::Channel*)MapViewOfFile(buttonsMap,0xf001f,0,0,sizeof(controller_input::Channel));if(buttons){memset(buttons,0,sizeof(*buttons));buttons->version=controller_input::VERSION;buttons->bytes=sizeof(*buttons);buttons->gamePid=pid;buttons->hostPid=GetCurrentProcessId();buttons->hapticGain=hapticGain;buttons->snapDegrees=30;buttons->turnRate=90;barrier();buttons->magic=controller_input::MAGIC;}}
#endif
 fpsChannel.create(pid,GetCurrentProcessId(),31);
 if(!frameExtra.create(pid,GetCurrentProcessId())){logg.msg("F6_TRANSPORT_CREATE_FAILED");return 3;}
 wchar_t markerOption[8]{};markersEnabled=GetEnvironmentVariableW(L"NOVA_LAB_MARKERS",markerOption,8)>0&&markerOption[0]==L'1';swapchainCount=markersEnabled?5:3;
#ifdef NOVA_LAB_UNIVERSAL
 wchar_t resolution[12]{};if(GetEnvironmentVariableW(L"NOVA_EYE_RESOLUTION",resolution,12)>0){U32 n=0;for(U32 i=0;resolution[i]>=L'0'&&resolution[i]<=L'9';i++)n=n*10+U32(resolution[i]-L'0');if(n==1024||n==1280||n==1440||n==2048||n==2560||n==3072||n==3584)bus->width=bus->height=n;}
#endif
 for(int i=0;i<4;i++)bus->images[i].phase=VACANT;
 logg.msg("NOVA_HOST_START",pid,bus->hostPid);logg.msg("R4_HOST_CAMERA_BUILD_R3_NATIVE_PRESERVED",VERSION,sizeof(Shared),sizeof(CapturedStereo));
 if(!dynamic_openxr()||!make_d3d11()||!make_session()||!graphics_resources())return 10;
#ifdef NOVA_LAB_UNIVERSAL
 if(!imageQuality.initialize(gpu,context,logg,pid,GetCurrentProcessId(),bus->width,bus->height)){
  logg.msg("IMAGE_AA_UNAVAILABLE_COPY_ONLY");
  if(!native_bgra_copy_compatible(U32(swapchainFormat))){logg.msg("XR_REQUIRED_COLOR_CONVERTER_UNAVAILABLE");return 10;}
 }
#endif
 int runtimeCode=0;U64 last=GetTickCount64();
 while(running_game()&&!load32(&bus->stop)&&!exiting){
#ifdef NOVA_LAB_UNIVERSAL
  imageQuality.poll();poll_controller_settings();
#endif
  fpsFeatures=fpsChannel.page?load32(&fpsChannel.page->features):0;if(fpsChannel.page)store32(&fpsChannel.page->applied,I32(fpsFeatures));
  nova_perf::set_enabled((fpsFeatures&128u)==0);
  events();if(running){if(!frame()){runtimeCode=running_game()?21:0;break;}}else Sleep(10);
  nova_perf::report();
  if(GetTickCount64()-last>5000){last=GetTickCount64();logg.msg("NOVA_PROGRESS",bus->sceneCalls,bus->presentCalls,bus->submitStereo);logg.msg("NOVA_MENU",bus->menuPixels,bus->submitMenu,bus->graphicsErrors);logg.msg("NOVA_PRODUCER",bus->publishMenu,bus->publishStereo,bus->rendererState);logg.msg("NOVA_GPU_LINK",bus->openMask,bus->stereoPixels,bus->resourceReady);
   logg.msg("R3_PACKETS",bus->capturedPackets,bus->consumedPackets,bus->submitPosePairs);logg.msg("F6_RING",frameExtra.page->superseded,frameExtra.page->noSource,(fpsFeatures&512u)!=0);
   logg.msg("R3_REJECTIONS",bus->slotBusy,bus->poseRejects,bus->epochRejects);
   logg.msg("R3_XR_END",bus->endFrameSuccess,bus->emptyFrames,bus->xrErrors);
   logg.msg("R4_VIEW_REGIONS",regionExact,regionFallback,copyStarted);logg.msg("R4_PENDING_POLLS",pendingPolls);logg.msg("F5_RECEIVER_PHASE",preWaitPolls,preWaitPromotions,postWaitPromotions);if(fpsChannel.page){logg.msg("F2_WORLD_CONTENT",fpsChannel.page->worldUploads,fpsChannel.page->worldReuses,fpsFeatures);logg.msg("F2_QUAD_CONTENT",fpsChannel.page->quadUploads,fpsChannel.page->quadReuses);}}
 }
 logg.msg("NOVA_HOST_EXIT",bus->sceneCalls,bus->submitStereo,bus->submitMenu);
 logg.msg("R3_FINAL_ERRORS",bus->graphicsErrors,bus->xrErrors,bus->sourceFenceFaults);
 logg.msg("R3_FINAL_PACKETS",bus->capturedPackets,bus->consumedPackets,bus->submitPosePairs);
 logg.msg("R4_FINAL_REGIONS",regionExact,regionFallback,copyStarted);
 logg.msg("F5_FINAL_RECEIVER_PHASE",preWaitPolls,preWaitPromotions,postWaitPromotions);
 if(fpsChannel.page){logg.msg("F2_FINAL_WORLD_CONTENT",fpsChannel.page->worldUploads,fpsChannel.page->worldReuses,fpsFeatures);logg.msg("F2_FINAL_QUAD_CONTENT",fpsChannel.page->quadUploads,fpsChannel.page->quadReuses);}
 return runtimeCode;
}
}
extern "C" void mainCRTStartup(){int ec=run();cleanup();ExitProcess(ec);}
