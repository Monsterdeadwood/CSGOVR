#pragma once
#include "wire.hpp"
// Additive fourth source pair. The accepted 2536-byte image/pose ABI and the
// 64-byte FPS page stay byte-identical. F6 endpoints authenticate this page.
namespace nova_frame_transport {
using namespace nova;
static constexpr U32 MAGIC=0x3654464e,VERSION=1;
struct alignas(8) Extension {
 U32 magic,version,bytes,gamePid,hostPid,reserved;
 U64 handles[2];Image image;CapturedStereo captured;
 U64 superseded,noSource;
};
static_assert(sizeof(Extension)==408,"F6 fourth-pair ABI");
static_assert(offsetof(Extension,image)==40,"F6 image offset");
static_assert(offsetof(Extension,captured)==80,"F6 capture offset");
static inline void name(wchar_t*out,U32 pid){const wchar_t*s=L"Local\\NOVA_FRAME_F6_";U32 n=0;while(s[n]){out[n]=s[n];++n;}decimal_(out+n,pid);}
struct Endpoint {
 HANDLE mapping{};Extension*page{};
 void close(){if(page)UnmapViewOfFile(page);if(mapping)CloseHandle(mapping);page=nullptr;mapping=nullptr;}
 bool valid(U32 pid,U32 host){return page&&page->magic==MAGIC&&page->version==VERSION&&page->bytes==sizeof(Extension)&&page->gamePid==pid&&page->hostPid==host;}
 bool create(U32 pid,U32 host){
  wchar_t text[64]{};name(text,pid);mapping=CreateFileMappingW(BAD_HANDLE,nullptr,4,0,sizeof(Extension),text);
  if(!mapping||GetLastError()==183){close();return false;}page=(Extension*)MapViewOfFile(mapping,0xf001f,0,0,sizeof(Extension));if(!page){close();return false;}
  memset(page,0,sizeof(*page));page->version=VERSION;page->bytes=sizeof(*page);page->gamePid=pid;page->hostPid=host;barrier();page->magic=MAGIC;return true;
 }
 bool connect(U32 pid,U32 host){wchar_t text[64]{};name(text,pid);mapping=OpenFileMappingW(0xf001f,0,text);if(!mapping)return false;page=(Extension*)MapViewOfFile(mapping,0xf001f,0,0,sizeof(Extension));if(!valid(pid,host)){close();return false;}return true;}
};
inline bool reserve(Image&i){return compare32(&i.phase,VACANT,WRITER_RESERVED);}
inline void fail(Image&i){store32(&i.phase,VACANT);}
inline bool commit(Image&i,U64 frame,U64 epoch,U32 w,U32 h){if(load32(&i.phase)!=WRITER_RESERVED)return false;i.frame=frame;i.generation=epoch;i.width=w;i.height=h;i.writeMs=GetTickCount64();barrier();store32(&i.phase,WAITING_GPU);return true;}
inline bool ready(Image&i){return compare32(&i.phase,WAITING_GPU,GPU_FINISHED);}
inline bool acquire(Image&i){return compare32(&i.phase,GPU_FINISHED,READER_HELD);}
inline void release(Image&i){store32(&i.phase,VACANT);}
}
