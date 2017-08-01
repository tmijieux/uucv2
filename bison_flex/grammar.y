
%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "scanner.h"
#include "statement.h"
#include "constant.h"
#include "expression.h"
#include "declarator.h"
#include "specifier.h"
#include "symbol_table.h"
#include "list.h"
#include "type.h"
#include "function.h"
#include "enumerator.h"

    // TODO Designated initializers


extern void uuc_error(const char *msg, ...);
void yyerror(const char* val)
{
uuc_error(val);
}

%}

%union {
long long int_value;
long double float_value;
void *ptr_value;
char *string_value;

struct uuc_expression *expression;
struct uuc_statement *statement;
struct uuc_program *program;
struct uuc_constant *constant;
struct uuc_list *list;
struct uuc_declarator *declarator;
struct uuc_specifier *specifier;
struct uuc_enumerator *enumerator;
struct uuc_type *type;

void *generic;
};


// KEYWORDS TOKEN
%token TOKEN_ALIGNAS TOKEN_THREAD_LOCAL TOKEN_ATOMIC
%token TOKEN_STATIC TOKEN_REGISTER TOKEN_AUTO TOKEN_EXTERN TOKEN_TYPEDEF
%token TOKEN_RESTRICT TOKEN_INLINE TOKEN_NORETURN
%token TOKEN_CONST TOKEN_VOLATILE
%token TOKEN_STRUCT TOKEN_UNION TOKEN_ENUM
%token TOKEN_SIZEOF TOKEN_ALIGNOF

%token TOKEN_UNSIGNED TOKEN_SIGNED
%token TOKEN_CHAR TOKEN_SHORT TOKEN_INT TOKEN_LONG
%token TOKEN_FLOAT TOKEN_DOUBLE
%token TOKEN_VOID TOKEN_COMPLEX

%token TOKEN_IF TOKEN_ELSE TOKEN_WHILE TOKEN_DO TOKEN_FOR TOKEN_CONTINUE
%token TOKEN_SWITCH TOKEN_CASE TOKEN_DEFAULT
%token TOKEN_RETURN TOKEN_BREAK TOKEN_GOTO

%token TOKEN_STATIC_ASSERT TOKEN_GENERIC


// OPERATORS TOKEN
%token TOKEN_ASSIGN_ADD TOKEN_ASSIGN_SUB TOKEN_ASSIGN_MUL TOKEN_ASSIGN_DIV TOKEN_ASSIGN_MOD
%token TOKEN_AND TOKEN_OR
%token TOKEN_ASSIGN_BITAND TOKEN_ASSIGN_BITOR TOKEN_ASSIGN_BITXOR
%token TOKEN_ASSIGN_SHIFT_LEFT TOKEN_ASSIGN_SHIFT_RIGHT
%token TOKEN_SHIFT_LEFT TOKEN_SHIFT_RIGHT
%token TOKEN_EQUAL TOKEN_NOT_EQUAL TOKEN_GREATER_EQUAL TOKEN_LOWER_EQUAL
%token TOKEN_ARROW TOKEN_ELLIPSIS
%token TOKEN_INCREMENT TOKEN_DECREMENT

%token <int_value> TOKEN_INT_LITERAL TOKEN_CHAR_LITERAL
%token <float_value> TOKEN_FLOAT_LITERAL
%token <string_value> TOKEN_IDENTIFIER TOKEN_STRING_LITERAL TOKEN_TYPENAME

// EXPRESSION TYPE
%type <expression> expression and_expression or_expression constant_expression
%type <expression> assign_expression conditional_expression compare_expression
%type <expression> mul_expression add_expression bitand_expression bitor_expression bitxor_expression
%type <expression> shift_expression unary_expression cast_expression equal_expression
%type <expression> postfix_expression primary_expression

// LIST TYPE
%type <list> argument_list parameter_declaration_list parameter_declaration_list_ellipsis
%type <list> type_qualifier_list struct_declaration
%type <list> statement_list declaration_list declaration struct_declaration_list
%type <list> struct_declarator_list specifier_qualifier_list init_declarator_list
%type <list> enumerator_list
%type <list> declaration_specifiers

// STATEMENT TYPE
%type <statement> statement selection_statement iteration_statement labeled_statement
%type <statement> jump_statement compound_statement expression_statement

