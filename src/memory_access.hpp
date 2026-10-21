#pragma once
#include "platform.hpp"
// Exact Windows target, 4 KiB pages. A resident-page query avoids asking the
// kernel to find the END of a potentially very large VirtualQuery region.
// The current Win32Protection is queried afresh; this is not a persistent
// pointer or permission cache. Inconclusive and cross-page requests fall back.
namespace native_memory_access {
using Query=BOOL(WINCALL*)(HANDLE,void*,U32);
struct Page {void*address;uintptr_t attributes;};
static Query query{};static volatile I32 enabled{},attempts{},approved{},fallbacks{},blocked{};
inline void initialize(){query=(Query)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"K32QueryWorkingSetEx");}
inline void enable(bool value){store32(&enabled,value&&query?1:0);}
// 1 = allowed; -1 = current page protection rejects; 0 = original query needed.
inline int probe(const void*pointer,SIZE_T bytes,bool writing=false){
 if(!load32(&enabled)||!query)return 0;
 uintptr_t address=(uintptr_t)pointer;inc32(&attempts);
 if(!address||!bytes||bytes>~uintptr_t(0)-address){inc32(&blocked);return -1;}
 uintptr_t offset=address&4095;
 if(bytes>4096-offset){inc32(&fallbacks);return 0;}
 Page page{pointer?const_cast<void*>(pointer):nullptr,0};
 if(!query((HANDLE)(intptr_t)-1,&page,sizeof(page))||!(page.attributes&1)||(page.attributes&(uintptr_t(1)<<31))){inc32(&fallbacks);return 0;}
 U32 protection=U32((page.attributes>>4)&0x7ff),basic=protection&255;
 bool allowed=!(protection&0x100)&&basic&&basic!=1&&basic!=0x10;
 if(writing)allowed=allowed&&(basic==4||basic==8||basic==0x40||basic==0x80);
 if(!allowed){inc32(&blocked);return -1;}inc32(&approved);return 1;
}
}
