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

#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>

#include "dll_list.h"
#include "dll_util.h"

#define CU_ADD_TEST(suite, test) (CU_add_test(suite, #test, (CU_TestFunc)test))

/* Never use any value smaller than 10 for DLL_TEST_LISTSIZE */
#define DLL_TEST_LISTSIZE   (5000)
#define DLL_TEST_GENERROR   "Unknown error"

/* Test dll_append() functionality  */
static void test_append(void) 
{
    int rc, i, *data = NULL;
    dll_list_t list;

    rc = dll_init();
    CU_ASSERT(rc == EDLLOK);

    rc = dll_new(&list);
    CU_ASSERT(rc == EDLLOK);

    /* Fill the list with numbers 1..20 */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_append(&list, (void**)&data, sizeof(int));
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = i+1;
    }

    /* Read back and check for consistency */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_get(&list, (void**)&data, (unsigned int)i);
        CU_ASSERT(rc == EDLLOK);
        CU_ASSERT(*data == i+1);
    }

    rc = dll_free(&list);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_close();
    CU_ASSERT(rc == EDLLOK);
}

/* Test dll_insert() functionality  */
static void test_insert(void) 
{
    int rc, i, *data = NULL;
    dll_list_t list;
    unsigned int count;

    rc = dll_init();
    CU_ASSERT(rc == EDLLOK);

    rc = dll_new(&list);
    CU_ASSERT(rc == EDLLOK);

    /* Fill the list by appending items */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_count(&list, &count);
        CU_ASSERT(rc == EDLLOK);

        rc = dll_insert(&list, (void**)&data, sizeof(int), count);
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = i+1;
    }

    /* Read back and check for consistency */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_get(&list, (void**)&data, (unsigned int)i);
        CU_ASSERT(rc == EDLLOK);
        CU_ASSERT(*data == i+1);
    }

    /* Add an item in the middle, then check if first, last and item are OK */
    rc = dll_insert(&list, (void**)&data, sizeof(int), DLL_TEST_LISTSIZE/2);
    CU_ASSERT(rc == EDLLOK);

    if (rc == EDLLOK)
        *data = 999;

    rc = dll_get(&list, (void**)&data, 0);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(*data == 1);

    rc = dll_get(&list, (void**)&data, DLL_TEST_LISTSIZE/2);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(*data == 999);

    rc = dll_count(&list, &count);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_get(&list, (void**)&data, count-1);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(*data == DLL_TEST_LISTSIZE);

    /* Insert invalid index */
    rc = dll_count(&list, &count);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_insert(&list, (void**)&data, sizeof(int), count+1);
    CU_ASSERT(rc != EDLLOK);

    /* Start afresh */
    dll_clear(&list);

    /* Fill the list by prepending items */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_insert(&list, (void**)&data, sizeof(int), 0);
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = i+1;
    }

    /* Read back and check for consistency */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_get(&list, (void**)&data, (unsigned int)i);
        CU_ASSERT(rc == EDLLOK);
        CU_ASSERT(*data == DLL_TEST_LISTSIZE-i);
    }

    rc = dll_free(&list);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_close();
    CU_ASSERT(rc == EDLLOK);
}

/* Test dll_extend() functionality  */
static void test_extend(void) 
{
    int rc, i, *data = NULL;
    unsigned int count;
    dll_list_t list, lext;

    rc = dll_init();
    CU_ASSERT(rc == EDLLOK);

    rc = dll_new(&list);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_new(&lext);
    CU_ASSERT(rc == EDLLOK);

    /* Fill the first list with numbers 1..DLL_TEST_LISTSIZE/2 */
    for(i=0;i<DLL_TEST_LISTSIZE/2;i++) {
        rc = dll_append(&list, (void**)&data, sizeof(int));
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = i+1;
    }

    /* Fill the second list with numbers (DLL_TEST_LISTSIZE/2)+1..DLL_TEST_LISTSIZE */
    for(i=DLL_TEST_LISTSIZE/2;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_append(&lext, (void**)&data, sizeof(int));
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = i+1;
    }

    /* Append lext to list */
    rc = dll_extend(&list, &lext);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_count(&list, &count);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(count == DLL_TEST_LISTSIZE);

    rc = dll_count(&lext, &count);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(count == 0);

    /* Read back and check for consistency */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_get(&list, (void**)&data, (unsigned int)i);
        CU_ASSERT(rc == EDLLOK);
        CU_ASSERT(*data == i+1);
    }

    rc = dll_free(&list);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_free(&lext);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_close();
    CU_ASSERT(rc == EDLLOK);
}

