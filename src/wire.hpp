#pragma once
#include "platform.hpp"
// Owned by NOVA. Independent from all C1-C5 IPC layouts / epochs / scheduler.
namespace nova {
static constexpr U32 MAGIC=0x41564F4E; // NOVA, little endian
static constexpr U32 VERSION=3;
static constexpr U32 TEX_COUNT=8;
enum ImagePhase:I32{VACANT=0,WRITER_RESERVED=1,WAITING_GPU=2,GPU_FINISHED=3,READER_HELD=4};
enum Use:I32{MENU=1,STEREO=2};
struct Vec3{float x,y,z;};struct Quat{float x,y,z,w;};struct Fov{float l,r,u,d;};
struct Eye {Vec3 pos;Quat rot;Fov fov;};
struct HandPose {Vec3 pos;Quat rot;I32 valid;I32 active;};
struct Tracking {I64 predictedTime;U64 epoch,sampleId,sampledMs;Eye eye[2];HandPose hand[2];I32 valid,focused;U32 reserved;};
// Written while the producer holds WRITER_RESERVED. Read only under READER_HELD.
// Two raw XR eyes, the exact native views and matrices belong to ONE pixel pair.
struct CapturedStereo {
 U64 pairId,trackingEpoch,sampleId; I64 predictedTime; U64 capturedMs;
 Eye eye[2]; Vec3 sourceEye[2]; float sourceAngles[2][3],projection[2][16];
 U32 projectionKind,valid;
};
struct Image {volatile I32 phase;I32 purpose;U64 generation,frame;U32 width,height;U64 writeMs;};
struct Shared {
 U32 magic,version,bytes,gamePid,hostPid,width,height,textureCount;
 volatile I32 stop,resourceReady,gameReady,openMask,sceneCalls,presentCalls,menuPixels,stereoPixels;
 volatile I32 publishMenu,publishStereo,submitMenu,submitStereo,graphicsErrors,xrErrors;
 volatile I32 poseSequence,worldEvidence,rendererState,deviceGeneration;
 wchar_t logDir[512]; // Host-owned immutable location, survives Steam process spawning without env inheritance.
 U64 sharedHandles[TEX_COUNT];
 Image images[4]; // [0..2] eye pairs indexed as 0,1,2; [3] menu; reserve one spare texture
 Tracking xr;
 CapturedStereo captured[3];
 volatile I32 slotBusy,poseRejects,projectionPairs,epochRejects,capturedPackets,consumedPackets;
 volatile I32 submitPosePairs,emptyFrames,endFrameSuccess,sourceFenceFaults;

};
static_assert(sizeof(Image)==40,"NOVA Image storage byte contract");
static_assert(sizeof(CapturedStereo)==312,"CapturedStereo cross-process ABI");
static_assert(sizeof(Tracking)==208,"Tracking cross-process ABI");
#ifdef _WIN32
static_assert(sizeof(Shared)==2536,"Win32/Win64 shared ABI");
static_assert(offsetof(Shared,captured)==1560,"Capture array ABI");
static_assert(offsetof(Shared,slotBusy)==2496,"Telemetry ABI");
#endif
inline bool read_tracking(const Shared*s,Tracking&out){for(int n=0;n<8;++n){I32 a=(I32)load32(&s->poseSequence);if(a&1)continue;memcpy(&out,&s->xr,sizeof(out));barrier();if(a==(I32)load32(&s->poseSequence))return true;}return false;}
inline void write_tracking(Shared*s,const Tracking&in){I32 n=(I32)load32(&s->poseSequence);store32(&s->poseSequence,n+1);barrier();memcpy(&s->xr,&in,sizeof(in));barrier();store32(&s->poseSequence,n+2);}
inline void mapping_name(wchar_t*out,U32 pid){const wchar_t*p=L"Local\\NOVA_R3_SCENE_";size_t i=0;while(p[i]){out[i]=p[i];i++;}decimal_(out+i,pid);}
inline int reserve(Shared*s,int index){Image&i=s->images[index];return compare32(&i.phase,VACANT,WRITER_RESERVED);}
inline void fail_reserve(Shared*s,int index){store32(&s->images[index].phase,VACANT);}
inline bool commit(Shared*s,int index,U64 frame,U64 generation,U32 w,U32 h){Image&i=s->images[index];if(load32(&i.phase)!=WRITER_RESERVED)return false;i.frame=frame;i.generation=generation;i.width=w;i.height=h;i.writeMs=GetTickCount64();barrier();store32(&i.phase,WAITING_GPU);return true;}
inline bool ready(Shared*s,int index){return compare32(&s->images[index].phase,WAITING_GPU,GPU_FINISHED);}
inline bool acquire(Shared*s,int index){return compare32(&s->images[index].phase,GPU_FINISHED,READER_HELD);}
inline void release(Shared*s,int index){store32(&s->images[index].phase,VACANT);}
}
