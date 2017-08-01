#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "expression.h"
#include "specifier.h"
#include "error.h"

enum uuc_type_type {

    UUC_TYPE_ERROR = -12,
    UUC_TYPE_UNRESOLVED = 17,

    UUC_TYPE_ARRAY,
    UUC_TYPE_POINTER,
    UUC_TYPE_FUNCTION,
    UUC_TYPE_STRUCT,
    UUC_TYPE_UNION,
    UUC_TYPE_ENUM,

    UUC_TYPE_VOID,
    UUC_TYPE_CHAR,
    UUC_TYPE_UNSIGNED_CHAR,
    UUC_TYPE_SIGNED_CHAR,
    UUC_TYPE_SIGNED_SHORT,
    UUC_TYPE_UNSIGNED_SHORT,
    UUC_TYPE_SIGNED_INT,
    UUC_TYPE_UNSIGNED_INT,
    UUC_TYPE_SIGNED_LONG,
    UUC_TYPE_UNSIGNED_LONG,
    UUC_TYPE_SIGNED_LONG_LONG,
    UUC_TYPE_UNSIGNED_LONG_LONG,
    UUC_TYPE_FLOAT,
    UUC_TYPE_DOUBLE,
    UUC_TYPE_LONG_DOUBLE,
    UUC_TYPE_FLOAT_COMPLEX,
    UUC_TYPE_DOUBLE_COMPLEX,
    UUC_TYPE_LONG_DOUBLE_COMPLEX,
};

typedef enum uuc_type_type uuc_type_type_t;

struct uuc_type {
    uuc_type_type_t type_type;
    uuc_list_t *declarator_specifier_list;
    uuc_list_t *qualifier_list; /*!< cv-qualifier list */
    int type_list_length;
    uuc_list_t *type_list; /*!< list of 'nested' types
                            *   - if type_type == TYPE_ARRAY, length(type_list) == 1 and
                            *    the type contained in type_list is the type of array values
                            *
                            *   - if type_type == TYPE_POINTER, length(type_list) == 1 and
                            *    the type contained in type_list is the pointed type
                            *
                            *   - if type_type == TYPE_FUNCTION, first element of
                            *     type_list is the is the return value of the function
                            *     and the orther elements are the types of the function parameters
                            *
                            *   - if type_type == TYPE_STRUCT or type_type == TYPE_UNION
                            *       type_list is the list of type inside the struct/union
                            */

    uuc_expression_t *array_size; /*!< if type_type == TYPE_ARRAY, an expression that evaluate
                                   *  to the size of the array.
                                   *
                                   * The type of the expression must be an integral type.
                                   *
                                   * If the expression is not evaluatable at compile time, then
                                   * the array type is a VLA array type
                                   */
    char *tag_name; /*!< tag name for struct/union or enum */
};


static uuc_type_type_t signed_type_type(uuc_type_type_t type_type)
{
    switch (type_type) {
    case UUC_TYPE_CHAR: /* fall-thru */
    case UUC_TYPE_UNSIGNED_CHAR: return UUC_TYPE_SIGNED_CHAR;
    case UUC_TYPE_UNSIGNED_INT: return UUC_TYPE_SIGNED_INT;
    case UUC_TYPE_UNSIGNED_SHORT: return UUC_TYPE_SIGNED_SHORT;
    case UUC_TYPE_UNSIGNED_LONG: return UUC_TYPE_SIGNED_LONG;
    case UUC_TYPE_UNSIGNED_LONG_LONG: return UUC_TYPE_SIGNED_LONG_LONG;
    default: return type_type;
    };
}

static uuc_type_type_t unsigned_type_type(uuc_type_type_t type_type)
{
    switch (type_type) {
    case UUC_TYPE_CHAR: /* fall-thru */
    case UUC_TYPE_SIGNED_CHAR: return UUC_TYPE_UNSIGNED_CHAR;
    case UUC_TYPE_SIGNED_INT: return UUC_TYPE_UNSIGNED_INT;
    case UUC_TYPE_SIGNED_SHORT: return UUC_TYPE_UNSIGNED_SHORT;
    case UUC_TYPE_SIGNED_LONG: return UUC_TYPE_UNSIGNED_LONG;
    case UUC_TYPE_SIGNED_LONG_LONG: return UUC_TYPE_UNSIGNED_LONG_LONG;
    default: return type_type;
    };
}

