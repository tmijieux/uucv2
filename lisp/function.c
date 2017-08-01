#include <stdio.h>
#include <string.h>

#include "function.h"
#include "error.h"
#include "symbol_table.h"
#include "expression.h"

static uuc_expression_t *native_plus(uuc_function_t *plus, uuc_expression_t *parameter_list)
{
    (void) plus;

    uuc_expression_t *iter = parameter_list;
    double val = 0.0;
    while (iter != uuc_nil) {
        uuc_expression_t *expr = iter->car;
        expr = uuc_expression_eval(expr);
        if (expr->expression_type == UUC_EXPR_INT) {
            val += expr->i;
        }
        else if  (expr->expression_type == UUC_EXPR_FLOAT) {
            val += expr->f;
        }
        else {
            uuc_error("expression have not a numeric type\n");
        }
        iter = iter->cdr;
    }
    return uuc_expression_float(val);
}

static uuc_expression_t *native_minus(uuc_function_t *minus, uuc_expression_t *parameter_list)
{
    (void) minus;

    if (parameter_list == uuc_nil) {
        return uuc_expression_float(0.0);
    }

    double val = 0.0;
    uuc_expression_t *expr = parameter_list->car;
    expr = uuc_expression_eval(expr);
    if (expr->expression_type == UUC_EXPR_INT) {
        val = expr->i;
    }
    else if (expr->expression_type == UUC_EXPR_FLOAT) {
        val = expr->f;
    }
    else {
        uuc_error("expression have not a numeric type\n");
    }

    if (parameter_list->cdr == uuc_nil) {
        return uuc_expression_float(-val);
    }

    uuc_expression_t *iter = parameter_list->cdr;
    while (iter != uuc_nil) {
        uuc_expression_t *expr = iter->car;
        expr = uuc_expression_eval(expr);
        if (expr->expression_type == UUC_EXPR_INT) {
            val -= expr->i;
        }
        else if  (expr->expression_type == UUC_EXPR_FLOAT) {
            val -= expr->f;
        }
        else {
            uuc_error("expression have not a numeric type\n");
        }
        iter = iter->cdr;
    }
    return uuc_expression_float(val);
}

static uuc_expression_t *native_setq(uuc_function_t *setq, uuc_expression_t *parameter_list)
{
    (void) setq;

    if (parameter_list == uuc_nil) {
        uuc_fatal_error("'setq' take a least one parameter!\n");
    }
    uuc_expression_t *symbol = parameter_list->car;
    uuc_expression_t *value = parameter_list->cdr->car;
    if (symbol->expression_type != UUC_EXPR_SYMBOL) {
        uuc_fatal_error("first parameter to 'setq' must be a symbol!\n");
    }
    value = uuc_expression_eval(value);
    symbol_table_insert(symbol->identifier, value);
    return symbol;
}

static uuc_expression_t *native_defun(uuc_function_t *defun, uuc_expression_t *parameter_list)
{
    (void) defun;
    if (  parameter_list->expression_type != UUC_EXPR_CONS
          || parameter_list->cdr->expression_type != UUC_EXPR_CONS ) {
        uuc_fatal_error("unexpected parameters to 'defun' construct!\n");
    }

    uuc_expression_t *symbol = parameter_list->car;
    uuc_expression_t *bind_parameter = parameter_list->cdr->car;

    if (symbol->expression_type != UUC_EXPR_SYMBOL) {
        uuc_fatal_error("first parameter to 'defun' must be a symbol!\n");
    }
    if (bind_parameter->expression_type != UUC_EXPR_CONS
        && bind_parameter->expression_type != UUC_EXPR_NIL ) {
        uuc_fatal_error("second parameter to 'defun' must be a list!\n");
    }

    uuc_function_t *fun = calloc(1,sizeof*fun);
    fun->name = symbol->identifier;
    fun->parameter_bind_list = bind_parameter;
    fun->body_expression_list = parameter_list->cdr->cdr;

    uuc_expression_t *funexpr  = uuc_expression_function(fun);
    symbol_table_insert(symbol->identifier, funexpr);
    return funexpr;
}

static uuc_expression_t *native_void(uuc_function_t *voidfun, uuc_expression_t *parameter_list)
{
    (void) voidfun;
    if (parameter_list != uuc_nil) {
        uuc_fatal_error("function 'void' take no parameters.");
    }
    return uuc_void;
}

uuc_expression_t *native_eval(uuc_function_t *eval, uuc_expression_t *parameter_list)
{
    (void) eval;

    if (parameter_list == uuc_nil || parameter_list->cdr != uuc_nil) {
        uuc_fatal_error("invalid number of argument to function 'eval'\n");
    }
    uuc_expression_t *expr = parameter_list->car;
    return uuc_expression_eval(uuc_expression_eval(expr));
}

static void bind_let_parameters(uuc_expression_t *bindings)
{
    uuc_expression_t *iter = bindings;
    while (iter != uuc_nil) {
        uuc_expression_t *expr = iter->car;
        if (expr->expression_type != UUC_EXPR_CONS) {
            uuc_fatal_error("list expected inside first parameter of let construct");
        }
        uuc_expression_t *symbol = expr->car;
        if (symbol->expression_type != UUC_EXPR_SYMBOL) {
            uuc_fatal_error("let construct must contains pairs of (symbol value)");
        }
        uuc_expression_t *value = ( (expr->cdr != uuc_nil)
                                    ? expr->cdr->car
                                    : uuc_nil     );
        symbol_table_insert(symbol->identifier, value);
        iter = iter->cdr;
    }
}

static uuc_expression_t *execute_code(uuc_expression_t *body_expression_list)
{
    uuc_expression_t *iter = body_expression_list;
    uuc_expression_t *retvalue = uuc_nil;

    while (iter != uuc_nil) {
        uuc_expression_t *expr = iter->car;
        retvalue = uuc_expression_eval(expr);
        iter = iter->cdr;
    }
    return retvalue;
}

uuc_expression_t *native_let(uuc_function_t *let, uuc_expression_t *parameter_list)
{
    // a bit like function evalutation
    // except parameters are not bind in the same way
    // and the code is evaluated immediately

    (void) let;

    if (parameter_list == uuc_nil) {
        uuc_fatal_error("'let' construct must have at least one arguments\n");
    }
    uuc_expression_t *bindings = parameter_list->car;
    uuc_expression_t *body_expr_list = parameter_list->cdr;

    if (bindings->expression_type != UUC_EXPR_CONS) {
        uuc_fatal_error("first parameter to 'let' construct must be a list\n");
    }
    if (body_expr_list == uuc_nil) {
        return uuc_nil;
    }
    symbol_table_push(); // we need a lexical push
    bind_let_parameters(bindings);
    uuc_expression_t *retvalue = execute_code(body_expr_list);
    symbol_table_pop(); // lexical pop
    return retvalue;
}

uuc_expression_t *native_lambda(uuc_function_t *lambda, uuc_expression_t *parameter_list)
{
    // a bit like function evalutation
    // except parameters are not bind in the same way
    // and the code is evaluated immediately

    (void) lambda;

    if (parameter_list == uuc_nil) {
        uuc_fatal_error("'let' construct must have at least one arguments\n");
    }
    uuc_expression_t *bindings = parameter_list->car;
    uuc_expression_t *body_expression_list = parameter_list->cdr;

    if (bindings->expression_type != UUC_EXPR_CONS) {
        uuc_fatal_error("first parameter to 'lambda' construct must be a list\n");
    }

    uuc_function_t *fun = calloc(1,sizeof*fun);
    fun->name = strdup("<anonymous lambda>");
    fun->parameter_bind_list = bindings;
    fun->body_expression_list = body_expression_list;

    return uuc_expression_function(fun);
}

uuc_expression_t *native_cons(uuc_function_t *cons, uuc_expression_t *parameter_list)
{
    (void) cons;

    if (  parameter_list->expression_type != UUC_EXPR_CONS
          || parameter_list->cdr->expression_type != UUC_EXPR_CONS
          || parameter_list->cdr->cdr != uuc_nil ) {
        uuc_fatal_error("'cons' must have exactly two arguments\n");
    }
    uuc_expression_t *car = parameter_list->car;
    uuc_expression_t *cdr = parameter_list->cdr->car;
    car = uuc_expression_eval(car);
    cdr = uuc_expression_eval(cdr);

    return uuc_expression_cons(car, cdr);
}

void function_init(void)
{
    uuc_expression_t *expr;

    uuc_function_t *plus_function = calloc(1,sizeof*plus_function);
    plus_function->is_native = 1;
    plus_function->native_call = &native_plus;
    plus_function->name = strdup("+");
    expr = uuc_expression_function(plus_function);
    symbol_table_insert("+", expr);

    uuc_function_t *minus_function = calloc(1,sizeof*minus_function);
    minus_function->is_native = 1;
    minus_function->native_call = &native_minus;
    minus_function->name = strdup("-");
    expr = uuc_expression_function(minus_function);
    symbol_table_insert("-", expr);

    uuc_function_t *setq_function = calloc(1,sizeof*setq_function);
    setq_function->is_native = 1;
    setq_function->native_call = &native_setq;
    setq_function->name = strdup("setq");
    expr = uuc_expression_function(setq_function);
    symbol_table_insert("setq", expr);

    uuc_function_t *defun_function = calloc(1,sizeof*defun_function);
    defun_function->is_native = 1;
    defun_function->native_call = &native_defun;
    defun_function->name = strdup("defun");
    expr = uuc_expression_function(defun_function);
    symbol_table_insert("defun", expr);

    uuc_function_t *eval_function = calloc(1,sizeof*eval_function);
    eval_function->is_native = 1;
    eval_function->native_call = &native_eval;
    eval_function->name = strdup("eval");
    expr = uuc_expression_function(eval_function);
    symbol_table_insert("eval", expr);

    uuc_function_t *let_function = calloc(1,sizeof*let_function);
    let_function->is_native = 1;
    let_function->native_call = &native_let;
    let_function->name = strdup("let");
    expr = uuc_expression_function(let_function);
    symbol_table_insert("let", expr);

    uuc_function_t *lambda_function = calloc(1,sizeof*lambda_function);
    lambda_function->is_native = 1;
    lambda_function->native_call = &native_lambda;
    lambda_function->name = strdup("lambda");
    expr = uuc_expression_function(lambda_function);
    symbol_table_insert("lambda", expr);

    uuc_function_t *cons_function = calloc(1,sizeof*cons_function);
    cons_function->is_native = 1;
    cons_function->native_call = &native_cons;
    cons_function->name = strdup("cons");
    expr = uuc_expression_function(cons_function);
    symbol_table_insert("cons", expr);

    uuc_function_t *void_function = calloc(1,sizeof*void_function);
    void_function->is_native = 1;
    void_function->native_call = &native_void;
    void_function->name = strdup("void");
    expr = uuc_expression_function(void_function);
    symbol_table_insert("void", expr);
}

static void bind_parameters(uuc_expression_t *parameter_bind_list, uuc_expression_t *expr_list)
{
    uuc_expression_t *iter1 = parameter_bind_list;
    uuc_expression_t *iter2 = expr_list;

    while (iter1 != uuc_nil && iter2 != uuc_nil) {
        uuc_expression_t *name = iter1->car;
        uuc_expression_t *value = uuc_expression_eval(iter2->car);

        if (name->expression_type != UUC_EXPR_SYMBOL) {
            uuc_fatal_error("parameter list is not symbolic");
        }
        symbol_table_insert(name->identifier, value);
        iter1 = iter1->cdr;
        iter2 = iter2->cdr;
    }

    if (iter1 != uuc_nil || iter2 != uuc_nil) {
        uuc_fatal_error("number of parameter does not match number of argument");
    }
}

uuc_expression_t *uuc_function_apply(uuc_function_t *fun, uuc_expression_t *expr_list)
{
    uuc_expression_t *retvalue = uuc_nil;
    ++ fun->recursion_level;

    if (fun->recursion_level >= 5000) {
        uuc_fatal_error("Maximum recursion depth reached <function '%s'>", fun->name);
    }

    if (fun->is_native) {
        retvalue = fun->native_call(fun, expr_list);
    } else {
        if (fun->body_expression_list == uuc_nil) {
            retvalue = uuc_nil;
        }
        else {
            symbol_table_push();
            bind_parameters(fun->parameter_bind_list, expr_list);
            retvalue = execute_code(fun->body_expression_list);
            symbol_table_pop();
        }
    }
    -- fun->recursion_level;
    return retvalue;
}
