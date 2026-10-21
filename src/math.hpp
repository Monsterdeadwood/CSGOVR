#pragma once
#include "wire.hpp"
namespace nova {
static constexpr float UNITS_PER_METRE=39.37007874015748f;
static inline float deg(double x){return float(x*57.29577951308232);}
static inline float rad(double x){return float(x*0.017453292519943295);}
static inline float absf(float x){return x<0?-x:x;}
static inline float maxf(float a,float b){return a>b?a:b;}
static inline bool finite_in_range(float x,float b){return x==x&&x>-b&&x<b;}
static inline Vec3 add(Vec3 a,Vec3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
static inline Vec3 sub(Vec3 a,Vec3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
static inline Vec3 scale(Vec3 v,float s){return {v.x*s,v.y*s,v.z*s};}
static inline float dot(Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static inline float length(Vec3 v){return float(sqrt(dot(v,v)));}
static inline Quat qmul(Quat a,Quat b){return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};}
static inline Quat conjugate(Quat q){return {-q.x,-q.y,-q.z,q.w};}
static inline Quat normalized(Quat q){float n=float(sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w));return {q.x/n,q.y/n,q.z/n,q.w/n};}
static inline Vec3 rotate(Quat q,Vec3 v){Vec3 t{2*(q.y*v.z-q.z*v.y),2*(q.z*v.x-q.x*v.z),2*(q.x*v.y-q.y*v.x)};return {v.x+q.w*t.x+q.y*t.z-q.z*t.y,v.y+q.w*t.y+q.z*t.x-q.x*t.z,v.z+q.w*t.z+q.x*t.y-q.y*t.x};}
// Proper basis change: XR right/up/back -> Source forward/left/up. Determinant +1.
static inline Vec3 to_source(Vec3 v){return {-v.z,-v.x,v.y};}
struct Angles{float p,y,r;};
struct Basis{Vec3 forward,right,up;};
static inline Basis source_basis(Angles a){
 const float cp=float(cos(rad(a.p))),sp=float(sin(rad(a.p))),cy=float(cos(rad(a.y))),sy=float(sin(rad(a.y))),cr=float(cos(rad(a.r))),sr=float(sin(rad(a.r)));
 return {{cp*cy,cp*sy,-sp},{-sr*sp*cy+cr*sy,-sr*sp*sy-cr*cy,-sr*cp},{cr*sp*cy+sr*sy,cr*sp*sy-sr*cy,cr*cp}};
}
static inline Vec3 basis_apply(Basis b,Vec3 v){return add(add(scale(b.forward,v.x),scale(b.right,-v.y)),scale(b.up,v.z));}
static inline Angles basis_angles(Basis b){
 const float xy=float(sqrt(b.forward.x*b.forward.x+b.forward.y*b.forward.y));
 // At the vertical pole yaw and roll are not separately identifiable. This
 // representative preserves the orientation, rather than dividing by cos(pitch).
 if(xy<0.00001f)return {deg(atan2(-b.forward.z,xy)),deg(atan2(b.right.x,-b.right.y)),0};
 return {deg(atan2(-b.forward.z,xy)),deg(atan2(b.forward.y,b.forward.x)),deg(atan2(-b.right.z,b.up.z))};
}
static inline Basis head_basis(Quat q){q=normalized(q);return {to_source(rotate(q,{0,0,-1})),to_source(rotate(q,{1,0,0})),to_source(rotate(q,{0,1,0}))};}
static inline Angles head_angles(Quat q){return basis_angles(head_basis(q));}
static inline Basis compose(Basis parent,Basis child){return {basis_apply(parent,child.forward),basis_apply(parent,child.right),basis_apply(parent,child.up)};}
static inline Quat anchor_yaw(Quat a,Quat b){
 // Average both eye orientations, not the left canted eye alone. Recenter yaw
 // only: initial physical head tilt must not tilt the room's translation axes.
 float sign=a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w<0?-1.f:1.f;
 Quat q=normalized({a.x+sign*b.x,a.y+sign*b.y,a.z+sign*b.z,a.w+sign*b.w});
 Vec3 f=rotate(q,{0,0,-1});float yaw=float(atan2(-f.x,-f.z));
 return {0,float(sin(yaw*0.5f)),0,float(cos(yaw*0.5f))};
}
static inline bool valid_fov(Fov f){return finite_in_range(f.l,1.56f)&&finite_in_range(f.r,1.56f)&&finite_in_range(f.u,1.56f)&&finite_in_range(f.d,1.56f)&&f.l<0&&f.r>0&&f.u>0&&f.d<0;}
static inline bool valid_eye(Eye e){float n=e.rot.x*e.rot.x+e.rot.y*e.rot.y+e.rot.z*e.rot.z+e.rot.w*e.rot.w;return n>0.8f&&n<1.2f&&finite_in_range(e.pos.x,30)&&finite_in_range(e.pos.y,30)&&finite_in_range(e.pos.z,30)&&valid_fov(e.fov);}
static inline bool projection(Fov f,float nearZ,float farZ,float out[16]){
 if(!valid_fov(f)||!finite_in_range(nearZ,1000000)||!finite_in_range(farZ,100000000)||nearZ<=0||farZ<=nearZ)return false;
 double l=tan(f.l),r=tan(f.r),u=tan(f.u),d=tan(f.d),w=r-l,h=u-d;
 for(int i=0;i<16;i++)out[i]=0;
 // Row-major view-to-clip, column vectors, right-handed view space (-Z forward), D3D z in [0,1].
 out[0]=float(2/w);out[2]=float((r+l)/w);out[5]=float(2/h);out[6]=float((u+d)/h);
 out[10]=-farZ/(farZ-nearZ);out[11]=-farZ*nearZ/(farZ-nearZ);out[14]=-1;
 return true;
}
static inline void culling_envelope(Fov left,Fov right,float&fov,float&aspect){
 float x=0,y=0;const Fov fs[2]={left,right};
 for(int i=0;i<2;i++){x=maxf(x,maxf(absf(float(tan(fs[i].l))),absf(float(tan(fs[i].r)))));y=maxf(y,maxf(absf(float(tan(fs[i].u))),absf(float(tan(fs[i].d)))));}
 fov=2*deg(atan2(x,1));aspect=x/y;
}
// Native lens boundary: the game receives an ordinary centered perspective.
// XR receives the FOV actually rendered, not the runtime-recommended asymmetric FOV.
// A wider submitted projection is allowed by OpenXR; the compositor maps its rays.
static inline bool native_lens(Fov left,Fov right,float&fov,float&aspect,Fov&rendered){
 if(!valid_fov(left)||!valid_fov(right))return false;
 culling_envelope(left,right,fov,aspect);
 if(!finite_in_range(fov,179)||fov<=0||!finite_in_range(aspect,100)||aspect<=0)return false;
 const float hx=rad(fov)*0.5f;
 const float hy=float(atan2(tan(hx),aspect));
 rendered={-hx,hx,hy,-hy};return valid_fov(rendered);
}
static inline bool centered_fov(Fov f){return valid_fov(f)&&absf(f.l+f.r)<0.000001f&&absf(f.u+f.d)<0.000001f;}
static inline bool write_eye_view(const U8*stock,U8*out,const Eye&e,Vec3 anchor,Quat anchorRotation,float envelopeFov,float envelopeAspect,U32 w,U32 h,Vec3&position,Angles&angles,float matrix[16]){
 if(!valid_eye(e)||!centered_fov(e.fov)||!w||!h)return false;
 // Enforce the native fov/aspect -> submitted image FOV identity before rendering.
 if(!finite_in_range(envelopeFov,179)||envelopeFov<=0||!finite_in_range(envelopeAspect,100)||envelopeAspect<=0)return false;
 float hx=rad(envelopeFov)*0.5f,hy=float(atan2(tan(hx),envelopeAspect));
 if(absf(e.fov.r-hx)>0.000002f||absf(e.fov.u-hy)>0.000002f)return false;
 // This branch is for ordinary perspective WORLD. Do not repurpose ortho/custom-view callbacks.
 if(stock[0x20]||stock[0x34])return false;
 float nearZ,farZ;memcpy(&nearZ,stock+0xd0,4);memcpy(&farZ,stock+0xd4,4);
 if(!projection(e.fov,nearZ,farZ,matrix))return false;
 Vec3 origin{};Angles body{};memcpy(&origin,stock+0xb8,12);memcpy(&body,stock+0xc4,12);
 if(!finite_in_range(origin.x,10000000)||!finite_in_range(origin.y,10000000)||!finite_in_range(origin.z,10000000)||!finite_in_range(body.p,10000)||!finite_in_range(body.y,10000)||!finite_in_range(body.r,10000))return false;
 Quat inv=conjugate(normalized(anchorRotation));Basis parent=source_basis(body);
 position=add(origin,scale(basis_apply(parent,to_source(rotate(inv,sub(e.pos,anchor)))),UNITS_PER_METRE));
 angles=basis_angles(compose(parent,head_basis(qmul(inv,normalized(e.rot)))));
 memcpy(out,stock,0x14c);memcpy(out+0xb8,&position,12);memcpy(out+0xc4,&angles,12);
 // Exact retail CViewSetup span, not the larger historical wrapper copy span.
 I32 viewport[8]={0,0,0,0,I32(w),I32(w),I32(h),I32(h)};memcpy(out,viewport,sizeof(viewport));
 // No custom projection injection. Opaque +0x6C bytes remain exactly stock.
 // The exact engine native builder owns raster depth and perspective math.
 out[0x68]=0;
 memcpy(out+0xb0,&envelopeFov,4);memcpy(out+0xe0,&envelopeAspect,4);
 out[0x148]&=U8(~1u); // ordinary native perspective, no off-center/custom path
 return true;
}
static inline bool capture_valid(const CapturedStereo&c,U64 pair,U64 epoch){
 // Kind 3 has the same centered native camera, but pixels outside the runtime
 // sample region may be skipped. The receiving host must require exact history.
 return c.valid==1&&(c.projectionKind==2||c.projectionKind==3)&&c.pairId==pair&&c.trackingEpoch==epoch&&c.sampleId&&c.predictedTime>0&&valid_eye(c.eye[0])&&valid_eye(c.eye[1])&&centered_fov(c.eye[0].fov)&&centered_fov(c.eye[1].fov);
}
}
