
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "statement.h"

struct uuc_statement {
    uuc_statement_type_t statement_type;
};

uuc_statement_t *uuc_statement_expression(uuc_expression_t *expr)
{
    (void) expr;
    return NULL;
}

uuc_statement_t *uuc_statement_compound(uuc_list_t *declaration, uuc_list_t *statements)
{
    (void) declaration;
    (void) statements;
    return NULL;
}

uuc_statement_t *uuc_statement_if(uuc_expression_t *condition,
                                  uuc_statement_t *then_stmt,
                                  uuc_statement_t *else_stmt)
{
    (void) condition;
    (void) then_stmt;
    (void) else_stmt;
    return NULL;
}

uuc_statement_t *uuc_statement_switch(uuc_expression_t *condition,
                                      uuc_statement_t *statement)
{
    (void) condition;
    (void) statement;
    return NULL;
}

uuc_statement_t *uuc_statement_while(uuc_expression_t *condition,
                                     uuc_statement_t *statement)
{
    (void) condition;
    (void) statement;
    return NULL;
}

uuc_statement_t *uuc_statement_do_while(uuc_expression_t *condition,
                                        uuc_statement_t *statement)
{
    (void) condition;
    (void) statement;
    return NULL;
}

uuc_statement_t *uuc_statement_for(uuc_statement_t *init_stmt,
                                   uuc_statement_t *condition,
                                   uuc_expression_t *next_stmt,
                                   uuc_statement_t *statement)
{
    (void) init_stmt;
    (void) condition;
    (void) next_stmt;
    (void) statement;
    return NULL;
}


uuc_statement_t *uuc_statement_goto(const char *label)
{
    (void) label;
    return NULL;
}

uuc_statement_t *uuc_statement_break(void)
{
    return NULL;
}

uuc_statement_t *uuc_statement_continue(void)
{
    return NULL;
}

uuc_statement_t *uuc_statement_return(uuc_expression_t *expr)
{
    (void) expr;
    return NULL;
}


uuc_statement_t *uuc_statement_labeled(const char *label, uuc_statement_t *statement)
{
    (void) label;
    (void) statement;
    return NULL;
}

uuc_statement_t *uuc_statement_case(uuc_expression_t *case_label, uuc_statement_t *statement)
{
    (void) case_label;
    (void) statement;
    return NULL;
}
