#pragma once
#include "platform.hpp"
#include "fps_channel.hpp"
#include "memory_access.hpp"
namespace nova_fps {
// Independent reversible candidates. 0 is the unchanged R1 behaviour.
enum Feature { PalmIntake=1, AuxiliaryCopies=2, WorldDistance=4, PixelCrop=8, ImageReuse=16, ResidentPermission=32, TestVrFireOnly=64, ProfilingOff=128, ReceiverOverlap=256, FourthSourcePair=512, SourcePostPresent=1024, CoalesceCrop=2048 };
static volatile I32 features=2623;
static nova_fps_channel::Endpoint channel;
static inline void initialize(U32 pid,U32 host){native_memory_access::initialize();native_memory_access::enable((load32(&features)&ResidentPermission)!=0);if(channel.connect(pid,host))store32(&channel.page->features,I32(load32(&features)));}
static inline void set(U32 value){native_memory_access::enable((value&ResidentPermission)!=0);store32(&features,I32(value));if(channel.page)store32(&channel.page->features,I32(value));}
static inline void close(){channel.close();}
static U64 skippedPalm{},skippedMenu{},skippedMirror{},distanceViews{};
static inline bool on(U32 feature){return (load32(&features)&feature)!=0;}
static inline void distance(U8*view,bool magnified=false){
 if(!on(WorldDistance)||magnified)return;
 float farZ=0;memcpy(&farZ,view+0xd4,4);
 constexpr float limit=300.0f*39.37007874f;
 if(farZ>limit&&farZ<10000000){memcpy(view+0xd4,&limit,4);++distanceViews;}
}
}
