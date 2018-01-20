#pragma once
#include "MemoryPool.h"

template <class DATA>
class CLockfreeStack
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
	CLockfreeStack()
	{
		_stackusecount = 0;
		_stackmemorypool = new CMemoryPool<BLOCK>;

		_pTop = (TOP*)_aligned_malloc(sizeof(TOP), 16);
		_pTop->pTopnode = nullptr;
		_pTop->uniquenum = 0;
	}

	virtual ~CLockfreeStack()
	{
		while (_pTop->pTopnode)
		{
			BLOCK * pNode = _pTop->pTopnode;
			_pTop->pTopnode = pNode->pNextblock;
			delete pNode;
		}
		_aligned_free(_pTop);
		delete _stackmemorypool;
	}

	void Push(DATA InData)
	{
		BLOCK * pNewnode = _stackmemorypool->Alloc();
		new (pNewnode) BLOCK();
		TOP curtop;
		TOP newtop;
		LONG64 key = InterlockedIncrement64(&(_pTop->uniquenum));
		
		newtop.pTopnode = pNewnode;
		newtop.uniquenum = key;
		pNewnode->data = InData;

		do
		{
			curtop.uniquenum = _pTop->uniquenum;
			curtop.pTopnode = _pTop->pTopnode;

			newtop.pTopnode->pNextblock = curtop.pTopnode;
		} while (!InterlockedCompareExchange128((volatile LONG64*)_pTop,
			(LONG64)newtop.uniquenum, (LONG64)newtop.pTopnode, (LONG64*)&curtop));
		InterlockedIncrement64(&_stackusecount);
		return;
	}

	bool Pop(DATA &OutData)
	{
		if (InterlockedDecrement64(&_stackusecount) < 0)
		{
			InterlockedIncrement64(&_stackusecount);
			OutData = nullptr;
			return false;
		}
		else
		{
			TOP curtop;
			TOP newtop;
			newtop.uniquenum = InterlockedIncrement64(&(_pTop->uniquenum));

			do
			{
				curtop.uniquenum = _pTop->uniquenum;
				curtop.pTopnode = _pTop->pTopnode;

				newtop.pTopnode = curtop.pTopnode->pNextblock;
			}while(!InterlockedCompareExchange128((volatile LONG64*)_pTop,
				(LONG64)newtop.uniquenum, (LONG64)newtop.pTopnode, (LONG64*)&curtop));
			OutData = curtop.pTopnode->data;
			_stackmemorypool->Free(curtop.pTopnode);

			return true;
		}
	}

	LONG64 GetStackUseCount() { return _stackusecount; }
	LONG64 GetStackMemoryPoolUseCount() { return _stackmemorypool->GetUseCount(); }
	LONG64 GetStackMemoryPoolAllocCount() { return _stackmemorypool->GetAllocCount(); }
private:
	CMemoryPool<BLOCK> * _stackmemorypool;
	TOP * _pTop;
	LONG64 _stackusecount;
};