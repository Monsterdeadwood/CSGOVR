#pragma once
#include "math.hpp"
namespace ray_optics {
using namespace nova;
struct Slopes{float x,y;};
inline bool ray_slopes(Vec3 ray,Basis basis,Slopes&out){
 float z=dot(ray,basis.forward);if(!(z>.0001f))return false;
 out={dot(ray,basis.right)/z,dot(ray,basis.up)/z};
 return finite_in_range(out.x,1000)&&finite_in_range(out.y,1000);
}
inline float magnification(float nativeFov){return float(tan(rad(90)*.5)/tan(rad(nativeFov)*.5));}
// A lens samples angular rays. A UV-fixed picture cannot represent this
// relation when the eye moves relative to the weapon.
inline bool sample(Vec3 eye,Vec3 point,Basis basis,float zoom,float halfTan,Slopes&uv){
 Slopes q{};if(!(zoom>=1&&halfTan>0)||!ray_slopes(sub(point,eye),basis,q))return false;
 uv={.5f+.5f*q.x/(zoom*halfTan),.5f-.5f*q.y/(zoom*halfTan)};
 return finite_in_range(uv.x,1000)&&finite_in_range(uv.y,1000);
}
inline float envelope(Vec3 lens,float radius,Basis basis,const Vec3 eyes[2],float zoom){
 float result=.01f;
 for(int e=0;e<2;e++){
  Vec3 d=sub(lens,eyes[e]);float z=dot(d,basis.forward);
  if(z<=radius+.01f)continue;
  float x=(absf(dot(d,basis.right))+radius)/(z-radius)/zoom;
  float y=(absf(dot(d,basis.up))+radius)/(z-radius)/zoom;
  result=maxf(result,maxf(x,y)*1.12f);
 }
 return result<.9f?result:.9f;
}
}