static uuc_type_type_t long_type_type(uuc_type_type_t type_type)
{
    switch (type_type) {
    case UUC_TYPE_SIGNED_INT: return UUC_TYPE_SIGNED_LONG;
    case UUC_TYPE_UNSIGNED_INT: return UUC_TYPE_UNSIGNED_LONG;
    case UUC_TYPE_DOUBLE: return UUC_TYPE_LONG_DOUBLE;
    case UUC_TYPE_DOUBLE_COMPLEX: return UUC_TYPE_LONG_DOUBLE_COMPLEX;
    default: return type_type;
    };
}

static uuc_type_type_t long_long_type_type(uuc_type_type_t type_type)
{
    switch (type_type) {
    case UUC_TYPE_SIGNED_INT: return UUC_TYPE_SIGNED_LONG_LONG;
    case UUC_TYPE_UNSIGNED_INT: return UUC_TYPE_UNSIGNED_LONG_LONG;
    default: return type_type;
    };
}

static uuc_type_type_t short_type_type(uuc_type_type_t type_type)
{
    switch (type_type) {
    case UUC_TYPE_SIGNED_INT: return UUC_TYPE_SIGNED_SHORT;
    case UUC_TYPE_UNSIGNED_INT: return UUC_TYPE_UNSIGNED_SHORT;
    default: return type_type;
    };
}

