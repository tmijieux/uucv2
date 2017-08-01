#ifndef UUC_SYMBOL_TABLE_H
#define UUC_SYMBOL_TABLE_H

#include "list.h"
#include "expression.h"

void symbol_table_init(void);
void symbol_table_exit(void);

void symbol_table_push(void);
void symbol_table_pop(void);

void symbol_table_insert(const char *identifier, uuc_expression_t *symbol_type);
uuc_expression_t *symbol_table_get(const char *identifier);

#endif // UUC_SYMBOL_TABLE_H
