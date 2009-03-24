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

#include "dll_list.h"
#include "dll_list_prv.h"

/* ######################################################################### */
/*                            TODO / Notes                                   */
/* ######################################################################### */

/* ######################################################################### */
/*                            Types & Defines                                */
/* ######################################################################### */

/* ######################################################################### */
/*                           Private interface (Module)                      */
/* ######################################################################### */
static int dll_quicksort(dll_list_t* list, dll_fctcompare_t compar, int hi, int lo);

/* ######################################################################### */
/*                           Implementation                                  */
/* ######################################################################### */
int dll_init(void)
{
    /* Doing nothing right now. Anything that is needed for libdll to work (e.g.
     * any memory pooling) should be organized here */
    return EDLLOK;
}

int dll_close(void)
{
    /* Doing nothing right now. Should be used to clean up after using libdll */
    return EDLLOK;
}

int dll_new(dll_list_t* list)
{
    if (!list)
        return EDLLINV;

    /* Initialize the new list */
    list->count = 0;
    list->first = NULL;
    list->last = NULL;

    return EDLLOK;
}

int dll_free(dll_list_t* list)
{
    /* Try to make sure we were passed some kind of valid pointer */
    if (!list)
        return EDLLINV;

    /* Free all items. */
    return dll_clear(list);
}

int dll_clear(dll_list_t* list)
{
    unsigned int i;
    dll_item_t *itemcurrent, *itemnext;

    /* Free each item's data member and each item itself */
    itemcurrent = list->first;
    for (i=0; i<(list->count); i++)
    {
        /* Remember the next pointer for it will be lost after the item has been
         * freed */
        itemnext = itemcurrent->next;

        dll_mem_free(itemcurrent->data);
        dll_mem_free(itemcurrent);

        itemcurrent = itemnext;
    }

    list->count = 0;
    list->first = NULL;
    list->last = NULL;
    
    return EDLLOK;
}

int dll_append(dll_list_t* list, void** data, size_t datasize)
{
    dll_item_t *itemnew = NULL;

    if(!list)
        return EDLLINV;
    if(!data)
        return EDLLINV;

    /* Make a new item */
    if ((itemnew = (dll_item_t*)dll_mem_alloc(sizeof(dll_item_t))) == NULL)
        return EDLLNOMEM;

    if ((itemnew->data = dll_mem_alloc(datasize)) == NULL) {
        dll_mem_free(itemnew);
        return EDLLNOMEM;
    }

    /* newitem is now the last element in the list */
    itemnew->prev = list->last;

    if (list->last != NULL)
        list->last->next = itemnew;

    list->last = itemnew;
    itemnew->next = NULL;

    if (list->count == 0)
        list->first = itemnew;

    /* There's one more element in the list now */
    list->count++;

    /* Return data pointer */
    *data = itemnew->data;

    return EDLLOK;
}

int dll_extend(dll_list_t* list, dll_list_t* lext)
{
    /* Basic checks */
    if (!list)
        return EDLLINV;
    if (!lext)
        return EDLLINV;

    /* Nothing to do, good for us */
    if (lext->count == 0)
        return EDLLOK;

    /* Either there's already elements in list and we have to 'append' lext or
     * we can basically just copy it. The 'last' member in the list instance can
     * only be != NULL if there exist any elements. */
    if (list->last != NULL) {
        /* lext might be empty */
        if (lext->first != NULL)
            lext->first->prev = list->last;

        /* NULL if lext is empty, doesn't matter */
        list->last->next = lext->first;
        
        /* lext might be empty */
        if (lext->last != NULL)
            list->last = lext->last;
    } else {
        list->first = lext->first;
        list->last = lext->last;
    }

    /* Add the number of elements in lext to list */    
    list->count += lext->count;
    
    /* 'Empty' lext */
    lext->count = 0;
    lext->first = NULL;
    lext->last = NULL;

    return EDLLOK;
}

int dll_insert(dll_list_t* list, void** data, size_t datasize, unsigned int position)
{
    unsigned int i;
    dll_item_t *itemnew = NULL;
    dll_item_t *itemseek = NULL;

    /* Basic checks */
    if (!list) {
        return EDLLINV;
    }
    if (position > list->count) {
        return EDLLINV;
    }

    /* Create a new item */
    if ((itemnew = (dll_item_t*)dll_mem_alloc(sizeof(dll_item_t))) == NULL) {
        return EDLLNOMEM;
    }

    if ((itemnew->data = dll_mem_alloc(datasize)) == NULL) {
        dll_mem_free(itemnew);
        return EDLLNOMEM;
    }

    /* Seek to item position, which is prev for your new item */
    itemseek = list->first; 
    for (i=1; i<position; i++)
        itemseek = itemseek->next;

    /* First item */
    if (position == 0) {
        itemnew->prev = NULL;
        itemnew->next = list->first;

        if (list->first != NULL)
            list->first->prev = itemnew;

        list->first = itemnew;
    } 
    /* Any other item */
    else {
        itemnew->prev = itemseek;
        itemnew->next = itemseek->next;
        
        if (itemseek->next != NULL)
            itemseek->next->prev = itemnew;

        itemseek->next = itemnew;
    }

    /* Last item? */
    if (position == list->count)
        list->last = itemnew;

    /* List element added */
    list->count += 1;

    /* Return data pointer */
    *data = itemnew->data;

    return EDLLOK;
}