static uuc_type_type_t get_type_type(uuc_list_t *declarator_specifier_list,
                                     int *is_typename, uuc_type_t **type)
{
    uuc_list_t *iter = declarator_specifier_list;
    int error = 0;
    int is_signed = 0;
    int is_unsigned = 0;
    int is_long = 0;
    int is_short = 0;
    int is_complex = 0;
    int data_type = 0;

    uuc_type_type_t type_type = UUC_TYPE_SIGNED_INT;

    *is_typename = 0;

    while (iter != NULL) {
        uuc_specifier_t *spec = iter->value;
        switch (spec->spec_type) {
        case UUC_SPEC_VOID:       ++data_type; type_type = UUC_TYPE_VOID; break;
        case UUC_SPEC_CHAR:       ++data_type; type_type = UUC_TYPE_CHAR; break;
        case UUC_SPEC_INT:        ++data_type; type_type = UUC_TYPE_SIGNED_INT; break;
        case UUC_SPEC_DOUBLE:     ++data_type; type_type = UUC_TYPE_DOUBLE; break;
        case UUC_SPEC_FLOAT:      ++data_type; type_type = UUC_TYPE_FLOAT; break;

        case UUC_SPEC_TYPENAME:   ++data_type;
            *is_typename = 1;
            *type = spec->type;
            type_type = spec->type->type_type;
            assert ( spec->type != NULL );
            break;

        case UUC_SPEC_COMPLEX:    ++is_complex; break;
        case UUC_SPEC_SHORT:      ++is_short; break;
        case UUC_SPEC_LONG:       ++is_long; break;
        case UUC_SPEC_SIGNED:     ++is_signed; break;
        case UUC_SPEC_UNSIGNED:   ++is_unsigned; break;


        case UUC_SPEC_ENUM:       ++data_type; type_type = -1;/*FIXME*/ break;
        case UUC_SPEC_STRUCT:     ++data_type; type_type = -1;/*FIXME*/ break;
        case UUC_SPEC_UNION:      ++data_type; type_type = -1;/*FIXME*/ break;

        default: break;
        }
        iter = iter->next;
    }

    if (is_complex && (type_type != UUC_TYPE_FLOAT && type_type != UUC_TYPE_DOUBLE ) ) {
        uuc_error("Only floating point types can be '_Complex'\n");
        error = 1;
    }
    if (data_type > 1) {
        uuc_error("two or more data types in declaration specifiers");
        error = 1;
    }
    if (is_signed > 1) {
        uuc_error("duplicate 'signed' in declaration specifiers");
        error = 1;
    }
    if (is_unsigned > 1) {
        uuc_error("duplicate 'unsigned' in declaration specifiers");
        error = 1;
    }
    if (is_long > 2) {
        uuc_error("too much 'long' is too much\n");
        error = 1;
    }
    if (is_short > 1) {
        uuc_error("duplicate 'short' in declaration specifiers\n");
        error = 1;
    }
    if (is_complex > 1) {
        uuc_error("duplicate '_Complex' in declaration specifiers\n");
        error = 1;
    }
    if (is_signed && is_unsigned) {
        uuc_error("both 'signed' and 'unsigned' in declaration specifier\n");
        error = 1;
    }
    if (is_long && is_short) {
        uuc_error("both 'long' and 'short' in declaration specifier\n");
        error = 1;
    }
    if (  (is_signed || is_unsigned) && (type_type == UUC_TYPE_DOUBLE )) {
        uuc_error("'double' cannot be signed or unsigned\n");
        error = 1;
    }
    if (  (is_signed || is_unsigned) && (type_type == UUC_TYPE_FLOAT )) {
        uuc_error("'float' cannot be signed or unsigned\n");
        error = 1;
    }
    if (  (is_long || is_short) && (type_type == UUC_TYPE_FLOAT )) {
        uuc_error("'float' cannot be 'short' or 'long'\n");
        error = 1;
    }
    if (  (is_long || is_short) && (type_type == UUC_TYPE_CHAR )) {
        uuc_error("'char' cannot be 'short' or 'long'\n");
        error = 1;
    }
    if (  is_short && (type_type == UUC_TYPE_DOUBLE )) {
        uuc_error("'double' cannot be 'short'\n");
        error = 1;
    }
    if ( (is_long > 1) && (type_type == UUC_TYPE_DOUBLE)) {
        uuc_error("'double' cannot be 'long long'\n");
        error = 1;
    }
    if (error) {
        *is_typename = 0;
    }
    if (!error ) {
        if (is_signed) {
            type_type = signed_type_type(type_type);
        } else if (is_unsigned) {
            type_type = unsigned_type_type(type_type);
        }
        if (is_long == 1) {
            type_type = long_type_type(type_type);
        } else if (is_long == 2) {
            type_type = long_long_type_type(type_type);
        } else if (is_short) {
            type_type = short_type_type(type_type);
        }
    }
    return error ? UUC_TYPE_ERROR : type_type;
}

uuc_type_t *uuc_type_basic(uuc_list_t *declarator_specifier_list)
{
    uuc_type_t *type = calloc(1, sizeof*type);
    type->declarator_specifier_list =  declarator_specifier_list;

    type->type_list_length = 0;
    type->type_list = NULL;

    if (declarator_specifier_list == NULL) {
        type->type_type = UUC_TYPE_UNRESOLVED;
    } else {
        int is_typename = 0;
        uuc_type_t *typename_type = NULL;
        type->type_type = get_type_type(declarator_specifier_list, &is_typename, &typename_type);

        if (is_typename) {
            memcpy(type, typename_type, sizeof*type);
        }
    }
    return type;
}

uuc_type_t *uuc_type_pointer(uuc_type_t *base_type, uuc_list_t *qualifier_list)
{
    uuc_type_t *type = calloc(1, sizeof*type);

    type->type_type = UUC_TYPE_POINTER;

    type->qualifier_list = qualifier_list;
    type->type_list_length = 1;

    if (base_type == NULL) {
        base_type = uuc_type_basic(NULL);
    }
    type->type_list = uuc_list_prepend(NULL, base_type);
    return type;
}

