// Explicit, application-owned test backend. This does not change the Khronos
// loader, registry, OS policy, or the stock headset executable. All calls after
// instance creation go through the unchanged xr-tape dispatch into xr-sim.
#include "openxr/openxr_loader_negotiation.h"
static HMODULE labSimModule=nullptr,labTapeModule=nullptr;
static PFN_xrGetInstanceProcAddr labRuntimeGipa=nullptr,labTapeGipa=nullptr;
static PFN_xrCreateInstance labRuntimeCreate=nullptr;
static PFN_xrCreateApiLayerInstance labTapeCreate=nullptr;
static XrResult XRAPI_CALL lab_next_create(const XrInstanceCreateInfo*info,const XrApiLayerCreateInfo*,XrInstance*out){return labRuntimeCreate(info,out);}
static XrResult XRAPI_CALL lab_create(const XrInstanceCreateInfo*info,XrInstance*out){
 XrApiLayerNextInfo next{};next.structType=XR_LOADER_INTERFACE_STRUCT_API_LAYER_NEXT_INFO;
 next.structVersion=XR_API_LAYER_NEXT_INFO_STRUCT_VERSION;next.structSize=sizeof(next);
 const char*name="XR_APILAYER_XRTAPE_recorder";for(unsigned i=0;name[i]&&i+1<sizeof(next.layerName);i++)next.layerName[i]=name[i];
 next.nextGetInstanceProcAddr=labRuntimeGipa;next.nextCreateApiLayerInstance=lab_next_create;
 XrApiLayerCreateInfo layer{};layer.structType=XR_LOADER_INTERFACE_STRUCT_API_LAYER_CREATE_INFO;
 layer.structVersion=XR_API_LAYER_CREATE_INFO_STRUCT_VERSION;layer.structSize=sizeof(layer);layer.nextInfo=&next;
 return labTapeCreate(info,&layer,out);
}
static XrResult XRAPI_CALL lab_gipa(XrInstance inst,const char*name,PFN_xrVoidFunction*out){
 if(!out||!name)return XR_ERROR_VALIDATION_FAILURE;
 if(inst==XR_NULL_HANDLE&&streq(name,"xrCreateInstance")){*out=(PFN_xrVoidFunction)lab_create;return XR_SUCCESS;}
 return inst==XR_NULL_HANDLE?labRuntimeGipa(inst,name,out):labTapeGipa(inst,name,out);
}
static bool lab_load_backend(){
 wchar_t own[1024]{};U32 len=GetModuleFileNameW(nullptr,own,1024);if(!len||len>=1000)return false;
 while(len&&own[len-1]!=L'\\')--len;if(!len)return false;own[len]=0;
 wchar_t simPath[1200]{},tapePath[1200]{};wcat_(simPath,own,1200);wcat_(tapePath,own,1200);
#ifdef NOVA_LAB_NESTED_PROFILE
 wcat_(simPath,L"..\\..\\..\\diagnostics\\xr-sim\\xrsim.dll",1200);wcat_(tapePath,L"..\\..\\..\\diagnostics\\xr-tape\\xrtape_layer.dll",1200);
#else
 wcat_(simPath,L"..\\diagnostics\\xr-sim\\xrsim.dll",1200);wcat_(tapePath,L"..\\diagnostics\\xr-tape\\xrtape_layer.dll",1200);
#endif
 labSimModule=LoadLibraryW(simPath);labTapeModule=LoadLibraryW(tapePath);
 if(!labSimModule||!labTapeModule){logg.msg("LAB_BACKEND_LOAD_FAILED",GetLastError());return false;}
 auto negotiateRuntime=(PFN_xrNegotiateLoaderRuntimeInterface)GetProcAddress(labSimModule,"xrNegotiateLoaderRuntimeInterface");
 auto negotiateTape=(PFN_xrNegotiateLoaderApiLayerInterface)GetProcAddress(labTapeModule,"xrNegotiateLoaderApiLayerInterface");
 if(!negotiateRuntime||!negotiateTape)return false;
 XrNegotiateLoaderInfo li{};li.structType=XR_LOADER_INTERFACE_STRUCT_LOADER_INFO;li.structVersion=XR_LOADER_INFO_STRUCT_VERSION;
 li.structSize=sizeof(li);li.minInterfaceVersion=1;li.maxInterfaceVersion=1;li.minApiVersion=XR_MAKE_VERSION(1,0,0);li.maxApiVersion=XR_CURRENT_API_VERSION;
 XrNegotiateRuntimeRequest runtime{};runtime.structType=XR_LOADER_INTERFACE_STRUCT_RUNTIME_REQUEST;runtime.structVersion=XR_RUNTIME_INFO_STRUCT_VERSION;runtime.structSize=sizeof(runtime);
 if(XR_FAILED(negotiateRuntime(&li,&runtime))||!runtime.getInstanceProcAddr)return false;
 labRuntimeGipa=runtime.getInstanceProcAddr;
 if(XR_FAILED(labRuntimeGipa(XR_NULL_HANDLE,"xrCreateInstance",(PFN_xrVoidFunction*)&labRuntimeCreate))||!labRuntimeCreate)return false;
 XrNegotiateApiLayerRequest tape{};tape.structType=XR_LOADER_INTERFACE_STRUCT_API_LAYER_REQUEST;tape.structVersion=XR_API_LAYER_INFO_STRUCT_VERSION;tape.structSize=sizeof(tape);
 if(XR_FAILED(negotiateTape(&li,"XR_APILAYER_XRTAPE_recorder",&tape))||!tape.getInstanceProcAddr||!tape.createApiLayerInstance)return false;
 labTapeGipa=tape.getInstanceProcAddr;labTapeCreate=tape.createApiLayerInstance;getXR=lab_gipa;
 logg.msg("LAB_EXPLICIT_SIM_TAPE_BOUND",runtime.runtimeInterfaceVersion,tape.layerInterfaceVersion);
 return true;
}
