#ifndef UUC_TYPE_H
#define UUC_TYPE_H

struct uuc_type;
typedef struct uuc_type uuc_type_t;

#include "list.h"
#include "declarator.h"
#include "expression.h"

uuc_type_t *uuc_type_basic(uuc_list_t *declarator_specifier_list);
uuc_type_t *uuc_type_declaration(uuc_type_t *declarator_specifier_type, uuc_declarator_t *declarator);
uuc_type_t *uuc_type_pointer(uuc_type_t *type, uuc_list_t *qualifier_list);
uuc_type_t *uuc_type_function(uuc_type_t *return_type, uuc_list_t *parameter_type_list);
uuc_type_t *uuc_type_array(uuc_type_t *base_type, uuc_expression_t *array_size);

uuc_type_t *uuc_type_struct(const char *tag_name, uuc_list_t *member_declarator_list);
uuc_type_t *uuc_type_union(const char *tag_name, uuc_list_t *member_declarator_list);
uuc_type_t *uuc_type_enum(const char *tag_name, uuc_list_t *member_declarator_list);

void uuc_type_resolve(uuc_type_t *unresolved_type, const uuc_type_t *resolver);
void uuc_type_free(uuc_type_t *type);


int uuc_type_is_function(uuc_type_t *type);

#endif // UUC_TYPE_H
