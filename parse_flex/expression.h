#ifndef UUC_EXPRESSION_H
#define UUC_EXPRESSION_H

struct uuc_expression;
typedef struct uuc_expression uuc_expression_t;

#include "type.h"
#include "list.h"
#include "constant.h"

enum uuc_expression_type {
    UUC_EXPR_COMMA,

    UUC_EXPR_ASSIGN,

    UUC_EXPR_ASSIGN_BITAND,
    UUC_EXPR_ASSIGN_BITOR,
    UUC_EXPR_ASSIGN_BITXOR,
    UUC_EXPR_ASSIGN_ADD,
    UUC_EXPR_ASSIGN_SUB,
    UUC_EXPR_ASSIGN_MUL,
    UUC_EXPR_ASSIGN_DIV,
    UUC_EXPR_ASSIGN_MOD,
    UUC_EXPR_ASSIGN_SHL,
    UUC_EXPR_ASSIGN_SHR,

    UUC_EXPR_TERNARY,

    UUC_EXPR_GEQ,
    UUC_EXPR_LEQ,
    UUC_EXPR_LT,
    UUC_EXPR_GT,

    UUC_EXPR_EQ,
    UUC_EXPR_NE,

    UUC_EXPR_MUL,
    UUC_EXPR_DIV,
    UUC_EXPR_MOD,

    UUC_EXPR_ADD,
    UUC_EXPR_SUB,

    UUC_EXPR_SHL,
    UUC_EXPR_SHR,

    UUC_EXPR_PLUS,
    UUC_EXPR_MINUS,

    UUC_EXPR_NOT,
    UUC_EXPR_AND,
    UUC_EXPR_OR,

    UUC_EXPR_BITNOT,
    UUC_EXPR_BITAND,
    UUC_EXPR_BITOR,
    UUC_EXPR_BITXOR,

    UUC_EXPR_DEREF,
    UUC_EXPR_ADDRESS_OF,
    // UUC_EXPR_SIZEOF,
    // UUC_EXPR_ALIGNOF,

    UUC_EXPR_PRE_INCREMENT,
    UUC_EXPR_PRE_DECREMENT,
    UUC_EXPR_POST_INCREMENT,
    UUC_EXPR_POST_DECREMENT,

    UUC_EXPR_MEMBER_ACCESS,

    UUC_EXPR_ARRAY_SUBSCRIPT,
    UUC_EXPR_FUNCALL,

    UUC_EXPR_CONSTANT,
    UUC_EXPR_SYMBOl,

    // SIGN_EXTEND
    // ZERO_EXTEND
    // BIT_CAST
    UUC_EXPR_CAST,
};
typedef enum uuc_expression_type uuc_expression_type_t;

/*  COMMA EXPRESSION */
uuc_expression_t *uuc_expression_comma(uuc_expression_t *lhs,
                                       uuc_expression_t *rhs);

/*  ASSIGNMENT EXPRESSION */
uuc_expression_t *uuc_expression_assign(uuc_expression_t *lhs,
                                        uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_bitand(uuc_expression_t *lhs,
                                               uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_bitor(uuc_expression_t *lhs,
                                              uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_bitxor(uuc_expression_t *lhs,
                                               uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_add(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_sub(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_mul(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_div(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_mod(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_shift_left(uuc_expression_t *lhs,
                                                   uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_assign_shift_right(uuc_expression_t *lhs,
                                                    uuc_expression_t *rhs);


/*  TERNARY EXPRESSION */
uuc_expression_t *uuc_expression_ternary(uuc_expression_t *lhs,
                                         uuc_expression_t *mhs,
                                         uuc_expression_t *rhs);

/*  AND EXPRESSION */
uuc_expression_t *uuc_expression_and(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_or(uuc_expression_t *lhs,
                                    uuc_expression_t *rhs);

/*  COMPARE EXPRESSION */
uuc_expression_t *uuc_expression_greater_equal(uuc_expression_t *lhs,
                                               uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_lower_equal(uuc_expression_t *lhs,
                                             uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_greater(uuc_expression_t *lhs,
                                         uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_lower(uuc_expression_t *lhs,
                                       uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_equal(uuc_expression_t *lhs,
                                       uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_notequal(uuc_expression_t *lhs,
                                          uuc_expression_t *rhs);

/* MUL EXPRESSION */
uuc_expression_t *uuc_expression_mul(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_div(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_mod(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs);


/* ADD EXPRESSION */
uuc_expression_t *uuc_expression_add(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_sub(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_shift_left(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_shift_right(uuc_expression_t *lhs,
                                             uuc_expression_t *rhs);


/* BITAND EXPRESSION */
uuc_expression_t *uuc_expression_bitand(uuc_expression_t *lhs,
                                        uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_bitor(uuc_expression_t *lhs,
                                       uuc_expression_t *rhs);
uuc_expression_t *uuc_expression_bitxor(uuc_expression_t *lhs,
                                        uuc_expression_t *rhs);


/* UNARY EXPRESSION */
uuc_expression_t *uuc_expression_plus(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_minus(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_not(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_bitnot(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_dereference(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_address_of(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_sizeof_type(uuc_type_t *type);
uuc_expression_t *uuc_expression_sizeof_expr(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_alignof_type(uuc_type_t *expr);
uuc_expression_t *uuc_expression_pre_decrement(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_pre_increment(uuc_expression_t *expr);


/* POSTFIX EXPRESSION */
uuc_expression_t *uuc_expression_array_subscript(uuc_expression_t *array,
                                                 uuc_expression_t *subscript);
uuc_expression_t *uuc_expression_function_call(uuc_expression_t *function,
                                               uuc_list_t *parameter_list);
uuc_expression_t *uuc_expression_member_access(uuc_expression_t *compound,
                                               char *member_identifier);
uuc_expression_t *uuc_expression_ptr_member_access(uuc_expression_t *compound,
                                                   char *member_identifier);
uuc_expression_t *uuc_expression_post_decrement(uuc_expression_t *expr);
uuc_expression_t *uuc_expression_post_increment(uuc_expression_t *expr);


/*  EXPRESSION */
uuc_expression_t *uuc_expression_symbol(const char *identifier);
uuc_expression_t *uuc_expression_constant(uuc_constant_t *constant );


uuc_expression_t *uuc_expression_cast(uuc_type_t *type, uuc_expression_t *expr);

#endif // UUC_EXPRESSION_H
