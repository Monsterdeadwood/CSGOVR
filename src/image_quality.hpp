#pragma once
#include "gfx.hpp"
// An owned D3D11 image pass. Source's render, projection, skeleton and shot
// coordinates are untouched. Only acquired stereo swapchain images are drawn.
namespace image_quality {
using namespace nova;
struct Blob:Com {void*data(){return ((void* (WINCALL*)(void*))v[3])(this);}SIZE_T size(){return ((SIZE_T (WINCALL*)(void*))v[4])(this);}};
struct Sampler {U32 filter,addressU,addressV,addressW;float bias;U32 anisotropy,comparison;float border[4],minLod,maxLod;};
struct Raster {U32 fill,cull;BOOL front;I32 bias;float clamp,slope;BOOL clip,scissor,multisample,line;};
struct Viewport {float x,y,width,height,minDepth,maxDepth;};
static_assert(sizeof(Sampler)==52&&sizeof(Raster)==40&&sizeof(Viewport)==24,"D3D11 descriptor ABI");
struct alignas(8) Control {U32 magic,version,bytes,gamePid,hostPid;volatile I32 request,ack,enabled;U32 ready,errors;U64 passes;U32 width,height;float sharpness;I32 aa;};
static_assert(sizeof(Control)==64,"Owned image control ABI");
static constexpr U32 MAGIC=0x5149564e;
struct View {D3DTexture*texture{};Com*object{};U32 format{};};
struct Pass {
 D3DDevice*device{};D3DContext*context{};Log*log{};Com*vertex{},*pixel{},*sampler{},*raster{},*constants{};
 View sources[8]{},targets[64]{};U32 sourceCount{},targetCount{};bool ready{},enabled{true},aa{true};float sharpness{.25f};HANDLE mapping{};Control*control{};
 void error(const char*name,HRESULT hr){if(control)control->errors++;if(log)log->msg(name,U32(hr));}
 bool compile(const char*source,const char*entry,const char*profile,Blob**out){
  HMODULE module=LoadLibraryW(L"d3dcompiler_47.dll");if(!module)return false;
  using Compile=HRESULT(WINCALL*)(const void*,SIZE_T,const char*,const void*,void*,const char*,const char*,U32,U32,Blob**,Blob**);
  auto fn=(Compile)GetProcAddress(module,"D3DCompile");if(!fn)return false;SIZE_T length=0;while(source[length])length++;
  Blob*errors=nullptr;HRESULT hr=fn(source,length,"nova_owned_image",nullptr,nullptr,entry,profile,1u<<15,0,out,&errors);
  if(errors){if(failed(hr)&&log&&log->file!=BAD_HANDLE){U32 done=0;SIZE_T n=errors->size();if(n>4096)n=4096;WriteFile(log->file,errors->data(),U32(n),&done,nullptr);}errors->release();}
  if(failed(hr)||!*out){error("IMAGE_AA_COMPILE_FAILED",hr);return false;}return true;
 }
 bool initialize(D3DDevice*d,D3DContext*c,Log&logger,U32 pid,U32 host,U32 w,U32 h){
  device=d;context=c;log=&logger;wchar_t option[8]{};if(GetEnvironmentVariableW(L"NOVA_IMAGE_AA",option,8)>0)enabled=option[0]!=L'0';
  wchar_t name[64]{};const wchar_t*prefix=L"Local\\NOVA_IMAGE_QUALITY_";U32 n=0;while(prefix[n]){name[n]=prefix[n];n++;}decimal_(name+n,pid);
  mapping=CreateFileMappingW(BAD_HANDLE,nullptr,4,0,sizeof(Control),name);
  if(mapping&&GetLastError()!=183){control=(Control*)MapViewOfFile(mapping,0xf001f,0,0,sizeof(Control));if(control){memset(control,0,sizeof(*control));control->version=1;control->bytes=sizeof(Control);control->gamePid=pid;control->hostPid=host;control->enabled=enabled;control->width=w;control->height=h;control->sharpness=sharpness;control->aa=aa;barrier();control->magic=MAGIC;}}
  static const char shader[]=R"HLSL(
Texture2D<float4> image:register(t0);SamplerState filterSampler:register(s0);
cbuffer Settings:register(b0){float aaEnabled;float sharpness;float destinationSrgb;float padding;};
float4 vertex(uint id:SV_VertexID):SV_Position {float2 p=float2((id<<1)&2,id&2);return float4(p*float2(2,-2)+float2(-1,1),0,1);}
float luma(float3 c){return dot(c,float3(.299,.587,.114));}
float4 pixel(float4 position:SV_Position):SV_Target {
 uint width,height;image.GetDimensions(width,height);float2 texel=1.0/float2(width,height);float2 uv=position.xy*texel;
 float3 center=image.SampleLevel(filterSampler,uv,0).rgb;
 float3 nw=image.SampleLevel(filterSampler,uv+float2(-1,-1)*texel,0).rgb;
 float3 ne=image.SampleLevel(filterSampler,uv+float2(1,-1)*texel,0).rgb;
 float3 sw=image.SampleLevel(filterSampler,uv+float2(-1,1)*texel,0).rgb;
 float3 se=image.SampleLevel(filterSampler,uv+float2(1,1)*texel,0).rgb;
 float m=luma(center),a=luma(nw),b=luma(ne),c=luma(sw),d=luma(se);
 float lo=min(m,min(min(a,b),min(c,d))),hi=max(m,max(max(a,b),max(c,d)));
 float3 result=center;
 if(aaEnabled>.5&&hi-lo>=max(.0312,hi*.125)){
 float2 direction=float2(-((a+b)-(c+d)),(a+c)-(b+d));
 float reduce=max((a+b+c+d)*(.25*.125),1.0/128.0);
 direction=clamp(direction/(min(abs(direction.x),abs(direction.y))+reduce),-8.0,8.0)*texel;
 float3 first=.5*(image.SampleLevel(filterSampler,uv+direction*(-1.0/6.0),0).rgb+image.SampleLevel(filterSampler,uv+direction*(1.0/6.0),0).rgb);
 float3 second=first*.5+.25*(image.SampleLevel(filterSampler,uv+direction*(-.5),0).rgb+image.SampleLevel(filterSampler,uv+direction*.5,0).rgb);
 float value=luma(second);result=value<lo||value>hi?first:second;}
 if(sharpness>0){
  float3 north=image.SampleLevel(filterSampler,uv+float2(0,-1)*texel,0).rgb;
  float3 south=image.SampleLevel(filterSampler,uv+float2(0,1)*texel,0).rgb;
  float3 west=image.SampleLevel(filterSampler,uv+float2(-1,0)*texel,0).rgb;
  float3 east=image.SampleLevel(filterSampler,uv+float2(1,0)*texel,0).rgb;
  float3 minimum=min(center,min(min(north,south),min(west,east)));
  float3 maximum=max(center,max(max(north,south),max(west,east)));
  result=clamp(result+sharpness*(center-(north+south+west+east)*.25),minimum,maximum);
 }
 // Source contains the accepted game's display-encoded BGRA bytes. An sRGB
 // RTV encodes linear shader output; decode first to preserve those bytes.
 if(destinationSrgb>.5){
  result=float3(result.r<=.04045?result.r/12.92:pow((result.r+.055)/1.055,2.4),
                result.g<=.04045?result.g/12.92:pow((result.g+.055)/1.055,2.4),
                result.b<=.04045?result.b/12.92:pow((result.b+.055)/1.055,2.4));
 }
 return float4(result,1);
})HLSL";
  Blob*vs=nullptr,*ps=nullptr;if(!compile(shader,"vertex","vs_4_0",&vs))return false;
  HRESULT hr=((HRESULT(WINCALL*)(void*,const void*,SIZE_T,void*,Com**))device->v[12])(device,vs->data(),vs->size(),nullptr,&vertex);vs->release();if(failed(hr)||!vertex){error("IMAGE_AA_VERTEX_FAILED",hr);return false;}
  if(!compile(shader,"pixel","ps_4_0",&ps))return false;
  hr=((HRESULT(WINCALL*)(void*,const void*,SIZE_T,void*,Com**))device->v[15])(device,ps->data(),ps->size(),nullptr,&pixel);ps->release();if(failed(hr)||!pixel){error("IMAGE_AA_PIXEL_FAILED",hr);return false;}
  Sampler s{0x15,3,3,3,0,1,8,{0,0,0,0},0,3.402823e38f};
  hr=((HRESULT(WINCALL*)(void*,const Sampler*,Com**))device->v[23])(device,&s,&sampler);if(failed(hr)||!sampler){error("IMAGE_AA_SAMPLER_FAILED",hr);return false;}
  Raster r{3,1,0,0,0,0,1,0,0,0};hr=((HRESULT(WINCALL*)(void*,const Raster*,Com**))device->v[22])(device,&r,&raster);if(failed(hr)||!raster){error("IMAGE_AA_RASTER_FAILED",hr);return false;}
  U32 bufferDesc[6]={16,0,4,0,0,0};hr=((HRESULT(WINCALL*)(void*,const void*,const void*,Com**))device->v[3])(device,bufferDesc,nullptr,&constants);if(failed(hr)||!constants){error("IMAGE_QUALITY_CONSTANTS_FAILED",hr);return false;}
  ready=true;if(control)control->ready=1;log->msg("IMAGE_QUALITY_READY_FXAA_SHARPEN",w,h,enabled);return true;
 }
 void poll(){if(!control)return;I32 request=load32(&control->request);if(request==I32(load32(&control->ack)))return;barrier();enabled=load32(&control->enabled)!=0;aa=control->aa!=0;float value=control->sharpness;sharpness=value>=0&&value<=.6f?value:.25f;store32(&control->ack,request);log->msg("IMAGE_QUALITY_LIVE_ENABLED",enabled,aa,U32(sharpness*1000));}
 Com*view(D3DTexture*t,bool target,U32 format=0){
  View*cache=target?targets:sources;U32&count=target?targetCount:sourceCount;U32 capacity=target?64:8;
  for(U32 i=0;i<count;i++)if(cache[i].texture==t&&cache[i].format==format)return cache[i].object;
  if(count==capacity)return nullptr;Com*object=nullptr;U32 desc[5]={format,4,0,0,0};
  HRESULT hr=((HRESULT(WINCALL*)(void*,void*,const void*,Com**))device->v[target?9:7])(device,t,target&&format?desc:nullptr,&object);
  if(failed(hr)||!object){error("IMAGE_AA_VIEW_FAILED",hr);return nullptr;}cache[count++]={t,object,format};return object;
 }
 bool draw(D3DTexture*from,D3DTexture*to,bool applyQuality=true,U32 targetFormat=0){
  if(!ready||(applyQuality&&!enabled)||!from||!to)return false;D3DTextureDesc a{},b{};from->desc(&a);to->desc(&b);
  U32 format=targetFormat?targetFormat:b.format;
  bool compatible=(format==28||format==29)?(b.format==27||b.format==format):(format==87||format==91)?(b.format==90||b.format==format):false;
  if(a.width!=b.width||a.height!=b.height||a.format!=87||!compatible||a.scount!=1||b.scount!=1)return false;
  Com*srv=view(from,false),*rtv=view(to,true,format);if(!srv||!rtv)return false;void**v=context->v;
  float settings[4]={(applyQuality&&aa)?1.f:0.f,applyQuality?sharpness:0.f,(format==29||format==91)?1.f:0.f,0};((void(WINCALL*)(void*,Com*,U32,const void*,const void*,U32,U32))v[48])(context,constants,0,nullptr,settings,0,0);
  ((void(WINCALL*)(void*,U32,U32,Com*const*))v[16])(context,0,1,&constants);
  ((void(WINCALL*)(void*,U32,Com*const*,void*))v[33])(context,1,&rtv,nullptr);
  Viewport viewport{0,0,float(a.width),float(a.height),0,1};((void(WINCALL*)(void*,U32,const Viewport*))v[44])(context,1,&viewport);
  ((void(WINCALL*)(void*,Com*))v[43])(context,raster);((void(WINCALL*)(void*,void*))v[17])(context,nullptr);((void(WINCALL*)(void*,U32))v[24])(context,4);
  ((void(WINCALL*)(void*,Com*,void*,U32))v[11])(context,vertex,nullptr,0);((void(WINCALL*)(void*,Com*,void*,U32))v[9])(context,pixel,nullptr,0);
  ((void(WINCALL*)(void*,U32,U32,Com*const*))v[8])(context,0,1,&srv);((void(WINCALL*)(void*,U32,U32,Com*const*))v[10])(context,0,1,&sampler);
  ((void(WINCALL*)(void*,U32,U32))v[13])(context,3,0);
  Com*empty=nullptr;((void(WINCALL*)(void*,U32,U32,Com*const*))v[8])(context,0,1,&empty);((void(WINCALL*)(void*,U32,Com*const*,void*))v[33])(context,0,nullptr,nullptr);
  if(control){control->passes++;if(control->passes<4||control->passes%480==0)log->msg("IMAGE_AA_DRAW",control->passes,a.width,a.height);}return true;
 }
 void cleanup(){
  if(context&&ready){Com*empty=nullptr;((void(WINCALL*)(void*,U32,U32,Com*const*))context->v[8])(context,0,1,&empty);((void(WINCALL*)(void*,U32,Com*const*,void*))context->v[33])(context,0,nullptr,nullptr);((void(WINCALL*)(void*,Com*,void*,U32))context->v[9])(context,nullptr,nullptr,0);((void(WINCALL*)(void*,Com*,void*,U32))context->v[11])(context,nullptr,nullptr,0);context->flush();}
  for(U32 i=0;i<sourceCount;i++)if(sources[i].object)sources[i].object->release();for(U32 i=0;i<targetCount;i++)if(targets[i].object)targets[i].object->release();
  Com*owned[5]={vertex,pixel,sampler,raster,constants};for(auto*p:owned)if(p)p->release();
  if(control)UnmapViewOfFile(control);if(mapping)CloseHandle(mapping);ready=false;control=nullptr;mapping=nullptr;
 }
};
}