/* Test dll_remove() functionality  */
static void test_remove(void) 
{
    int rc, i, *data = NULL;
    unsigned int count;
    dll_list_t list;

    rc = dll_init();
    CU_ASSERT(rc == EDLLOK);

    rc = dll_new(&list);
    CU_ASSERT(rc == EDLLOK);

    /* Fill the list with numbers 1..DLL_TEST_LISTSIZE */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_append(&list, (void**)&data, sizeof(int));
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = i+1;
    }

    /* Remove item in the middle */
    rc = dll_remove(&list, DLL_TEST_LISTSIZE/2);
    CU_ASSERT(rc == EDLLOK);

    /* Remove the last item (value DLL_TEST_LISTSIZE) */
    rc = dll_count(&list, &count);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_remove(&list, count-1);
    CU_ASSERT(rc == EDLLOK);

    /* Remove the first item (value 1) */
    rc = dll_remove(&list, 0);
    CU_ASSERT(rc == EDLLOK);

    /* Check for correct number of items in the list */
    rc = dll_count(&list, &count);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(count == (DLL_TEST_LISTSIZE-3));

    /* First item must be 2 now */
    rc = dll_get(&list, (void**)&data, (unsigned int)0);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(*data == 2);

    /* Last item must be DLL_TEST_LISTSIZE-1 */
    rc = dll_get(&list, (void**)&data, count-1);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(*data == (DLL_TEST_LISTSIZE-1));

    rc = dll_free(&list);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_close();
    CU_ASSERT(rc == EDLLOK);
}

/* Test dll_indexof() functionality  */
static void test_indexof(void) 
{
    int rc, i, cmpitem, *data = NULL;
    unsigned int index;
    dll_list_t list;

    rc = dll_init();
    CU_ASSERT(rc == EDLLOK);

    rc = dll_new(&list);
    CU_ASSERT(rc == EDLLOK);

    /* Fill the list with numbers 1..DLL_TEST_LISTSIZE */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_append(&list, (void**)&data, sizeof(int));
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = i+1;
    }

    /* Try to find a specific item */
    cmpitem = DLL_TEST_LISTSIZE/2;
    rc = dll_indexof(&list, dll_compar_int, (void*)&cmpitem, &index);
    CU_ASSERT(rc == EDLLOK);
    CU_ASSERT(index == (unsigned int)((DLL_TEST_LISTSIZE/2)-1));

    rc = dll_free(&list);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_close();
    CU_ASSERT(rc == EDLLOK);
}

/* Test dll_sort() functionality  */
static void test_sort(void) 
{
    int rc, i, *data = NULL;
    dll_list_t list;

    rc = dll_init();
    CU_ASSERT(rc == EDLLOK);

    rc = dll_new(&list);
    CU_ASSERT(rc == EDLLOK);

    /* Fill the list with numbers DLL_TEST_LISTSIZE..1 */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_append(&list, (void**)&data, sizeof(int));
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = DLL_TEST_LISTSIZE-i;
    }

    /* Sort the list */
    rc = dll_sort(&list, dll_compar_int);
    CU_ASSERT(rc == EDLLOK);

    /* Make sure the list is in right order now */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_get(&list, (void**)&data, (unsigned int)i);
        CU_ASSERT(rc == EDLLOK);
        CU_ASSERT(*data == i+1);
    }

    rc = dll_free(&list);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_close();
    CU_ASSERT(rc == EDLLOK);
}

