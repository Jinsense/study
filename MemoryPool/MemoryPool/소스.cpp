#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Winmm.lib")

#include <iostream>
#include <Windows.h>
#include <process.h>
#include <map>
#include <time.h>

#include "Profiling.h"
//#include "CCrashDump.h"
#include "ChatServer_MemoryPool.h"

using std::map;
using std::pair;

long g_order = 0;
int g_TEST_Count = 10000;

struct Test_Struct
{
	unsigned long long AA;
	unsigned long long BB;
};
SRWLOCK lock;
HANDLE hEvent;

CMemoryPoolTLS<Test_Struct> TLSMemPool_Test;
//	KoreanDNS::CMemoryPool_TLS<Test_Struct> TLSMemPool_Test;

unsigned _stdcall TLSMemPoolthread(void *pParam)
{
	int TestCount = g_TEST_Count;

	Test_Struct **Test = new Test_Struct*[TestCount];
	DWORD ThreadID = GetCurrentThreadId();

	DWORD retval = WaitForSingleObject(hEvent, INFINITE);

	for (int x = 0; x < TestCount; ++x)
	{
		for (int i = 0; i < TestCount; ++i)
		{
			Begin("TLSMempool_Alloc");
			//Begin("Alloc");
			Test[i] = TLSMemPool_Test.Alloc();
			//End("Alloc");
			End("TLSMempool_Alloc");
		}

		for (int i = 0; i < TestCount; ++i)
		{
			Begin("TLSMempool_Free");
			//Begin("Free");
			TLSMemPool_Test.Free(Test[i]);
			//End("Free");
			End("TLSMempool_Free");
		}
	}

	return 0;
}


int main()
{
	timeBeginPeriod(1);
	//   ProFileInit::ProFileInit();

	int Thread_Count = 4;

	int TestCase = 3;

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	HANDLE *hThread = new HANDLE[Thread_Count];

	for (int i = 0; i < Thread_Count; ++i)
	{
		switch (TestCase)
		{
			//case 1:
			//  hThread[i] = (HANDLE)_beginthreadex(NULL, 0, thread, NULL, NULL, NULL);
			//  break;

			//case 2:
			//  hThread[i] = (HANDLE)_beginthreadex(NULL, 0, MemPoolthread, NULL, NULL, NULL);
			//  break;

		case 3:
			hThread[i] = (HANDLE)_beginthreadex(NULL, 0, TLSMemPoolthread, NULL, NULL, NULL);
			break;
		}
	}

	printf("Thread Count : %d \n", Thread_Count);
	printf("TEST Count : %d \n", g_TEST_Count);
	printf("TEST Case : %d \n", TestCase);
	printf("시작!\n");
	printf("하는중...\n");

	SetEvent(hEvent);

	DWORD retval = WaitForMultipleObjects(Thread_Count, hThread, TRUE, INFINITE);

	//Sleep(3600000);

	//SaveData("profile111");

	ProfileSave();

	printf("완료!\n");
	char g = getchar();

	timeEndPeriod(1);

	return 0;
}