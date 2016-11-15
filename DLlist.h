/** @file   DList.h
 *  @brief  List function library
 *  @author Inbar Shalitin
 *  @date 	15/03/2016
 *  @bug No known bugs.
 */
#include "ADTErr.h"
#include <stddef.h>

typedef struct List  List;
typedef void*        Data;

  /** @brief Create List
 * Insert void
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @returns List*.
 * @retval NULL if alocation failed
*/
List*   ListCreate(void);


  /** @brief Destroy List
 * Insert list
 * Average time complexity O(n).
 * Worst: O(n)
 *
*/
void    ListDestroy(List* _list);
 
 
   /** @brief Insert to the head of the list, input pointer and data
 * Insert list and data
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _list : List created by API function ListCreate.
 * @params _data : Item to insert the list.
 * @returns ADTErr.
 * @retval ERR_OK on success
 * @retval ERR_NULL_POINTER
 * @retval ERR_ALLOCATION_FAILED
 * @retval ERR_NOT_INITIALIZED 
*/
ADTErr  ListPushHead(List* _list, Data _data);


   /** @brief Insert to the Tail of the list, input pointer and data
 * Insert list and data
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _list : List created by API function ListCreate.
 * @params _data : Item to insert the list.
 * @returns ADTErr.
 * @retval ERR_OK on success
 * @retval ERR_NULL_POINTER
 * @retval ERR_ALLOCATION_FAILED
 * @retval ERR_NOT_INITIALIZED 
*/

ADTErr  ListPushTail(List* _list, Data _data);


   /** @brief Pop from the head of the list, input pointer pointer to the data removed
 * Insert list and data pointer
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _list : List created by API function ListCreate.
 * @params _data : Item to send the data to.
 * @returns ADTErr.
 * @retval ERR_OK on success
 * @retval ERR_NULL_POINTER
 * @retval ERR_UNDERFLOW
 * @retval ERR_NOT_INITIALIZED 
*/

ADTErr  ListPopHead(List* _list, Data* _data);

   /** @brief Pop from the tail of the list, input pointer pointer to the data removed
 * Insert list and data pointer
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _list : List created by API function ListCreate.
 * @params _data : Item to send the data to.
 * @returns ADTErr.
 * @retval ERR_OK on success
 * @retval ERR_NULL_POINTER
 * @retval ERR_UNDERFLOW
 * @retval ERR_NOT_INITIALIZED 
*/

ADTErr  ListPopTail(List* _list, Data* _data);


   /** @brief return the number of items of the list
 * Insert list and count pointer
 * Average time complexity O(n).
 * Worst: O(n)
 *
 * @params _list : List created by API function ListCreate.
 * @params _count : counter to send the number of items to.
 * @returns ADTErr.
 * @retval ERR_OK on success
 * @retval ERR_NULL_POINTER
 * @retval ERR_NOT_INITIALIZED 
*/

ADTErr  ListCountItems(const List* _list, size_t* _count);


   /** @brief Return if list is empty, 1 for empty, 0 for not
 * Insert list 
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _list : List created by API function ListCreate.

 * @returns int.
 * @retval 1 if empty
 * @retval 0 if full
*/

int     ListIsEmpty(const List* _list);


    /** @brief Prints the List by its order. input the pointer, print out the list
 * Insert list and print function
 * Average time complexity O(n).
 * Worst: O(n)
 *
 * @params _list : List created by API function ListCreate.
 * @params _print : Function to print the items of the list with.
*/
 
typedef void* ListItr;

typedef int(*CmpFunc)(Data _data1, Data _data2);
typedef int(*DoFunc) (Data _data, void* _params);

  /** @brief Returns the begin of the list
 * Insert list
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _list : List created by API function ListCreate.
 * @returns ListItr.
 * @retval NULL if list is NULL
*/

ListItr ListBegin(List* _list);


  /** @brief Returns the tail of the list
 * Insert list
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _list : List created by API function ListCreate.
 * @returns ListItr.
 * @retval NULL if list is NULL
*/

ListItr ListEnd(List* _list);


  /** @brief Returns next in list
 * Insert _itr
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _itr : Node adress of List created by API function ListCreate.
 * @returns ListItr.
 * @retval NULL if list is NULL
*/

ListItr ListNext(ListItr _itr);


  /** @brief Returns prev in list
 * Insert _itr
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _itr : Node adress of List created by API function ListCreate.
 * @returns ListItr.
 * @retval NULL if list is NULL
*/
ListItr ListPrev(ListItr _itr);


  /** @brief Returns Data of the list
 * Insert _itr
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _itr : Node adress of List created by API function ListCreate.
 * @returns Data.
 * @retval NULL if list is NULL
*/

Data ListGetData(ListItr _itr);
ListItr ListSetData(ListItr _itr,Data _item);


  /** @brief Insert to list befor specific node
 * Insert _itr and _data
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _itr : Node adress of List created by API function ListCreate.
  * @params _data : Data that will be inseted to the list.
 * @returns ListItr.
 * @retval NULL if list is NULL
*/

ListItr ListInsertBefore(ListItr _itr, Data _data);



  /** @brief Remove from list
 * Insert _itr and _item
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _item : Data that will be removed to the list.
 * @returns Data.
 * @retval NULL if list is NULL
*/

Data ListRemove(ListItr _item);



  /** @brief Find first item equals to data
 * Insert  _from, _to, _cmpFunc, _data
 * Average time complexity O(n).
 * Worst: O(n)
 *
 * @params _from : from which node to start.
 * @params  _to : to which node to run.
 * @params _cmpFunc : The func to use on data- compare func.
 * @params _data : The data that will be sent to the func for compare.
 * @returns ListItr.
 * @retval NULL if list is NULL
*/

ListItr ListFindFirst(ListItr _from, ListItr _to, CmpFunc _cmpFunc, Data _data);



  /** @brief Uses function on each item on list
 * Insert  _from, _to, _doFunc, _params
 * Average time complexity O(n).
 * Worst: O(n)
 *
 * @params _from : from which node to start.
 * @params  _to : to which node to run.
 * @params _doFunc : The func to use on data.
 * @params _params : The data that will be sent to the func .
 * @retval ERR_OK on success
 * @retval ERR_NULL_POINTER
*/

/*Uses function on each item on list*/
ADTErr ListForEach(ListItr _from, ListItr _to, DoFunc _doFunc, void* _params);
 
