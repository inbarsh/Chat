#ifndef __HASH_H__
#define __HASH_H__

#include "ADTErr.h"
#include <stddef.h>

#define _DEBUG

/** @file   Hash.h
 *  @brief  Hash management functions
 *  @author Inbar Shalitin
 *  @bug No known bugs.
 */


/*defines*/

typedef void* HashData;

/*Struct of the Hash map*/
typedef struct HashMap HashMap;

/*The key that will determine the order the table will be sorted by*/
typedef void* HashKey;

/*The function that will be used to convert the key to the right index*/
typedef int (*HashFunc)( HashKey _key);

/*Function to compare between two key, 1 equal 0 if not*/
typedef int (*IsEqualFunc)(HashKey _key1, HashKey _key2);

/*Function to use on hash items by its own rules determined by params, data and key, when returns !0 the prosses needs to stop*/
typedef int (*DoSomethingFunc)( HashKey _key, HashData _data, void* _params);



  /** @brief Create Hash
 * Insert _size , _hasFunc, _isEqualFunc
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _size :size of the hash map .
 * @params _hashFunc :hash function that the user sends, the hash map will be build by it .
 * @params _isEqualFunc :function of the user to determine if two items are equal, 1 equal 0 if not .

 * @retval Pointer to hash map on success
 * @retval NULL if memory alocation failes the functions is NULL or size is 0.
*/
HashMap* HashCreate(size_t _size, HashFunc _hashFunc, IsEqualFunc _isEqualFunc);


  /** @brief Destroy Hash  (Notice! the data will be lost if the user didn't save it)
 * Insert _map
 * Average time complexity O(n).
 * Worst: O(n)
 *
 * @params _map :map was created by HashCreate API function.
*/
void HashDestroy(HashMap* _map);


  /** @brief Insert Hash
 * Insert _map, _key, _data
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _map :map was created by HashCreate API function.
 * @params _key :The key the user wants to enter by.
 * @params _data :The data the user wants to insert.
 *
 * @returns ADTErr.
 * @retval ERR_OK on success
 * @retval ERR_NOT_INITIALIZED if _map is null
 * @retval ERR_NULL_POINTER if _data or _key are null.
 * @retval ERR_ALLOCATION_FAILED
 * @retval ERR_DUPLICATE_KEY: if Key already exists
*/
ADTErr HashInsert(HashMap* _map, HashKey _key, HashData _data);


  /** @brief  Hash Remove
 * Insert _map, _key, _data
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _map :map was created by HashCreate API function.
 * @params _key :The key the user wants to enter by.
 * @params _data :The data the user wants to get back from the removed item.
 *
 * @returns ADTErr.
 * @retval ERR_OK on success
 * @retval ERR_NOT_INITIALIZED if _map is null
 * @retval ERR_NULL_POINTER if _data or _key are null.
 * @retval ERR_NOT_FOUND if key to remove was not found
*/
ADTErr HashRemove(HashMap* _map, HashKey _key, HashData* _data);


  /** @brief  Return number of items in hash
 * Insert _map,
 * Average time complexity O(n).
 * Worst: O(n)
 *
 * @params _map :map was created by HashCreate API function.
 *
 * @returns number of items in Hash.
*/
size_t HashCountItems(HashMap* _map);



  /** @brief  Return number of acupied buckets in hash
 * Insert _map,
 * Average time complexity O(n).
 * Worst: O(n)
 *
 * @params _map :map was created by HashCreate API function.
 *
 * @returns number of ocupid buckets in hash.
*/
size_t HashCountAcupiedBuckets(HashMap* _map);



  /** @brief  Applies DoFuncHash on all items or until the function returns !0
 * Insert _map,
 * Average time complexity O(n).
 * Worst: O(n)
 *
 * @params _map :map was created by HashCreate API function.
 * @params _DoFuncHash :map was created by HashCreate API function.
 * @params _params :parameters to compare items to.
 * @returns The return value of _DoFuncHash.
*/
int HashForEach(HashMap* _map, DoSomethingFunc _doFuncHash, void* _params);


  /** @brief  Finds data by key and returns it
 * Insert _map,_key
 * Average time complexity O(1).
 * Worst: O(1)
 *
 * @params _map :map was created by HashCreate API function.
 * @params _key :The key the user wants to find.
 *
 * @returns The data the user was looking for.
 * @returns Null if key was not found.
*/
HashData HashFind(HashMap* _map, HashKey _key);

#ifdef _DEBUG
int HashPrint(HashMap* _map, DoSomethingFunc _printFunc); /*to print not must*/
#endif /*_DEBUG*/




#endif
