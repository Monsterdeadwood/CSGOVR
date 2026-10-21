#pragma once
#include "muzzle_core.hpp"
// A rigid weapon frame and its anatomical idle mount. Animation roots are
// adapters to this frame; they are not the source of native command aiming.
namespace weapon_body {
using namespace nova;using namespace handcore;using namespace muzzle;
inline Matrix matrix(const Pose&p){Rotation r=pose_rotation(p.a);return {{r.a[0],r.a[1],r.a[2],p.p.x,r.a[3],r.a[4],r.a[5],p.p.y,r.a[6],r.a[7],r.a[8],p.p.z}};}
inline Matrix matrix(Rotation r,Vec3 p){return {{r.a[0],r.a[1],r.a[2],p.x,r.a[3],r.a[4],r.a[5],p.y,r.a[6],r.a[7],r.a[8],p.z}};}
inline Matrix inverse(const Matrix&m){auto r=transpose(rotation(m));return matrix(r,scale(apply(r,position(m)),-1));}
inline bool close(const Matrix&a,const Matrix&b,float distance=.03f,float rotationTolerance=.001f){
 if(!matrix_valid(a)||!matrix_valid(b)||length(sub(position(a),position(b)))>distance)return false;
 auto x=rotation(a),y=rotation(b);for(U32 k=0;k<9;k++)if(absf(x.a[k]-y.a[k])>rotationTolerance)return false;return true;
}
// Match the native baseAngle + AimPunch * recoil_scale operation, but apply
// its visual rotation around the anatomical palm. Command/base data is separate.
inline bool recoil_delta(const Pose&base,Vec3 pivot,Angles punch,float scale,Matrix&out){
 if(!finite(base.a)||!finite(pivot)||!finite(punch)||!finite_in_range(scale,20))return false;
 Angles next{base.a.p+punch.p*scale,base.a.y+punch.y*scale,base.a.r+punch.r*scale};
 Rotation r=multiply(pose_rotation(next),transpose(pose_rotation(base.a)));
 out=matrix(r,sub(pivot,apply(r,pivot)));return matrix_valid(out);
}
struct Mount {
 Identity id{};U32 checksum{},count{},bodyBone{},attachment{},boneFlags[256]{};
 U32 attachmentCount{},attachmentBones[64]{};Matrix attachmentLocal[64]{};
 U64 revision{},learnedMs{};Matrix bodyFromPalm{},bodyFromBarrel{};bool valid{};
};
struct Reference {
 Identity id{};U64 epoch{},pair{},atMs{},anchorRevision{},displaySample{};Vec3 eyeOffset{},anchor{};Quat anchorRot{};
};
struct Sample {Frame frame{};Matrix body{},barrel{};U64 mountRevision{},anchorRevision{};I64 predictedTime{};U64 poseMs{};bool valid{};};
// One spatial state evaluated independently for the render sample and for the
// command sample. Native firing adds punch to angles, so its input carries the
// unpunched angles AND the physical recoiled muzzle position. Display and optics
// consume the full recoiled transform; neither can invent a separate weapon pose.
struct State {Sample base{},display{};Frame firing{};Matrix recoil{};bool valid{};};
inline bool state(const Sample&base,Vec3 palm,Angles punch,float recoilScale,State&out){
 out={};if(!base.valid||!base.frame.valid||!matrix_valid(base.body)||!matrix_valid(base.barrel))return false;
 Matrix d{};if(!recoil_delta(base.frame.barrel,palm,punch,recoilScale,d))return false;
 out.base=base;out.display=base;out.recoil=d;
 out.display.body=compose(d,base.body);out.display.barrel=compose(d,base.barrel);
 if(!matrix_valid(out.display.body)||!matrix_valid(out.display.barrel))return false;
 out.display.frame.barrel=matrix_pose(out.display.barrel);
 out.firing=base.frame;out.firing.barrel.p=out.display.frame.barrel.p;
 out.valid=finite(out.firing.barrel.p);return out.valid;
}
inline bool solve(const Mount&m,const Tracking&t,U64 now,Vec3 origin,Angles body,Vec3 anchor,Quat anchorRot,U64 pair,U64 anchorRevision,Sample&out){
 out={};if(!m.valid||!m.revision||!matrix_valid(m.bodyFromPalm)||!matrix_valid(m.bodyFromBarrel))return false;
 PalmRig grip{};grip.mounted=true;grip.rootFromPalm=rotation(m.bodyFromPalm);grip.center=position(m.bodyFromPalm);
 Pose p{};Vec3 target{};if(!solve_palm(t,now,origin,body,anchor,anchorRot,grip,p,target))return false;
 out.body=matrix(p);out.barrel=compose(out.body,m.bodyFromBarrel);
 if(!matrix_valid(out.body)||!matrix_valid(out.barrel)||length(sub(position(out.barrel),origin))>=120)return false;
 out.frame.id=m.id;out.frame.modelChecksum=m.checksum;out.frame.muzzleAttachment=m.attachment;
 out.frame.pair=pair;out.frame.sample=t.sampleId;out.frame.epoch=t.epoch;out.frame.capturedMs=now;
 out.frame.barrel=matrix_pose(out.barrel);out.frame.bodyEye=origin;out.frame.valid=1;
 out.mountRevision=m.revision;out.anchorRevision=anchorRevision;out.predictedTime=t.predictedTime;out.poseMs=t.sampledMs;out.valid=true;return true;
}
// The caller owns pointer/range admission. This only adapts an already naturally
// returned draw palette. Native animation relationships inside it are preserved.
inline bool adapt(const Matrix*in,Matrix*out,U32 count,const Mount&m,const Sample&s,Matrix&delta){
 if(!in||!out||!s.valid||!m.valid||count<m.count||m.count>256||m.bodyBone>=m.count||!matrix_valid(in[m.bodyBone]))return false;
 delta=compose(s.body,inverse(in[m.bodyBone]));if(!matrix_valid(delta))return false;
 for(U32 i=0;i<m.count;i++){
  out[i]=in[i];if(!(m.boneFlags[i]&0x600))continue;
  if(!matrix_valid(in[i]))return false;out[i]=compose(delta,in[i]);if(!matrix_valid(out[i]))return false;
 }
 return close(out[m.bodyBone],s.body,.002f,.00001f);
}
}
