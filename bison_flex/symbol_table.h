#ifndef UUC_SYMBOL_TABLE_H
#define UUC_SYMBOL_TABLE_H

enum symbol_type {
    UUC_SYMBOL_ERROR = -37,
    UUC_SYMBOL_TYPEDEF = 8,
    UUC_SYMBOL_ENUMERATOR,
    UUC_SYMBOL_VARIABLE,
    UUC_SYMBOL_EXTERN_VARIABLE,
    UUC_SYMBOL_FUNCTION,
};

typedef enum symbol_type uuc_symbol_type_t;
typedef struct uuc_symbol uuc_symbol_t;

#include "list.h"
#include "type.h"
#include "declarator.h"

struct uuc_symbol {
    uuc_symbol_type_t symbol_type;
    char *identifier;
    uuc_type_t *type;
};

void symbol_table_init(void);
void symbol_table_exit(void);

void symbol_table_push(void);
void symbol_table_pop(void);

void symbol_table_insert(const char *identifier, uuc_list_t *declaration_specifiers, uuc_type_t *symbol_type);
uuc_symbol_t *symbol_table_get(const char *identifier);

void symbol_table_insert_function_parameter(uuc_declarator_t *declarator);

#endif // UUC_SYMBOL_TABLE_H
