#pragma once
#include "platform.hpp"
// Public Microsoft D3D9 / D3D11 / DXGI vtable ABI subsets, newly declared for NOVA.
struct Com {void** v;
 ULONG add(){return ((ULONG(WINCALL*)(void*))v[1])(this);}ULONG release(){return ((ULONG(WINCALL*)(void*))v[2])(this);}
 HRESULT qi(REFIID id,void**out){return ((HRESULT(WINCALL*)(void*,REFIID,void**))v[0])(this,id,out);}
};
struct D9SurfaceDesc{U32 fmt,type,usage,pool,multisampleType,multisampleQuality,width,height;};
struct D9Surface:Com {HRESULT desc(D9SurfaceDesc*d){return ((HRESULT(WINCALL*)(void*,D9SurfaceDesc*))v[12])(this,d);}};
struct D9Texture:Com {HRESULT surface(D9Surface**out){return ((HRESULT(WINCALL*)(void*,U32,D9Surface**))v[18])(this,0,out);}};
struct D9Query:Com {HRESULT issue(U32 flags){return ((HRESULT(WINCALL*)(void*,U32))v[6])(this,flags);}HRESULT end(){return issue(1);}HRESULT data(void*out,U32 size){return ((HRESULT(WINCALL*)(void*,void*,U32,U32))v[7])(this,out,size,0);}HRESULT poll(){return data(nullptr,0);}};
struct D9View{U32 X,Y,Width,Height;float MinZ,MaxZ;};
struct D9Device:Com{
 HRESULT back(D9Surface**p){return ((HRESULT(WINCALL*)(void*,U32,U32,U32,D9Surface**))v[18])(this,0,0,0,p);}
 HRESULT texture(U32 w,U32 h,U32 fmt,D9Texture**t,HANDLE*share){return ((HRESULT(WINCALL*)(void*,U32,U32,U32,U32,U32,U32,D9Texture**,HANDLE*))v[23])(this,w,h,1,1,fmt,0,t,share);}
 HRESULT depth(U32 w,U32 h,D9Surface**s){return ((HRESULT(WINCALL*)(void*,U32,U32,U32,U32,U32,U32,BOOL,D9Surface**,HANDLE*))v[29])(this,w,h,75,0,0,1,0,s,nullptr);}
 HRESULT stretch(D9Surface*src,const RECT*sr,D9Surface*dst,const RECT*dr,U32 filter=0){return ((HRESULT(WINCALL*)(void*,D9Surface*,const RECT*,D9Surface*,const RECT*,U32))v[34])(this,src,sr,dst,dr,filter);}
 HRESULT target(U32 w,U32 h,U32 fmt,D9Surface**out){return ((HRESULT(WINCALL*)(void*,U32,U32,U32,U32,U32,BOOL,D9Surface**,HANDLE*))v[28])(this,w,h,fmt,0,0,0,out,nullptr);}
 HRESULT color(D9Surface*surf,U32 color){return ((HRESULT(WINCALL*)(void*,D9Surface*,const RECT*,U32))v[35])(this,surf,nullptr,color);}
 HRESULT setrt(D9Surface*s){return ((HRESULT(WINCALL*)(void*,U32,D9Surface*))v[37])(this,0,s);}
 HRESULT getrt(D9Surface**out){return ((HRESULT(WINCALL*)(void*,U32,D9Surface**))v[38])(this,0,out);}
 HRESULT setds(D9Surface*s){return ((HRESULT(WINCALL*)(void*,D9Surface*))v[39])(this,s);}
 HRESULT getds(D9Surface**out){return ((HRESULT(WINCALL*)(void*,D9Surface**))v[40])(this,out);}
 HRESULT setvp(const D9View*vp){return ((HRESULT(WINCALL*)(void*,const D9View*))v[47])(this,vp);}
 HRESULT getvp(D9View*vout){return ((HRESULT(WINCALL*)(void*,D9View*))v[48])(this,vout);}
 HRESULT typed_query(U32 type,D9Query**out){return ((HRESULT(WINCALL*)(void*,U32,D9Query**))v[118])(this,type,out);}
 HRESULT query(D9Query**out){return typed_query(8,out);}
};
struct D3DTextureDesc{U32 width,height,mips,array,format,scount,squality,usage,bind,cpu,misc;};
struct D3DInitialData{const void*data;U32 pitch,slicePitch;};
struct D3DTexture:Com{void desc(D3DTextureDesc*out){((void(WINCALL*)(void*,D3DTextureDesc*))v[10])(this,out);}};
struct D3DDevice:Com{
 HRESULT create(const D3DTextureDesc*desc,D3DTexture**t){return ((HRESULT(WINCALL*)(void*,const D3DTextureDesc*,const void*,D3DTexture**))v[5])(this,desc,nullptr,t);}
 HRESULT create_data(const D3DTextureDesc*desc,const D3DInitialData*data,D3DTexture**t){return ((HRESULT(WINCALL*)(void*,const D3DTextureDesc*,const D3DInitialData*,D3DTexture**))v[5])(this,desc,data,t);}
 HRESULT removed(){return ((HRESULT(WINCALL*)(void*))v[39])(this);}
};
struct D3DContext:Com {
 void copy(D3DTexture*d,D3DTexture*s){((void(WINCALL*)(void*,D3DTexture*,D3DTexture*))v[47])(this,d,s);}
 void flush(){((void(WINCALL*)(void*))v[111])(this);}
};
struct DXGIAdapter:Com{HRESULT desc(DXGI_ADAPTER_DESC1*d){return ((HRESULT(WINCALL*)(void*,DXGI_ADAPTER_DESC1*))v[10])(this,d);}};
struct DXGIFactory:Com{HRESULT at(U32 n,DXGIAdapter**a){return ((HRESULT(WINCALL*)(void*,U32,DXGIAdapter**))v[12])(this,n,a);}};
struct DXGIResource:Com{HRESULT handle(HANDLE*h){return ((HRESULT(WINCALL*)(void*,HANDLE*))v[8])(this,h);}};
static const GUID IID_DXGI_FACTORY1={0x770aae78,0xf26f,0x4dba,{0xa8,0x29,0x25,0x3c,0x83,0xd1,0xb3,0x87}};
static const GUID IID_DXGI_RESOURCE={0x035f3ab4,0x482e,0x4e50,{0xb4,0x1f,0x8a,0x7f,0x8b,0xd8,0x96,0x0b}};
static const GUID IID_D3D_TEXTURE={0x6f15aaf2,0xd208,0x4e89,{0x9a,0xb4,0x48,0x95,0x35,0xd3,0x4f,0x9c}};
extern "C" {
 WINIMPORT HRESULT WINCALL D3D11CreateDevice(DXGIAdapter*,U32,HMODULE,U32,const U32*,U32,U32,D3DDevice**,U32*,D3DContext**);
 WINIMPORT HRESULT WINCALL CreateDXGIFactory1(REFIID,void**);
}


struct D3D11_QUERY_DESC{U32 Query;U32 MiscFlags;};
struct D3DQuery:Com{};
