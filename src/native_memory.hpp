#pragma once
#include "handcore.hpp"
#include "memory_access.hpp"
namespace handcore_native {
using namespace nova;
struct NativeMemory {
 U32 base{},thread{};
 bool fastRead{},batch{};U32 batchDepth{};U64 permissionQueries{},permissionCacheHits{};
 struct Region {U64 begin{},end{};U32 hits{};} regions[12]{};U32 regionCount{};
 void begin_read_batch(){
  if(!fastRead||thread!=GetCurrentThreadId())return;
  if(batchDepth++==0){batch=true;regionCount=0;}
 }
 void end_read_batch(){if(thread!=GetCurrentThreadId()||!batchDepth)return;if(--batchDepth==0){batch=false;regionCount=0;}}
 void cancel_read_batch(){if(thread==GetCurrentThreadId()){batchDepth=0;batch=false;regionCount=0;}}
 bool read(U32 address,void*out,size_t n){
  if(!address||!out||!n||U64(address)+n>0x100000000ull)return false;
  // Cache permissions only inside ONE read-only admission operation. Values
  // are always read fresh. Revalidate after at most sixteen range hits.
  // Only this memory adapter's render owner can use/mutate its small cache.
  // Command and listen-server threads independently query every native range.
  bool owner=thread&&thread==GetCurrentThreadId(),cached=owner&&batch;
  if(cached)for(U32 i=0;i<regionCount;i++){auto&r=regions[i];if(U64(address)>=r.begin&&U64(address)+n<=r.end&&r.hits<16){++r.hits;++permissionCacheHits;memcpy(out,(const void*)(uintptr_t)address,n);return true;}}
  MEMORY_BASIC_INFORMATION m{};const void*p=(const void*)(uintptr_t)address;
  if(owner)++permissionQueries;
  int resident=native_memory_access::probe(p,n);if(resident<0)return false;
  U64 begin=0,end=0;
  if(resident>0){begin=U64(address&~4095u);end=begin+4096;}
  else{
   if(VirtualQuery(p,&m,sizeof(m))<sizeof(m)||m.State!=0x1000||(m.Protect&0x100))return false;
   U32 prot=m.Protect&0xff;if(prot==0||prot==0x01||prot==0x10)return false;
   if((uintptr_t)m.BaseAddress>address||U64(address)+n>U64((uintptr_t)m.BaseAddress)+m.RegionSize)return false;
   begin=U64((uintptr_t)m.BaseAddress);end=begin+m.RegionSize;
  }
  if(cached){
   U32 slot=regionCount;
   for(U32 i=0;i<regionCount;i++)if(regions[i].begin==begin&&regions[i].end==end){slot=i;break;}
   if(slot<12){regions[slot]={begin,end,0};if(slot==regionCount)++regionCount;}
  }
  memcpy(out,p,n);return true;
 }
 void set_position(U32 e,Vec3 p){cancel_read_batch();if(thread!=GetCurrentThreadId())return;using Fn=void(__thiscall*)(void*,const Vec3*);((Fn)(uintptr_t)(base+0x1eacd0))((void*)(uintptr_t)e,&p);}
 void set_angles(U32 e,Angles a){cancel_read_batch();if(thread!=GetCurrentThreadId())return;using Fn=void(__thiscall*)(void*,const Angles*);((Fn)(uintptr_t)(base+0x1ead40))((void*)(uintptr_t)e,&a);}
};
}
