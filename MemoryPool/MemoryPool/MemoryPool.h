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

//	Inline �Լ��� �ۼ����� ���� �ƴ� ���� �ӵ� �� �غ���
//	������ ���� ���� �������

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