int dll_remove(dll_list_t* list, unsigned int position)
{
    unsigned int i;
    dll_item_t *itemseek = NULL;

    /* Basic secrity precautions */
    if (!list)
        return EDLLINV;
    if (position >= list->count)
        return EDLLINV;

    /* Seek to item position, which is prev for your new item */
    itemseek = list->first; 
    for (i=0; i<position; i++)
        itemseek = itemseek->next;

    /* Adjust first/last pointers if necessary */
    if (position == 0)
        list->first = itemseek->next;
    if (position == (list->count-1))
        list->last = itemseek->prev;

    /* Remove the item (interconnect it's prev and next neighbours) */
    if (itemseek->prev != NULL)
        itemseek->prev->next = itemseek->next;
    if (itemseek->next != NULL)
        itemseek->next->prev = itemseek->prev;

    /* Free the item */
    if (itemseek->data != NULL)
        dll_mem_free(itemseek->data);
    
    dll_mem_free(itemseek);
    
    list->count--;
    
    return EDLLOK;
}

int dll_get(dll_list_t* list, void** data, unsigned int position)
{
    unsigned int i;
    dll_item_t *itemseek = NULL;

    /* Basic secrity precautions */
    if (!list)
        return EDLLINV;
    if (list->count == 0)
        return EDLLINV;
    if (position >= list->count)
        return EDLLINV;

    /* Seek to item position */
    itemseek = list->first; 
    for (i=0; i<position; i++)
        itemseek = itemseek->next;

    *data = itemseek->data;

    return EDLLOK;
}

int dll_count(dll_list_t* list, unsigned int* count)
{
    if (!list)
        return EDLLINV;

    *count = list->count;

    return EDLLOK;
}

int dll_sort(dll_list_t* list, dll_fctcompare_t compar)
{
    if (!list)
        return EDLLINV;
    if (!compar)
        return EDLLINV;

    return dll_quicksort(list, compar, list->count-1, 0);
}

int dll_indexof(dll_list_t* list, dll_fctcompare_t compar, void* cmpitem, unsigned int *index)
{
    unsigned int i;
    dll_item_t *itemseek = NULL;

    if (!list)
        return EDLLINV;
    if (list->count == 0)
        return EDLLINV;

    i = 0;
    itemseek = list->first;
    do
    {
        if (compar(itemseek->data, cmpitem) == 0) {
            if (index != NULL)
                *index = i;

            return EDLLOK;
        }

        itemseek = itemseek->next;
        i++;
    } while (itemseek != NULL);

    return EDLLERROR;
}

static int dll_quicksort(dll_list_t* list, dll_fctcompare_t compar, int hi, int lo)
{
    int i, j, k;
    dll_item_t *itempivot = NULL, *itemi = NULL, *itemj = NULL;
    void *datatmp = NULL;

    /* If hi < lo don't do anything */
    if (hi <= lo) {
        return EDLLOK;
    }
    /* If there are only two items in the partition compare them directly */
    else if ((hi-lo) == 1)
    {
        itemi = list->first;
        for (k=0; k<lo; k++)
            itemi = itemi->next;

        itemj = list->first;
        for (k=0; k<hi; k++)
            itemj = itemj->next;

        if (compar(itemi->data, itemj->data) > 0) {
            datatmp = itemi->data;
            itemi->data = itemj->data;
            itemj->data = datatmp;  
        }

        return EDLLOK;
    }

    i = lo+1;
    j = hi;

    /* Pivot item (lo item) */
    itempivot = list->first;
    for (k=0; k<lo; k++)
        itempivot = itempivot->next;

    while (j>i) {

        /* Get "up" pointer */
        itemi = list->first;
        for (k=0; k<i; k++)
            itemi = itemi->next;

        /* Move pointer to the first item value > pivot */
        while ((compar(itemi->data, itempivot->data) < 0) && (i<hi)) {          
            itemi = itemi->next;
            i++;
        }

        /* Get "down" pointer */
        itemj = list->first;
        for (k=0; k<j; k++)
            itemj = itemj->next;

        /* Move pointer to the first item value < pivot */
        while ((compar(itemj->data, itempivot->data) >= 0) && (j>lo)) {
            itemj = itemj->prev;
            j--;
        }

        /* If the two pointers have not passed each other switch them */
        if (j>i) {
            datatmp = itemi->data;
            itemi->data = itemj->data;
            itemj->data = datatmp;
        }
    }

    /* Switch pivot and the current "down" pointer if they don't equal each
     * other anyway */
    if (j != lo) {
        datatmp = itemj->data;
        itemj->data = itempivot->data;
        itempivot->data = datatmp;
    }

    /* Recursion */
    dll_quicksort(list, compar, j-1, lo);
    dll_quicksort(list, compar, hi, j+1);

    return EDLLOK;
}

void *dll_mem_alloc(size_t size)
{
    return malloc(size);
}

void dll_mem_free(void *ptr)
{
    free(ptr);
}

