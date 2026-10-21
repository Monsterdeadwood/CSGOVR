#pragma once
#include "gfx.hpp"
#include "fps_policy.hpp"
// Public D3D9 timestamp ABI (Windows SDK d3d9types.h). Graphics-owner only.
// GetData(flags=0) never flushes or waits. Pending cells cannot be overwritten.
namespace nova_gpu_perf {
enum Stage { Optic, LeftEye, RightEye, EyeCopy, Menu, Mirror, Count };
static const char*names[Count]={"optic_native","left_native","right_native","eye_copy","menu_copy","mirror_copy"};
struct Cell {D9Query *begin{},*end{},*frequency{},*disjoint{};U32 state{},policy{};U64 pair{},issued{};};
static Cell cells[Count][4]{};
struct Record {U64 calls{},rejected{},skipped{};double sum{},maximum{};};
static Record records[Count]{};static U32 calls[Count]{};
static D9Device*device{};static Log logger;static U64 reportMs{};
static bool disabled{};
static void retire(){for(auto&stage:cells){for(auto&c:stage){
 D9Query**qs[]={&c.begin,&c.end,&c.frequency,&c.disjoint};for(auto**q:qs)if(*q){(*q)->release();*q=nullptr;}c={};
 }}device=nullptr;}
static void initialize(D9Device*d,const wchar_t*directory){device=d;logger.open(L"nova_perf_gpu.log",directory);reportMs=GetTickCount64();logger.msg("GPU_TIMESTAMP_NONBLOCKING_RING",4,16);}
static void poll(){
 if(!device||disabled)return;
 for(U32 s=0;s<Count;++s)for(auto&c:cells[s]){
  if(c.state!=1)continue;U64 a=0,b=0,f=0;BOOL bad=1;
  HRESULT status[4]={c.end->data(&b,8),c.begin->data(&a,8),c.frequency->data(&f,8),c.disjoint->data(&bad,4)};
  bool pending=false,fault=false;for(auto r:status){pending|=r==1;fault|=r<0;}
  if(fault){c.state=2;++records[s].rejected;logger.msg("GPU_TIMESTAMP_FAILED_CELL_QUARANTINED",s,c.pair);continue;}
  if(pending)continue;
  auto&r=records[s];double ms=(!bad&&f&&b>=a)?double(b-a)*1000/double(f):-1;
  if(ms>=0&&ms<1000){++r.calls;r.sum+=ms;if(ms>r.maximum)r.maximum=ms;}else ++r.rejected;
  c.state=0;
 }
 U64 now=GetTickCount64();if(now-reportMs<2000)return;U64 window=now-reportMs;reportMs=now;
 if(nova_fps::on(nova_fps::ProfilingOff)){for(auto&r:records)r={};return;}
 for(U32 s=0;s<Count;++s){auto&r=records[s];if(!r.calls&&!r.rejected&&!r.skipped)continue;char b[420];
  int n=_snprintf(b,sizeof(b),"NOVA_GPU tick=%llu window_ms=%llu stage=%s samples=%llu mean_ms=%.6f max_ms=%.6f rejected=%llu ring_busy=%llu policy=%u\r\n",now,window,names[s],r.calls,r.calls?r.sum/double(r.calls):0,r.maximum,r.rejected,r.skipped,load32(&nova_fps::features));
  U32 done=0;if(n>0&&logger.file!=BAD_HANDLE)WriteFile(logger.file,b,U32(n<419?n:419),&done,nullptr);r={};
 }
}
struct Scope {
 Cell*c{};
 Scope(Stage s,U64 pair){
  if(!device||disabled||nova_fps::on(nova_fps::ProfilingOff)||(++calls[s]&15))return;
  for(auto&candidate:cells[s])if(candidate.state==0){c=&candidate;break;}
  if(!c){++records[s].skipped;return;}
  if(!c->begin){
   HRESULT a=device->typed_query(10,&c->begin),b=device->typed_query(10,&c->end),f=device->typed_query(12,&c->frequency),d=device->typed_query(11,&c->disjoint);
   if(failed(a)||failed(b)||failed(f)||failed(d)||!c->begin||!c->end||!c->frequency||!c->disjoint){disabled=true;logger.msg("GPU_TIMESTAMP_UNAVAILABLE",s);c=nullptr;return;}
  }
  c->policy=load32(&nova_fps::features);c->pair=pair;c->issued=GetTickCount64();
  if(failed(c->disjoint->issue(2))||failed(c->frequency->end())||failed(c->begin->end())){c->state=2;c=nullptr;++records[s].rejected;}
 }
 ~Scope(){if(c){bool good=ok(c->end->end())&&ok(c->disjoint->end());c->state=good?1:2;}}
};
}
