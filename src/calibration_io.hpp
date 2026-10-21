#include "weapon_input.hpp"
static weapon_input::Calibration handCalibration=weapon_input::defaults();
static U64 calibrationNextPoll=0;
static void poll_hand_calibration(){
 U64 now=GetTickCount64();if(now<calibrationNextPoll)return;calibrationNextPoll=now+250;
 wchar_t path[1200]{};U32 n=GetModuleFileNameW(nullptr,path,1024);if(!n||n>=1024)return;
 while(n&&path[n-1]!=L'\\')--n;if(!n)return;path[n]=0;wcat_(path,L"..\\config\\hand-calibration.bin",1200);
 HANDLE f=CreateFileW(path,0x80000000u,7,nullptr,3,0x80,nullptr);if(f==BAD_HANDLE)return;
 weapon_input::Calibration candidate{};U32 got=0,extra=0;U8 byte=0;
 bool read=ReadFile(f,&candidate,sizeof(candidate),&got,nullptr)!=0&&got==sizeof(candidate)&&ReadFile(f,&byte,1,&extra,nullptr)!=0&&extra==0;CloseHandle(f);
 if(!read||!weapon_input::valid(candidate)){logg.msg("LAB_CALIBRATION_REJECTED_KEEP_LAST_GOOD");return;}
 if(candidate.revision!=handCalibration.revision||candidate.checksum!=handCalibration.checksum){weapon_input::accept(handCalibration,candidate);logg.msg("LAB_CALIBRATION_APPLIED",candidate.revision,candidate.source,candidate.checksum);}
}
