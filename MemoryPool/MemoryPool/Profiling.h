#pragma once
#include <chrono>
#include <windows.h>

#define			MAX_PROFILE_SAMPLE			1000		//	PrifileSample�� �ִ� ����
#define			MAX_THREAD_NUM				100			//	�� ������ ����


typedef struct
{
	bool									bFirstFlag;				//	ù ȣ�� ����
	bool									bFlag;					//	��뿩��

	char									szName[64];				//	�������� ���� �̸�	
	std::chrono::system_clock::time_point	StartTime;				//	�������� ���� ���� �ð�
	std::chrono::duration<double>			EndTime;
	std::chrono::microseconds				AccumulatorTime;		//	�������� ���� �� ��� �ð�
	std::chrono::microseconds				MinTime;				//	�������� ���� �ּ� ��� �ð�
	std::chrono::microseconds				MaxTime;				//	�������� ���� �ִ� ��� �ð�

	int										iCall;					//	�� ȣ��Ƚ��
	DWORD									dwThreadID;				//	�ش� ������ ID




}PROFILE_SAMPLE;

static PROFILE_SAMPLE		g_ProfileSample[MAX_PROFILE_SAMPLE];

void ProfileInitial();									//	�ʱ�ȭ
void ProfileStart(char * szName);						//	Profile ����
void ProfileEnd(char * szName);							//	Profile ��
void ProfileOutTextInit(char * szFileName);
void ProfileDataOutText(char * szFileName);				//	Text ���


