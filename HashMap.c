#include "HashMap.h"
#include "DLlist.h"
#include <stdlib.h>

#define LOAD_FACTOR 1.3
#define LIST(x) _pMap->m_bucketArr[x].m_list

typedef struct
{
	HashKey m_key;
	HashData m_data;
}Pair;

typedef struct
{
	List* m_list;
} Bucket;

/*package for key compare smuggle*/
typedef struct
{
	HashKey m_key;
	IsEqualFunc m_smuggledFunc;
} FindData;

/* package for do func smuggle*/
typedef struct
{
	void* m_params;
	DoSomethingFunc m_smuggledFunc;
} ForEachData;

struct HashMap
{
	Bucket* m_bucketArr;
	size_t m_capacity;
	IsEqualFunc m_keyCmpFunc;
	HashFunc m_hashFunc;
};


static void ListDestroyData(List* _list)
{
		ListItr _from;
		ListItr _to = ListEnd(_list);
		for (_from = ListBegin(_list); _from != _to; _from = ListNext(_from))
		{
				free(ListGetData(_from));
		}
}

static void BucketDestroy(Bucket* _from, Bucket* _to)
{
	for (;_from != _to; ++_from)
	{
		ListDestroyData(_from->m_list);
		ListDestroy(_from->m_list);
	}
}

static int BucketInitialize(HashMap* _pMap, size_t _size)
{
	int i;
	for (i=0; i < _size; ++i)
	{
		LIST(i) = ListCreate();
		if (LIST(i) == NULL)
		{
			BucketDestroy(_pMap->m_bucketArr, _pMap->m_bucketArr + i);
			return 1;
		}
	}
	return 0;
}

HashMap* HashCreate(size_t _size, HashFunc _hashFunc, IsEqualFunc _isEqualFunc)
{
	HashMap* pMap;
	int allocError;
	size_t hashSize;
	if (_size == 0 || _hashFunc == NULL || _isEqualFunc == NULL)
	{
		return NULL;
	}
	hashSize = _size * LOAD_FACTOR;
	pMap = malloc(sizeof(HashMap));
	if (pMap == NULL)
	{
		return NULL;
	}
	pMap->m_bucketArr = malloc(hashSize * sizeof(Bucket));
	if (pMap->m_bucketArr == NULL)
	{
		free(pMap);
		return NULL;
	}
	allocError = BucketInitialize(pMap, hashSize);
	if (allocError)
	{
		free(pMap->m_bucketArr);
		free(pMap);
		return NULL;
	}
	pMap->m_capacity = hashSize;
	pMap->m_hashFunc = _hashFunc;
	pMap->m_keyCmpFunc = _isEqualFunc;
	return pMap;
}

void HashDestroy(HashMap* _pMap)
{
	if (_pMap == NULL)
	{
		return;
	}
	BucketDestroy(_pMap->m_bucketArr, _pMap->m_bucketArr + _pMap->m_capacity);
	free(_pMap->m_bucketArr);
	free(_pMap);
}

static int SmuggleFunction(Data _data, void* _package)
{
	DoSomethingFunc func = ((ForEachData*)_package)->m_smuggledFunc;
	void* params = ((ForEachData*)_package)->m_params;
	return func(((Pair*)_data)->m_key, ((Pair*)_data)->m_data, params);
}

static int CmpFuncInternal(Data _data, Data _package)
{
	IsEqualFunc func = ((FindData*)_package)->m_smuggledFunc;
	return func(((Pair*)_data)->m_key,((FindData*)_package)->m_key);
}

static ListItr GetKeyLocation(HashMap* _pMap, HashKey _key)
{
	ListItr begin = NULL;
	ListItr end = NULL;
	FindData package;
	size_t index = _pMap->m_hashFunc(_key);

	begin = ListBegin(LIST(index));
	end = ListEnd(LIST(index));

	package.m_key = _key;
	package.m_smuggledFunc = _pMap->m_keyCmpFunc;
	return ListFindFirst(begin, end, CmpFuncInternal, (Data)&package);
}

static Pair* CreatePair(HashKey _key, HashData _data)
{
	Pair* pPair = malloc(sizeof(Pair));
	if (pPair == NULL)
	{
		return NULL;
	}
	pPair->m_key = _key;
	pPair->m_data = _data;
	return pPair;
}

ADTErr HashInsert(HashMap* _pMap, HashKey _key, HashData _data)
{
	Pair* pPair;
	ListItr itr;
	if (_pMap == NULL || _key == NULL)
	{
		return ERR_NOT_INITIALIZED;
	}
	itr = GetKeyLocation(_pMap, _key);
	if (itr != ListNext(itr))
	{
		return ERR_DUPLICATE_KEY;
	}
	pPair = CreatePair(_key, _data);
	if (pPair == NULL || ListInsertBefore(itr, pPair) == NULL)
	{
		return ERR_ALLOCATION_FAILED;
	}
	return ERR_OK;
}

ADTErr HashRemove(HashMap* _pMap, HashKey _key, HashData* _data)
{
	ListItr itr;
	HashData removedValue;
	if (_pMap == NULL || _key == NULL)
	{
		return ERR_NOT_INITIALIZED;
	}
	itr = GetKeyLocation(_pMap, _key);
	if (itr == ListNext(itr))
	{
		return ERR_NOT_FOUND;
	}
	removedValue = ListRemove(itr);
	if (_data != NULL)
	{
		*_data = ((Pair*)removedValue)->m_data;
	}
	return ERR_OK;
}

size_t HashCountItems(HashMap* _pMap)
{
	size_t count = 0;
	size_t index;
	size_t result = 0;
	if (_pMap == NULL)
	{
		return 0;
	}
	for (index = 0; index < _pMap->m_capacity; ++index)
	{
		ListCountItems(LIST(index), &count);
		result += count;
	}
	return result;
}

size_t HashCountOccupiedBuckets(HashMap* _pMap)
{
	size_t index;
	size_t result = 0;
	if (_pMap == NULL)
	{
		return 0;
	}
	for (index = 0; index < _pMap->m_capacity; ++index)
	{
		result += ListIsEmpty(LIST(index));
	}
	return _pMap->m_capacity - result;
}

int HashForEach(HashMap* _pMap, DoSomethingFunc _doFunc, void* _params)
{
	int index = 0;
	ForEachData package;
	ListItr from;
	ListItr to;
	if (_pMap == NULL || _doFunc == NULL)
	{
		return 0;
	}
	package.m_params = _params;
	package.m_smuggledFunc = _doFunc;

	for (; index < _pMap->m_capacity; ++index)
	{
		from = ListBegin(LIST(index));
		to = ListEnd(LIST(index));
		ListForEach(from, to, SmuggleFunction, &package);
	}
	return 0;
}

HashData HashFind(HashMap* _pMap, HashKey _key)
{
	ListItr itr;
	if (_pMap == NULL || _key == NULL)
	{
		return NULL;
	}
	itr = GetKeyLocation(_pMap, _key);
	return (itr == ListNext(itr)) ? NULL : ((Pair*)ListGetData(itr))->m_data;
}
