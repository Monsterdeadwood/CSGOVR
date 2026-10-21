#pragma once
#include "handcore.hpp"
// Exact-target MSVC RTTI, read only. Match the firearm base at offset zero;
// normalize the client C_ / server C spelling to bind the same native class.
namespace muzzle {
using namespace nova;using namespace handcore;
template<class Memory> bool gun_class(Memory&m,U32 base,U32 size,U32 vt,bool server,U32&tag){
 auto inside=[&](U32 a,U32 n){return a>=base&&U64(a)+n<=U64(base)+size;};
 auto rd=[&](U32 a,U32&v){return inside(a,4)&&m.read(a,&v,4);};
 auto name=[&](U32 td,char*out){
  if(!inside(td,9))return false;
  for(U32 j=0;j<96;j++){if(!inside(plus(td,8+j),1)||!m.read(plus(td,8+j),out+j,1))return false;if(!out[j])return j>6;if(out[j]<32||out[j]>126)return false;}
  return false;
 };
 U32 col=0,sig=1,off=1,cd=1,td=0,h=0,n=0,array=0;
 if(vt<4||!inside(vt,4)||!rd(vt-4,col)||!rd(col,sig)||sig||!rd(plus(col,4),off)||off||!rd(plus(col,8),cd)||cd||!rd(plus(col,12),td)||!rd(plus(col,16),h)||!rd(plus(h,8),n)||n<2||n>64||!rd(plus(h,12),array)||!inside(array,n*4))return false;
 char concrete[96]{};if(!name(td,concrete))return false;
 const char*prefix=server?".?AVC":".?AVC_";U32 skip=server?5:6;
 for(U32 i=0;i<skip;i++)if(concrete[i]!=prefix[i])return false;
 // These three native shotguns inherit CSBase directly in this exact build.
 // Their real EyeAngles/WeaponShootPosition callers are already admitted.
 // Do not broaden CSBase admission to knives, grenades or other items.
 bool standalone=streq(concrete,server?".?AVCWeaponNOVA@@":".?AVC_WeaponNOVA@@")||streq(concrete,server?".?AVCWeaponXM1014@@":".?AVC_WeaponXM1014@@")||streq(concrete,server?".?AVCWeaponSawedoff@@":".?AVC_WeaponSawedoff@@");
 bool found=false;
 for(U32 i=0;i<n;i++){
  U32 d=0,t=0,mdisp=1,pdisp=0;
  if(!rd(plus(array,i*4),d)||!rd(d,t))return false;
  if(i==0&&t!=td)return false;
  char baseName[96]{};if(!name(t,baseName))return false;
  if(streq(baseName,server?".?AVCWeaponCSBaseGun@@":".?AVC_WeaponCSBaseGun@@")||(standalone&&streq(baseName,server?".?AVCWeaponCSBase@@":".?AVC_WeaponCSBase@@"))){
   if(!rd(plus(d,8),mdisp)||mdisp||!rd(plus(d,12),pdisp)||pdisp!=0xffffffffu)return false;
   found=true;
  }
 }
 if(!found)return false;
 U32 hash=2166136261u;for(U32 i=skip;concrete[i];i++)hash=(hash^U8(concrete[i]))*16777619u;
 tag=hash;return tag!=0;
}
}
