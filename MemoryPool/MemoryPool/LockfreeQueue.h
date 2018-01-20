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

	void Enqueue(DATA InData)
	{
		TOP curtail;
		TOP newtail;
		BLOCK * pNode = _queuememorypool->Alloc();
		pNode->pNextblock = nullptr;
		pNode->data = InData;

		newtail.uniquenum = InterlockedIncrement64(&(_pTail->uniquenum));
		newtail.pTopnode = pNode;

		do
		{
			curtail.uniquenum = _pTail->uniquenum;
			curtail.pTopnode = _pTail->pTopnode;

			curtail.pTopnode->pNextblock = newtail.pTopnode;
		} while (!InterlockedCompareExchange128((volatile LONG64*)_pTail,
			(LONG64)newtail.uniquenum, (LONG64)newtail.pTopnode, (LONG64*)&curtail));
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

		if (_pHead == _pTail && _pTail->pTopnode->pNextblock != nullptr)
		{
			InterlockedCompareExchangePointer((PVOID*)&_pTail,
				_pTail->pTopnode->pNextblock, _pTail);
		}
			
		TOP curhead;
		TOP newhead;
		newhead.uniquenum = InterlockedIncrement64(&(_pHead->uniquenum));
	
		do
		{

		} while ();
		_queuememorypool->Free(head.pTopnode);
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