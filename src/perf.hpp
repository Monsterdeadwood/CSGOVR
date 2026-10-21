#pragma once
#include "platform.hpp"
namespace nova_perf {
enum Stage { NativeRender, HandBegin, HandEye, HandEnd, Stereo, SourcePoll, MenuCopy,
             Present, WaitFrame, Tracking, Consume, SwapCopy, EndFrame, OpticImage, BodyBegin, BodyOutput, BodyIntake, FrameRoot, GameControl, FramePeriod, Count };
#ifdef NOVA_LAB_PERF
extern "C" WINIMPORT BOOL WINCALL QueryPerformanceCounter(I64*);
extern "C" WINIMPORT BOOL WINCALL QueryPerformanceFrequency(I64*);
struct Record { U64 calls{}, ticks{}, maximum{}; };
static Record records[Count]{};
static I64 frequency{};
static U32 ownerThread{};
static U64 lastReport{};
static bool enabled=true;
static U64 previousFrame{};
static Log logger;
static const char*names[Count]={"native_render","hand_begin","hand_eye","hand_end","stereo_total","source_poll","menu_copy","present_api","xr_wait","tracking","consume","swap_copy","xr_end","optic_image","body_begin","body_output","body_intake","frame_root","game_control","frame_period"};
inline U64 clock(){I64 t=0;QueryPerformanceCounter(&t);return U64(t);}
inline void initialize(const wchar_t*name,const wchar_t*directory=nullptr){
 if(frequency)return;ownerThread=GetCurrentThreadId();QueryPerformanceFrequency(&frequency);logger.open(name,directory);lastReport=GetTickCount64();
}
inline void set_enabled(bool value){if(enabled==value)return;enabled=value;for(auto&r:records)r={};previousFrame=0;lastReport=GetTickCount64();}
inline void frame_boundary(){if(!enabled||!frequency||GetCurrentThreadId()!=ownerThread)return;U64 now=clock();if(previousFrame){U64 elapsed=now-previousFrame;auto&r=records[FramePeriod];++r.calls;r.ticks+=elapsed;if(elapsed>r.maximum)r.maximum=elapsed;}previousFrame=now;}
struct Scope {
 Stage stage;U64 start{};bool owned{};
 explicit Scope(Stage s):stage(s),owned(enabled&&frequency&&GetCurrentThreadId()==ownerThread){if(owned)start=clock();}
 ~Scope(){if(!owned)return;U64 elapsed=clock()-start;auto&r=records[stage];++r.calls;r.ticks+=elapsed;if(elapsed>r.maximum)r.maximum=elapsed;}
};
inline void report(){
 if(!enabled)return;
 U64 now=GetTickCount64();if(!frequency||GetCurrentThreadId()!=ownerThread||now-lastReport<2000)return;
 U64 window=now-lastReport;lastReport=now;
 for(unsigned i=0;i<Count;i++){auto&r=records[i];if(!r.calls)continue;
  char text[320];int n=_snprintf(text,sizeof(text),"NOVA_PERF tick=%llu window_ms=%llu stage=%s calls=%llu total_ms=%.6f mean_ms=%.6f max_ms=%.6f\r\n",now,window,names[i],r.calls,double(r.ticks)*1000/double(frequency),double(r.ticks)*1000/double(frequency)/double(r.calls),double(r.maximum)*1000/double(frequency));
  if(n>0&&logger.file!=BAD_HANDLE){U32 done=0;WriteFile(logger.file,text,U32(n<319?n:319),&done,nullptr);}r={};
 }
}
#else
inline void set_enabled(bool){}
inline void frame_boundary(){}
inline void initialize(const wchar_t*,const wchar_t* =nullptr){}
struct Scope {explicit Scope(Stage){};};
inline void report(){}
#endif
}
