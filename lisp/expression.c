#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "expression.h"
#include "symbol_table.h"
#include "error.h"
#include "function.h"

static uuc_expression_t uuc_nil_v = {
    .expression_type = UUC_EXPR_NIL
};
uuc_expression_t * restrict const uuc_nil = &uuc_nil_v;

static uuc_expression_t uuc_void_v = {
    .expression_type = UUC_EXPR_VOID
};
uuc_expression_t * restrict const uuc_void = &uuc_void_v;

uuc_expression_t *uuc_expression_eval(uuc_expression_t *expr)
{
    if (  expr->expression_type    == UUC_EXPR_NIL
          || expr->expression_type == UUC_EXPR_INT
          || expr->expression_type == UUC_EXPR_FLOAT
          || expr->expression_type == UUC_EXPR_STRING
          || expr->expression_type == UUC_EXPR_FUNCTION ) {
        return expr;
    }
    else if (expr->expression_type == UUC_EXPR_QUOTE ) {
        return expr->quoted_expr;
    }
    else if ( expr->expression_type == UUC_EXPR_SYMBOL ) {
        uuc_expression_t *value;
        value = symbol_table_get(expr->identifier);
        if (value == NULL) {
            uuc_fatal_error("symbol '%s' is unbound\n", expr->identifier);
        }
        return value;
    }
    else {
        assert( expr->expression_type == UUC_EXPR_CONS );
        assert( expr->car != NULL );
        uuc_expression_t *function_expr = uuc_expression_eval(expr->car);
        if (function_expr->expression_type != UUC_EXPR_FUNCTION) {
            uuc_fatal_error("first element of list is not a function\n");
        }
        uuc_function_t *function = function_expr->function;
        return uuc_function_apply(function, expr->cdr);
    }
}

static uuc_expression_t *expr_alloc(uuc_expression_type_t expr_type)
{
    uuc_expression_t *expr = calloc(1,sizeof*expr);
    expr->expression_type = expr_type;
    return expr;
}

uuc_expression_t *uuc_expression_cons(uuc_expression_t *car, uuc_expression_t *cdr)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_CONS);
    expr->car = car;
    expr->cdr = cdr;
    return expr;
}

uuc_expression_t *uuc_expression_int(int i)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_INT);
    expr->i = i;
    return expr;
}

uuc_expression_t *uuc_expression_float(double f)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_FLOAT);
    expr->f = f;
    return expr;
}

uuc_expression_t *uuc_expression_identifier(const char *id)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_SYMBOL);
    expr->identifier = strdup(id);
    return expr;
}

uuc_expression_t *uuc_expression_string(const char *id)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_STRING);
    expr->string = strdup(id);
    return expr;
}

uuc_expression_t *uuc_expression_function(uuc_function_t *function)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_FUNCTION);
    expr->function = function;
    return expr;
}

uuc_expression_t *uuc_expression_quote(uuc_expression_t *quoted)
{
    /* if (  quoted->expression_type == UUC_EXPR_NIL
       || quoted->expression_type == UUC_EXPR_INT
       || quoted->expression_type == UUC_EXPR_FLOAT
       || quoted->expression_type == UUC_EXPR_STRING
       || quoted->expression_type == UUC_EXPR_QUOTE  ) {
       return quoted;
       } */

    uuc_expression_t *expr = expr_alloc(UUC_EXPR_QUOTE);
    expr->quoted_expr = quoted;
    return expr;
}

void uuc_expression_print(uuc_expression_t *expr)
{
    switch (expr->expression_type) {
    case UUC_EXPR_FLOAT:    printf("%g", expr->f); break;
    case UUC_EXPR_NIL:      printf("nil"); break;
    case UUC_EXPR_INT:      printf("%d", expr->i); break;
    case UUC_EXPR_SYMBOL:   printf("%s", expr->identifier); break;
    case UUC_EXPR_FUNCTION: printf("<function '%s'>", expr->function->name); break;
    case UUC_EXPR_STRING:   printf("%s", expr->string); break;
    case UUC_EXPR_QUOTE:    printf("'");  uuc_expression_print(expr->quoted_expr); break;
    case UUC_EXPR_VOID:     break;
    case UUC_EXPR_CONS:
        printf("(");
        uuc_expression_t *iter = expr;
        while (iter->expression_type == UUC_EXPR_CONS) {
            uuc_expression_print(iter->car);
            if (  iter->cdr->expression_type != UUC_EXPR_CONS
                  && iter->cdr->expression_type != UUC_EXPR_NIL ) {
                printf(" . ");
            }
            else if (iter->cdr->expression_type == UUC_EXPR_CONS) {
                printf(" ");
            }
            iter = iter->cdr;
        }
        if (iter != uuc_nil) {
            uuc_expression_print(iter);
        }
        printf(")");
        break;
    }
}

