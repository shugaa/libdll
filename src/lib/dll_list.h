/*
* Copyright (c) 2008, Björn Rehm (bjoern@shugaa.de)
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*  * Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*  * Neither the name of the author nor the names of its contributors may be
*    used to endorse or promote products derived from this software without
*    specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/** @file dll_list.h
 *
 * @brief Public library interface
 *
 * */

/** @mainpage A small and portable, yet feature rich doubly linked list implementation
 *
 * Installation instructions can be found in INSTALL.txt in the software
 * package's root directory.
 *
 * Feedback to bjoern@shugaa.de is always highly appreciated.
 */

#ifndef _DLL_LIST_H
#define _DLL_LIST_H

#include <stdio.h>
#include <stdlib.h>

/* ######################################################################### */
/*                            TODO / Notes                                   */
/* ######################################################################### */

/* ######################################################################### */
/*                            Types & Defines                                */
/* ######################################################################### */

#define EDLLERROR   (0)   /* General error */
#define EDLLOK      (1)   /* Success */
#define EDLLTILT    (2)   /* Iterator turnaround */
#define EDLLNOMEM   (3)   /* Unable to allocate enough memory */
#define EDLLINV     (4)   /* Invalid argument */

/** List item type */
typedef struct dll_item dll_item_t;

/** List instance type */
typedef struct dll_list dll_list_t;

/** List iterator type */
typedef struct dll_iterator dll_iterator_t;

struct dll_list
{
    unsigned int count;
    dll_item_t* first;
    dll_item_t* last;
};

struct dll_iterator
{
    int flags;
    dll_item_t* item;
    dll_list_t* list;
};

/** Comparator function prototype */
typedef int(*dll_fctcompare_t)(const void*, const void*);

/* ######################################################################### */
/*                            Public interface                               */
/* ######################################################################### */

/** Initialise the DLL library. Should be called before using any other library
 * functions
 */
int dll_init(void);

/** Close the DLL library, free any resources in use by libdll.
 */
int dll_close(void);

/** Create a new doubly-linked list instance
 *
 * @param list       Pointer to a dll_list_t to be initialized
 *
 * @return EDLLOK    No errors occured
 * @return EDLLERROR Something went wrong
 */
int dll_new(dll_list_t* list);

/** Free all resources in use by a doubly-linked list
 *
 * @param list       Pointer to the list
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_free(dll_list_t* list);

/** Clear all items from the linked list
 *
 * Note: Even though a list has been cleared, you will still need to free it in
 * order to free any resources eventually being used by the list. 
 *
 * @param list       Pointer to the list
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_clear(dll_list_t* list);

/** Append an item to the end of the list
 *
 * @param list       Pointer to the list
 * @param data       Where to store the reference to the allocated memory
 * @param datasize   Size of memory to be allocated for this item's data    
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_append(dll_list_t* list, void** data, size_t datasize);

/** Extend a list with another one
 *
 * Note: lext will be an empty list afterwards, meaning it does not contain
 * any items. However it still needs to be freed of course.
 *
 * @param list       Pointer to the list to be extended
 * @param lext       Pointer to extension list
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_extend(dll_list_t* list, dll_list_t* lext);

/** Insert a new item into the list at the specified position
 *
 * @param list       Pointer to the list
 * @param data       Where to store the reference to the allocated memory
 * @param datasize   Size of memory to be allocated for this item's data   
 * @param position   Position in the list to insert the new item (starts with 0)
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_insert(dll_list_t* list, void** data, size_t datasize, unsigned int position);

/** Remove a specific item from te list
 *
 * @param list       Pointer to the list
 * @param position   Position of the item to be removed
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_remove(dll_list_t* list, unsigned int position);

/** Get an item from the list
 *
 * @param list       Pointer to the list
 * @param data       Where to store the reference to the specified item data
 * @param position   Position in the list of the requested item
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_get(dll_list_t* list, void** data, unsigned int position);

/** Get the current item count of the list
 *
 * @param list       Pointer to the list
 * @param count      Pointer to an unsigned int to store the count in
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_count(dll_list_t* list, unsigned int* count);

/** Sort a doubly linked list
 * This implementation uses quicksort which is fast and in-place 
 *
 * @param list       List to be sorted
 * @param compar     Pointer to function comparing two data items
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_sort(dll_list_t* list, dll_fctcompare_t compar);

/** Returns the first occurence of the item which successfully compares to
 * 'cmpitem'
 *
 * @param list       List to be searched
 * @param compar     Pointer to function comparing two data items
 * @param cmpitem    pointer to an item to compare to        
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong / Item not found
 */
int dll_indexof(dll_list_t* list, dll_fctcompare_t compar, void* cmpitem, unsigned int *index);

/** Create a new doubly-linked list iterator instance
 *
 * Call dll_iterator_next() to move the iterator to the first list item after 
 * you have created it. Call dll_iterator_prev() to point to the last element 
 * of the list. 
 * Turnarounds are possible. If a jump from the last to the first element or 
 * vice versa occured upon a call to dll_iterator_next() or dll_iterator_prev()
 * the status code EDLLTILT is returned.
 *  
 * Using an iterator to traverse a list should be faster then getting each 
 * item using dll_get() since it's not necessary anymore to walk from element
 * 0 to the requested element multiple times. The next element is always just 
 * a reference away.
 *
 * @param iterator   Pointer to a dll_iterator_t to be initialized 
 * @param list       Pointer to dll_list_t for which the iterator is to be created
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_iterator_new(dll_iterator_t* iterator, dll_list_t* list);

/** Move the iterator to the next position
 *
 * @param iterator   The iterator which is to be moved to the next element
 * @param data       Storage for the reference to this item's data
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLTILT  Iterator turnaround (jump from last to first item)
 * @return EDLLERROR Something went wrong
 */
int dll_iterator_next(dll_iterator_t* iterator, void** data);

/** Move the iterator to the previous position
 *
 * @param iterator   The iterator which is to be moved to the next element
 * @param data       Storage for the reference to this item's data
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLTILT  Iterator turnaround (jump from first to last item)
 * @return EDLLERROR Something went wrong
 */
int dll_iterator_prev(dll_iterator_t* iterator, void** data);

/** Free any resources in use by a previously allocated iterator
 *
 * @param iterator   The iterator to be freed
 *
 * @return EDLLOK    No errors occured
 * @return EDLLINV   An invalid argument has been passed
 * @return EDLLERROR Something went wrong
 */
int dll_iterator_free(dll_iterator_t* iterator);

#endif /* _DLL_LIST_H */
