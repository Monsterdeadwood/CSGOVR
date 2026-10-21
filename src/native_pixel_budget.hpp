#pragma once
#include "view_region.hpp"
// Crop only rays outside the region the unchanged host submits. Twelve source
// pixels remain for the host FXAA/sharpen filter. No FOV/projection/resolution
// changes, no scope visibility test. Native per-draw scissor is restored exactly.
namespace pixel_budget {
using Target=HRESULT(WINCALL*)(D9Device*,U32,D9Surface*);
using Draw=HRESULT(WINCALL*)(D9Device*,U32,U32,U32);
using Indexed=HRESULT(WINCALL*)(D9Device*,U32,I32,U32,U32,U32,U32);
using Up=HRESULT(WINCALL*)(D9Device*,U32,U32,const void*,U32);
using IndexedUp=HRESULT(WINCALL*)(D9Device*,U32,U32,U32,U32,const void*,U32,const void*,U32);
static Target target{};static Draw draw{};static Indexed indexed{};static Up up{};static IndexedUp indexedUp{};
static D9Surface*current{},*owned{};static RECT crop{};static bool installed{},active{},failedRestore{};
static U64 draws{},cropped{},otherTargets{},restores{},faults{},views{};static U64 reportMs{};
static HRESULT WINCALL target_hook(D9Device*d,U32 n,D9Surface*s){HRESULT r=target(d,n,s);if(d==gpu&&n==0&&ok(r)&&GetCurrentThreadId()==graphicsThread)current=s;return r;}
struct Scissor {
 D9Device*d{};RECT prior{};U32 enabled{};bool changed{},skip{};
 explicit Scissor(D9Device*device):d(device){
  if(!active||device!=gpu||GetCurrentThreadId()!=graphicsThread)return;
  ++draws;if(current!=owned){++otherTargets;return;}
  D9View vp{};if(failed(d->getvp(&vp))||vp.X||vp.Y||vp.Width!=bus->width||vp.Height!=bus->height)return;
  auto getRS=(HRESULT(WINCALL*)(D9Device*,U32,U32*))d->v[58];auto getRect=(HRESULT(WINCALL*)(D9Device*,RECT*))d->v[76];
  if(failed(getRS(d,174,&enabled))||failed(getRect(d,&prior))){++faults;return;}
  RECT r=crop;
  if(enabled){if(r.left<prior.left)r.left=prior.left;if(r.top<prior.top)r.top=prior.top;if(r.right>prior.right)r.right=prior.right;if(r.bottom>prior.bottom)r.bottom=prior.bottom;}
  if(r.left>=r.right||r.top>=r.bottom){skip=true;++cropped;return;}
  auto setRect=(HRESULT(WINCALL*)(D9Device*,const RECT*))d->v[75];auto setRS=(HRESULT(WINCALL*)(D9Device*,U32,U32))d->v[57];
  // Mark before the first mutation so partial failure also restores native state.
  changed=true;bool good=ok(setRect(d,&r))&&ok(setRS(d,174,1));
  if(good)++cropped;else{++faults;restore();}
 }
 void restore(){if(!changed)return;changed=false;auto setRect=(HRESULT(WINCALL*)(D9Device*,const RECT*))d->v[75];auto setRS=(HRESULT(WINCALL*)(D9Device*,U32,U32))d->v[57];
  bool a=ok(setRect(d,&prior)),b=ok(setRS(d,174,enabled));if(a&&b)++restores;else{++faults;failedRestore=true;active=false;logg.msg("F1_PIXEL_NATIVE_STATE_RESTORE_FAILED");}}
 ~Scissor(){restore();}
};
static HRESULT WINCALL draw_hook(D9Device*d,U32 type,U32 first,U32 count){Scissor s(d);return s.skip?0:draw(d,type,first,count);}
static HRESULT WINCALL indexed_hook(D9Device*d,U32 type,I32 base,U32 min,U32 n,U32 first,U32 count){Scissor s(d);return s.skip?0:indexed(d,type,base,min,n,first,count);}
static HRESULT WINCALL up_hook(D9Device*d,U32 type,U32 count,const void*p,U32 stride){Scissor s(d);return s.skip?0:up(d,type,count,p,stride);}
static HRESULT WINCALL indexed_up_hook(D9Device*d,U32 type,U32 min,U32 n,U32 count,const void*i,U32 f,const void*v,U32 stride){Scissor s(d);return s.skip?0:indexedUp(d,type,min,n,count,i,f,v,stride);}
static void shutdown(){
 active=false;owned=nullptr;current=nullptr;
 if(!installed)return;
 undo(gpu->v+37,(void*)target_hook,(void*)target);undo(gpu->v+81,(void*)draw_hook,(void*)draw);undo(gpu->v+82,(void*)indexed_hook,(void*)indexed);undo(gpu->v+83,(void*)up_hook,(void*)up);undo(gpu->v+84,(void*)indexed_up_hook,(void*)indexedUp);installed=false;
 logg.msg("F1_PIXEL_HOOKS_RETIRED",cropped,restores,faults);
}
static bool install(){
 if(installed)return true;if(!gpu||GetCurrentThreadId()!=graphicsThread)return false;
 target=(Target)gpu->v[37];draw=(Draw)gpu->v[81];indexed=(Indexed)gpu->v[82];up=(Up)gpu->v[83];indexedUp=(IndexedUp)gpu->v[84];
 installed=true;
 if(!hook(gpu->v+37,(void*)target,(void*)target_hook)||!hook(gpu->v+81,(void*)draw,(void*)draw_hook)||!hook(gpu->v+82,(void*)indexed,(void*)indexed_hook)||!hook(gpu->v+83,(void*)up,(void*)up_hook)||!hook(gpu->v+84,(void*)indexedUp,(void*)indexed_up_hook)){shutdown();return false;}
 logg.msg("F1_PIXEL_HOOKS_READY",12);return true;
}
struct Scope {
 Scope(D9Surface*surface,Fov rendered,Fov desired){
  current=owned=surface;active=false;ViewRegion region{};
  if(!installed||failedRestore||!nova_fps::on(nova_fps::PixelCrop)||!view_region(rendered,desired,bus->width,bus->height,region))return;
  crop={region.x-12,region.y-12,region.x+region.width+12,region.y+region.height+12};
  if(crop.left<0)crop.left=0;if(crop.top<0)crop.top=0;if(crop.right>I32(bus->width))crop.right=I32(bus->width);if(crop.bottom>I32(bus->height))crop.bottom=I32(bus->height);
  active=crop.left||crop.top||crop.right<I32(bus->width)||crop.bottom<I32(bus->height);++views;
  if(views<=4||views%240==0)logg.msg("F1_PIXEL_AREA",U64(crop.right-crop.left)*U64(crop.bottom-crop.top),U64(bus->width)*bus->height,active);
 }
 ~Scope(){active=false;owned=nullptr;U64 now=GetTickCount64();if(now-reportMs>2000){reportMs=now;logg.msg("F1_PIXEL_DRAW_COUNTS",draws,cropped,otherTargets);logg.msg("F1_PIXEL_RESTORE_COUNTS",restores,faults,views);}}
};
}
