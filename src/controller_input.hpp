#pragma once
#include "math.hpp"
namespace controller_input {
using namespace nova;
constexpr U32 MAGIC=0x4942564e,VERSION=2;
enum Button:U32 {A=1,B=2,X=4,Y=8,Menu=16,LeftClick=32,RightClick=64,RightGrip=128,LeftGrip=256,Use=512,Walk=1024};
enum Feature:U32 {Pose=1,TriggerInput=2,Buttons=4,Sticks=8,Haptics=16};
enum FeedbackKind:U32 {Shot=1,ReloadInput=2,WeaponInput=3,MenuInput=4,Test=5};
struct Sample {U64 sampledMs,sample,epoch;U32 valid,focused;float trigger;U32 active;float leftTrigger,leftGrip,rightGrip,leftX,leftY,rightX,rightY;U32 buttons,actionMask;};
struct Feedback {U64 serial,createdMs,epoch;I32 command;U32 kind,weapon;float value;};
struct alignas(8) Channel {
 U32 magic,version,bytes,gamePid,hostPid;volatile I32 sequence;U32 features;volatile I32 screenExtent;Sample sample;
 volatile I32 feedbackBusy;U32 padding;Feedback feedback;U32 hapticApplied,hapticErrors,hapticDropped;
 volatile I32 settingsRequest,settingsAck;float hapticGain;U32 snapDegrees,turnMode;float turnRate;volatile I32 gameUiOpen;
};
static_assert(sizeof(Sample)==80&&sizeof(Feedback)==40&&sizeof(Channel)==200&&offsetof(Channel,sample)==32&&offsetof(Channel,feedback)==120&&offsetof(Channel,hapticGain)==180,"Controller v2 duplex ABI");
inline void name(wchar_t*out,U32 pid){const wchar_t*base=L"Local\\NOVA_XR_BUTTONS_";U32 i=0;while(base[i]){out[i]=base[i];i++;}decimal_(out+i,pid);}
inline bool read(const Channel*c,Sample&out){for(U32 i=0;i<8;i++){U32 before=load32(&c->sequence);if(before&1)continue;memcpy(&out,&c->sample,sizeof(out));barrier();if(before==load32(&c->sequence))return true;}return false;}
inline void write(Channel*c,const Sample&sample){U32 before=load32(&c->sequence);store32(&c->sequence,I32(before+1));memcpy(&c->sample,&sample,sizeof(sample));barrier();store32(&c->sequence,I32(before+2));}
inline bool live(const Sample&s,U64 now){return s.valid&&s.focused&&s.active&&s.sample&&s.epoch&&now>=s.sampledMs&&now-s.sampledMs<=250;}
inline bool feedback_read(Channel*c,Feedback&out){if(!compare32(&c->feedbackBusy,0,1))return false;memcpy(&out,&c->feedback,sizeof(out));store32(&c->feedbackBusy,0);return true;}
inline float axis(float value){return finite_in_range(value,1.05f)?value:0;}
inline void movement(float x,float y,float headYaw,float&forward,float&side){
 x=axis(x);y=axis(y);float magnitude=float(sqrt(x*x+y*y));forward=side=0;if(magnitude<=.18f)return;
 float scale=((magnitude<1.f?magnitude:1.f)-.18f)/(.82f*magnitude);x*=scale;y*=scale;float c=float(cos(rad(headYaw))),s=float(sin(rad(headYaw)));
 forward=450*(y*c+x*s);side=450*(x*c-y*s);
}
struct AxisGate {bool armed{};int update(float value,bool ready){if(!ready){armed=false;return 0;}value=axis(value);if(absf(value)<.25f){armed=true;return 0;}if(armed&&absf(value)>.7f){armed=false;return value>0?1:-1;}return 0;}};
struct WalkHold {
 U32 held{};
 int update(U32 down,U32 up){
  bool before=held!=0;held=((held&~up)|down)&(Walk|LeftGrip);
  return before==(held!=0)?0:(held?1:-1);
 }
};
struct ButtonEdges {
 U32 held{},armed{},analogs{};U64 epoch{};
 void update(const Sample&s,U64 now,bool ready,U32&down,U32&up){
  U32 released=0;bool okay=ready&&live(s,now);if(!okay||s.epoch!=epoch){down=0;up=released=held;held=armed=analogs=0;epoch=s.epoch;if(!okay)return;}
  float values[4]={s.rightGrip,s.leftGrip,s.leftTrigger,-axis(s.rightY)};U32 bits[4]={RightGrip,LeftGrip,Use,Walk};
  for(U32 i=0;i<4;i++){float threshold=(analogs&bits[i])?.25f:.65f;if(axis(values[i])>=threshold)analogs|=bits[i];else analogs&=~bits[i];}
  U32 raw=(s.buttons&127)|analogs;armed|=(~raw)&2047u;U32 next=raw&armed;down=next&~held;up=released|(held&~next);held=next;
 }
 U32 release(){U32 up=held;held=armed=analogs=0;epoch=0;return up;}
};
struct MenuGesture {
 enum Result {None,ShortPress,LongPress};
 bool armed{},pressed{},longSent{};U64 epoch{},pressedMs{};
 void cancel(){armed=pressed=longSent=false;pressedMs=0;}
 Result update(const Sample&s,U64 now,bool ready,U32 button=Menu){
  bool okay=ready&&live(s,now);
  if(!okay||epoch!=s.epoch){cancel();epoch=s.epoch;if(!okay)return None;}
  if(!(s.buttons&button)){Result result=pressed&&!longSent?ShortPress:None;armed=true;pressed=longSent=false;return result;}
  if(armed&&!pressed){pressed=true;pressedMs=now;}
  if(pressed&&!longSent&&now>=pressedMs&&now-pressedMs>=700){longSent=true;return LongPress;}
  return None;
 }
};
// Edges only, hysteresis, release on focus/tracking/heartbeat loss. A held
// trigger after re-entry must be released before it may start a new attack.
struct Trigger {
 bool down{},armed{};U64 epoch{};
 int release(){int edge=down?-1:0;down=armed=false;epoch=0;return edge;}
 int update(const Sample&s,U64 now,bool gameReady){
  bool okay=gameReady&&s.valid&&s.focused&&s.active&&s.sample&&s.epoch&&now>=s.sampledMs&&now-s.sampledMs<=250&&finite_in_range(s.trigger,1.001f)&&s.trigger>=0;
  bool next=down;
  if(!okay||epoch!=s.epoch){next=false;armed=false;epoch=s.epoch;}
  if(okay){if(s.trigger<=.25f){next=false;armed=true;}else if(armed&&s.trigger>=.65f)next=true;}
  if(next==down)return 0;down=next;return down?1:-1;
 }
};
}
