#pragma once

template<class DATA>
class CMemoryPool
{
private:
	typedef struct st_MemoryBlock
	{
		st_MemoryBlock * pNext;
		DATA pNode;
	}BLOCK;

	typedef struct st_FreeNode
	{
		st_MemoryBlock * pFreeNode;
		unsigned __int64 iUniqueNum;
	}TOP;

public:
	CMemoryPool();
	~CMemoryPool();

	DATA * Alloc();
	void Free(DATA * _pInData);

	unsigned __int64 GetUseCount() { return _iUseCount; }
	unsigned __int64 GetAllocCount() { return _iAllocCount; }

private:
	unsigned __int64 _iUseCount;
	unsigned __int64 _iAllocCount;
};

//	Inline 함수로 작성했을 때와 아닐 때의 속도 비교 해보고
//	성능이 높은 것을 사용하자

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
void CMemoryPool::Free(DATA * _pInData)
{

}