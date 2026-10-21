#pragma once
#include "platform.hpp"
// Additive private control page. The accepted image, tracking and input ABIs
// remain unchanged. Native code owns features; the XR thread owns the rest.
namespace nova_fps_channel {
static constexpr U32 MAGIC=0x5046564e,VERSION=1;
struct alignas(8) Channel {
 U32 magic,version,bytes,gamePid,hostPid;volatile I32 features,applied,submittedKind;
 U64 worldUploads,worldReuses,quadUploads,quadReuses;
};
static_assert(sizeof(Channel)==64,"FPS channel cross-process ABI");
static inline void name(wchar_t*out,U32 pid){const wchar_t*s=L"Local\\NOVA_FPS_";U32 n=0;while(s[n]){out[n]=s[n];++n;}decimal_(out+n,pid);}
struct Endpoint {
 HANDLE mapping{};Channel*page{};
 bool valid(U32 pid,U32 host){return page&&page->magic==MAGIC&&page->version==VERSION&&page->bytes==sizeof(Channel)&&page->gamePid==pid&&page->hostPid==host;}
 void close(){if(page)UnmapViewOfFile(page);if(mapping)CloseHandle(mapping);page=nullptr;mapping=nullptr;}
 bool create(U32 pid,U32 host,U32 features){
  wchar_t text[64]{};name(text,pid);mapping=CreateFileMappingW(BAD_HANDLE,nullptr,4,0,sizeof(Channel),text);
  if(!mapping||GetLastError()==183){close();return false;}
  page=(Channel*)MapViewOfFile(mapping,0xf001f,0,0,sizeof(Channel));if(!page){close();return false;}
  memset(page,0,sizeof(Channel));page->version=VERSION;page->bytes=sizeof(Channel);page->gamePid=pid;page->hostPid=host;page->features=I32(features);barrier();page->magic=MAGIC;return true;
 }
 bool connect(U32 pid,U32 host){
  wchar_t text[64]{};name(text,pid);mapping=OpenFileMappingW(0xf001f,0,text);if(!mapping)return false;
  page=(Channel*)MapViewOfFile(mapping,0xf001f,0,0,sizeof(Channel));if(!valid(pid,host)){close();return false;}return true;
 }
};
}
