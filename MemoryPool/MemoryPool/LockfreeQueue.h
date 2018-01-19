#pragma once
#include "MemoryPool.h"

template <class DATA>
class CLockFreeQueue
{
private:
	typedef struct st_MemoryBlock
	{
		st_MemoryBlock()
		{
			pNextblock = nullptr;
		}
		st_MemoryBlock * pNextblock;
		Data data;
	}BLOCK;

	typedef struct st_FreeNode
	{
		st_MemoryBlock * pTopnode;
		LONG64 uniquenum;
	}TOP;

public:
	CLockFreeQueue()
	{
		_queueusecount = 0;
		_queuememorypool = new CMemoryPool<BLOCK>;

		_pHead = (TOP*)_aligned_malloc(sizeof(TOP), 16);
		_pHead->pTopnode = _queuememorypool->Alloc();
		_pHead->uniquenum = 0;

		_pTail = (TOP*)_aligned_malloc(sizeof(TOP), 16);
		_pTail->pTopnode = _pHead->pTopnode;
		_pTail->uniquenum = 0;
	}

	virtual ~CLockFreeQueue()
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

	}

	bool Dequeue(DATA &OutData)
	{

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