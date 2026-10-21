#pragma once
#include "platform.hpp"
namespace xr_content_cache {
// A key describes released PIXELS. Pose/FOV always comes from those pixels'
// captured native pair, never from the newest tracking sample.
struct Key {U64 epoch{},generation{},quality{};U32 kind{};};
static inline bool equal(const Key&a,const Key&b){return a.epoch==b.epoch&&a.generation==b.generation&&a.quality==b.quality&&a.kind==b.kind;}
struct Released {
 Key key{};bool valid{};
 bool needs(const Key&candidate,bool enabled)const{return !enabled||!valid||!equal(key,candidate);}
 void invalidate(){valid=false;}
 // Call only after BOTH world eyes, or the single quad, released successfully.
 void committed(const Key&candidate){key=candidate;valid=true;}
};
}
