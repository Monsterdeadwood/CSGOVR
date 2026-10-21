#pragma once
#include "native_memory.hpp"
// Private G1 experiment. No player/punch field or engine view angle is written.
// Both eyes, the palm mount and the movement reference consume one owned copy.
namespace stable_camera {
using namespace nova;
static bool enabled=true,opened=false,admitted=false;static Log log;
static handcore_native::NativeMemory memory;static U64 nextLog=0;static U32 owner=0;
static void mode(bool value){enabled=value;nextLog=0;if(opened)log.msg("G1_CAMERA_MODE",enabled,GetCurrentThreadId());}
static bool equal(U32 p,const U8*bytes,U32 n){U8 actual[64]{};if(n>64||!memory.read(p,actual,n))return false;for(U32 i=0;i<n;i++)if(actual[i]!=bytes[i])return false;return true;}
static const void*prepare(const void*stock,U8*copy,U8*client,U8*engine,U64 frame,U64 sample){
 if(!stock||!copy||!client||!engine)return stock;
 U32 thread=GetCurrentThreadId();if(owner&&owner!=thread)return stock;owner=thread;
 if(!opened){log.open(L"nova_camera_g1.log");opened=true;log.msg("G1_CAMERA_OBSERVER_START",thread);}
 U32 c=U32((uintptr_t)client),e=U32((uintptr_t)engine),object=0,vt=0,fn=0;
 if(!memory.read(c+0x533478c,&object,4)||!memory.read(object,&vt,4)||vt!=e+0x47d850||!memory.read(vt+18*4,&fn,4)||fn!=e+0xb6940)return stock;
 if(!admitted){
  const U8 prefix[]={0x55,0x8b,0xec,0x56,0x57,0x8d,0x7d,0x04,0x8b,0x0f};
  const U8 tail[]={0x8b,0x4d,0x08,0x8b,0x86,0x90,0x4d,0,0,0x5f,0x89,0x01,0x8b,0x86,0x94,0x4d,0,0,0x89,0x41,0x04,0x8b,0x86,0x98,0x4d,0,0,0x89,0x41,0x08,0x5e,0x5d,0xc2,0x04,0};
  if(!equal(fn,prefix,sizeof(prefix))||!equal(fn+0x30,tail,sizeof(tail)))return stock;
  admitted=true;log.msg("G1_ENGINE_ANGLES_GETTER_ADMITTED",0xb6940,18,thread);
 }
 Angles base{},original{};
 using Get=void(__thiscall*)(void*,Angles&);((Get)(uintptr_t)fn)((void*)(uintptr_t)object,base);
 if(!finite_in_range(base.p,10000)||!finite_in_range(base.y,10000)||!finite_in_range(base.r,10000)||!memory.read(U32((uintptr_t)stock)+0xc4,&original,sizeof(original)))return stock;
 if(!finite_in_range(original.p,10000)||!finite_in_range(original.y,10000)||!finite_in_range(original.r,10000))return stock;
 bool applied=enabled&&memory.read(U32((uintptr_t)stock),copy,0x14c);
 if(applied)memcpy(copy+0xc4,&base,sizeof(base));
 U64 now=GetTickCount64();
 if(now>=nextLog){
  nextLog=now+50;char line[640];Angles result=applied?base:original;
  int n=_snprintf(line,sizeof(line),"NOVA_G1_CAMERA tick=%llu frame=%llu sample=%llu enabled=%u applied=%u stock=(%.7f,%.7f,%.7f) base=(%.7f,%.7f,%.7f) output=(%.7f,%.7f,%.7f)\r\n",now,frame,sample,enabled?1:0,applied?1:0,double(original.p),double(original.y),double(original.r),double(base.p),double(base.y),double(base.r),double(result.p),double(result.y),double(result.r));
  if(n>0&&log.file!=BAD_HANDLE){U32 written=0;WriteFile(log.file,line,U32(n<639?n:639),&written,nullptr);}
 }
 return applied?copy:stock;
}
}