// DECLARATOR TYPE
%type <declarator> declarator parameter_declaration
%type <declarator> direct_declarator abstract_declarator struct_declarator
%type <declarator> direct_abstract_declarator init_declarator

// SPECIFIER TYPE
%type <specifier> storage_class_specifier type_specifier type_qualifier
%type <specifier> struct_or_union_specifier enum_specifier

 // ENUMERATOR TYPE
%type <enumerator> enumerator

// TYPE TYPE
%type <type> type_name

 // START SYMBOL

%start translation_unit

%%

/* ************ EXPRESSION ********************* */

primary_expression
: TOKEN_IDENTIFIER      { $$ = uuc_expression_symbol($1); }
| TOKEN_INT_LITERAL     { $$ = uuc_expression_constant(NULL); }
| TOKEN_FLOAT_LITERAL   { $$ = uuc_expression_constant(NULL); }
| TOKEN_CHAR_LITERAL    { $$ = uuc_expression_constant(NULL); }
| TOKEN_STRING_LITERAL  { $$ = uuc_expression_constant(NULL); }
| '(' expression ')'    { $$ = $2; }
;

postfix_expression
: primary_expression
| postfix_expression '[' constant_expression ']' { $$ = uuc_expression_array_subscript($1, $3); }
| postfix_expression '(' argument_list ')' { $$ = uuc_expression_function_call($1, $3); }
| postfix_expression '.' TOKEN_IDENTIFIER { $$ = uuc_expression_member_access($1, $3); }
| postfix_expression TOKEN_ARROW TOKEN_IDENTIFIER { $$ = uuc_expression_ptr_member_access($1, $3); }
| postfix_expression TOKEN_INCREMENT { $$ = uuc_expression_post_increment($1); }
| postfix_expression TOKEN_DECREMENT { $$ = uuc_expression_post_decrement($1); }
;

argument_list
: assign_expression                   { $$ = uuc_list_prepend(NULL, $1); }
| argument_list ',' assign_expression { $$ = uuc_list_prepend($1, $3); }
;

unary_expression
: postfix_expression
| '+' unary_expression { $$ = uuc_expression_plus($2); }
| '-' unary_expression { $$ = uuc_expression_minus($2); }
| '~' unary_expression { $$ = uuc_expression_bitnot($2); }
| '!'  unary_expression { $$ = uuc_expression_not($2); }
| '&' unary_expression { $$ = uuc_expression_address_of($2); }
| '*' unary_expression { $$ = uuc_expression_dereference($2); }
| TOKEN_INCREMENT unary_expression { $$ = uuc_expression_pre_increment($2); }
| TOKEN_DECREMENT unary_expression { $$ = uuc_expression_pre_decrement($2); }
| TOKEN_SIZEOF '(' type_name ')' { $$ = uuc_expression_sizeof_type($3); }
| TOKEN_SIZEOF unary_expression { $$ = uuc_expression_sizeof_expr($2); }
| TOKEN_ALIGNOF '(' type_name ')' { $$ = uuc_expression_alignof_type($3); }
;

cast_expression
: unary_expression
| '(' type_name ')' cast_expression { $$ = uuc_expression_cast($2, $4); }
;

mul_expression
: cast_expression
| mul_expression '*' cast_expression { $$ = uuc_expression_mul($1, $3); }
| mul_expression '/' cast_expression { $$ = uuc_expression_div($1, $3); }
| mul_expression '%' cast_expression { $$ = uuc_expression_mod($1, $3); }
;

add_expression
: mul_expression
| add_expression '+' mul_expression { $$ = uuc_expression_add($1, $3); }
| add_expression '-' mul_expression { $$ = uuc_expression_sub($1, $3); }
;

shift_expression
: add_expression
| shift_expression TOKEN_SHIFT_LEFT add_expression { $$ = uuc_expression_shift_left($1, $3); }
| shift_expression TOKEN_SHIFT_RIGHT add_expression { $$ = uuc_expression_shift_right($1, $3); }
;

compare_expression
: shift_expression
| compare_expression '<' shift_expression { $$ = uuc_expression_lower($1, $3); }
| compare_expression '>' shift_expression { $$ = uuc_expression_greater($1, $3); }
| compare_expression TOKEN_GREATER_EQUAL shift_expression { $$ = uuc_expression_greater_equal($1, $3); }
| compare_expression TOKEN_LOWER_EQUAL   shift_expression { $$ = uuc_expression_lower_equal($1, $3); }
;

