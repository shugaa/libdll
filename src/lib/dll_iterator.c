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

#define DLL_ITERATOR_INIT       (1<<0)

/* ######################################################################### */
/*                           Private interface (Module)                      */
/* ######################################################################### */

/* ######################################################################### */
/*                           Implementation                                  */
/* ######################################################################### */
int dll_iterator_new(dll_iterator_t* iterator, dll_list_t* list)
{
        if (!iterator)
                return EDLLINV;
        if (!list)
                return EDLLINV; 

        /* Initialize the iterator */
        iterator->flags = 0;
        iterator->list = list;
        iterator->item = NULL;

        return EDLLOK;
}

int dll_iterator_next(dll_iterator_t* iterator, void** data)
{
        int ret = EDLLOK;

        if (!iterator)
                return EDLLINV;
        if (!data)
                return EDLLINV;

        if ((iterator->flags & DLL_ITERATOR_INIT) < DLL_ITERATOR_INIT) {
                iterator->flags = DLL_ITERATOR_INIT;
                iterator->item = iterator->list->first;
        } else {
                if (iterator->item == iterator->list->last) {
                        iterator->item = iterator->list->first;
                        ret = EDLLTILT;
                } else if (iterator->item != NULL) {
                        iterator->item = iterator->item->next;
                }
        }

        if (iterator->item == NULL)
                return EDLLERROR;

        *data = iterator->item->data;

        return ret;
}

int dll_iterator_prev(dll_iterator_t* iterator, void** data)
{
        int ret = EDLLOK;

        if (!iterator)
                return EDLLINV;
        if (!data)
                return EDLLINV;

        if ((iterator->flags & DLL_ITERATOR_INIT) < DLL_ITERATOR_INIT) {
                iterator->flags = DLL_ITERATOR_INIT;
                iterator->item = iterator->list->last;
        } else
        {
                if (iterator->item == iterator->list->first) {
                        iterator->item = iterator->list->last;
                        ret = EDLLTILT;
                } else if (iterator->item != NULL) {
                        iterator->item = iterator->item->prev;
                }
        }

        if (iterator->item == NULL)
                return EDLLERROR;

        *data = iterator->item->data;

        return ret;
}

int dll_iterator_free(dll_iterator_t* iterator)
{   
        if (!iterator)
                return EDLLINV;

        /* Set some reasonable values */
        iterator->flags = 0;
        iterator->list = NULL;
        iterator->item = NULL;

        return EDLLOK;
}

