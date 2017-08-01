#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symbol_table.h"
#include "hash_table.h"

#include "type.h"
#include "specifier.h"
#include "error.h"


typedef struct symbol_table symbol_table_t;

struct symbol_table {
    struct symbol_table *next;
    uuc_hash_table_t *symbol_table;
    int level;
};

static symbol_table_t *symbol_table_top_level = NULL;
static uuc_list_t *parameter_declarator_to_be_inserted = NULL;


void symbol_table_init(void)
{
    symbol_table_t *table = calloc(1, sizeof*table);
    table->level = 0;
    table->next = NULL;
    table->symbol_table = uuc_hash_table_new();

    symbol_table_top_level = table;
}

void symbol_table_exit(void)
{
    symbol_table_t *table = symbol_table_top_level;
    symbol_table_top_level = NULL;

    while ( table != NULL ) {
        uuc_hash_table_destroy(table->symbol_table);

        symbol_table_t *tmp = table;
        table = table->next;

        memset(tmp, -1, sizeof*tmp);
        free(tmp);
    }
}

void symbol_table_push(void)
{
    symbol_table_t *table = calloc(1, sizeof*table);
    table->next = symbol_table_top_level;
    table->symbol_table = uuc_hash_table_new();
    table->level = symbol_table_top_level->level+1;

    symbol_table_top_level = table;

    if (parameter_declarator_to_be_inserted != NULL) {
        uuc_list_t *iter = parameter_declarator_to_be_inserted;
        parameter_declarator_to_be_inserted = NULL;

        int i = 0;
        while (iter != NULL) {
            uuc_declarator_t *decl = iter->value;
            if (decl->identifier == NULL) {
                uuc_error("Missing name for parameter %d in function definition.", i);
            } else {
                symbol_table_insert(decl->identifier, NULL, decl->type);
            }
            ++i;
            iter = iter->next;
        }
    }
}

void symbol_table_pop(void)
{
    symbol_table_t *table = symbol_table_top_level;
    assert(table->level > 0);
    symbol_table_top_level = table->next;

    uuc_hash_table_destroy(table->symbol_table);
    memset(table, -1, sizeof*table);
    free(table);
}

static uuc_symbol_type_t
get_symbol_type(uuc_list_t *declaration_specifiers, uuc_type_t *type)
{
    int error = 0;
    int is_static = 0;
    int is_extern = 0;
    int is_typedef = 0;
    int is_register = 0;
    int is_auto = 0;

    int is_thread_local = 0;
    int is_atomic = 0;
    int have_alignas = 0;
    int storage_class = 0;

    uuc_symbol_type_t sym_type = UUC_SYMBOL_VARIABLE;

    uuc_list_t *iter = declaration_specifiers;
    while (iter != NULL) {
        uuc_specifier_t *spec = iter->value;
        switch (spec->spec_type) {
        case UUC_SPEC_STATIC:       if (!is_static++)   ++storage_class;      break;
        case UUC_SPEC_EXTERN:       if (!is_extern++)   ++storage_class;      break;
        case UUC_SPEC_TYPEDEF:      if (!is_typedef++)  ++storage_class;      break;
        case UUC_SPEC_REGISTER:     if (!is_register++) ++storage_class;      break;
        case UUC_SPEC_AUTO:         if (!is_auto++)     ++storage_class;      break;

        case UUC_SPEC_ALIGNAS:      ++have_alignas;     break;
        case UUC_SPEC_THREAD_LOCAL: ++is_thread_local;  break;
        case UUC_SPEC_ATOMIC:       ++is_atomic;        break;
        default: break;
        }
        iter = iter->next;
    }

    if ( uuc_type_is_function(type) ) {
        if (is_auto) {
            uuc_error("illegal storage class 'auto' for function");
            error = 1;
        }
        if (is_register) {
            uuc_error("illegal storage class 'register' for function");
            error = 1;
        }
        if (is_static && symbol_table_top_level->next != NULL) {
            uuc_error("function declared in block scope cannot have 'static' storage class");
            error = 1;
        }
    }
    if (is_typedef > 1) {
        uuc_error("duplicate 'typedef' storage class specifier");
        error = 1;
    }
    if (is_static > 1) {
        uuc_error("duplicate 'static' storage class specifier");
        error = 1;
    }
    if (is_extern > 1) {
        uuc_error("duplicate 'extern' storage class specifier");
        error = 1;
    }
    if (is_register > 1) {
        uuc_error("duplicate 'register' storage class specifier");
        error = 1;
    }
    if (is_auto > 1) {
        uuc_error("duplicate 'auto' storage class specifier");
        error = 1;
    }
    if ( storage_class > 1 ) {
        uuc_error("multiple storage class in declaration specifiers");
        error = 1;
    }

    if (!error ) {
        if (is_typedef) {
            sym_type = UUC_SYMBOL_TYPEDEF;
        } else if (is_register || is_auto) {
            sym_type = UUC_SYMBOL_VARIABLE;
        }
    }
    return error ? UUC_SYMBOL_ERROR : sym_type;
}

void symbol_table_insert(const char *identifier, uuc_list_t *declaration_specifiers, uuc_type_t *type)
{
    void *dummy = NULL;
    symbol_table_t *table = symbol_table_top_level;

    if (uuc_hash_table_get(table->symbol_table, identifier, &dummy) != -1 ) {
        uuc_error("Redefinition of symbol '%s'\n", identifier);
    } else {
        uuc_symbol_t *symbol = calloc(1,sizeof*symbol);
        symbol->symbol_type = get_symbol_type(declaration_specifiers, type);
        symbol->identifier = strdup(identifier);
        symbol->type = type;
        uuc_hash_table_insert(table->symbol_table, identifier, symbol);
    }
}

uuc_symbol_t *symbol_table_get(const char *identifier)
{
    symbol_table_t *table = symbol_table_top_level;

    assert(table != NULL);

    while (table != NULL) {
        void *symb = NULL;
        int err = uuc_hash_table_get(table->symbol_table, identifier, &symb);
        if ( err == 0 ) {
            assert( symb != NULL );
            return symb;
        }
        table = table->next;
    }
    return NULL;
}

void symbol_table_insert_function_parameter(uuc_declarator_t *declarator)
{
    assert( declarator->declarator_type == UUC_DECLARATOR_FUNCTION );
    parameter_declarator_to_be_inserted = declarator->function_declarator_parameters;
    declarator->function_declarator_parameters = NULL;
}
