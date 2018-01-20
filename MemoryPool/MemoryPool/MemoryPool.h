#pragma once
#include <Windows.h>

template<class DATA>
class CMemoryPool
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
	CMemoryPool()
	{
		_usecount = 0;
		_alloccount = 0;

		_pTop = (TOP*)_aligned_malloc(sizeof(TOP), 16);
		_pTop->pTopnode = nullptr;
		_pTop->uniquenum = 0;
	}
	virtual ~CMemoryPool()
	{
		while (_pTop->pTopnode)
		{
			BLOCK * pNode = _pTop->pTopnode;
			_pTop->pTopnode = pNode->pNextblock;
			delete pNode;
		}
		_aligned_free(_pTop);
	}

	DATA * Alloc()
	{
		if (InterlockedIncrement64(&_usecount) > _alloccount)
		{
			InterlockedIncrement64(&_alloccount);
			BLOCK * pBlock = (BLOCK*)malloc(sizeof(BLOCK));
			return &(pBlock->data);
		}
		else
		{
			LONG64 key = InterlockedIncrement64(&(_pTop->uniquenum));
			TOP curtop;
			TOP newtop;
			DATA * pData;

			do
			{
				curtop.uniquenum = _pTop->uniquenum;
				curtop.pTopnode = _pTop->pTopnode;

				newtop.uniquenum = key;
				newtop.pTopnode = curtop.pTopnode->pNextblock;

				pData = &(curtop.pTopnode->data);

			} while (!InterlockedCompareExchange128((volatile LONG64*)_pTop, 
				(LONG64)newtop.uniquenum, (LONG64)newtop.pTopnode, (LONG64*)&curtop));
			return pData;
		}				
	}

	void Free(DATA * pInData)
	{
		TOP curtop;
		TOP newtop;
		LONG64 key = InterlockedIncrement64(&(_pTop->uniquenum));
		do
		{
			curtop.uniquenum = _pTop->uniquenum;
			curtop.pTopnode = _pTop->pTopnode;

			newtop.uniquenum = key;
			newtop.pTopnode = (BLOCK*)((char*)pInData - sizeof(BLOCK*));

			newtop.pTopnode->pNextblock = curtop.pTopnode;

		} while (!InterlockedCompareExchange128((volatile LONG64*)_pTop,
			(LONG64)newtop.uniquenum, (LONG64)newtop.pTopnode, (LONG64*)&curtop));
		InterlockedDecrement64(&_usecount);
		return;
	}

	LONG64 GetUseCount() { return _usecount; }
	LONG64 GetAllocCount() { return _alloccount; }

private:
	TOP * _pTop;
	LONG64 _usecount;
	LONG64 _alloccount;
};

//	Inline 함수로 작성했을 때와 아닐 때의 속도 비교 해보고
//	성능이 높은 것을 사용하자

/*
template<class DATA>
CMemoryPool::CMemoryPool()
{
	
}

template<class DATA>
CMemoryPool::~CMemoryPool()
{

}

template<class DATA>
DATA CMemoryPool::Alloc()
{

}

template<class DATA>
void CMemoryPool::Free(DATA * pInData)
{

}
*/