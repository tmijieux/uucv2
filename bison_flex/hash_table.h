#ifndef UUC_HASH_TABLE_H
#define UUC_HASH_TABLE_H

struct uuc_hash_table;
typedef struct uuc_hash_table uuc_hash_table_t;


uuc_hash_table_t *uuc_hash_table_new(void);
void uuc_hash_table_destroy(uuc_hash_table_t*);
int uuc_hash_table_insert(uuc_hash_table_t *hash_table, const char *str, void *value);
int uuc_hash_table_replace(uuc_hash_table_t *hash_table, const char *str, void *new_value, void **old_value);
int uuc_hash_table_get(uuc_hash_table_t *hash_table, const char *str, void **value);

#endif // UUC_HASH_TABLE_H
