#include <iostream>
#include <conio.h>
//	#include "MemoryPool.h"
#include "ChatServer_MemoryPool.h"
#include "LockfreeStack.h"
#include "LockfreeQueue.h"
#include "CDump.h"
#include "Profiling.h"


#define		TEST_MAX	10000
#define		THREAD_NUM	4

struct TestData
{
	__int64 Data;
	__int64 Count;
};


bool	g_Shutdown = false;

//CMemoryPool<TestData> g_Pool;
CMemoryPoolTLS<TestData> g_Pool;
CLockfreeStack<TestData*> g_LFStack;
CLockfreeQueue<TestData*> g_LFQueue;


unsigned __stdcall MemoryPool_Test(void * Param)
{
	TestData * pDataArray[TEST_MAX];
	while (!g_Shutdown)
	{
//		printf("ThreadID %d \n", GetCurrentThreadId());
		for (auto i = 0; i < TEST_MAX; i++)
		{
			Begin("MemoryPoolTLS");
			pDataArray[i] = g_Pool.Alloc();	
			End("MemoryPoolTLS");
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
unsigned __stdcall LFStack_Test(void * Param)
{
	TestData * pDataArray[TEST_MAX];
	while (!g_Shutdown)
	{
		for (auto i = 0; i < TEST_MAX; i++)
		{
			g_LFStack.Pop(pDataArray[i]);
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
			g_LFStack.Push(pDataArray[i]);
		}
		Sleep(10);

	}
	return 0;
}
unsigned __stdcall LFQueue_Test(void * Param)
{
	TestData * pDataArray[TEST_MAX];
	while (!g_Shutdown)
	{
		for (auto i = 0; i < TEST_MAX; i++)
		{
			g_LFQueue.Dequeue(pDataArray[i]);
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
			g_LFQueue.Enqueue(pDataArray[i]);
		}
		Sleep(10);

	}
	return 0;
}
void main()
{
	g_Shutdown = false;
	TestData ** pDataArray = new TestData *[TEST_MAX * THREAD_NUM];
//	ProfileInitial();
	int num = 0;
	std::cout << "What's test ?" << std::endl;
	std::cout << "1 : LockFreeMemoryPool" << std::endl;
	std::cout << "2 : LockFreeStack" << std::endl;
	std::cout << "3 : LockFreeQueue" << std::endl;
	std::cin >> num;

	switch (num)
	{
	case 1 :
	{
		for (auto i = 0; i < TEST_MAX * THREAD_NUM; i++)
		{
//			Begin("MemoryPoolTLS");
			pDataArray[i] = g_Pool.Alloc();
//			End("MemoryPoolTLS");
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
			hThread[i] = (HANDLE)_beginthreadex(NULL, 0, &MemoryPool_Test, (void*)rand(), 0, NULL);
		}
		while (!g_Shutdown)
		{
			Sleep(1000);
//			printf("MemoryPool AllocCount %lu \t MemoryPool UseCount %lu\n", g_Pool.GetAllocCount(), g_Pool.GetUseCount());

			if (_kbhit())
			{
				int input = _getch();
				if (input == 'q' || input == 'Q')
					g_Shutdown = true;

				if (input == 's')
				{
					ProfileSave();
//					ProfileOutTextInit("MemoryPool_Test");
//					ProfileDataOutText("MemoryPool_Test");
				}
			}
		}
		WaitForMultipleObjects(10, hThread, true, INFINITE);
	}
	break;
	case 2 :
	{
		for (auto i = 0; i < TEST_MAX * THREAD_NUM; i++)
		{
			pDataArray[i] = g_Pool.Alloc();
			pDataArray[i]->Data = 0x00000000ffffffff;
			pDataArray[i]->Count = 0;
			g_LFStack.Push(pDataArray[i]);
		}

		HANDLE hThread[THREAD_NUM];
		for (auto i = 0; i < THREAD_NUM; i++)
		{
			hThread[i] = (HANDLE)_beginthreadex(NULL, 0, &LFStack_Test, (void*)rand(), 0, NULL);
		}

		while (!g_Shutdown)
		{
			Sleep(1000);
			printf("M_Pool AllocCount %lu   M_Pool UseCount %lu \t Stack AllocCount %lu\n", g_LFStack.GetStackMemoryPoolAllocCount(), g_LFStack.GetStackMemoryPoolUseCount(), g_LFStack.GetStackUseCount());

			if (_kbhit())
			{
				int input = _getch();
				if (input == 'q' || input == 'Q')
					g_Shutdown = true;

				if (input == 's')
				{
//					ProfileOutTextInit("LFStack_Test");
//					ProfileDataOutText("LFStack_Test");
				}
			}
		}
		WaitForMultipleObjects(10, hThread, true, INFINITE);
	}
	break;
	case 3:
	{
		for (auto i = 0; i < TEST_MAX * THREAD_NUM; i++)
		{
			pDataArray[i] = g_Pool.Alloc();
			pDataArray[i]->Data = 0x00000000ffffffff;
			pDataArray[i]->Count = 0;
			g_LFQueue.Enqueue(pDataArray[i]);
		}

		HANDLE hThread[THREAD_NUM];
		for (auto i = 0; i < THREAD_NUM; i++)
		{
			hThread[i] = (HANDLE)_beginthreadex(NULL, 0, &LFQueue_Test, (void*)rand(), 0, NULL);
		}

		while (!g_Shutdown)
		{
			Sleep(1000);
			printf("M_Pool AllocCount %lu   M_Pool UseCount %lu \t Queue AllocCount %lu\n", g_LFQueue.GetQueueMemoryPoolAllocCount(), g_LFQueue.GetQueueMemoryPoolUseCount(), g_LFQueue.GetQueueUseCount());

			if (_kbhit())
			{
				int input = _getch();
				if (input == 'q' || input == 'Q')
					g_Shutdown = true;

				if (input == 's')
				{
//					ProfileOutTextInit("LFStack_Test");
//					ProfileDataOutText("LFStack_Test");
				}
			}
		}
		WaitForMultipleObjects(10, hThread, true, INFINITE);
	}
	break;
	default:
		std::cout << "Wrong Number Input" << std::endl;
		break;
	}
}