/* Test dll_iterator_*() functionality  */
static void test_iterator(void) 
{
    int rc, i, *data = NULL;
    dll_list_t list;
    dll_iterator_t it;

    rc = dll_init();
    CU_ASSERT(rc == EDLLOK);

    rc = dll_new(&list);
    CU_ASSERT(rc == EDLLOK);

    /* Fill the list with numbers DLL_TEST_LISTSIZE..1 */
    for(i=0;i<DLL_TEST_LISTSIZE;i++) {
        rc = dll_append(&list, (void**)&data, sizeof(int));
        CU_ASSERT(rc == EDLLOK);

        if (rc == EDLLOK)
            *data = i+1;
    }

    /* Iterate forward */
    rc = dll_iterator_new(&it, &list);
    CU_ASSERT(rc == EDLLOK);
    
    i = 1;
    while ((rc = dll_iterator_next(&it, (void**)&data)) == EDLLOK) {
        CU_ASSERT(i == *data);
        i++;
    }
    CU_ASSERT(rc == EDLLTILT);

    rc = dll_iterator_free(&it);
    CU_ASSERT(rc == EDLLOK);

    /* Iterate backward */
    rc = dll_iterator_new(&it, &list);
    CU_ASSERT(rc == EDLLOK);
    
    i = DLL_TEST_LISTSIZE;
    while ((rc = dll_iterator_prev(&it, (void**)&data)) == EDLLOK) {
        CU_ASSERT(i == *data);
        i--;
    }
    CU_ASSERT(rc == EDLLTILT);

    rc = dll_iterator_free(&it);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_free(&list);
    CU_ASSERT(rc == EDLLOK);

    rc = dll_close();
    CU_ASSERT(rc == EDLLOK);
}

static void test_strerror(void)
{
    int rc;
    
    rc = strcmp(dll_strerror(EDLLERROR), DLL_TEST_GENERROR);
    CU_ASSERT(rc != 0);

    rc = strcmp(dll_strerror(EDLLOK), DLL_TEST_GENERROR);
    CU_ASSERT(rc != 0);

    rc = strcmp(dll_strerror(EDLLTILT), DLL_TEST_GENERROR);
    CU_ASSERT(rc != 0);

    rc = strcmp(dll_strerror(EDLLNOMEM), DLL_TEST_GENERROR);
    CU_ASSERT(rc != 0);

    rc = strcmp(dll_strerror(EDLLINV), DLL_TEST_GENERROR);
    CU_ASSERT(rc != 0);

    rc = strcmp(dll_strerror(-1), DLL_TEST_GENERROR);
    CU_ASSERT(rc == 0);
}

int main(int argc, char *argv[]) 
{
    int ret = 0;
    CU_ErrorCode cu_rc;
    CU_pSuite cu_suite01;
    CU_pTest cu_test;

    /* Initialize test registry */
    cu_rc = CU_initialize_registry();
    if (cu_rc != CUE_SUCCESS) {
        ret = 1;
        goto finish;
    }

    /* Add a suite to the registry */
    cu_suite01 = CU_add_suite("dlltest", NULL, NULL);
    if (cu_suite01 == NULL) {
        ret = 2;
        goto finish;
    }

    /* Add tests to suite */
    cu_test = CU_ADD_TEST(cu_suite01, test_append);
    if (cu_test == NULL) {
        ret = 3;
        goto finish;
    }
    cu_test = CU_ADD_TEST(cu_suite01, test_insert);
    if (cu_test == NULL) {
        ret = 3;
        goto finish;
    }
    cu_test = CU_ADD_TEST(cu_suite01, test_extend);
    if (cu_test == NULL) {
        ret = 3;
        goto finish;
    }
    cu_test = CU_ADD_TEST(cu_suite01, test_remove);
    if (cu_test == NULL) {
        ret = 3;
        goto finish;
    }
    cu_test = CU_ADD_TEST(cu_suite01, test_indexof);
    if (cu_test == NULL) {
        ret = 3;
        goto finish;
    }
    cu_test = CU_ADD_TEST(cu_suite01, test_sort);
    if (cu_test == NULL) {
        ret = 3;
        goto finish;
    }
    cu_test = CU_ADD_TEST(cu_suite01, test_iterator);
    if (cu_test == NULL) {
        ret = 3;
        goto finish;
    }
    cu_test = CU_ADD_TEST(cu_suite01, test_strerror);
    if (cu_test == NULL) {
        ret = 3;
        goto finish;
    }

    /* Be verbose */
    CU_basic_set_mode(CU_BRM_VERBOSE);

    /* Run all tests in our suite */
    cu_rc = CU_basic_run_tests();
    if (cu_rc != CUE_SUCCESS) {
        ret = 4;
        goto finish;
    }

finish:
    switch (ret) {
        case 1:
            printf("CU_initialize_registry() failed\n");
            break;
        case 2:
            printf("CU_add_suite() failed\n");
            break;
        case 3:
            printf("CU_ADD_TEST() failed\n");
            break;
        case 4:
            printf("CU_basic_run_tests() failed\n");
            break;
        default:
            break;
    }

    CU_cleanup_registry();
    return ret;
}
