#ifndef UUC_STATEMENT_H
#define UUC_STATEMENT_H

struct uuc_statement;
typedef struct uuc_statement uuc_statement_t;

enum uuc_statement_type {
    UUC_STATEMENT_EXPRESSION,
    UUC_STATEMENT_COMPOUND,
    UUC_STATEMENT_IF,
    UUC_STATEMENT_WHILE,
    UUC_STATEMENT_DO_WHILE,
    UUC_STATEMENT_FOR,
    UUC_STATEMENT_GOTO,
    UUC_STATEMENT_BREAK,
    UUC_STATEMENT_CONTINUE,
    UUC_STATEMENT_RETURN,
};
typedef enum uuc_statement_type uuc_statement_type_t;

#include "list.h"
#include "expression.h"

uuc_statement_t *uuc_statement_expression(uuc_expression_t *expr);
uuc_statement_t *uuc_statement_compound(uuc_list_t *declaration, uuc_list_t *statements);
uuc_statement_t *uuc_statement_if(uuc_expression_t *condition,
                                  uuc_statement_t *then_stmt,
                                  uuc_statement_t *else_stmt);
uuc_statement_t *uuc_statement_switch(uuc_expression_t *condition,
                                      uuc_statement_t *statement);
uuc_statement_t *uuc_statement_while(uuc_expression_t *condition,
                                        uuc_statement_t *statement);
uuc_statement_t *uuc_statement_do_while(uuc_expression_t *condition,
                                        uuc_statement_t *statement);
uuc_statement_t *uuc_statement_for(uuc_statement_t *init_stmt,
                                   uuc_statement_t *condition,
                                   uuc_expression_t *next_stmt,
                                   uuc_statement_t *statement);
uuc_statement_t *uuc_statement_goto(const char *label);
uuc_statement_t *uuc_statement_break(void);
uuc_statement_t *uuc_statement_continue(void);
uuc_statement_t *uuc_statement_return(uuc_expression_t *expr);
uuc_statement_t *uuc_statement_labeled(const char *label, uuc_statement_t *statement);
uuc_statement_t *uuc_statement_case(uuc_expression_t *case_label, uuc_statement_t *statement);

#endif // UUC_STATEMENT_H
