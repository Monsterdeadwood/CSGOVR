#pragma once
#include "muzzle_core.hpp"
namespace weapon_fx {
using namespace muzzle;
// Native attachment indices remain 1-based. No weapon-specific offsets and no
// writes to the game's bone palette or formatted attachment cache.
struct Attachments {U32 count{},checksum{};Matrix matrices[64]{};};
inline bool same(const Attachments&a,const Attachments&b){
 if(!a.count||a.count>64||a.count!=b.count||a.checksum!=b.checksum)return false;
 for(U32 i=0;i<a.count;i++){
  if(!matrix_valid(a.matrices[i])||!matrix_valid(b.matrices[i])||length(sub(position(a.matrices[i]),position(b.matrices[i])))>.02f)return false;
  auto x=rotation(a.matrices[i]),y=rotation(b.matrices[i]);
  for(U32 k=0;k<3;k++)if(dot(column(x,k),column(y,k))<.99999f)return false;
 }return true;
}
inline bool get(const Attachments&a,int index,Matrix&out){
 if(index<1||a.count>64||U32(index)>a.count||!matrix_valid(a.matrices[index-1]))return false;out=a.matrices[index-1];return true;
}
template<class Memory> bool read(Driver<Memory>&d,Attachments&out,U32&counter){
 if(!d.leased||!d.still_ours())return false;
 const U32 vm=d.lease.vm,h=d.lease.header;U32 size=0,ac=0,ao=0,bones=0,bo=0,palette=0,alias=0,number=0,mask=0,busy=1,depth=1,after=0,sum=0;
 if(!d.rd(plus(h,76),size)||size<0x100||size>32u*1024u*1024u||!d.rd(plus(h,8),sum)||!d.rd(plus(h,0xf0),ac)||ac<1||ac>64||!d.rd(plus(h,0xf4),ao)||ao>size||ac*92>size-ao||!d.rd(plus(h,0x9c),bones)||bones<1||bones>256||!d.rd(plus(h,0xa0),bo)||bo>size||bones*216>size-bo)return false;
 if(!d.rd(plus(vm,0x2914),palette)||!palette||!d.rd(plus(vm,0x26a8),alias)||alias!=palette||!d.rd(plus(vm,0x2920),number)||number!=bones||!d.rd(plus(vm,0x26ac),mask)||!(mask&0x200)||!d.rd(plus(vm,0x26b8),busy)||busy||!d.rd(plus(vm,0x26bc),depth)||depth||!d.rd(plus(vm,0x2690),counter)||!counter)return false;
 Attachments value{};value.count=ac;value.checksum=sum;
 for(U32 i=0;i<ac;i++){
  U32 a=plus(h,ao+i*92),bone=0,flags=0,boneFlags=0;Matrix local{},world{};
  if(!d.rd(plus(a,8),bone)||bone>=bones||!d.rd(plus(a,4),flags)||(flags&0x10000)||!d.rd(plus(h,bo+bone*216+160),boneFlags)||!(boneFlags&0x200)||!d.m->read(plus(a,12),&local,sizeof(local))||!matrix_valid(local)||!d.m->read(plus(palette,bone*48),&world,sizeof(world))||!matrix_valid(world))return false;
  value.matrices[i]=compose(world,local);
  if(!matrix_valid(value.matrices[i])||length(sub(position(value.matrices[i]),d.gripWorld))>120.f)return false;
 }
 if(!d.rd(plus(vm,0x2690),after)||after!=counter||!d.still_ours())return false;
 out=value;return true;
}
}
