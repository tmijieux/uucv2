#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symbol_table.h"
#include "hash_table.h"

#include "error.h"


typedef struct symbol_table symbol_table_t;

struct symbol_table {
    struct symbol_table *next;
    uuc_hash_table_t *symbol_table;
    int level;
};

static symbol_table_t *symbol_table_top_level = NULL;

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

void symbol_table_insert(const char *identifier, uuc_expression_t *expr)
{
    void *dummy = NULL;
    symbol_table_t *table = symbol_table_top_level;

    if (uuc_hash_table_get(table->symbol_table, identifier, &dummy) != -1 ) {
        void *old_expr;
        uuc_hash_table_replace(table->symbol_table, identifier, expr, &old_expr);
    } else {
        uuc_hash_table_insert(table->symbol_table, identifier, expr);
    }
}

uuc_expression_t *symbol_table_get(const char *identifier)
{
    symbol_table_t *table = symbol_table_top_level;

    assert(table != NULL);

    while (table != NULL) {
        void *expr = NULL;
        int err = uuc_hash_table_get(table->symbol_table, identifier, &expr);
        if ( err == 0 ) {
            assert( expr != NULL );
            return expr;
        }
        table = table->next;
    }
    return NULL;
}
