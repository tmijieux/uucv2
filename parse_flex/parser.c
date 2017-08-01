#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "token.h"
#include "type.h"
#include "list.h"
#include "function.h"
#include "error.h"
#include "symbol_table.h"
#include "specifier.h"


extern int yylex();
extern char *yytext;

static void top_level_parser(uuc_parser_t *parser);
static void parse_declaration(uuc_parser_t *parser,
                              uuc_list_t **declarator_list,
                              uuc_list_t **declaration_specifiers,
                              int *finisher_token);

void uuc_parser_init(uuc_parser_t *parser)
{
    memset(parser, 0, sizeof*parser);
}

void uuc_parser_free(uuc_parser_t *parser)
{
    (void) parser;
}

void uuc_parser_parse(uuc_parser_t *parser)
{
    top_level_parser(parser);
}

static inline int token_is_typename(int token)
{
    return (token == TOKEN_STRUCT
            || token == TOKEN_UNION
            || token == TOKEN_ENUM
            || token == TOKEN_CHAR
            || token == TOKEN_INT
            || token == TOKEN_FLOAT
            || token == TOKEN_DOUBLE
            || token == TOKEN_VOID
            || token == TOKEN_SIGNED
            || token == TOKEN_UNSIGNED
            || token == TOKEN_SHORT
            || token == TOKEN_LONG   );
}

static inline int token_is_unary_operator(int token)
{
    return (token == '-'
            || token == '+'
            || token == '~'
            || token == '!'
            || token == '*'
            || token == '&'
            || token == TOKEN_INCREMENT
            || token == TOKEN_DECREMENT  );
}

static inline int token_is_storage_class(int token)
{
    return (  token == TOKEN_STATIC
              || token == TOKEN_EXTERN
              || token == TOKEN_REGISTER
              || token == TOKEN_AUTO
              || token == TOKEN_THREAD_LOCAL
              || token == TOKEN_ATOMIC  );
}

static inline int token_is_qualifier(int token)
{
    return (token == TOKEN_CONST
            || token == TOKEN_VOLATILE );
}

static inline int token_is_atom(int token)
{
    return (token == TOKEN_IDENTIFIER
            || token == TOKEN_INT_LITERAL
            || token == TOKEN_FLOAT_LITERAL
            || token == TOKEN_CHAR_LITERAL
            || token == TOKEN_STRING_LITERAL  );
}

static inline int token_is_specifier(int token)
{
    return (token_is_qualifier(token)
            || token_is_storage_class(token)
            || token_is_typename(token) );
}


uuc_specifier_t *make_specifier_from_token(int token)
{
    switch (token) {
    case TOKEN_STRUCT:
    case TOKEN_UNION:
    case TOKEN_ENUM:
    case TOKEN_CHAR:
    case TOKEN_INT:
    case TOKEN_FLOAT:
    case TOKEN_DOUBLE:
    case TOKEN_VOID: break;
    }
    return uuc_specifier(UUC_SPEC_VOID); // FIXME
}

void parse_declaration_list(uuc_parser_t *parser, uuc_list_t **declaration_list, int *finisher)
{
    do {
        uuc_list_t *declarator_list = NULL;
        uuc_list_t *declaration_specifiers = NULL;
        parse_declaration(parser, &declarator_list, &declaration_specifiers, finisher);

        printf("declaration list : --finisher = '%s'\n", yytext);

        *declaration_list = uuc_list_prepend(*declaration_list, declarator_list);
    } while (*finisher == ';');
}

void parse_enumerator_list(uuc_parser_t *parser, uuc_list_t **enumerator_list, int *finisher)
{
    (void) parser;
    *enumerator_list = NULL;
    *finisher = 0;
}

void parse_enum_specifier(uuc_parser_t *parser, uuc_specifier_t **enum_specifier, int *finisher)
{
    int token;
    uuc_list_t *enumerator_list = NULL;

    token = yylex();
    if (token == TOKEN_IDENTIFIER) {
        char *tag_name = strdup(yytext);

        token = yylex();
        if (token == '{') {
            parse_enumerator_list(parser, &enumerator_list, finisher);
        }
        *enum_specifier = uuc_specifier_union(tag_name, enumerator_list);

    } else if (token == '{')  {
        parse_enumerator_list(parser, &enumerator_list, finisher);
        *enum_specifier = uuc_specifier_union(NULL, enumerator_list);
    }
}


void parse_struct_union_specifier(uuc_parser_t *parser, uuc_specifier_t **struct_specifier, int *finisher)
{
    int token;
    uuc_list_t *struct_declaration_list = NULL;
    char *tag_name = NULL;


    token = yylex();
    if (token == TOKEN_IDENTIFIER) {
        tag_name = strdup(yytext);
        token = yylex();
    }
    if (token == '{') {
        parse_declaration_list(parser, &struct_declaration_list, finisher);
        *struct_specifier = uuc_specifier_struct(tag_name, struct_declaration_list);
    } else {
        *finisher = token;
    }
    free(tag_name);
}

