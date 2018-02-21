#include "Profiling_My.h"
using namespace std;

void ProfileInitial()
{
	for (auto j = 0; j < MAX_THREAD_NUM; j++)
	{
		for (auto i = 0; i < MAX_PROFILE_SAMPLE; i++)
		{
			g_ProfileSample[i].bFirstFlag = FALSE;
			g_ProfileSample[i].bFlag = FALSE;
			g_ProfileSample[i].iCall = 0;
			g_ProfileSample[i].szName[0] = { 0, };
			g_ProfileSample[i].StartTime = std::chrono::system_clock::now();
			g_ProfileSample[i].EndTime = std::chrono::system_clock::now() - g_ProfileSample[i].StartTime;
			g_ProfileSample[i].AccumulatorTime = (std::chrono::microseconds)0;
			g_ProfileSample[i].MinTime = (std::chrono::microseconds)0xffffffffffffffff;
			g_ProfileSample[i].MaxTime = (std::chrono::microseconds)0;
			g_ProfileSample[i].dwThreadID = NULL;
		}
	}
}

void ProfileStart(char * szName)
{
	//	파라미터로 받은 함수명을 검색해 있는지 체크한다
	//	없을경우 빈 곳에 새로 생성, 있을경우 해당 인덱스로 시간측정 시작
	for (auto i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (strcmp(szName, g_ProfileSample[i].szName))
		{
			//	찾은 경우
			g_ProfileSample[i].bFlag = TRUE;
			g_ProfileSample[i].iCall++;
			g_ProfileSample[i].dwThreadID = GetCurrentThreadId();
			g_ProfileSample[i].StartTime = std::chrono::system_clock::now();
			return;
		}
	}

	//	처음 생성하는 경우
	for (auto i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (g_ProfileSample[i].bFirstFlag == FALSE)
		{
			strcpy_s(g_ProfileSample[i].szName, szName);
			g_ProfileSample[i].bFirstFlag = TRUE;
			g_ProfileSample[i].bFlag = TRUE;
			g_ProfileSample[i].iCall++;
			g_ProfileSample[i].dwThreadID = GetCurrentThreadId();
			g_ProfileSample[i].StartTime = std::chrono::system_clock::now();
			g_ProfileSample[i].AccumulatorTime = (std::chrono::microseconds)0;
			break;
		}
	}
}

void ProfileEnd(char * szName)
{
	for (auto i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (strcmp(szName, g_ProfileSample[i].szName))
		{
			g_ProfileSample[i].EndTime = std::chrono::system_clock::now() - g_ProfileSample[i].StartTime;
			g_ProfileSample[i].AccumulatorTime += std::chrono::duration_cast<std::chrono::microseconds>(g_ProfileSample[i].EndTime);

			if (g_ProfileSample[i].MinTime > std::chrono::duration_cast<std::chrono::microseconds>(g_ProfileSample[i].EndTime))
				g_ProfileSample[i].MinTime = std::chrono::duration_cast<std::chrono::microseconds>(g_ProfileSample[i].EndTime);
			if (g_ProfileSample[i].MaxTime < std::chrono::duration_cast<std::chrono::microseconds>(g_ProfileSample[i].EndTime))
				g_ProfileSample[i].MaxTime = std::chrono::duration_cast<std::chrono::microseconds>(g_ProfileSample[i].EndTime);

			g_ProfileSample[i].bFlag = FALSE;
			return;
		}
	}
}

void ProfileOutTextInit(char * szFileName)
{
	FILE * pFile;

	if (0 != fopen_s(&pFile, szFileName, "w"))
		return;
	fprintf_s(pFile, "	ThreadID	|		Name	|	Average		|	Min		|	Max		|	Call	|	\n");
	fprintf_s(pFile, "------------------------------------------------------------------\n");

	fclose(pFile);
}

void ProfileDataOutText(char * szFileName)
{
	//	텍스트 출력을 할 때 최대, 최소값은 평균을 구할 때 제외한다.
	//	ex) 총 호출 시간 - ( 최소 + 최대 ) / 총 호출 횟수 - 2
	char szSpace[64] = { 0, };
	FILE * pFile;
	PROFILE_SAMPLE * pSample;

	if (0 != fopen_s(&pFile, szFileName, "a"))
		return;

	pSample = g_ProfileSample;

	for (auto i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (TRUE == pSample->bFirstFlag)
		{
			fprintf_s(pFile, "	%d	|		%s	|	%f㎲	|	%f㎲	|	%f㎲	|	%d	\n",
				pSample->dwThreadID, pSample->szName, pSample->AccumulatorTime.count(), pSample->MinTime.count(), pSample->MaxTime.count(), pSample->iCall);
		}
		pSample++;
	}
	fprintf_s(pFile, "------------------------------------------------------------------\n");
	fclose(pFile);
}