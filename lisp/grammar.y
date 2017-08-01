
%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "scanner.h"
#include "expression.h"
#include "symbol_table.h"
#include "list.h"
#include "function.h"

extern void uuc_error(const char *msg, ...);
void yyerror(const char* val)
{
uuc_error(val);
}

%}

%union {
long long int_value;
long double float_value;
char *string_value;
struct uuc_expression *expression;
};


%token TOKEN_IF TOKEN_WHILE
%token TOKEN_AND TOKEN_OR TOKEN_NIL

%token <int_value> TOKEN_INT_LITERAL TOKEN_CHAR_LITERAL
p%token <float_value> TOKEN_FLOAT_LITERAL
%token <string_value> TOKEN_IDENTIFIER TOKEN_STRING_LITERAL TOKEN_TYPENAME

%type <expression> expression atom expression_list

 // START SYMBOL

%start translation_unit

%%

translation_unit
:
| translation_unit expression { uuc_expression_print(uuc_expression_eval($2)); printf("\n"); }
;

expression_list
: expression '.' expression { $$ = uuc_expression_cons($1, $3); }
| expression { $$ = uuc_expression_cons($1, uuc_nil); }
| expression expression_list { $$ = uuc_expression_cons($1, $2); }
;

expression
: '(' expression_list ')' { $$ = $2; }
| '\'' expression { $$ = uuc_expression_quote($2); }
| atom
;

atom
: '(' ')'                 { $$ = uuc_nil; }
| TOKEN_NIL               { $$ = uuc_nil; }
| TOKEN_STRING_LITERAL    { $$ = uuc_expression_string($1); }
| TOKEN_INT_LITERAL       { $$ = uuc_expression_int($1); }
| TOKEN_FLOAT_LITERAL     { $$ = uuc_expression_float($1); }
| TOKEN_IDENTIFIER        { $$ = uuc_expression_identifier($1); }
;

%%

