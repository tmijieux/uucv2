#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "specifier.h"
#include "list.h"
#include "symbol_table.h"
#include "error.h"

uuc_specifier_t *uuc_specifier(uuc_specifier_type_t type)
{
    uuc_specifier_t *spec = calloc(1, sizeof*spec);
    spec->spec_type = type;
    return spec;
}

uuc_specifier_t *uuc_specifier_alignas(uuc_type_t *type)
{
    uuc_specifier_t *spec = calloc(1, sizeof*spec);
    spec->spec_type = UUC_SPEC_ALIGNAS;
    spec->type = type;
    return spec;
}

uuc_specifier_t *uuc_specifier_struct(const char *str, uuc_list_t *struct_declarator)
{
    uuc_specifier_t *spec = calloc(1, sizeof*spec);
    spec->spec_type = UUC_SPEC_STRUCT;
    spec->type = uuc_type_struct(str, struct_declarator);
    return spec;
}

uuc_specifier_t *uuc_specifier_union(const char *str, uuc_list_t *union_declarator)
{
    uuc_specifier_t *spec = calloc(1, sizeof*spec);
    spec->spec_type = UUC_SPEC_ENUM;
    spec->type = uuc_type_union(str, union_declarator);
    return spec;
}

uuc_specifier_t *uuc_specifier_enum(const char *str, uuc_list_t *enum_declarator)
{
    uuc_specifier_t *spec = calloc(1, sizeof*spec);
    spec->spec_type = UUC_SPEC_ENUM;
    spec->type = uuc_type_enum(str, enum_declarator);
    return spec;
}

uuc_specifier_t *uuc_specifier_typename(const char *type_name)
{
    uuc_symbol_t *symbol;
    uuc_specifier_t *spec = calloc(1, sizeof*spec);
    symbol = symbol_table_get(type_name);
    if (symbol != NULL) {
        if (symbol->symbol_type == UUC_SYMBOL_TYPEDEF) {
            spec->type = symbol->type;
            spec->spec_type = UUC_SPEC_TYPENAME;
        } else {
            uuc_error("'%s' is not a typename.", type_name);
            spec->type = NULL;
            spec->spec_type = UUC_SPEC_ERROR;
        }
    } else {
        uuc_error("unknown typename '%s'.", type_name);
        spec->type = NULL;
        spec->spec_type = UUC_SPEC_ERROR;
    }
    return spec;
}
