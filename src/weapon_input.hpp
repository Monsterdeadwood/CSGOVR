#pragma once
#include "math.hpp"
// Explicit weapon target construction. The grip point and aim orientation are
// different XR observations at the SAME predicted time. The published HandPose
// is a constructed weapon target, never a claim that grip and aim are equal.
namespace weapon_input {
using namespace nova;
constexpr U32 MAGIC=0x31434c4e,VERSION=1,AIM_SCHEMA=0x41494d31,GRIP_CONTROL_SCHEMA=0x47525031;
struct Calibration {
 U32 magic=MAGIC,version=VERSION,revision=0,source=0;
 float pitch=0,yaw=0,roll=0;Vec3 offset{};
 U32 checksum=0,reserved=0;
};
static_assert(sizeof(Calibration)==48,"NOVA calibration file ABI");
static_assert(offsetof(Calibration,checksum)==40,"NOVA calibration hash boundary");
inline U32 hash(const Calibration&c){U32 h=2166136261u;const U8*p=(const U8*)&c;for(unsigned i=0;i<40;i++){h^=p[i];h*=16777619u;}return h;}
inline bool valid(const Calibration&c){return c.magic==MAGIC&&c.version==VERSION&&c.source<=1&&!c.reserved&&c.checksum==hash(c)&&finite_in_range(c.pitch,180.001f)&&finite_in_range(c.yaw,180.001f)&&finite_in_range(c.roll,180.001f)&&finite_in_range(c.offset.x,.251f)&&finite_in_range(c.offset.y,.251f)&&finite_in_range(c.offset.z,.251f)&&length(c.offset)<=.25f;}
inline Calibration defaults(){Calibration c{};c.checksum=hash(c);return c;}
inline bool accept(Calibration&current,const Calibration&candidate){if(!valid(candidate))return false;current=candidate;return true;}
struct Sample {Vec3 position{};Quat rotation{};bool active=false,positionValid=false,orientationValid=false;};
inline bool quat_valid(Quat q){float n=dot({q.x,q.y,q.z},{q.x,q.y,q.z})+q.w*q.w;return finite_in_range(n,1.21f)&&n>.8f;}
inline Quat rotation(const Calibration&c){
 float p=rad(c.pitch)*.5f,y=rad(c.yaw)*.5f,r=rad(c.roll)*.5f;
 return normalized(qmul({0,float(sin(y)),0,float(cos(y))},qmul({float(sin(p)),0,0,float(cos(p))},{0,0,-float(sin(r)),float(cos(r))})));
}
inline HandPose target(const Sample&grip,const Sample&aim,const Calibration&c){
 HandPose out{};const Sample&orientation=c.source==0?aim:grip;
 if(!valid(c)||!grip.active||!grip.positionValid||!grip.orientationValid||!orientation.active||!orientation.orientationValid||!quat_valid(grip.rotation)||!quat_valid(orientation.rotation)||!finite_in_range(grip.position.x,30)||!finite_in_range(grip.position.y,30)||!finite_in_range(grip.position.z,30))return out;
 out.pos=add(grip.position,rotate(normalized(grip.rotation),c.offset));
 out.rot=normalized(qmul(normalized(orientation.rotation),rotation(c)));
 out.valid=out.active=1;return out;
}
}