equal_expression
: compare_expression
| equal_expression TOKEN_EQUAL     compare_expression { $$ = uuc_expression_equal($1, $3); }
| equal_expression TOKEN_NOT_EQUAL compare_expression { $$ = uuc_expression_notequal($1, $3); }
;

bitand_expression
: equal_expression
| bitand_expression '&' equal_expression { $$ = uuc_expression_bitand($1, $3); }
;

bitxor_expression
: bitand_expression
| bitxor_expression '^' bitand_expression { $$ = uuc_expression_bitxor($1, $3); }
;

bitor_expression
: bitxor_expression
| bitor_expression '|' bitxor_expression { $$ = uuc_expression_bitor($1, $3); }
;

and_expression
: bitor_expression
| and_expression TOKEN_AND bitor_expression { $$ = uuc_expression_and($1, $3); }
;

or_expression
: and_expression
| or_expression TOKEN_OR and_expression { $$ = uuc_expression_or($1, $3); }
;

conditional_expression
: or_expression
| or_expression '?' expression ':' conditional_expression { $$ = uuc_expression_ternary($1, $3, $5); }
;

assign_expression
: conditional_expression
| unary_expression '='                 assign_expression { $$ = uuc_expression_assign($1, $3); }
| unary_expression TOKEN_ASSIGN_BITAND assign_expression { $$ = uuc_expression_assign_bitand($1, $3); }
| unary_expression TOKEN_ASSIGN_BITOR  assign_expression { $$ = uuc_expression_assign_bitor($1, $3); }
| unary_expression TOKEN_ASSIGN_BITXOR assign_expression { $$ = uuc_expression_assign_bitxor($1, $3); }
| unary_expression TOKEN_ASSIGN_ADD    assign_expression { $$ = uuc_expression_assign_add($1, $3); }
| unary_expression TOKEN_ASSIGN_SUB    assign_expression { $$ = uuc_expression_assign_sub($1, $3); }
| unary_expression TOKEN_ASSIGN_MUL    assign_expression { $$ = uuc_expression_assign_mul($1, $3); }
| unary_expression TOKEN_ASSIGN_DIV    assign_expression { $$ = uuc_expression_assign_div($1, $3); }
| unary_expression TOKEN_ASSIGN_MOD    assign_expression { $$ = uuc_expression_assign_mod($1, $3); }
| unary_expression TOKEN_ASSIGN_SHIFT_LEFT assign_expression  { $$ = uuc_expression_assign_shift_left($1, $3); }
| unary_expression TOKEN_ASSIGN_SHIFT_RIGHT assign_expression { $$ = uuc_expression_assign_shift_right($1, $3); }
;

expression
: assign_expression
| expression ',' assign_expression { $$ = uuc_expression_comma($1, $3); }
;

constant_expression
: conditional_expression
;

/* ************ DECLARATION ********************* */

declaration
: declaration_specifiers ';' /* empty declaration // MAYBE struct/union or
                                enum definition or struct/union forward declaration */
| declaration_specifiers init_declarator_list ';' { uuc_declarator_list_resolve_and_insert($2, $1); }
;

/* TODO IDEA 'combine function for declarations specifiers' (instead of list) */

declaration_specifiers
: storage_class_specifier                        { $$ = uuc_list_prepend(NULL, $1); }
| storage_class_specifier declaration_specifiers { $$ = uuc_list_prepend(  $2, $1); }
| type_specifier                                 { $$ = uuc_list_prepend(NULL, $1); }
| type_specifier declaration_specifiers          { $$ = uuc_list_prepend(  $2, $1); }
| type_qualifier                                 { $$ = uuc_list_prepend(NULL, $1); }
| type_qualifier declaration_specifiers          { $$ = uuc_list_prepend(  $2, $1); }
;

init_declarator_list
: init_declarator                          { $$ = uuc_list_prepend(NULL, $1); }
| init_declarator_list ',' init_declarator { $$ = uuc_list_prepend(  $1, $3); }
;

init_declarator
: declarator
| declarator '=' initializer
;