static void parse_qualifier_list(uuc_parser_t *parser, uuc_list_t **qualifier_list, int *finisher)
{
    (void) parser;
    do {
        int token = yylex();
        if (token_is_qualifier(token)) {
            uuc_specifier_t *spec;
            spec = make_specifier_from_token(token);
            *qualifier_list = uuc_list_prepend(*qualifier_list, spec);
            continue;
        }
        else {
            *finisher = token;
            break;
        }
    } while(1);
}

static void build_unary_expression(int token, uuc_expression_t **expr)
{
    switch (token) {
    case '-': *expr = uuc_expression_minus(*expr); break;
    case '+': *expr = uuc_expression_plus(*expr); break;
    case '*': *expr = uuc_expression_dereference(*expr); break;
    case '&': *expr = uuc_expression_address_of(*expr); break;
    case '!': *expr = uuc_expression_not(*expr); break;
    case '~': *expr = uuc_expression_bitnot(*expr); break;
    }
}

static void parse_cast_expression(uuc_parser_t *parser, int token, uuc_expression_t **expr, int *finisher)
{

}

static void parse_expression(uuc_parser_t *parser, int token, uuc_expression_t **expr, int *finisher)
{
    if (token_is_unary_operator(token)) {
        int nexttoken = yylex();
        parse_expression(parser, nexttoken, expr, finisher);
        build_unary_expression(token, expr);
    }
    else if (token == TOKEN_ALIGNOF) {

    }
    else if (token == TOKEN_SIZEOF) {

    }
    else if (token == '(') { // nested expression of CAST expression
        token = yylex();
        if (token_is_specifier(token)) {
            parse_cast_expression(parser, token, expr, finisher);
        }
        else if (token == TOKEN_IDENTIFIER) {
            uuc_symbol_t *sym = symbol_table_get(yytext);
            if (sym->symbol_type == UUC_SYMBOL_TYPEDEF) {
                parse_cast_expression(parser, token, expr, finisher);
            }
            else {
                int finish;
                parse_expression(parser, token, expr, &finish);
                if ( finish != ')' ) {
                    uuc_fatal_error("expected ')' before '%s'", yytext);
                }
            }
        }
    }
    else if (token_is_atom(token)) {

    }

    /* phase 2 */
    do {
        token = yylex();
        if (token == '(' ) {
            int finish;
            uuc_list_t *arglist;
            parse_argument_list(parser, &arglist, &finish);
            if (finish != ')') {
                uuc_fatal_error("expected ')' before token '%s'", yytext);
            }
            *finisher = yylex();
            *expr = uuc_expression_function_call(*expr, arglist);
        }
        else if (token == '[') {
            int finish;
            uuc_expression_t *index_expr;
            token = yylex();
            parse_expression(parser, token, &index_expr, &finish);
            if (finish != ']') {
                uuc_fatal_error("expected ']' before token '%s'", yytext);
            }
            *finisher = yylex();
            *expr = uuc_expression_array_subscript(*expr, index_expr);
        }
        else if ( token_is_operator(token) ) {
            int nexttoken = yylex();
            // WONT work for operator precedence if token_is_operator match all of them
            parse_expression(parser, nexttoken, expr, finisher);
            build_binary_expression(token , expr);
            break;
        }
    } while ( token == '(' || token == '[' || token == '.' || token == TOKEN_ARROW ) ;
}


static void parse_declarator(uuc_parser_t *parser, int token, uuc_declarator_t **declarator, int *finisher);

static void parse_direct_declarator(uuc_parser_t *parser, int token, uuc_declarator_t **declarator, int *finisher)
{
    char *name = NULL;

    if (token == '(') {
        int finish;
        token = yylex();
        parse_declarator(parser, token, declarator, &finish);
        if (finish != ')') {
            uuc_fatal_error("unexpected token '%s'", yytext);
        }
    }
    else if (token == TOKEN_IDENTIFIER) {
        *declarator = uuc_declarator_symbol(yytext);
    }

    /* phase 2 */
    do {
        token = yylex();
        if (token == '(' ) {
            int finish;
            uuc_list_t *decl_list;
            parse_parameter_declaration_list(parser, &decl_list, &finish);
            if (finish != ')') {
                uuc_fatal_error("expected ')' before token '%s'", yytext);
            }
            *finisher = yylex();
            *declarator = uuc_declarator_function(*declarator, decl_list);
        }
        else if (token == '[') {
            int finish;
            uuc_expression_t *expr;
            token = yylex();
            parse_expression(parser, token, &expr, &finish);
            if (finish != ']') {
                uuc_fatal_error("expected ']' before token '%s'", yytext);
            }
            *finisher = yylex();
            *declarator = uuc_declarator_array(*declarator, expr);
        }
    } while (token == '(' || token == '[') ;
    *finisher = token;
}

