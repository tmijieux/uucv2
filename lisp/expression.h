#ifndef UUC_EXPRESSION_H
#define UUC_EXPRESSION_H

struct uuc_expression;
typedef struct uuc_expression uuc_expression_t;

#include "list.h"
#include "function.h"

enum uuc_expression_type {
    UUC_EXPR_CONS = 37,
    UUC_EXPR_NIL,
    UUC_EXPR_INT,
    UUC_EXPR_FLOAT,
    UUC_EXPR_SYMBOL,
    UUC_EXPR_STRING,
    UUC_EXPR_FUNCTION,
    UUC_EXPR_QUOTE,
    UUC_EXPR_VOID,
};

typedef enum uuc_expression_type uuc_expression_type_t;

struct uuc_expression {
    uuc_expression_type_t expression_type;

    uuc_expression_t *car;
    uuc_expression_t *cdr;
    uuc_expression_t *quoted_expr;

    uuc_function_t *function;
    char *string;
    char *identifier;
    double f;
    int i;
};


uuc_expression_t *uuc_expression_eval(uuc_expression_t *expr);
void uuc_expression_print(uuc_expression_t *expr);

extern uuc_expression_t * restrict const uuc_nil;
extern uuc_expression_t * restrict const uuc_void;

uuc_expression_t *uuc_expression_cons(uuc_expression_t *car, uuc_expression_t *cdr);
uuc_expression_t *uuc_expression_int(int i);
uuc_expression_t *uuc_expression_float(double f);
uuc_expression_t *uuc_expression_identifier(const char *id);
uuc_expression_t *uuc_expression_string(const char *id);
uuc_expression_t *uuc_expression_quote(uuc_expression_t *quoted);


uuc_expression_t *uuc_expression_function(uuc_function_t *function);

#endif // UUC_EXPRESSION_H