storage_class_specifier
: TOKEN_TYPEDEF      { $$ = uuc_specifier(UUC_SPEC_TYPEDEF); }
| TOKEN_EXTERN       { $$ = uuc_specifier(UUC_SPEC_EXTERN); }
| TOKEN_STATIC       { $$ = uuc_specifier(UUC_SPEC_STATIC); }
| TOKEN_AUTO         { $$ = uuc_specifier(UUC_SPEC_AUTO); }
| TOKEN_REGISTER     { $$ = uuc_specifier(UUC_SPEC_REGISTER); }
| TOKEN_THREAD_LOCAL { $$ = uuc_specifier(UUC_SPEC_THREAD_LOCAL); }
| TOKEN_ATOMIC       { $$ = uuc_specifier(UUC_SPEC_ATOMIC); }
| TOKEN_ALIGNAS '(' type_name ')' { $$ = uuc_specifier_alignas($3); }
| TOKEN_INLINE  { $$ = NULL; /*FIXME */ }
| TOKEN_RESTRICT { $$ = NULL; /*FIXME */ }
| TOKEN_NORETURN { $$ = NULL; /*FIXME */ }
;

type_specifier
: TOKEN_VOID     { $$ = uuc_specifier(UUC_SPEC_VOID);     }
| TOKEN_CHAR     { $$ = uuc_specifier(UUC_SPEC_CHAR);     }
| TOKEN_SHORT    { $$ = uuc_specifier(UUC_SPEC_SHORT);    }
| TOKEN_INT      { $$ = uuc_specifier(UUC_SPEC_INT);      }
| TOKEN_LONG     { $$ = uuc_specifier(UUC_SPEC_LONG);     }
| TOKEN_FLOAT    { $$ = uuc_specifier(UUC_SPEC_FLOAT);    }
| TOKEN_DOUBLE   { $$ = uuc_specifier(UUC_SPEC_DOUBLE);   }
| TOKEN_COMPLEX  { $$ = uuc_specifier(UUC_SPEC_COMPLEX);  }
| TOKEN_SIGNED   { $$ = uuc_specifier(UUC_SPEC_SIGNED);   }
| TOKEN_UNSIGNED { $$ = uuc_specifier(UUC_SPEC_UNSIGNED); }
| struct_or_union_specifier
| enum_specifier
| TOKEN_TYPENAME { $$ = uuc_specifier_typename($1);       }
;

struct_or_union_specifier
: TOKEN_STRUCT TOKEN_IDENTIFIER '{' struct_declaration_list '}' { $$ = uuc_specifier_struct(  $2,   $4); }
| TOKEN_STRUCT '{' struct_declaration_list '}'                  { $$ = uuc_specifier_struct(NULL,   $3); }
| TOKEN_STRUCT TOKEN_IDENTIFIER                                 { $$ = uuc_specifier_struct(  $2, NULL); }
| TOKEN_UNION  TOKEN_IDENTIFIER '{' struct_declaration_list '}' { $$ = uuc_specifier_union(  $2,   $4);  }
| TOKEN_UNION  '{' struct_declaration_list '}'                  { $$ = uuc_specifier_union(NULL,   $3);  }
| TOKEN_UNION  TOKEN_IDENTIFIER                                 { $$ = uuc_specifier_union(  $2, NULL);  }
;

struct_declaration_list
: struct_declaration                         { $$ = uuc_list_prepend(NULL, $1); }
| struct_declaration_list struct_declaration { $$ = uuc_list_prepend($1, $2);   }
;

struct_declaration
: specifier_qualifier_list struct_declarator_list ';' { $$ = uuc_declarator_struct_declaration($2, $1); }
;

specifier_qualifier_list
: type_specifier specifier_qualifier_list { $$ = uuc_list_prepend(  $2, $1); }
| type_specifier                          { $$ = uuc_list_prepend(NULL, $1); }
| type_qualifier specifier_qualifier_list { $$ = uuc_list_prepend(  $2, $1); }
| type_qualifier                          { $$ = uuc_list_prepend(NULL, $1); }
;

struct_declarator_list
: struct_declarator                            { $$ = uuc_list_prepend(NULL, $1); }
| struct_declarator_list ',' struct_declarator { $$ = uuc_list_prepend(  $1, $3); }
;

struct_declarator
: declarator
| ':' constant_expression             { $$ = uuc_declarator_bitfield(NULL, $2); }
| declarator ':' constant_expression  { $$ = uuc_declarator_bitfield(  $1, $3); }
;

