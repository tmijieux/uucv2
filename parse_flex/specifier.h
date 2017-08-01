#ifndef UUC_SPECIFIER_H
#define UUC_SPECIFIER_H


enum uuc_specifier_type {
    UUC_SPEC_ERROR  = -19,

    // type
    UUC_SPEC_VOID  = 33,
    UUC_SPEC_CHAR,
    UUC_SPEC_INT ,
    UUC_SPEC_DOUBLE ,
    UUC_SPEC_FLOAT ,
    UUC_SPEC_COMPLEX ,
    UUC_SPEC_SHORT ,
    UUC_SPEC_LONG ,
    UUC_SPEC_SIGNED ,
    UUC_SPEC_UNSIGNED ,

    UUC_SPEC_TYPENAME , // TYPEDEF'D NAME

    UUC_SPEC_STRUCT,
    UUC_SPEC_UNION,
    UUC_SPEC_ENUM,

    // storage class
    UUC_SPEC_ALIGNAS ,
    UUC_SPEC_STATIC ,
    UUC_SPEC_EXTERN ,
    UUC_SPEC_TYPEDEF ,
    UUC_SPEC_REGISTER ,
    UUC_SPEC_AUTO ,
    UUC_SPEC_THREAD_LOCAL ,
    UUC_SPEC_ATOMIC ,

    // qualifier
    UUC_SPEC_CONST ,
    UUC_SPEC_VOLATILE ,
};


typedef enum uuc_specifier_type uuc_specifier_type_t;
typedef struct uuc_specifier uuc_specifier_t;

#include "type.h"

struct uuc_specifier {
    uuc_specifier_type_t spec_type;
    uuc_type_t *type;
};

uuc_specifier_t *uuc_specifier(enum uuc_specifier_type type);
uuc_specifier_t *uuc_specifier_typename(const char *type_name);
uuc_specifier_t *uuc_specifier_alignas(uuc_type_t *type);
uuc_specifier_t *uuc_specifier_struct(const char *str, uuc_list_t *struct_declarator);
uuc_specifier_t *uuc_specifier_union(const char *str, uuc_list_t *struct_declarator);
uuc_specifier_t *uuc_specifier_enum(const char *str, uuc_list_t *enum_declarator);

#endif // UUC_SPECIFIER_H
