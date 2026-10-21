#pragma once
// Included inside scene32's graphics owner. Geometry/VS/depth/viewport remain
// native. Only the admitted VM material's original indexed draw is shaded.
#include "ray_optics.hpp"
#include "optic_surface_profiles.hpp"
namespace lens_material {
struct Blob:Com{void*data(){return ((void*(WINCALL*)(void*))v[3])(this);}SIZE_T size(){return ((SIZE_T(WINCALL*)(void*))v[4])(this);}};
using Bind=void(__thiscall*)(void*,void*,void*);
using Draw=HRESULT(WINCALL*)(D9Device*,U32,I32,U32,U32,U32,U32);
using Paint=void(__thiscall*)(void*);static Paint originalPaint{};static void**paintCell{};
static U64 hudSuppressed{};
static Vec3 eyeWorld[2]{},lensWorld{};static Basis boreBasis{};static float zoom{},halfTan{};static ray_optics::Slopes reticle{};
static Bind originalBind{};static Draw originalDraw{};static void**bindCell{},**drawCell{};
static void*nativeTarget{};static D9Texture*image{};static D9Surface*imageSurface{};static Com*pixelShader[2]{};
static bool viewActive{},selected{},imageReady{},installed{};static U32 expectedVm{},model{};static U64 pair{},imagePair{},sourcePair{},draws{},restores{};static int currentEye{};
static U64 imageAttempts{},imageReasons[9]{},fallbackDraws{};
static bool supported(U32 checksum){return ray_optics::profile(checksum)!=nullptr;}
static bool same_text(const char*a,const char*b){if(!a||!b)return false;for(U32 k=0;k<160;k++){if(a[k]!=b[k])return false;if(!a[k])return true;}return false;}
static void __fastcall paint_hook(void*self,void*){
 if(viewActive&&GetCurrentThreadId()==graphicsThread&&readable(self,4)&&*(void**)self==client+0xc44398){
  ++hudSuppressed;if(hudSuppressed<=4||hudSuppressed%240==0)logg.msg("O5_SCOPE_HUD_ONLY_SUPPRESSED",pair,currentEye,hudSuppressed);return;
 }
 originalPaint(self);
}
static void __fastcall bind_hook(void*self,void*,void*value,void*proxy){
 originalBind(self,value,proxy);selected=false;
 const auto*profile=ray_optics::profile(model);
 if(!viewActive||!profile||GetCurrentThreadId()!=graphicsThread||!value||proxy!=(void*)(uintptr_t)(expectedVm+4)||!readable(value,4))return;
 auto**vt=*(void***)value;U8*getter=(U8*)vt[0];
 const U8 prefix[]={0x8b,0x49,4,0x8b,1,0xff,0x20};if(!bytes(mat,0x10070,prefix,sizeof(prefix))||getter!=mat+0x10070)return;
 const char*name=((const char*(__thiscall*)(void*))getter)(value);selected=readable(name,1)&&same_text(name,profile->material);
}
static bool contains(const char*text,SIZE_T length,const char*needle){
 SIZE_T n=0;while(needle[n])++n;if(n>length)return false;
 for(SIZE_T i=0;i+n<=length;i++){SIZE_T k=0;while(k<n&&text[i+k]==needle[k])++k;if(k==n)return true;}return false;
}
using Disassemble=HRESULT(WINCALL*)(const void*,SIZE_T,U32,const char*,Blob**);static Disassemble disassemble{};
struct ShaderContract{Com*shader{};U32 semantic{},hash{};bool admitted{};};
static ShaderContract shaderCache[12]{};static U32 shaderCount{};static U8 shaderBytes[32768]{};
static bool contract(D9Device*d,U32 semantic){
 Com*shader{};if(failed(((HRESULT(WINCALL*)(void*,Com**))d->v[93])(d,&shader))||!shader)return false;
 for(U32 k=0;k<shaderCount;k++)if(shaderCache[k].shader==shader){bool result=shaderCache[k].admitted&&shaderCache[k].semantic==semantic;shader->release();return result;}
 if(shaderCount==12){shader->release();return false;}
 U32 n=0;auto get=(HRESULT(WINCALL*)(void*,void*,U32*))shader->v[4];Blob*assembly{};
 bool good=ok(get(shader,nullptr,&n))&&n>=16&&n<=sizeof(shaderBytes)&&ok(get(shader,shaderBytes,&n));
 U32 hash=2166136261u;if(good)for(U32 i=0;i<n;i++){hash^=shaderBytes[i];hash*=16777619u;}
 good=good&&*(U32*)shaderBytes==0xfffe0300u&&disassemble&&ok(disassemble(shaderBytes,n,0,nullptr,&assembly))&&assembly;
 bool found=false;
 if(good){
  const char*text=(const char*)assembly->data();SIZE_T size=assembly->size();char decl[50],move[50],project[50];
  _snprintf(decl,sizeof(decl),"dcl_texcoord%u o%u",semantic,semantic+1);
  if(contains(text,size,decl))for(U32 r=0;r<32;r++){
   _snprintf(move,sizeof(move),"mov o%u.xyz, r%u\n",semantic+1,r);
   _snprintf(project,sizeof(project),", r%u, c8\n",r);
   if(contains(text,size,move)&&contains(text,size,project)){found=true;break;}
  }
 }
 if(assembly)assembly->release();good=good&&found;
 shaderCache[shaderCount++]={shader,semantic,hash,good};
 logg.msg(good?"O5_NATIVE_WORLD_SEMANTIC_ADMITTED":"O5_NATIVE_WORLD_SEMANTIC_REJECTED",semantic,hash,n);return good;
}
static HRESULT WINCALL draw_hook(D9Device*d,U32 type,I32 base,U32 min,U32 vertices,U32 first,U32 primitives){
 const auto*profile=ray_optics::profile(model);
 if(!viewActive||!selected||!profile||d!=gpu||GetCurrentThreadId()!=graphicsThread||!contract(d,profile->semantic))return originalDraw(d,type,base,min,vertices,first,primitives);
 bool currentImage=imageReady&&imagePair==pair&&muzzle_native::body_authority::drawSeen[currentEye];
 using GetObject=HRESULT(WINCALL*)(void*,void**);using SetObject=HRESULT(WINCALL*)(void*,void*);
 Com*oldPS{};Com*oldTexture{};float oldConstant[20]{};
 auto getPS=(GetObject)d->v[108];auto setPS=(SetObject)d->v[107];
 auto getTexture=(HRESULT(WINCALL*)(void*,U32,void**))d->v[64];auto setTexture=(HRESULT(WINCALL*)(void*,U32,void*))d->v[65];
 auto getRS=(HRESULT(WINCALL*)(void*,U32,U32*))d->v[58];auto setRS=(HRESULT(WINCALL*)(void*,U32,U32))d->v[57];
 auto getSS=(HRESULT(WINCALL*)(void*,U32,U32,U32*))d->v[68];auto setSS=(HRESULT(WINCALL*)(void*,U32,U32,U32))d->v[69];
 auto getCF=(HRESULT(WINCALL*)(void*,U32,float*,U32))d->v[110];auto setCF=(HRESULT(WINCALL*)(void*,U32,const float*,U32))d->v[109];
 const U32 states[]={15,27,194};U32 previous[3]{};const U32 samplers[]={1,2,5,6,7,11};U32 oldSampler[6]{};
 bool good=ok(getPS(d,(void**)&oldPS))&&ok(getTexture(d,0,(void**)&oldTexture))&&ok(getCF(d,0,oldConstant,5));
 for(U32 k=0;k<3;k++)good=ok(getRS(d,states[k],&previous[k]))&&good;
 for(U32 k=0;k<6;k++)good=ok(getSS(d,0,samplers[k],&oldSampler[k]))&&good;
 if(!good){if(oldPS)oldPS->release();if(oldTexture)oldTexture->release();return originalDraw(d,type,base,min,vertices,first,primitives);}
 Vec3 eye=eyeWorld[currentEye];float values[20]={eye.x,eye.y,eye.z,zoom,boreBasis.forward.x,boreBasis.forward.y,boreBasis.forward.z,halfTan,
  boreBasis.right.x,boreBasis.right.y,boreBasis.right.z,reticle.x,boreBasis.up.x,boreBasis.up.y,boreBasis.up.z,reticle.y,currentImage?1.f:0.f,0,0,0};
 good=ok(setPS(d,pixelShader[profile->semantic==3?0:1]))&&ok(setTexture(d,0,image))&&ok(setCF(d,0,values,5));
 for(U32 state:states)good=ok(setRS(d,state,0))&&good;
 const U32 settings[]={3,3,2,2,0,0};for(U32 k=0;k<6;k++)good=ok(setSS(d,0,samplers[k],settings[k]))&&good;
 HRESULT result=good?originalDraw(d,type,base,min,vertices,first,primitives):HRESULT(0x80004005u);
 bool restored=ok(setPS(d,oldPS));restored=ok(setTexture(d,0,oldTexture))&&restored;restored=ok(setCF(d,0,oldConstant,5))&&restored;
 for(U32 k=0;k<3;k++)restored=ok(setRS(d,states[k],previous[k]))&&restored;
 for(U32 k=0;k<6;k++)restored=ok(setSS(d,0,samplers[k],oldSampler[k]))&&restored;
 if(!good&&restored)result=originalDraw(d,type,base,min,vertices,first,primitives);
 if(oldPS)oldPS->release();if(oldTexture)oldTexture->release();++draws;if(restored)++restores;else imageReady=false;
 if(!currentImage)++fallbackDraws;
 if(draws<=4||draws%120==0){char text[400];int n=_snprintf(text,sizeof(text),"NOVA_O5_RAY_LENS_DRAW pair=%llu sourcePair=%llu imagePair=%llu eye=%d checksum=%u vm=%08x vertices=%u primitives=%u hr=%u restored=%u\r\n",pair,sourcePair,imagePair,currentEye,model,expectedVm,vertices,primitives,U32(result),restored?1:0);U32 written=0;if(n>0)WriteFile(logg.file,text,U32(n<399?n:399),&written,nullptr);}
 return result;
}
static bool make_target(){
 using Create=void*(__thiscall*)(void*,const char*,int,int,int,int,int,U32,U32);
 char name[72];_snprintf(name,sizeof(name),"_nova_native_lens_%u",GetCurrentProcessId());
 nativeTarget=((Create)(*(void***)material)[97])(material,name,768,768,8,12,1,0xc,0);
 if(!nativeTarget||failed(gpu->texture(768,768,21,&image,nullptr))||!image||failed(image->surface(&imageSurface))||!imageSurface)return false;
 HMODULE compiler=LoadLibraryW(L"d3dcompiler_47.dll");
 using Compile=HRESULT(WINCALL*)(const void*,SIZE_T,const char*,const void*,void*,const char*,const char*,U32,U32,Blob**,Blob**);
 auto compile=compiler?(Compile)GetProcAddress(compiler,"D3DCompile"):nullptr;disassemble=compiler?(Disassemble)GetProcAddress(compiler,"D3DDisassemble"):nullptr;if(!compile||!disassemble)return false;
 const char body[]="sampler2D scene:register(s0);float4 eyeZoom:register(c0);float4 forwardTan:register(c1);float4 rightHair:register(c2);float4 upHair:register(c3);float4 validity:register(c4);float4 main(float3 world:TEXCOORD%u):COLOR0{if(validity.x<.5)return float4(0,0,0,1);float3 d=world-eyeZoom.xyz;float z=dot(d,forwardTan.xyz);if(z<=.0001)return float4(0,0,0,1);float2 q=float2(dot(d,rightHair.xyz),dot(d,upHair.xyz))/(z*eyeZoom.w);float2 uv=.5+float2(q.x,-q.y)/forwardTan.w*.5;if(any(uv<0)||any(uv>1))return float4(0,0,0,1);float3 color=tex2D(scene,uv).rgb;float2 p=q-float2(rightHair.w,upHair.w);float2 pixels=abs(p)/max(fwidth(p),.000001);float hair=1-smoothstep(.55,1.25,min(pixels.x,pixels.y));return float4(lerp(color,float3(.01,.01,.01),hair),1);}";
 for(int k=0;k<2;k++){
  char source[1800];int count=_snprintf(source,sizeof(source),body,k?4:3);Blob*code{},*errors{};
  HRESULT hr=compile(source,count,"nova_angular_ray_lens",nullptr,nullptr,"main","ps_3_0",0,0,&code,&errors);
  if(errors)errors->release();if(failed(hr)||!code)return false;
  hr=((HRESULT(WINCALL*)(void*,const U32*,Com**))gpu->v[106])(gpu,(const U32*)code->data(),&pixelShader[k]);code->release();if(failed(hr)||!pixelShader[k])return false;
 }
 return true;
}

static bool install(void*context){
 if(installed)return true;if(GetCurrentThreadId()!=graphicsThread||!readable(context,4))return false;
 bindCell=*(void***)context+9;drawCell=gpu->v+82;paintCell=(void**)(client+0xc44398)+128;
 const U8 prefix[]={0x55,0x8b,0xec,0x53,0x8b,0x5d,8,0x57,0x8b,0xf9,0x8b,0xcb};
 if(*bindCell!=mat+0x3c580||!bytes(mat,0x3c580,prefix,sizeof(prefix)))return false;
 const U8 paintPrefix[]={0x55,0x8b,0xec,0x83,0xe4,0xf8,0x83,0xec,0x78,0x56,0x89,0x4c,0x24,0x14};
 if(*paintCell!=client+0x4bdbb0||!bytes(client,0x4bdbb0,paintPrefix,sizeof(paintPrefix)))return false;
 originalPaint=(Paint)*paintCell;
 originalBind=(Bind)*bindCell;originalDraw=(Draw)*drawCell;
 if(!hook(bindCell,(void*)originalBind,(void*)bind_hook))return false;
 if(!hook(drawCell,(void*)originalDraw,(void*)draw_hook)){undo(bindCell,(void*)bind_hook,(void*)originalBind);return false;}
 if(!hook(paintCell,(void*)originalPaint,(void*)paint_hook)){undo(drawCell,(void*)draw_hook,(void*)originalDraw);undo(bindCell,(void*)bind_hook,(void*)originalBind);return false;}
 installed=true;logg.msg("O5_NATIVE_OPTICS_HOOKS_INSTALLED",graphicsThread);return true;
}
static void begin_view(bool active,U32 vm,U32 checksum,U64 id,int eye){viewActive=active&&supported(checksum);expectedVm=vm;model=checksum;pair=id;currentEye=eye;selected=false;}
static void end_view(){viewActive=false;selected=false;}
static void shutdown(){
 end_view();imageReady=false;
 if(installed){undo(paintCell,(void*)paint_hook,(void*)originalPaint);undo(drawCell,(void*)draw_hook,(void*)originalDraw);undo(bindCell,(void*)bind_hook,(void*)originalBind);installed=false;logg.msg("O5_NATIVE_OPTICS_HOOKS_RESTORED",draws,restores,hudSuppressed);}
 logg.msg("O6_OPTIC_IMAGE_COUNTS",imageAttempts,imageReasons[0],fallbackDraws);
 for(U32 reason=1;reason<9;reason++)if(imageReasons[reason])logg.msg("O6_OPTIC_IMAGE_REJECTIONS",reason,imageReasons[reason]);
 for(U32 i=0;i<shaderCount;i++)if(shaderCache[i].shader)shaderCache[i].shader->release();shaderCount=0;
 for(auto*&shader:pixelShader)if(shader){shader->release();shader=nullptr;}
 if(imageSurface)imageSurface->release();if(image)image->release();imageSurface=nullptr;image=nullptr;
}
}
