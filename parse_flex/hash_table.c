#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

struct hash_list {
    char *key;
    void *value;
    struct hash_list *next;
};
typedef struct hash_list hash_list_t;


static hash_list_t *hash_list_prepend(hash_list_t *list, const char *key, void *value)
{
    hash_list_t *new_list = calloc(1, sizeof *list);
    new_list->key = strdup(key);
    new_list->value = value;
    new_list->next = list;
    return new_list;
}

static void hash_list_free(hash_list_t *list)
{
    while (list != NULL) {
        hash_list_t *tmp = list;
        list = list->next;
        free(tmp->key);
        memset(tmp, -1, sizeof *tmp);
        free(tmp);
    }
}

struct uuc_hash_table {
    int nb_buckets;
    hash_list_t **buckets;
};

uuc_hash_table_t *uuc_hash_table_new(void)
{
    uuc_hash_table_t *hash_table = calloc(1, sizeof*hash_table);

    hash_table->nb_buckets = 117;
    hash_table->buckets = calloc(hash_table->nb_buckets,
                                 sizeof*hash_table->buckets);
    return hash_table;
}

void uuc_hash_table_destroy(uuc_hash_table_t *ht)
{
    for (int i = 0; i < ht->nb_buckets; ++i) {
        hash_list_free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
}


int uuc_hash_table_hash(uuc_hash_table_t *hash_table, const char *str)
{
    int k = 0, i = 0;
    for (;str[i]; ++i) {
        k += str[i];
    }
    return k % hash_table->nb_buckets;
}

int uuc_hash_table_insert(uuc_hash_table_t *hash_table, const char *str, void *value)
{
    int key = uuc_hash_table_hash(hash_table, str);
    hash_list_t **bucket = &hash_table->buckets[key];

    hash_list_t *iter = *bucket;
    while (iter != NULL) {
        if ( strcmp(iter->key, str) == 0) {
            return -1;
        }
        iter = iter->next;
    }
    *bucket = hash_list_prepend(*bucket, str, value);
    return 0;
}

int uuc_hash_table_replace(uuc_hash_table_t *hash_table, const char *str, void *new_value, void **old_value)
{
    int key = uuc_hash_table_hash(hash_table, str);
    hash_list_t *bucket = hash_table->buckets[key];

    hash_list_t *iter = bucket;
    while (iter != NULL) {
        if ( strcmp(iter->key, str) == 0 ) {
            *old_value = iter->value;
            iter->value = new_value;
            return 0;
        }
        iter = iter->next;
    }
    return -1;
}

int uuc_hash_table_get(uuc_hash_table_t *hash_table, const char *str, void **value)
{
    int key = uuc_hash_table_hash(hash_table, str);
    hash_list_t *bucket = hash_table->buckets[key];

    hash_list_t *iter = bucket;
    while (iter != NULL) {
        if ( strcmp(iter->key, str) == 0 ) {
            *value = iter->value;
            return 0;
        }
        iter = iter->next;
    }
    *value = NULL;
    return -1;
}
