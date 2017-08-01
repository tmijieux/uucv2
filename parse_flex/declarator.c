#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "declarator.h"
#include "type.h"
#include "symbol_table.h"
#include "error.h"

/*
Reminder:
int (*pointer_to_array_of_int)[];
int (*pointer_to_function)();
int (*pointer_to_int);
int *(*pointer_to_pointer);
int *(array_of_pointer[2]);
int *(function_returning_pointer());

int (function_returning_array())[];

int (array_of_function[])();


*/

static uuc_list_t *uuc_declarator_list_get_type_list(uuc_list_t *parameter_declarator_list)
{
    uuc_list_t *iter = parameter_declarator_list;
    uuc_list_t *type_list = NULL;
    while (iter != NULL) {
        uuc_declarator_t *decl = iter->value;
        type_list = uuc_list_prepend(type_list, decl->type);
        iter = iter->next;
    }
    return uuc_list_reverse(type_list);
}


uuc_declarator_t *uuc_declarator_symbol(char *identifier)
{
    uuc_declarator_t *declarator = calloc(1, sizeof*declarator);

    declarator->identifier = identifier;
    declarator->type = uuc_type_basic(NULL); // unresolved type

    return declarator;
}

uuc_declarator_t *uuc_declarator_pointer(uuc_list_t *qualifier_list, uuc_declarator_t *base_declarator)
{
    uuc_declarator_t *declarator = base_declarator;
    if (declarator == NULL) {
        declarator = calloc(1, sizeof*declarator);
        declarator->identifier = NULL;
        declarator->type = uuc_type_basic(NULL); // unresolved;
        declarator->declarator_type = UUC_DECLARATOR_SYMBOL;
    }
    uuc_type_t *type = uuc_type_pointer(NULL, qualifier_list); // pointer to unresolved
    uuc_type_resolve(declarator->type, type);
    declarator->declarator_type = ( declarator->declarator_type == UUC_DECLARATOR_SYMBOL
                                    ? UUC_DECLARATOR_POINTER
                                    : declarator->declarator_type );
    return declarator;
}

uuc_declarator_t *uuc_declarator_array(uuc_declarator_t *base_declarator, uuc_expression_t *array_size)
{
    uuc_declarator_t *declarator = base_declarator;
    if (declarator == NULL) {
        declarator = calloc(1, sizeof*declarator);
        declarator->identifier = NULL;
        declarator->type = uuc_type_basic(NULL); // unresolved;
        declarator->declarator_type = UUC_DECLARATOR_SYMBOL;
    }
    uuc_type_t *type = uuc_type_array(NULL, array_size);
    uuc_type_resolve(declarator->type, type);
    declarator->declarator_type = ( declarator->declarator_type == UUC_DECLARATOR_SYMBOL
                                    ? UUC_DECLARATOR_ARRAY
                                    : declarator->declarator_type );
    return declarator;
}

uuc_declarator_t *uuc_declarator_function(uuc_declarator_t *base_declarator, uuc_list_t *parameter_declarator_list)
{
    uuc_declarator_t *declarator = base_declarator;
    if (declarator == NULL) {
        declarator = calloc(1, sizeof*declarator);
        declarator->identifier = NULL;
        declarator->type = uuc_type_basic(NULL); // unresolved;
        declarator->declarator_type = UUC_DECLARATOR_SYMBOL;
    }

    uuc_list_t *parameter_type_list;
    parameter_type_list = uuc_declarator_list_get_type_list(parameter_declarator_list);
    declarator->function_declarator_parameters = parameter_declarator_list;

    uuc_type_t *type = uuc_type_function(NULL, parameter_type_list);
    uuc_type_resolve(declarator->type, type);
    declarator->declarator_type = ( declarator->declarator_type == UUC_DECLARATOR_SYMBOL
                                    ? UUC_DECLARATOR_FUNCTION
                                    : declarator->declarator_type );
    return declarator;
}

uuc_declarator_t *uuc_declarator_bitfield(uuc_declarator_t *base_declarator,
                                          uuc_expression_t *field_length)
{
    assert ( ((void)"unimplemented", 1 != 1) );
    (void) base_declarator;
    (void) field_length;
    return NULL;
}

/*  *************************** */

uuc_list_t *uuc_declarator_list_resolve_and_insert(uuc_list_t *declarator_list,
                                                   uuc_list_t *declaration_specifiers)
{
    uuc_list_t *iter = declarator_list;
    uuc_type_t *type = uuc_type_basic(declaration_specifiers);

    while (iter != NULL) {
        uuc_declarator_t *decl = iter->value;
        uuc_type_resolve(decl->type, type);

        assert( decl->identifier != NULL );
        symbol_table_insert(decl->identifier, declaration_specifiers, decl->type);
        iter = iter->next;
    }
    return declarator_list;
}

uuc_declarator_t *uuc_declarator_resolve_type(uuc_declarator_t *declarator,
                                              uuc_list_t *declarator_specifier_list)
{
    uuc_type_t *type = uuc_type_basic(declarator_specifier_list);
    uuc_type_resolve(declarator->type, type);
    uuc_type_free(type);
    return declarator;
}

uuc_list_t *uuc_declarator_struct_declaration(uuc_list_t *struct_declarator_list,
                                              uuc_list_t *specifier_qualifier_list)
{
    uuc_list_t *iter = struct_declarator_list;
    uuc_type_t *type = uuc_type_basic(specifier_qualifier_list);

    while (iter != NULL) {
        uuc_declarator_t *decl = iter->value;
        uuc_type_resolve(decl->type, type);
        assert( decl->identifier != NULL );
        iter = iter->next;
    }

    return struct_declarator_list;
}
