#include <stdio.h>
#include <stdlib.h>
#include <dll_list.h>
#include <dll_util.h>

int main(int argc, char *argv[])
{
        dll_list_t list;
        dll_list_t copy;
        dll_iterator_t it;
        int *data = NULL;
        int i;

        dll_new(&list);
        dll_new(&copy);

        for(i=0;i<20000;i++) {
                dll_append(&list, (void**)&data, sizeof(int));
                *data = (int)(((double)random()/(double)RAND_MAX)*1000.0);
        }

        dll_deepcopy(&list, &copy, sizeof(int));

        dll_sort(&list, dll_compar_int);
        dll_reverse(&list);

        dll_iterator_new(&it, &list);
        while(dll_iterator_next(&it, (void**)&data) == EDLLOK) {
                printf("data: %d\n", *data);
        }

        dll_iterator_new(&it, &copy);
        while(dll_iterator_next(&it, (void**)&data) == EDLLOK) {
                //printf("data: %d\n", *data);
        }

        dll_clear(&list);
        dll_clear(&copy);

        return 0;
}

