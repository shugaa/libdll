/*
* Copyright (c) 2008, Björn Rehm
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
#include "dll_util.h"

/* ######################################################################### */
/*                            TODO / Notes                                   */
/* ######################################################################### */

/* ######################################################################### */
/*                            Types & Defines                                */
/* ######################################################################### */
typedef struct {
    int error_code;
    const char* desc;
} dll_error_description_t;

/* ######################################################################### */
/*                           Private interface (Module)                      */
/* ######################################################################### */

/* ######################################################################### */
/*                           Implementation                                  */
/* ######################################################################### */
static const dll_error_description_t dll_errdesc[] = {
    {EDLLERROR, "Unspecified error"},
    {EDLLOK,    "No error"},
    {EDLLTILT,  "Iterator turnaround"},
    {EDLLNOMEM, "Not enough memory"},
    {EDLLINV,   "Invalid argument"},
};
static const char* dll_errdesc_unknown = "Unknown error";


char* dll_strerror(int errnum)
{
    int i;
    int items = sizeof(dll_errdesc)/sizeof(dll_error_description_t);

    /* Go through the list and try to find a matching error code */
    for (i=0;i<items;i++) {
        if (dll_errdesc[i].error_code == errnum) {
            return (char*)dll_errdesc[i].desc;
        }
    }

    /* Not found, return a generic string */
    return (char*)dll_errdesc_unknown;
}

int dll_compar_int(const void* item1, const void* item2)
{
    int int1 = *((int*)item1);
    int int2 = *((int*)item2);

    if (int1 > int2)
        return 1;

    if (int1 < int2)
        return -1;

    return 0;
}


