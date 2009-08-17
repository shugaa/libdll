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

#include <string.h>     /* memcpy() */
#include <stdlib.h>     /* malloc() / free() */

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
static int dll_quicksort(dll_list_t* list, dll_fctcompare_t compar, unsigned int lo, unsigned int hi);

/* Reimplement these for custom memory management */
static void *dll_memalloc(size_t size);
static void dll_memfree(void *ptr);
static void *dll_memcpy(void *dest, const void *src, size_t n);

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

                dll_memfree(itemcurrent->data);
                dll_memfree(itemcurrent);

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
        if ((itemnew = (dll_item_t*)dll_memalloc(sizeof(dll_item_t))) == NULL)
                return EDLLNOMEM;

        if ((itemnew->data = dll_memalloc(datasize)) == NULL) {
                dll_memfree(itemnew);
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
        if ((itemnew = (dll_item_t*)dll_memalloc(sizeof(dll_item_t))) == NULL) {
                return EDLLNOMEM;
        }

        if ((itemnew->data = dll_memalloc(datasize)) == NULL) {
                dll_memfree(itemnew);
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

        /* Seek to item position */
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
                dll_memfree(itemseek->data);

        dll_memfree(itemseek);

        list->count--;

        return EDLLOK;
}

int dll_get(dll_list_t* list, void** data, unsigned int position)
{
        int rc;
        unsigned int i;
        void *itemseek = NULL;
        dll_iterator_t it;

        /* Basic secrity precautions */
        if (!list)
                return EDLLINV;
        if (list->count == 0)
                return EDLLINV;
        if (position >= list->count)
                return EDLLINV;

        rc = dll_iterator_new(&it, list);
        if (rc != EDLLOK)
                return EDLLERROR;

        /* Seek to item position. Depending on where the item is located in the
         * list we move either up or down. Using an iterator will be slightly
         * slower than accessing the list containers directly by their prev and
         * next handles but it makes the code easier to read imho. */
        if (position < (list->count/2)) {
            for (i=0; i<(position+1); i++) {
                rc = dll_iterator_next(&it, &itemseek);
                if (rc != EDLLOK)
                    break;
            }
        } else {
            for (i=0; i<(list->count-position); i++)  {
                rc = dll_iterator_prev(&it, &itemseek);
                if (rc != EDLLOK)
                    break;
            } 
        }

        if (rc != EDLLOK)
            return EDLLERROR;

        /* Return the data in the item container */
        *data = itemseek;

        return EDLLOK;
}

int dll_count(dll_list_t* list, unsigned int* count)
{
        if (!list)
                return EDLLINV;
        if (!count)
                return EDLLINV;

        *count = list->count;

        return EDLLOK;
}

int dll_deepcopy(dll_list_t *from, dll_list_t *to, size_t datasize)
{
        int rc;
        dll_iterator_t it;
        void *datafrom, *datato;

        if (!from)
                return EDLLINV;
        if (!to)
                return EDLLINV;
        if (to->count != 0)
                return EDLLINV;
        if (datasize < 1)
                return EDLLINV;

        if (from->count == 0)
                return EDLLOK;

        rc = dll_iterator_new(&it, from);
        if (rc != EDLLOK)
                return EDLLERROR;

        while (dll_iterator_next(&it, &datafrom) == EDLLOK) {
                rc = dll_append(to, &datato, datasize);
                if (rc != EDLLOK)
                    break;

                dll_memcpy(datato, datafrom, datasize);
        }

        if (rc != EDLLOK)
                return EDLLERROR;

        return EDLLOK;
}

int dll_sort(dll_list_t* list, dll_fctcompare_t compar)
{
        if (!list)
                return EDLLINV;
        if (!compar)
                return EDLLINV;

        return dll_quicksort(list, compar, 0, list->count-1);
}

int dll_indexof(dll_list_t* list, dll_fctcompare_t compar, void* cmpitem, unsigned int *index)
{
        int rc = EDLLERROR;
        unsigned int i;
        dll_iterator_t it;
        void *data;

        if (!list)
                return EDLLINV;
        if (list->count == 0)
                return EDLLINV;

        i = 0;
        dll_iterator_new(&it, list);
        while(dll_iterator_next(&it, &data) == EDLLOK) {
                 if (compar(data, cmpitem) == 0) {
                        rc = EDLLOK;
                        break;
                 }

                 i++;
        }

        if ((rc == EDLLOK) && (index != NULL))
                *index = i;
        else
                rc = EDLLERROR;

        return rc;
}

int dll_reverse(dll_list_t *list)
{
        int rc;
        unsigned int upidx, downidx;
        dll_iterator_t upit, downit;
        void *datahi, *datalo;

        if (!list)
                return EDLLINV;
        if (list->count <= 1)
                return EDLLOK;

        /* Create up and down iterators and move them to the first and last item
         * respectively */
        rc = dll_iterator_new(&upit, list);
        if (rc != EDLLOK)
                return EDLLERROR;
        rc = dll_iterator_new(&downit, list);
        if (rc != EDLLOK)
                return EDLLERROR;

        rc = dll_iterator_next(&upit, &datalo);
        if (rc != EDLLOK)
                return EDLLERROR;
        rc = dll_iterator_prev(&downit, &datahi);
        if (rc != EDLLOK)
                return EDLLERROR;

        upidx = 0;
        downidx = list->count-1;

        do {
                /* Replace up and down elements and move the iterators to the
                 * next elements */
                void *datatmp = upit.item->data;
                upit.item->data = downit.item->data;
                downit.item->data = datatmp;

                rc = dll_iterator_next(&upit, &datalo);
                if (rc != EDLLOK)
                    break;

                rc = dll_iterator_prev(&downit, &datahi);
                if (rc != EDLLOK)
                    break;

                upidx++;
                downidx--;
        } while (upidx < downidx);

        if (rc != EDLLOK)
            return EDLLERROR;

        return EDLLOK;
}

static int dll_quicksort(dll_list_t* list, dll_fctcompare_t compar, unsigned int lo, unsigned int hi)
{
        int rc;
        unsigned int upidx, downidx, i;
        dll_iterator_t upit, downit;
        dll_item_t *pivotitem;
        void *datahi, *datalo;

        /* 
         * This function implements the quicksort algorithm presented in
         * http://www.cise.ufl.edu/~ddd/cis3020/summer-97/lectures/lec17/sld001.htm
         */

        /* 
         * Some very basic security precautions.
         * first == last
         * first > last
         */
        if (hi <= lo)
            return EDLLOK;

        /* Up and down 'pointers' are lo and hi initially */
        upidx = lo;
        downidx = hi;

        /* Create iterators and move to them to the respective lo hand hi
         * indexes */
        rc = dll_iterator_new(&upit, list);
        if (rc != EDLLOK)
                return EDLLERROR;
        rc = dll_iterator_new(&downit, list);
        if (rc != EDLLOK)
                return EDLLERROR;

        /* Check whether it's better to move the iterator up or down to reach
         * the desired element. This improves sort speeds tremendously (cuts
         * sort times in half on my machine). We might actually try to build an
         * index array of the list or something just for sorting but for now
         * this is fine with me. */
        if (lo < (list->count/2))
                for (i=0;i<(lo+1);i++) {
                        rc = dll_iterator_next(&upit, &datalo);
                        if (rc != EDLLOK)
                                break;
                }
        else
                for (i=0;i<(list->count-lo);i++) { 
                        rc = dll_iterator_prev(&upit, &datalo);
                        if (rc != EDLLOK)
                                break;
                }

        if (rc != EDLLOK)
                return EDLLERROR;

        if (hi < (list->count/2))
                for (i=0;i<(hi+1);i++) {
                        rc = dll_iterator_next(&downit, &datahi);
                        if (rc != EDLLOK)
                                break;
                }
        else
                for (i=0;i<(list->count-hi);i++) {
                        rc = dll_iterator_prev(&downit, &datahi);
                        if (rc != EDLLOK)
                                break;
                }

        if (rc != EDLLOK)
                return EDLLERROR;

        /* lo is the pivot element. Beware: Client code should never use
         * iterator instance internals! Do not take this as a good example. */
        pivotitem = upit.item;

        for(;;) {
                void *datatmp = NULL;

                /* Move up 'pointer' to an element >pivot */
                for(;;) {
                        /* End of partition */
                        if (upidx == hi)
                                break;
                        /* Found */
                        if (compar(datalo, pivotitem->data) > 0)
                                break;

                        rc = dll_iterator_next(&upit, &datalo);
                        if (rc != EDLLOK)
                                break;

                        upidx++;
                }

                if (rc != EDLLOK)
                        break;

                /* Move down 'pointer' to an element <= pivot */
                for(;;) {
                        /* End of partition */
                        if (downidx == lo)
                                break;
                        /* Found */
                        if (compar(datahi, pivotitem->data) <= 0)
                                break;

                        rc = dll_iterator_prev(&downit, &datahi);
                        if (rc != EDLLOK)
                                break;

                        downidx--;
                }

                if (rc != EDLLOK)
                        break;

                if (upidx < downidx) {
                        /* If the two pointers have not passed each other
                         * exchange their values. */
                        datatmp = datalo;
                        downit.item->data = datalo;
                        upit.item->data = datahi;

                        /* Prepare lo and hi data pointers for the next
                         * iteration */
                        datalo = datahi;
                        datahi = datatmp;
                } else {
                        /* Otherwise exchange pivot and the value downidx is
                         * pointing to */
                        datatmp = downit.item->data;
                        downit.item->data = pivotitem->data;
                        pivotitem->data = datatmp;

                        break;
                }
        }

        if (rc != EDLLOK)
                return EDLLERROR;

        /* Recurse the two partitions that have been created */
        if (lo != downidx)
                rc = dll_quicksort(list, compar, lo, downidx-1);
        
        if (rc != EDLLOK)
                return EDLLERROR;

        if (hi != downidx)
                rc = dll_quicksort(list, compar, downidx+1, hi);

        if (rc != EDLLOK)
                return EDLLERROR;

        return EDLLOK;
}

static void *dll_memalloc(size_t size)
{
        return malloc(size);
}

static void dll_memfree(void *ptr)
{
        free(ptr);
}

static void *dll_memcpy(void *dest, const void *src, size_t n)
{
        return memcpy(dest, src, n);
}

