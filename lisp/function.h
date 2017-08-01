#ifndef UUC_FUNCTION_H
#define UUC_FUNCTION_H

typedef struct uuc_function uuc_function_t;

#include "list.h"
#include "expression.h"

struct uuc_function {
    int is_special;
    int is_native;
    uuc_expression_t *(*native_call)(uuc_function_t *fun, uuc_expression_t *parameter_list);

    uuc_expression_t *parameter_bind_list;
    uuc_expression_t *body_expression_list;

    char *name;

    int recursion_level;
};

void function_init(void);
uuc_expression_t *uuc_function_apply(uuc_function_t *fun, uuc_expression_t *expr);

#endif // UUC_FUNCTION_H