enum_specifier
: TOKEN_ENUM '{' enumerator_list '}'                      { $$ = uuc_specifier_enum(NULL, $3);   }
| TOKEN_ENUM '{' enumerator_list ',' '}'                  { $$ = uuc_specifier_enum(NULL, $3);   }
| TOKEN_ENUM TOKEN_IDENTIFIER '{' enumerator_list '}'     { $$ = uuc_specifier_enum(  $2, $4);   }
| TOKEN_ENUM TOKEN_IDENTIFIER '{' enumerator_list ',' '}' { $$ = uuc_specifier_enum(  $2, $4);   }
| TOKEN_ENUM TOKEN_IDENTIFIER                             { $$ = uuc_specifier_enum(  $2, NULL); }
;

enumerator_list
: enumerator                     { $$ = uuc_list_prepend(NULL, $1); }
| enumerator_list ',' enumerator { $$ = uuc_list_prepend(  $1, $3); }
;

enumerator
: TOKEN_IDENTIFIER                         { $$ = uuc_enumerator($1, NULL); }
| TOKEN_IDENTIFIER '=' constant_expression { $$ = uuc_enumerator($1, $3);   }
;

type_qualifier
: TOKEN_CONST    { $$ = uuc_specifier(UUC_SPEC_CONST); }
| TOKEN_VOLATILE { $$ = uuc_specifier(UUC_SPEC_VOLATILE); }
;

/* ************ DECLARATOR ********************* */

declarator
: direct_declarator
| '*' declarator                     { $$ = uuc_declarator_pointer(NULL, $2); }
| '*' type_qualifier_list declarator { $$ = uuc_declarator_pointer(  $2, $3); }
;

direct_declarator
: TOKEN_IDENTIFIER                                              { $$ = uuc_declarator_symbol($1);         }
| '(' declarator ')'                                            { $$ = $2;                                }
| direct_declarator '[' expression ']'                          { $$ = uuc_declarator_array($1,   $3);    }
| direct_declarator '[' ']'                                     { $$ = uuc_declarator_array($1, NULL);    }
| direct_declarator '(' parameter_declaration_list_ellipsis ')' { $$ = uuc_declarator_function($1, $3);   }
| direct_declarator '(' ')'                                     { $$ = uuc_declarator_function($1, NULL); }
;

type_qualifier_list
: type_qualifier { $$ = uuc_list_prepend(NULL, $1); }
| type_qualifier_list type_qualifier { $$ = uuc_list_prepend($1, $2); }
;

parameter_declaration_list_ellipsis
: parameter_declaration_list
| parameter_declaration_list ',' TOKEN_ELLIPSIS { $$ = $1; /* FIXME */ }
;

parameter_declaration_list
: parameter_declaration                                { $$ = uuc_list_prepend(NULL, $1); }
| parameter_declaration_list ',' parameter_declaration { $$ = uuc_list_prepend(  $1, $3); }
;

parameter_declaration
: declaration_specifiers declarator          { $$ = uuc_declarator_resolve_type($2, $1); }
| declaration_specifiers abstract_declarator { $$ = uuc_declarator_resolve_type($2, $1); }
| declaration_specifiers { $$ = uuc_declarator_symbol(NULL); uuc_declarator_resolve_type($$, $1); }
;

type_name
: specifier_qualifier_list                     { $$ = uuc_type_basic($1); }
| specifier_qualifier_list abstract_declarator { uuc_type_resolve($2->type, uuc_type_basic($1)); $$ = $2->type; }
;

abstract_declarator
: direct_abstract_declarator
| '*'                                         { $$ = uuc_declarator_pointer(NULL, NULL); }
| '*' abstract_declarator                     { $$ = uuc_declarator_pointer(NULL,   $2); }
| '*' type_qualifier_list abstract_declarator { $$ = uuc_declarator_pointer(  $2,   $3); }
;

direct_abstract_declarator
: '(' abstract_declarator ')'        { $$ = $2; }
| '[' ']'                            { $$ = uuc_declarator_array(NULL, NULL); }
| '[' constant_expression ']'        { $$ = uuc_declarator_array(NULL, $2); }
| direct_abstract_declarator '[' ']' { $$ = uuc_declarator_array($1, NULL); }
| direct_abstract_declarator '[' constant_expression ']' { $$ = uuc_declarator_array($1, $3); }
| '(' ')'                            { $$ = uuc_declarator_function(NULL, NULL); }
| '(' parameter_declaration_list_ellipsis ')' { $$ = uuc_declarator_function(NULL, $2); }
| direct_abstract_declarator '(' ')' { $$ = uuc_declarator_function($1, NULL); }
| direct_abstract_declarator '(' parameter_declaration_list_ellipsis ')' { $$ = uuc_declarator_function($1, $3); }
;

