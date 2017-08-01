#ifndef UUC_LIST_H
#define UUC_LIST_H

#include <stdlib.h>

struct uuc_list {
    void *car;
    struct uuc_list *cdr;
};
typedef struct uuc_list uuc_list_t;

struct uuc_list *uuc_list_prepend(uuc_list_t *list, void *value);
void uuc_list_free(uuc_list_t *list);
uuc_list_t *uuc_list_reverse(uuc_list_t *list);
size_t uuc_list_length(uuc_list_t *list);

#endif // UUC_LIST_H
