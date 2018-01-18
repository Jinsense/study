#pragma once
#include <chrono>
#include <windows.h>

#define			MAX_PROFILE_SAMPLE			1000		//	PrifileSample의 최대 개수
#define			MAX_THREAD_NUM				100			//	총 쓰레드 개수


typedef struct
{
	bool									bFirstFlag;				//	첫 호출 여부
	bool									bFlag;					//	사용여부

	char									szName[64];				//	프로파일 샘플 이름	
	std::chrono::system_clock::time_point	StartTime;				//	프로파일 샘플 시작 시간
	std::chrono::duration<double>			EndTime;
	std::chrono::microseconds				AccumulatorTime;		//	프로파일 샘플 총 사용 시간
	std::chrono::microseconds				MinTime;				//	프로파일 샘플 최소 사용 시간
	std::chrono::microseconds				MaxTime;				//	프로파일 샘플 최대 사용 시간

	int										iCall;					//	총 호출횟수
	DWORD									dwThreadID;				//	해당 쓰레드 ID




}PROFILE_SAMPLE;

static PROFILE_SAMPLE		g_ProfileSample[MAX_PROFILE_SAMPLE];

void ProfileInitial();									//	초기화
void ProfileStart(char * szName);						//	Profile 시작
void ProfileEnd(char * szName);							//	Profile 끝
void ProfileOutTextInit(char * szFileName);
void ProfileDataOutText(char * szFileName);				//	Text 출력