static void parse_declarator(uuc_parser_t *parser, int token, uuc_declarator_t **declarator, int *finisher)
{
    if (token == TOKEN_IDENTIFIER || token == '(') {
        parse_direct_declarator(parser, token, declarator, finisher);
    }
    else if (token == '*') {
        uuc_list_t *qualifier_list = NULL;
        parse_qualifier_list(parser, &qualifier_list, &token);
        parse_declarator(parser, token, declarator, finisher);
        *declarator = uuc_declarator_pointer(qualifier_list, *declarator);
    }
    else {
        printf("DECLARATOR FINISHER=%d == '%c' == '%s'\n", token, token, yytext);
        *finisher = token;
    }
}

static void parse_declarator_list(uuc_parser_t *parser, int token,
                                  uuc_list_t **declarator_list, int *finisher)
{
    do {
        uuc_declarator_t *decl;
        int finish;
        parse_declarator(parser, token, &decl, &finish);
        *declarator_list = uuc_list_prepend(*declarator_list, decl);

        if (finish == ',' ) {
            token = yylex();
            continue;
        }
        else {
            printf("33 FINISHER=%d == '%c' == '%s'\n", finish, finish, yytext);
            *finisher = finish;
            break;
        }
    } while(1);
}

static void parse_declaration(uuc_parser_t *parser,
                              uuc_list_t **declarator_list,
                              uuc_list_t **declaration_specifiers,
                              int *finisher)
{
    uuc_specifier_t *spec;
    int token;
    *declarator_list = NULL;
    *declaration_specifiers = NULL;
    int typename_seen = 0;

    do {
        token = yylex();

        if ( token_is_typename(token) ) {
            typename_seen = 1;
            if (  token == TOKEN_STRUCT || token == TOKEN_UNION ) {
                int finish = 0;
                parse_struct_union_specifier( parser, &spec, &finish);
                *declaration_specifiers = uuc_list_prepend(*declaration_specifiers, spec);
                if (finish != '}') {
                    uuc_fatal_error("unexpected token 4 '%s'", yytext);
                }
            }
            else if (token == TOKEN_ENUM) {
                parse_enum_specifier( parser, &spec, &token);
                *declaration_specifiers = uuc_list_prepend(*declaration_specifiers, spec);
                if (token != '}') {
                    uuc_fatal_error("unexpected token 5 '%s'", yytext);
                }
            }
            else {
                spec = make_specifier_from_token(token);
                *declaration_specifiers = uuc_list_prepend(*declaration_specifiers, spec);
            }
        }
        else if (token_is_storage_class(token)) {
            spec = make_specifier_from_token(token);
            *declaration_specifiers = uuc_list_prepend(*declaration_specifiers, spec);
        }
        else if (token_is_qualifier(token) ) {
            spec = make_specifier_from_token(token);
            *declaration_specifiers = uuc_list_prepend(*declaration_specifiers, spec);
        }
        else if (token == TOKEN_IDENTIFIER) {
            if (typename_seen) {
                parse_declarator_list(parser, token, declarator_list, finisher);
                break;
            }
            else {// OHHHH we're going to need some serious lookahead because the following doesnt work
                typename_seen = 1;
                spec = uuc_specifier_typename(yytext);
            }
        }
        else if (token == '*') {
            parse_declarator_list(parser, token, declarator_list, finisher);
            break;
        }
        else {
            printf("FINISHER 37 =%d == '%c' == '%s'\n", token, token, yytext);
            *finisher = token;
            break;
        }
    } while(1);
}

static void parse_function_body()
{
    int nb_scope = 1;
    int token;

    for (;nb_scope;) {
        token = yylex();
        if (token == '{') {
            ++nb_scope;
        }
        else if (token == '}') {
            --nb_scope;
        }
        else if (token == 0) {
            uuc_fatal_error("unexpected end of file 2");
            break;
        }
    }
}

static void top_level_parser(uuc_parser_t *parser)
{
    while ( !parser->finished ) {
        uuc_list_t *declarator_list = NULL;
        uuc_list_t *declaration_specifiers = NULL;
        int finisher;

        parse_declaration(parser, &declarator_list, &declaration_specifiers, &finisher);

        if ( finisher == ';'  ) {
            uuc_declarator_list_resolve_and_insert(declarator_list, declaration_specifiers);
            continue;
        }
        else if ( finisher == '{' ) {
            if (declarator_list->next != NULL) {
                uuc_fatal_error("expected ';' after top level declarator");
            }
            uuc_declarator_t *fundecl = declarator_list->value;
            uuc_function_definition_declare(declaration_specifiers, fundecl);
            symbol_table_push();
            parse_function_body();
            symbol_table_pop();
        }
        else if ( finisher == 0 ) {
            parser->finished = 1; // GG
        } else {
            uuc_fatal_error("unexpected token 3 '%s'", yytext);
        }
    }
}
