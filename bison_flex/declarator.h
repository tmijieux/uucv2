#ifndef UUC_DECLARATOR_H
#define UUC_DECLARATOR_H

typedef struct uuc_declarator uuc_declarator_t;

#include "type.h"

enum uuc_declarator_type {
    UUC_DECLARATOR_SYMBOL,
    UUC_DECLARATOR_POINTER,
    UUC_DECLARATOR_ARRAY,
    UUC_DECLARATOR_FUNCTION,
    UUC_DECLARATOR_BITFIELD,
};

typedef enum uuc_declarator_type uuc_declarator_type_t;

struct uuc_declarator {
    char *identifier;
    uuc_type_t *type;
    uuc_list_t *function_declarator_parameters;
    uuc_declarator_type_t declarator_type;
};

#include "list.h"
#include "expression.h"

uuc_declarator_t *uuc_declarator_symbol(char *identifier);
uuc_declarator_t *uuc_declarator_pointer(uuc_list_t *qualifier_list, uuc_declarator_t *declarator);
uuc_declarator_t *uuc_declarator_array(uuc_declarator_t *base_declarator,
                                       uuc_expression_t *array_size);
uuc_declarator_t *uuc_declarator_function(uuc_declarator_t *base_declarator,
                                          uuc_list_t *parameter_declarator_list);
uuc_declarator_t *uuc_declarator_bitfield(uuc_declarator_t *base_declarator,
                                          uuc_expression_t *field_length);

uuc_list_t *uuc_declarator_list_resolve_and_insert(uuc_list_t *declarator_list,
                                                   uuc_list_t *declarator_specifier_list);

uuc_declarator_t *uuc_declarator_resolve_type(uuc_declarator_t *declarator,
                                              uuc_list_t *declarator_specifier_list);

uuc_list_t *uuc_declarator_struct_declaration(uuc_list_t *struct_declarator_list,
                                              uuc_list_t *specifier_qualifier_list);

#endif // UUC_DECLARATOR_H
