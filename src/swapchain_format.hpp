#pragma once
#include "platform.hpp"
namespace nova {
// Keep the user-accepted BGRA8 path when available; otherwise respect runtime
// order among formats for which the owned host has a real conversion path.
inline I64 choose_swapchain_format(const I64*formats,U32 count){
 for(U32 i=0;i<count;i++)if(formats[i]==87)return 87;
 for(U32 i=0;i<count;i++)if(formats[i]==28||formats[i]==29||formats[i]==91)return formats[i];
 return 0;
}
inline bool native_bgra_copy_compatible(U32 format){return format==87||format==91;}
}
