#include <stdio.h>
#include <stdlib.h>

#include "expression.h"
#include "symbol_table.h"
#include "error.h"
#include "type.h"


struct uuc_expression {
    uuc_expression_type_t expression_type;

    uuc_expression_t *lhs;
    uuc_expression_t *rhs;
    uuc_expression_t *mhs;

    uuc_type_t *type;
};

static uuc_expression_t *expr_alloc(uuc_expression_type_t expr_type,
                                    uuc_expression_t *lhs,
                                    uuc_expression_t *rhs,
                                    uuc_expression_t *mhs)
{
    uuc_expression_t *expr = calloc(1,sizeof*expr);
    expr->expression_type = expr_type;
    expr->lhs = lhs;
    expr->rhs = rhs;
    expr->mhs = mhs;
    return expr;
}


/*  COMMA EXPRESSION */
uuc_expression_t *uuc_expression_comma(uuc_expression_t *lhs,
                                       uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_COMMA, lhs, rhs, NULL);
    return expr;
}

/*  ASSIGNMENT EXPRESSION */
uuc_expression_t *uuc_expression_assign(uuc_expression_t *lhs,
                                        uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_bitand(uuc_expression_t *lhs,
                                               uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_BITAND, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_bitor(uuc_expression_t *lhs,
                                              uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_BITOR, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_bitxor(uuc_expression_t *lhs,
                                               uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_BITXOR, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_add(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_ADD, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_sub(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_SUB, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_mul(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_MUL, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_div(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_DIV, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_mod(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_MOD, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_shift_left(uuc_expression_t *lhs,
                                                   uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_SHL, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_assign_shift_right(uuc_expression_t *lhs,
                                                    uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ASSIGN_SHR, lhs, rhs, NULL);
    return expr;
}


/*  TERNARY EXPRESSION */
uuc_expression_t *uuc_expression_ternary(uuc_expression_t *lhs,
                                         uuc_expression_t *mhs,
                                         uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_TERNARY, lhs, rhs, mhs);
    return expr;
}

/*  AND EXPRESSION */
uuc_expression_t *uuc_expression_and(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_AND, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_or(uuc_expression_t *lhs,
                                    uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_OR, lhs, rhs, NULL);
    return expr;
}

/*  COMPARE EXPRESSION */
uuc_expression_t *uuc_expression_greater_equal(uuc_expression_t *lhs,
                                               uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_GEQ, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_lower_equal(uuc_expression_t *lhs,
                                             uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_LEQ, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_greater(uuc_expression_t *lhs,
                                         uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_GT, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_lower(uuc_expression_t *lhs,
                                       uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_LT, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_equal(uuc_expression_t *lhs,
                                       uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_EQ, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_notequal(uuc_expression_t *lhs,
                                          uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_NE, lhs, rhs, NULL);
    return expr;
}

/* MUL EXPRESSION */
uuc_expression_t *uuc_expression_mul(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_MUL, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_div(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_DIV, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_mod(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_MOD, lhs, rhs, NULL);
    return expr;
}


/* ADD EXPRESSION */
uuc_expression_t *uuc_expression_add(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ADD, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_sub(uuc_expression_t *lhs,
                                     uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_SUB, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_shift_left(uuc_expression_t *lhs,
                                            uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_SHL, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_shift_right(uuc_expression_t *lhs,
                                             uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_SHR, lhs, rhs, NULL);
    return expr;
}


/* BITAND EXPRESSION */
uuc_expression_t *uuc_expression_bitand(uuc_expression_t *lhs,
                                        uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_BITAND, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_bitor(uuc_expression_t *lhs,
                                       uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_BITOR, lhs, rhs, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_bitxor(uuc_expression_t *lhs,
                                        uuc_expression_t *rhs)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_BITXOR, lhs, rhs, NULL);
    return expr;
}

/* UNARY EXPRESSION */
uuc_expression_t *uuc_expression_plus(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_PLUS, operand, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_minus(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_MINUS, operand, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_not(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_NOT, operand, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_bitnot(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_BITNOT, operand, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_dereference(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_DEREF, operand, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_address_of(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_ADDRESS_OF, operand, NULL, NULL);
    return expr;
}


uuc_expression_t *uuc_expression_sizeof_type(uuc_type_t *type)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_CONSTANT, NULL, NULL, NULL);
    (void) type;
    return expr;
}

uuc_expression_t *uuc_expression_sizeof_expr(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_CONSTANT, NULL, NULL, NULL);
    (void) operand;
    return expr;
}

uuc_expression_t *uuc_expression_alignof_type(uuc_type_t *type)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_CONSTANT, NULL, NULL, NULL);
    (void) type;
    return expr;
}

uuc_expression_t *uuc_expression_pre_decrement(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_PRE_DECREMENT, operand, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_pre_increment(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_PRE_INCREMENT, operand, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_cast(uuc_type_t *type, uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_CAST, operand, NULL, NULL);
    (void) type;
    return expr;
}

/* POSTFIX EXPRESSION */
uuc_expression_t *uuc_expression_array_subscript(uuc_expression_t *array,
                                                 uuc_expression_t *subscript)
{
    (void) array; (void) subscript;
    return NULL;
}

uuc_expression_t *uuc_expression_function_call(uuc_expression_t *function,
                                               uuc_list_t *parameter_list)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_FUNCALL, function, NULL, NULL);
    (void) parameter_list;
    return expr;
}

uuc_expression_t *uuc_expression_member_access(uuc_expression_t *compound,
                                               char *member_identifier)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_MEMBER_ACCESS, compound, NULL, NULL);
    (void) member_identifier;
    return expr;
}

uuc_expression_t *uuc_expression_ptr_member_access(uuc_expression_t *compound,
                                                   char *member_identifier)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_MEMBER_ACCESS, compound, NULL, NULL);
    (void) member_identifier;
    return expr;
}

uuc_expression_t *uuc_expression_post_decrement(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_BITXOR, operand, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_post_increment(uuc_expression_t *operand)
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_BITXOR, operand, NULL, NULL);
    return expr;
}

/* PRIMARY EXPRESSION */
uuc_expression_t *uuc_expression_symbol(const char *identifier)
{
    uuc_symbol_t *symbol;
    symbol = symbol_table_get(identifier);
    if (symbol == NULL) {
        uuc_error("reference to undefined identifier '%s'\n", identifier);
        // exit(EXIT_FAILURE);
    }
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_SYMBOl, NULL, NULL, NULL);
    return expr;
}

uuc_expression_t *uuc_expression_constant( uuc_constant_t *constant )
{
    uuc_expression_t *expr = expr_alloc(UUC_EXPR_CONSTANT, NULL, NULL, NULL);
    (void) constant;
    return expr;
}
