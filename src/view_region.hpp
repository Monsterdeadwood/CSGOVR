#pragma once
#include "math.hpp"
// Host-owned, pixel-edge camera contract. No game projection or GPU contents are changed.
namespace nova {
struct ViewRegion {
 I32 x,y,width,height; Fov fov; U32 exactSample;
};
static inline ViewRegion full_region(Fov f,U32 w,U32 h){return {0,0,I32(w),I32(h),f,0};}
static inline I32 edge_floor(double x){return I32(x);}
static inline I32 edge_ceil(double x){I32 i=I32(x);return double(i)<x?i+1:i;}
static inline double bound(double x,double a,double b){return x<a?a:(x>b?b:x);}
// D3D image top-left -> tangent rays. Round OUT to pixel boundaries, then derive
// the submitted FOV FROM those boundaries. Never label a rounded crop with the
// unrounded desired FOV; that would introduce another optical scale error.
static inline bool view_region(Fov native,Fov desired,U32 w,U32 h,ViewRegion&out){
 if(!valid_fov(native)||!valid_fov(desired)||!w||!h||w>16384||h>16384)return false;
 double l=tan(native.l),r=tan(native.r),u=tan(native.u),d=tan(native.d);
 double dl=tan(desired.l),dr=tan(desired.r),du=tan(desired.u),dd=tan(desired.d);
 double sx=r-l,sy=u-d;const double eps=0.000002;
 if(dl<l-eps||dr>r+eps||du>u+eps||dd<d-eps)return false;
 double x0=bound((dl-l)/sx,0,1)*w,x1=bound((dr-l)/sx,0,1)*w;
 double y0=bound((u-du)/sy,0,1)*h,y1=bound((u-dd)/sy,0,1)*h;
 I32 left=edge_floor(x0),right=edge_ceil(x1),top=edge_floor(y0),bottom=edge_ceil(y1);
 if(left<0||top<0||right>I32(w)||bottom>I32(h)||left>=right||top>=bottom)return false;
 Fov actual{float(atan2(l+sx*left/w,1)),float(atan2(l+sx*right/w,1)),
            float(atan2(u-sy*top/h,1)),float(atan2(u-sy*bottom/h,1))};
 if(!valid_fov(actual))return false;
 out={left,top,right-left,bottom-top,actual,1};return true;
}
// The source R3 DLL already publishes sampleId+epoch+time and captured eye poses.
// Retain the corresponding original XR FOV here before it leaves the host. Do
// not reuse the latest FOV to describe pixels from an earlier sample.
static constexpr U32 VIEW_HISTORY_SIZE=1024;
struct ViewHistory {
 Tracking slots[VIEW_HISTORY_SIZE]{};
 void remember(const Tracking&t){if(t.valid&&t.sampleId)slots[t.sampleId%VIEW_HISTORY_SIZE]=t;}
 const Tracking* match(const CapturedStereo&c)const{
  const Tracking&t=slots[c.sampleId%VIEW_HISTORY_SIZE];
  if(!c.sampleId||!t.valid||t.sampleId!=c.sampleId||t.epoch!=c.trackingEpoch||t.predictedTime!=c.predictedTime)return nullptr;
  for(int e=0;e<2;e++){
   if(!valid_eye(t.eye[e])||!valid_eye(c.eye[e])||length(sub(t.eye[e].pos,c.eye[e].pos))>0.00005f)return nullptr;
   Quat a=normalized(t.eye[e].rot),b=normalized(c.eye[e].rot);
   float qdot=absf(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w);
   if(qdot<0.999999f)return nullptr;
  }
  return &t;
 }
};
static inline bool pair_regions(const CapturedStereo&c,const ViewHistory&history,U32 w,U32 h,ViewRegion out[2]){
 out[0]=full_region(c.eye[0].fov,w,h);out[1]=full_region(c.eye[1].fov,w,h);
 const Tracking*t=history.match(c);if(!t)return false;
 ViewRegion v[2]{};
 if(!view_region(c.eye[0].fov,t->eye[0].fov,w,h,v[0])||!view_region(c.eye[1].fov,t->eye[1].fov,w,h,v[1]))return false;
 out[0]=v[0];out[1]=v[1];return true;
}
// Evidence only: effective physical-room UP after the actual captured Source
// orientation. This exposes hidden parent pitch/roll without changing it.
static inline Vec3 captured_room_up(const CapturedStereo&c,int eye){
 Basis raw=head_basis(c.eye[eye].rot);
 Basis image=source_basis({c.sourceAngles[eye][0],c.sourceAngles[eye][1],c.sourceAngles[eye][2]});
 Vec3 up{0,0,1};Vec3 local{dot(raw.forward,up),-dot(raw.right,up),dot(raw.up,up)};
 return basis_apply(image,local);
}
}
