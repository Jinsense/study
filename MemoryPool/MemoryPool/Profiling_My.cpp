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
	//	�Ķ���ͷ� ���� �Լ����� �˻��� �ִ��� üũ�Ѵ�
	//	������� �� ���� ���� ����, ������� �ش� �ε����� �ð����� ����
	for (auto i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (strcmp(szName, g_ProfileSample[i].szName))
		{
			//	ã�� ���
			g_ProfileSample[i].bFlag = TRUE;
			g_ProfileSample[i].iCall++;
			g_ProfileSample[i].dwThreadID = GetCurrentThreadId();
			g_ProfileSample[i].StartTime = std::chrono::system_clock::now();
			return;
		}
	}

	//	ó�� �����ϴ� ���
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
	//	�ؽ�Ʈ ����� �� �� �ִ�, �ּҰ��� ����� ���� �� �����Ѵ�.
	//	ex) �� ȣ�� �ð� - ( �ּ� + �ִ� ) / �� ȣ�� Ƚ�� - 2
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
			fprintf_s(pFile, "	%d	|		%s	|	%f��	|	%f��	|	%f��	|	%d	\n",
				pSample->dwThreadID, pSample->szName, pSample->AccumulatorTime.count(), pSample->MinTime.count(), pSample->MaxTime.count(), pSample->iCall);
		}
		pSample++;
	}
	fprintf_s(pFile, "------------------------------------------------------------------\n");
	fclose(pFile);
}