#include "DLlist.h"
#include <stdio.h>
#include <stdlib.h>


#define MAGIC_NUM (Data)0xDEADC0DE

#define IS_LIST_OK(lst) (((lst) == NULL) || ((lst)->m_head.m_data != MAGIC_NUM))

#define IterAsNode(it)           ((Node*)it)
#define IS_BEGIN_ITER(it)        (NULL == IterAsNode(it)->m_prev->m_prev)
#define IS_END_ITER(it)          (NULL == IterAsNode(it)->m_next)
#define ITER_NEXT(it)            IterAsNode(it)->m_next
#define ITER_PREV(it)            IterAsNode(it)->m_prev

typedef struct Node Node;

struct Node
{
	Data m_data;
	Node* m_prev;
	Node* m_next;
};
struct List
{
	Node m_head;
	Node m_tail;
};

static ADTErr InsertBefore(Node* _pNode, Data _data)
{
	/*insert _data at new node befire _pNode*/
	Node* pNewNode = malloc(sizeof(Node));
	if (pNewNode == NULL)
	{
		return ERR_ALLOCATION_FAILED;
	}
	pNewNode->m_data = _data;
	pNewNode->m_next = _pNode; /*2 links to next*/
	pNewNode->m_prev = _pNode->m_prev;
	_pNode->m_prev->m_next = pNewNode; /*2 links to prev*/
	_pNode->m_prev = pNewNode;
	return ERR_OK;
}

static ADTErr Remove(Node* _pNode,Data* _data)
{
	/*remove pNode from DLlist and put removed data in _data*/
	*_data = _pNode->m_data;
	_pNode->m_prev->m_next = _pNode->m_next; /*link prev to next*/
	_pNode->m_next->m_prev = _pNode->m_prev; /*link next to prev*/
	free(_pNode);
	return ERR_OK;
}

List* ListCreate(void)
{
	List* pList = malloc(sizeof(List));
	if (pList == NULL)
	{
		return NULL;
	}
	pList->m_head.m_prev = NULL;
	pList->m_tail.m_next = NULL;
	pList->m_head.m_next = &(pList->m_tail);
	pList->m_tail.m_prev = &(pList->m_head);
	pList->m_head.m_data = MAGIC_NUM;
	pList->m_tail.m_data = NULL;
	return pList;
}

void ListDestroy(List* _list)
{
	Node* pNode;

	if IS_LIST_OK(_list)
	{
		return;
	}
	pNode = _list->m_tail.m_prev;
	while (pNode->m_prev != NULL)
	{
		/*starting from last item, moving backwards and deleting next*/
		pNode = pNode->m_prev;
		free(pNode->m_next);
	}
	_list->m_head.m_data = NULL;
	free(_list);
}

ADTErr ListPushHead(List* _list, Data _data)
{
	if IS_LIST_OK(_list)
	{
		return ERR_NOT_INITIALIZED;
	}
	return InsertBefore(_list->m_head.m_next,_data);
}

ADTErr ListPushTail(List* _list, Data _data)
{
	if IS_LIST_OK(_list)
	{
		return ERR_NOT_INITIALIZED;
	}
	return InsertBefore(&(_list->m_tail),_data);
}

ADTErr ListPopHead(List* _list, Data* _data)
{
	if IS_LIST_OK(_list)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (_list->m_head.m_next == &(_list->m_tail))
	{
		return ERR_UNDERFLOW;
	}
	if (_data == NULL)
	{
		return ERR_NULL_POINTER;
	}
	return Remove(_list->m_head.m_next,_data);
}

ADTErr ListPopTail(List* _list, Data* _data)
{
	if IS_LIST_OK(_list)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (_list->m_head.m_next == &(_list->m_tail))
	{
		return ERR_UNDERFLOW;
	}
	if (_data == NULL)
	{
		return ERR_NULL_POINTER;
	}
	return Remove(_list->m_tail.m_prev,_data);
}

ADTErr ListCountItems(const List* _list, size_t* _count)
{
	Node* pNode;
	int counter = 0;

	if IS_LIST_OK(_list)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (_count == NULL)
	{
		return ERR_NULL_POINTER;
	}
	for (pNode = _list->m_head.m_next; pNode->m_next != NULL; pNode = pNode->m_next)
	{
		++counter;
	}
	*_count = counter;
	return ERR_OK;
}

int ListIsEmpty(const List* _list)
{
	if IS_LIST_OK(_list)
	{
		return 1;
	}
	return (_list->m_head.m_next == &(_list->m_tail));
}

/****************ITERATOR FUNCTIONS**********************/
ListItr ListBegin(List* _list)
{
	if (_list == NULL)
	{
		return NULL;
	}
	return (ListItr)_list->m_head.m_next;
}

ListItr ListEnd(List* _list)
{
	if (_list == NULL)
	{
		return NULL;
	}
	return (ListItr)&_list->m_tail;
}

ListItr ListNext(ListItr _itr)
{
	if (IS_END_ITER(_itr))
	{
		return (_itr);
	}
	return ITER_NEXT(_itr);
}

ListItr ListPrev(ListItr _itr)
{
	if (IS_BEGIN_ITER(_itr))
	{
		return _itr;
	}
	return ITER_PREV(_itr);
}

Data ListGetData(ListItr _itr)
{
	return IterAsNode(_itr)->m_data;
}

ListItr ListSetData(ListItr _itr,Data _item)
{
	IterAsNode(_itr)->m_data = _item;
	return _itr;
}

ListItr ListInsertBefore(ListItr _itr, Data _data)
{
	if (InsertBefore(IterAsNode(_itr),_data) != ERR_OK)
	{
		return NULL;
	}
	return ITER_PREV(_itr);
}

Data ListRemove(ListItr _item)
{
	Data retVal;
	Remove(IterAsNode(_item),&retVal);
	return retVal;
}

/********** List Find First **************/
ListItr ListFindFirst(ListItr _from, ListItr _to, CmpFunc _cmpFunc, Data _data)
{
    Data _saved;

    if(_from == NULL || _to == NULL || _cmpFunc == NULL)
    {
        return NULL;
    }
    _saved = IterAsNode(_to)->m_data;
    IterAsNode(_to)->m_data = _data;
    while(_cmpFunc(IterAsNode(_from)->m_data, _data))
    {
        _from = ITER_NEXT(_from);
    }
    IterAsNode(_to)->m_data = _saved;
    return _from;
}

/********** List For Each **************/
ADTErr ListForEach(ListItr _from, ListItr _to, DoFunc _doFunc, void* _params)
{
    if(_from == NULL || _to == NULL || _doFunc == NULL)
    {
        return ERR_NULL_POINTER;
    }
    while(_from != _to && _doFunc(IterAsNode(_from)->m_data, _params))
    {
        _from = ITER_NEXT(_from);
    }
    return ERR_OK;
}
