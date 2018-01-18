#include <iostream>
#include <conio.h>
#include "MemoryPool.h"
#include "CDump.h"
#include "Profiling.h"


#define		TEST_MAX	10000
#define		THREAD_NUM	10

struct TestData
{
	__int64 Data;
	__int64 Count;
};


bool	g_Shutdown = false;

CMemoryPool<TestData> g_Pool;


unsigned __stdcall ThreadF(void * Param)
{
	TestData * pDataArray[TEST_MAX];
	while (!g_Shutdown)
	{
		for (auto i = 0; i < TEST_MAX; i++)
		{
			pDataArray[i] = g_Pool.Alloc();		
		}
		Sleep(10);
		for (auto i = 0; i < TEST_MAX; i++)
		{
			if (pDataArray[i]->Data == 0x00000000ffffffff && pDataArray[i]->Count == 0)
				continue;
			else
				CCrashDump::Crash();
		}
		Sleep(10);
		for (auto i = 0; i < TEST_MAX; i++)
		{
			InterlockedIncrement64(&(pDataArray[i]->Data));
			InterlockedIncrement64(&(pDataArray[i]->Count));
		}
		Sleep(10);
		for (auto i = 0; i < TEST_MAX; i++)
		{
			if (pDataArray[i]->Data == 0x0000000100000000 && pDataArray[i]->Count == 1)
				continue;
			else
				CCrashDump::Crash();
		}
		Sleep(10);
		for (auto i = 0; i < TEST_MAX; i++)
		{
			InterlockedDecrement64(&(pDataArray[i]->Data));
			InterlockedDecrement64(&(pDataArray[i]->Count));
		}
		Sleep(10);
		for (auto i = 0; i < TEST_MAX; i++)
		{
			if (pDataArray[i]->Data == 0x00000000ffffffff && pDataArray[i]->Count == 0)
				continue;
			else
				CCrashDump::Crash();
		}
		Sleep(10);
		for (auto i = 0; i < TEST_MAX; i++)
		{
			g_Pool.Free(pDataArray[i]);
		}
		Sleep(10);

	}
	return 0;
}

void main()
{
	g_Shutdown = false;
	TestData ** pDataArray = new TestData *[TEST_MAX * THREAD_NUM];
	ProfileInitial();
	
	for (auto i = 0; i < TEST_MAX * THREAD_NUM; i++)
	{
		pDataArray[i] = g_Pool.Alloc();
		pDataArray[i]->Data = 0x00000000ffffffff;
		pDataArray[i]->Count = 0;
	}

	for (auto i = 0; i < TEST_MAX * THREAD_NUM; i++)
	{
		g_Pool.Free(pDataArray[i]);
	}

	HANDLE hThread[THREAD_NUM];
	for (auto i = 0; i < THREAD_NUM; i++)
	{
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, &ThreadF, (void*)rand(), 0, NULL);
	}

	while (!g_Shutdown)
	{
		Sleep(1000);
		printf("MemoryPool AllocCount %lu \t MemoryPool UseCount %lu\n", g_Pool.GetAllocCount(), g_Pool.GetUseCount());
		
		if (_kbhit())
		{
			int input = _getch();
			if (input == 'q' || input == 'Q')
				g_Shutdown = true;

			if (input == 's')
			{
				ProfileOutTextInit("MemoryPool_Test");
				ProfileDataOutText("MemoryPool_Test");
			}
		}
	}
	WaitForMultipleObjects(10, hThread, true, INFINITE);
}