initializer
: assign_expression
| '{' initializer_list '}'
| '{' initializer_list ',' '}'
| '{' dot_designated_initializer_list '}'
| '{' dot_designated_initializer_list ',' '}'
| '{' array_designated_initializer_list '}'
| '{' array_designated_initializer_list ',' '}'
;

dot_designated_initializer
: '.' TOKEN_IDENTIFIER '=' initializer
;

array_designated_initializer
: '[' constant_expression ']' '=' initializer
;

initializer_list
: initializer
| initializer_list ',' initializer
;

dot_designated_initializer_list
: dot_designated_initializer
| dot_designated_initializer_list ',' dot_designated_initializer
;

array_designated_initializer_list
: array_designated_initializer
| array_designated_initializer_list ',' array_designated_initializer
;

/* ************ STATEMENTS ********************* */

statement
: labeled_statement
| compound_statement
| expression_statement
| selection_statement
| iteration_statement
| jump_statement
;

labeled_statement
: TOKEN_IDENTIFIER ':' statement               { $$ = uuc_statement_labeled($1, $3); }
| TOKEN_CASE constant_expression ':' statement { $$ = uuc_statement_case($2, $4); }
| TOKEN_DEFAULT ':' statement                  { $$ = uuc_statement_labeled("default", $3); }
;

compound_statement
: '{' '}'                { $$ = uuc_statement_compound(NULL, NULL); }
| '{' statement_list '}' { $$ = uuc_statement_compound(NULL, $2); }
| brace_open declaration_list brace_close { $$ = uuc_statement_compound($2, NULL); }
| brace_open declaration_list statement_list brace_close { $$ = uuc_statement_compound($2, $3); }
;

declaration_list
: declaration                  { $$ = uuc_list_prepend(NULL, $1); }
| declaration declaration_list { $$ = uuc_list_prepend(  $2, $1); }
;

statement_list
: statement                { $$ = uuc_list_prepend(NULL, $1); }
| statement statement_list { $$ = uuc_list_prepend($2, $1); }
;

expression_statement
: ';'            { $$ = NULL; /* FIXME */ }
| expression ';' { $$ = uuc_statement_expression($1); }
;

selection_statement
: TOKEN_IF '(' expression ')' statement                      { $$ = uuc_statement_if($3, $5, NULL); }
| TOKEN_IF '(' expression ')' statement TOKEN_ELSE statement { $$ = uuc_statement_if($3, $5, $7); }
| TOKEN_SWITCH '(' expression ')' statement                  { $$ = uuc_statement_switch($3, $5); }
;

iteration_statement
: TOKEN_WHILE '(' expression ')' statement              { $$ = uuc_statement_while($3, $5); }
| TOKEN_DO statement TOKEN_WHILE '(' expression ')' ';' { $$ = uuc_statement_do_while($5, $2); }
| TOKEN_FOR '(' expression_statement expression_statement expression ')' statement
{ $$ = uuc_statement_for($3, $4, $5, $7); }
| TOKEN_FOR '(' expression_statement expression_statement ')' statement
{ $$ = uuc_statement_for($3, $4, NULL, $4); }
;

jump_statement
: TOKEN_GOTO TOKEN_IDENTIFIER ';' { $$ = uuc_statement_goto($2); }
| TOKEN_CONTINUE ';'              { $$ = uuc_statement_continue(); }
| TOKEN_BREAK ';'                 { $$ = uuc_statement_break(); }
| TOKEN_RETURN expression ';'     { $$ = uuc_statement_return($2); }
;

/* ************ SYMBOL TABLE ********************* */

brace_open
: '{' { symbol_table_push(); }
;

brace_close
: '}' { symbol_table_pop(); }
;

/* ************ GLOBAL PROGRAM ********************* */

translation_unit
: program { printf("parsed\n"); }
;

program
: external_declaration
| program external_declaration
;

external_declaration
: function_definition
| declaration
;

function_definition_header
: declaration_specifiers declarator { uuc_function_definition_declare($1, $2); }
;

function_definition
: function_definition_header compound_statement
;

%%
