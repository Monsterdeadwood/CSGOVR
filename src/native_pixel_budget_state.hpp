#pragma once
#include "view_region.hpp"
// State-transition owner. Preserve native logical state, clear semantics,
// occlusion queries and partial/captured state blocks. No per-draw GPU calls.
namespace pixel_budget {
using Target=HRESULT(WINCALL*)(D9Device*,U32,D9Surface*);
using SetRS=HRESULT(WINCALL*)(D9Device*,U32,U32);using GetRS=HRESULT(WINCALL*)(D9Device*,U32,U32*);
using SetRect=HRESULT(WINCALL*)(D9Device*,const RECT*);using GetRect=HRESULT(WINCALL*)(D9Device*,RECT*);using SetVP=HRESULT(WINCALL*)(D9Device*,const D9View*);
using Clear=HRESULT(WINCALL*)(D9Device*,U32,const void*,U32,U32,float,U32);
using CreateBlock=HRESULT(WINCALL*)(D9Device*,U32,Com**);using Begin=HRESULT(WINCALL*)(D9Device*);using End=HRESULT(WINCALL*)(D9Device*,Com**);
using BlockFn=HRESULT(WINCALL*)(Com*);using Issue=HRESULT(WINCALL*)(D9Query*,U32);using CreateQuery=HRESULT(WINCALL*)(D9Device*,U32,D9Query**);
static Target target{};static SetRS setRS{};static GetRS getRS{};static SetRect setRect{};static GetRect getRect{};static SetVP setVP{};static Clear clear{};
static CreateBlock createBlock{};static Begin beginBlock{};static End endBlock{};static CreateQuery createQuery{};
struct BlockTable{void**v{};BlockFn capture{},apply{};};static BlockTable bt[8]{};static U32 nb{};
struct QueryTable{void**v{};Issue issue{};};static QueryTable qt[8]{};static U32 nq{};
static D9Surface*current{},*owned{};static RECT crop{},engineRect{};static D9View viewport{};static U32 engineEnabled{},occlusion{},suspended{};
static bool installed{},active{},masked{},recording{},failedRestore{};static RECT appliedCrop{};static U64 coalesced{};
static bool same_rect(const RECT&a,const RECT&b){return a.left==b.left&&a.top==b.top&&a.right==b.right&&a.bottom==b.bottom;}static U64 changes{},restores{},faults{},views{},clears{},blocks{},queries{},reportMs{};
// Native BeginStateBlock may rewrite the device's method cells in place.
// The scene owner repairs its Present/lens leases as well as these crop leases.
static bool(*repairDeviceTable)(){};
static bool owner(D9Device*d){return d==gpu&&GetCurrentThreadId()==graphicsThread;}
static void fault(){++faults;failedRestore=true;active=false;native_game_control::stopGameRequested=true;inc32(&bus->graphicsErrors);logg.msg("F2_PIXEL_NATIVE_STATE_FAILURE_STOP_REQUESTED",faults);}
static bool unmask(){if(!masked)return true;masked=false;bool a=ok(setRect(gpu,&engineRect)),b=ok(setRS(gpu,174,engineEnabled));if(a&&b){++restores;return true;}fault();return false;}
static bool read_native(){bool a=ok(getRect(gpu,&engineRect)),b=ok(getRS(gpu,174,&engineEnabled)),c=ok(gpu->getvp(&viewport));if(a&&b&&c)return true;fault();return false;}
static void enforce(){
 if(!active||failedRestore||recording||suspended||occlusion||current!=owned||viewport.X||viewport.Y||viewport.Width!=bus->width||viewport.Height!=bus->height){unmask();return;}
 RECT r=crop;if(engineEnabled){if(r.left<engineRect.left)r.left=engineRect.left;if(r.top<engineRect.top)r.top=engineRect.top;if(r.right>engineRect.right)r.right=engineRect.right;if(r.bottom>engineRect.bottom)r.bottom=engineRect.bottom;}
 // An empty intersection already excludes all submitted rays. Keep native state.
 if(r.left>=r.right||r.top>=r.bottom){unmask();return;}
 if(nova_fps::on(nova_fps::CoalesceCrop)&&masked&&same_rect(appliedCrop,r)){++coalesced;return;}
 masked=true;bool a=ok(setRect(gpu,&r)),b=ok(setRS(gpu,174,1));++changes;if(a&&b)appliedCrop=r;else{unmask();fault();}
}
static HRESULT WINCALL target_hook(D9Device*d,U32 n,D9Surface*s){bool ours=owner(d)&&n==0;if(ours)unmask();HRESULT r=target(d,n,s);if(ours&&ok(r)){current=s;if(active){read_native();enforce();}}return r;}
static HRESULT WINCALL setrs_hook(D9Device*d,U32 s,U32 v){if(s!=174||!owner(d)||!active||recording)return setRS(d,s,v);if(nova_fps::on(nova_fps::CoalesceCrop)&&masked&&v==engineEnabled){++coalesced;return 0;}unmask();HRESULT r=setRS(d,s,v);if(ok(r))engineEnabled=v;enforce();return r;}
static HRESULT WINCALL getrs_hook(D9Device*d,U32 s,U32*v){if(s==174&&v&&owner(d)&&active&&!recording){*v=engineEnabled;return 0;}return getRS(d,s,v);}
static HRESULT WINCALL setrect_hook(D9Device*d,const RECT*v){if(!owner(d)||!active||recording)return setRect(d,v);if(nova_fps::on(nova_fps::CoalesceCrop)&&masked&&v&&same_rect(*v,engineRect)){++coalesced;return 0;}unmask();HRESULT r=setRect(d,v);if(ok(r)&&v)engineRect=*v;enforce();return r;}
static HRESULT WINCALL getrect_hook(D9Device*d,RECT*v){if(v&&owner(d)&&active&&!recording){*v=engineRect;return 0;}return getRect(d,v);}
static HRESULT WINCALL viewport_hook(D9Device*d,const D9View*v){if(!owner(d)||!active||recording)return setVP(d,v);unmask();HRESULT r=setVP(d,v);if(ok(r)){read_native();enforce();}return r;}
static HRESULT WINCALL clear_hook(D9Device*d,U32 n,const void*rs,U32 flags,U32 color,float z,U32 stencil){bool ours=owner(d)&&active;if(ours){unmask();++clears;}HRESULT r=clear(d,n,rs,flags,color,z,stencil);if(ours)enforce();return r;}
static bool belongs(Com*b){D9Device*d{};bool good=ok(((HRESULT(WINCALL*)(Com*,D9Device**))b->v[3])(b,&d))&&d==gpu;if(d)d->release();return good;}
static BlockTable* block_table(Com*b){for(U32 i=0;i<nb;i++)if(bt[i].v==b->v)return &bt[i];return nullptr;}
static QueryTable* query_table(D9Query*q){for(U32 i=0;i<nq;i++)if(qt[i].v==q->v)return &qt[i];return nullptr;}
static HRESULT WINCALL capture_hook(Com*b){auto*t=block_table(b);if(!t)return HRESULT(0x80004005u);bool ours=active&&GetCurrentThreadId()==graphicsThread&&belongs(b);if(ours){unmask();++suspended;++blocks;}HRESULT r=t->capture(b);if(ours){--suspended;enforce();}return r;}
static HRESULT WINCALL apply_hook(Com*b){
 auto*t=block_table(b);if(!t)return HRESULT(0x80004005u);bool ours=active&&GetCurrentThreadId()==graphicsThread&&belongs(b);
 // A partial block must see native state BEFORE applying, including its omitted states.
 if(ours){unmask();++suspended;++blocks;}HRESULT r=t->apply(b);if(ours){--suspended;read_native();enforce();}return r;
}
static bool admit_block(Com*b){if(!b||!readable(b,4)||!readable(b->v,24))return false;if(block_table(b))return true;if(nb==8)return false;auto&t=bt[nb++];t={b->v,(BlockFn)b->v[4],(BlockFn)b->v[5]};return hook(t.v+4,(void*)t.capture,(void*)capture_hook)&&hook(t.v+5,(void*)t.apply,(void*)apply_hook);}
static HRESULT WINCALL create_block_hook(D9Device*d,U32 kind,Com**out){bool ours=owner(d)&&active;if(ours){unmask();++suspended;}HRESULT r=createBlock(d,kind,out);if(owner(d)&&ok(r)&&out&&*out&&!admit_block(*out))fault();if(ours){--suspended;enforce();}return r;}
static HRESULT WINCALL begin_hook(D9Device*d){bool ours=owner(d);if(ours&&active)unmask();HRESULT r=beginBlock(d);if(ours){if(ok(r))recording=true;if(repairDeviceTable&&!repairDeviceTable())fault();if(!ok(r))enforce();}return r;}
static HRESULT WINCALL end_hook(D9Device*d,Com**out){HRESULT r=endBlock(d,out);if(owner(d)){if(repairDeviceTable&&!repairDeviceTable())fault();if(ok(r)){recording=false;if(out&&*out&&!admit_block(*out))fault();if(active){read_native();enforce();}}}return r;}
static HRESULT WINCALL issue_hook(D9Query*q,U32 flags){
 auto*t=query_table(q);if(!t)return HRESULT(0x80004005u);bool ours=GetCurrentThreadId()==graphicsThread&&((U32(WINCALL*)(D9Query*))q->v[4])(q)==9&&belongs(q);
 if(ours){unmask();++queries;}HRESULT r=t->issue(q,flags);if(ours&&ok(r)){if(flags&2)++occlusion;if((flags&1)&&occlusion)--occlusion;enforce();}return r;
}
static bool admit_query(D9Query*q){if(!q||!readable(q,4)||!readable(q->v,32))return false;if(query_table(q))return true;if(nq==8)return false;auto&t=qt[nq++];t={q->v,(Issue)q->v[6]};return hook(t.v+6,(void*)t.issue,(void*)issue_hook);}
static HRESULT WINCALL create_query_hook(D9Device*d,U32 kind,D9Query**out){HRESULT r=createQuery(d,kind,out);if(owner(d)&&ok(r)&&out&&*out&&!admit_query(*out))fault();return r;}
static bool rearm_cell(void**cell,void*original,void*replacement){
 if(!readable(cell,4))return false;if(*cell==replacement)return true;
 // A foreign hook is never overwritten. Only our authenticated native value
 // may replace our lease at this synchronous native state-block boundary.
 return *cell==original&&hook(cell,original,replacement);
}
static bool rearm(){
 if(!installed)return true;
 struct Entry{U32 slot;void*original;void*replacement;};
 const Entry e[]={{37,(void*)target,(void*)target_hook},{47,(void*)setVP,(void*)viewport_hook},{43,(void*)clear,(void*)clear_hook},{57,(void*)setRS,(void*)setrs_hook},{58,(void*)getRS,(void*)getrs_hook},{75,(void*)setRect,(void*)setrect_hook},{76,(void*)getRect,(void*)getrect_hook},{59,(void*)createBlock,(void*)create_block_hook},{60,(void*)beginBlock,(void*)begin_hook},{61,(void*)endBlock,(void*)end_hook},{118,(void*)createQuery,(void*)create_query_hook}};
 for(auto&v:e)if(!rearm_cell(gpu->v+v.slot,v.original,v.replacement))return false;return true;
}
static void shutdown(){
 unmask();active=false;owned=current=nullptr;if(!installed)return;
 for(U32 i=0;i<nb;i++){auto&t=bt[i];undo(t.v+4,(void*)capture_hook,(void*)t.capture);undo(t.v+5,(void*)apply_hook,(void*)t.apply);}
 for(U32 i=0;i<nq;i++){auto&t=qt[i];undo(t.v+6,(void*)issue_hook,(void*)t.issue);}
 undo(gpu->v+37,(void*)target_hook,(void*)target);undo(gpu->v+47,(void*)viewport_hook,(void*)setVP);undo(gpu->v+43,(void*)clear_hook,(void*)clear);
 undo(gpu->v+57,(void*)setrs_hook,(void*)setRS);undo(gpu->v+58,(void*)getrs_hook,(void*)getRS);undo(gpu->v+75,(void*)setrect_hook,(void*)setRect);undo(gpu->v+76,(void*)getrect_hook,(void*)getRect);
 undo(gpu->v+59,(void*)create_block_hook,(void*)createBlock);undo(gpu->v+60,(void*)begin_hook,(void*)beginBlock);undo(gpu->v+61,(void*)end_hook,(void*)endBlock);undo(gpu->v+118,(void*)create_query_hook,(void*)createQuery);
 installed=false;logg.msg("F2_PIXEL_HOOKS_RETIRED",changes,restores,faults);
}
static bool install(){
 if(installed)return true;if(!gpu||GetCurrentThreadId()!=graphicsThread)return false;
 target=(Target)gpu->v[37];setVP=(SetVP)gpu->v[47];clear=(Clear)gpu->v[43];setRS=(SetRS)gpu->v[57];getRS=(GetRS)gpu->v[58];setRect=(SetRect)gpu->v[75];getRect=(GetRect)gpu->v[76];createBlock=(CreateBlock)gpu->v[59];beginBlock=(Begin)gpu->v[60];endBlock=(End)gpu->v[61];createQuery=(CreateQuery)gpu->v[118];installed=true;
 bool good=hook(gpu->v+37,(void*)target,(void*)target_hook)&&hook(gpu->v+47,(void*)setVP,(void*)viewport_hook)&&hook(gpu->v+43,(void*)clear,(void*)clear_hook)&&hook(gpu->v+57,(void*)setRS,(void*)setrs_hook)&&hook(gpu->v+58,(void*)getRS,(void*)getrs_hook)&&hook(gpu->v+75,(void*)setRect,(void*)setrect_hook)&&hook(gpu->v+76,(void*)getRect,(void*)getrect_hook)&&hook(gpu->v+59,(void*)createBlock,(void*)create_block_hook)&&hook(gpu->v+60,(void*)beginBlock,(void*)begin_hook)&&hook(gpu->v+61,(void*)endBlock,(void*)end_hook)&&hook(gpu->v+118,(void*)createQuery,(void*)create_query_hook);
 for(U32 kind=1;good&&kind<=3;kind++){Com*b{};good=ok(createBlock(gpu,kind,&b))&&b&&belongs(b)&&admit_block(b);if(b)b->release();}
 // Do not start a synthetic native recording as an installation probe: the
 // real runtime rewrites device cells there. Actual recordings use the hooks.
 const U32 kinds[]={8,9,10,11,12};for(U32 kind:kinds)if(good){D9Query*q{};good=ok(createQuery(gpu,kind,&q))&&q&&belongs(q)&&admit_query(q);if(q)q->release();}
 if(!good){shutdown();return false;}logg.msg("F2_PIXEL_STATE_OWNER_READY",nb,nq,12);return true;
}
struct Scope {
 Scope(D9Surface*s,Fov rendered,Fov desired){
  current=owned=s;active=false;ViewRegion r{};if(!installed||failedRestore||!nova_fps::on(nova_fps::PixelCrop)||!view_region(rendered,desired,bus->width,bus->height,r))return;
  crop={r.x-12,r.y-12,r.x+r.width+12,r.y+r.height+12};if(crop.left<0)crop.left=0;if(crop.top<0)crop.top=0;if(crop.right>I32(bus->width))crop.right=I32(bus->width);if(crop.bottom>I32(bus->height))crop.bottom=I32(bus->height);
  if(!crop.left&&!crop.top&&crop.right==I32(bus->width)&&crop.bottom==I32(bus->height))return;
  if(!read_native())return;active=true;++views;enforce();if(views<=4||views%240==0)logg.msg("F2_PIXEL_AREA",U64(crop.right-crop.left)*U64(crop.bottom-crop.top),U64(bus->width)*bus->height,active);
 }
 ~Scope(){unmask();active=false;owned=nullptr;U64 now=GetTickCount64();if(now-reportMs>=2000){reportMs=now;logg.msg("F2_PIXEL_STATE_COUNTS",changes,restores,faults);logg.msg("F6_SCISSOR_COALESCED",coalesced);logg.msg("F2_PIXEL_NATIVE_CUSTODY",clears,blocks,queries);}}
};
}
