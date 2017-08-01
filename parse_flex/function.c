#include <stdio.h>

#include "function.h"
#include "error.h"
#include "symbol_table.h"

static inline void lol_break_here() {}

/**
   Add function symbol into symbol table just before definition of body
   to allow for easy implementation of  recursive call

   This function is NOT for function prototypes
   but for definition with body BUT it is called before the body is parsed
   just after the function signature (return type and parameter) are parsed
*/
void uuc_function_definition_declare(uuc_list_t *declaration_specifiers, uuc_declarator_t *declarator)
{
    if (declarator->declarator_type != UUC_DECLARATOR_FUNCTION) {
        uuc_error("Expected function definition.");

        printf("%d\n", declarator->declarator_type);

        lol_break_here();
        return;
    }
    uuc_type_t *type = uuc_type_basic(declaration_specifiers);
    uuc_type_resolve(declarator->type, type);

    symbol_table_insert(declarator->identifier, declaration_specifiers, declarator->type);

    symbol_table_insert_function_parameter(declarator);
}

