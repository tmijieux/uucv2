#include <stdlib.h>
#include <stdio.h>

#include "list.h"

uuc_list_t *uuc_list_prepend(uuc_list_t *list, void *car)
{
    uuc_list_t *new_list = calloc(1, sizeof *list);
    new_list->cdr = list;
    new_list->car = car;
    return new_list;
}

void uuc_list_free(uuc_list_t *list)
{
    while (list != NULL) {
        uuc_list_t *tmp = list;
        list = list->cdr;
        free(tmp);
    }
}

uuc_list_t *uuc_list_reverse(uuc_list_t *list)
{
    uuc_list_t *rev = NULL;
    while (list != NULL) {
        uuc_list_t *tmp = list;
        list = list->cdr;
        tmp->cdr = rev;
        rev = tmp;
    }
    return rev;
}

size_t uuc_list_length(uuc_list_t *list)
{
    size_t size = 0;
    while (list != NULL) {
        ++size;
        list = list->cdr;
    }
    return size;
}
