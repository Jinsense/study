#pragma once
#include "MemoryPool.h"

template <class DATA>
class CLockfreeQueue
{
private:
	typedef struct st_MemoryBlock
	{
		st_MemoryBlock()
		{
			pNextblock = nullptr;
		}
		st_MemoryBlock * pNextblock;
		DATA data;
	}BLOCK;

	typedef struct st_FreeNode
	{
		st_MemoryBlock * pTopnode;
		LONG64 uniquenum;
	}TOP;

public:
	CLockfreeQueue()
	{
		_queueusecount = 0;
		_queuememorypool = new CMemoryPool<BLOCK>;

		_pHead = (TOP*)_aligned_malloc(sizeof(TOP), 16);
		_pHead->pTopnode = _queuememorypool->Alloc();
		_pHead->pTopnode->pNextblock = nullptr;
		_pHead->uniquenum = 0;

		_pTail = (TOP*)_aligned_malloc(sizeof(TOP), 16);
		_pTail->pTopnode = _pHead->pTopnode;
		_pTail->uniquenum = 0;
	}

	virtual ~CLockfreeQueue()
	{
		while (_pHead->pTopnode != _pTail->pTopnode)
		{
			BLOCK * pNode = _pHead->pTopnode;
			_pHead->pTopnode = pNode->pNextblock;
			delete pNode;
		}

		_queuememorypool->Free(_pHead->pTopnode);
		_aligned_free(_pHead);
		_aligned_free(_pTail);
		delete _queuememorypool;
	}

	//void Enqueue(DATA InData)
	//{
	//	BLOCK * pNode = _queuememorypool->Alloc();
	//	pNode->pNextblock = nullptr;
	//	pNode->data = InData;

	//	LONG64 key = InterlockedIncrement64(&(_pTail->uniquenum));
	//	while (1)
	//	{
	//		TOP * pTail = _pTail;
	//		BLOCK * pNext = pTail->pTopnode->pNextblock;

	//		if (NULL == pNext)
	//		{
	//			if (nullptr == InterlockedCompareExchangePointer((PVOID*)&pTail->pTopnode->pNextblock,
	//				pNode, pNext))
	//			{
	//				InterlockedCompareExchange128((LONG64*)_pTail, key, (LONG64)pNode, (LONG64*)pTail);
	//				break;
	//			}
	//		}
	//		else
	//		{
	//			InterlockedCompareExchange128((LONG64*)_pTail, pTail->uniquenum, 
	//				(LONG64)pTail->pTopnode->pNextblock, (LONG64*)pTail);
	//		}
	//	}
	//	InterlockedIncrement64(&_queueusecount);
	//	return;
	//}

	void Enqueue(DATA InData)
	{
		BLOCK * pNode = _queuememorypool->Alloc();
		pNode->pNextblock = nullptr;
		pNode->data = InData;

		TOP tail;
		tail.uniquenum = InterlockedIncrement64(&(_pTail->uniquenum));
		while (1)
		{
			tail.pTopnode = _pTail->pTopnode;
			BLOCK * pNext = tail.pTopnode->pNextblock;

			if (NULL == pNext)
			{
				if (nullptr == InterlockedCompareExchangePointer((PVOID*)&tail.pTopnode->pNextblock,
					pNode, pNext))
				{
					InterlockedCompareExchange128((LONG64*)_pTail, tail.uniquenum, 
						(LONG64)pNode, (LONG64*)&tail);
					break;
				}
			}
			else
			{
				InterlockedCompareExchange128((LONG64*)_pTail, tail.uniquenum,
					(LONG64)tail.pTopnode->pNextblock, (LONG64*)&tail);
			}
		}
		InterlockedIncrement64(&_queueusecount);
		return;
	}

	bool Dequeue(DATA &OutData)
	{
		if (InterlockedDecrement64(&_queueusecount) < 0)
		{
			InterlockedIncrement64(&_queueusecount);
			OutData = nullptr;
			return false;
		}

		TOP head;
		TOP tail;
		head.uniquenum = InterlockedIncrement64(&(_pHead->uniquenum));
		while (1)
		{
			head.pTopnode = _pHead->pTopnode;
			tail.pTopnode = _pTail->pTopnode;
			BLOCK * pNode = head.pTopnode->pNextblock;
			if (nullptr == pNode)
				continue;
			
			if(head.pTopnode == tail.pTopnode)
			{
				if (0 == InterlockedCompareExchange128((LONG64*)_pTail, tail.uniquenum,
					(LONG64)tail.pTopnode->pNextblock, (LONG64*)&tail))
					continue;
			}

			OutData = pNode->data;
			if (1 == InterlockedCompareExchange128((LONG64*)_pHead, head.uniquenum,
				(LONG64)head.pTopnode->pNextblock, (LONG64*)&head))
			{
				_queuememorypool->Free(head.pTopnode);
				break;
			}
		}
		return true;
	}

	LONG64 GetQueueUseCount() { return _queueusecount; }
	LONG64 GetQueueMemoryPoolUseCount() { return _queuememorypool->GetUseCount(); }
	LONG64 GetQueueMemoryPoolAllocCount() { return _queuememorypool->GetAllocCount(); }
private:
	CMemoryPool<BLOCK> * _queuememorypool;
	TOP * _pHead;
	TOP * _pTail;
	LONG64 _queueusecount;
};