int uuc_type_equal(uuc_type_t *type_a, uuc_type_t *type_b)
{
    if (type_a->type_type != type_b->type_type) {
        return 0;
    }
    if (  (type_a->type_type == UUC_TYPE_ARRAY
           || type_a->type_type == UUC_TYPE_POINTER) ) {
        uuc_type_t *type_a_in = type_a->type_list->value;
        uuc_type_t *type_b_in = type_b->type_list->value;
        return uuc_type_equal(type_a_in, type_b_in);
    } else if  ( (type_a->type_type == UUC_TYPE_FUNCTION
                  || type_a->type_type == UUC_TYPE_UNION
                  || type_a->type_type == UUC_TYPE_STRUCT) ) {
        if ( type_a->type_list_length != type_b->type_list_length ) {
            return 0;
        }

        uuc_list_t *iter_a = type_a->type_list;
        uuc_list_t *iter_b = type_b->type_list;
        while (iter_a != NULL && iter_b != NULL) {
            if ( !uuc_type_equal((uuc_type_t*)iter_a->value, (uuc_type_t*)iter_b->value) ) {
                return 0;
            }
            iter_a = iter_a->next;
            iter_b = iter_b->next;
        }
    }
    return 1;
}

uuc_type_t *uuc_type_function(uuc_type_t *return_type, uuc_list_t *parameter_type_list)
{
    uuc_type_t *type = calloc(1,sizeof*type);
    type->type_type = UUC_TYPE_FUNCTION;
    if (return_type == NULL) {
        return_type = uuc_type_basic(NULL);
    }

    type->type_list_length = uuc_list_length(parameter_type_list)+1;
    type->type_list = uuc_list_prepend(parameter_type_list, return_type);
    type->declarator_specifier_list = NULL;
    return type;
}

uuc_type_t *uuc_type_array(uuc_type_t *value_type, uuc_expression_t *array_size)
{
    uuc_type_t *type = calloc(1,sizeof*type);
    type->type_type = UUC_TYPE_ARRAY;
    type->declarator_specifier_list = NULL;
    if (value_type == NULL) {
        value_type = uuc_type_basic(NULL);
    }
    type->type_list = uuc_list_prepend(NULL, value_type);
    type->array_size = array_size;
    return type;
}

void uuc_type_resolve(uuc_type_t *unresolved_type, const uuc_type_t *resolver)
{
    if (  unresolved_type->type_type == UUC_TYPE_ARRAY
          ||  unresolved_type->type_type == UUC_TYPE_POINTER
          ||  unresolved_type->type_type == UUC_TYPE_FUNCTION ) {

        uuc_type_t *inner_type = unresolved_type->type_list->value;
        uuc_type_resolve(inner_type, resolver);
    } else {
        assert( unresolved_type->type_type == UUC_TYPE_UNRESOLVED );
        if (unresolved_type != resolver) {
            memcpy(unresolved_type, resolver, sizeof*resolver);
        }
    }
}

void uuc_type_free(uuc_type_t *type)
{
    (void) type; // FIXME
    free(type);
}


int uuc_type_is_function(uuc_type_t *type)
{
    return type->type_type == UUC_TYPE_FUNCTION;
}

uuc_type_t *uuc_type_struct(const char *tag_name, uuc_list_t *member_declarator_list)
{
    uuc_type_t *type = calloc(1,sizeof*type);
    type->type_type = UUC_TYPE_UNION;
    type->tag_name = tag_name != NULL ? strdup(tag_name) : NULL;
    type->type_list = member_declarator_list; // FIXME
    return type;
}

uuc_type_t *uuc_type_union(const char *tag_name, uuc_list_t *member_declarator_list)
{
    uuc_type_t *type = calloc(1,sizeof*type);
    type->type_type = UUC_TYPE_UNION;
    type->tag_name = tag_name != NULL ? strdup(tag_name) : NULL;
    type->type_list = member_declarator_list; // FIXME
    return type;
}

uuc_type_t *uuc_type_enum(const char *tag_name, uuc_list_t *member_declarator_list)
{
    uuc_type_t *type = calloc(1,sizeof*type);
    type->type_type = UUC_TYPE_ENUM;
    type->tag_name = tag_name != NULL ? strdup(tag_name) : NULL;
    type->type_list = member_declarator_list; // FIXME
    return type;
}
