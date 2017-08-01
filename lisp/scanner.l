D                       [0-9]
L                       [-a-zA-Z_+/*]
H                       [a-fA-F0-9]
E                       [Ee][+-]?{D}+
FS                      (f|F|l|L)
IS                      (u|U|l|L)*


%{
    #include <stdio.h>
    #include <errno.h>
    #include <stdlib.h>
    #include <string.h>
    #include <assert.h>

    #include "grammar.h"
    #include "error.h"
    #include "symbol_table.h"

    extern void yyerror(const char* val);

    int enum_seen = 0;

    int uuc_parser_nb_row = 1;
    int uuc_parser_nb_col = 0;
    int uuc_last_token_pos;
    char *uuc_last_tokens[20] = {NULL};
    char *uuc_parse_filename = NULL;

    void scanner_init(void) {}
    void scanner_exit(void) {}

    #define count()         uuc_parser_nb_col += strlen(yytext)

    #define upd()  do {                                          \
            count();                                                   \
            free(uuc_last_tokens[uuc_last_token_pos]);                  \
            uuc_last_tokens[uuc_last_token_pos] = strdup(yytext);       \
            ++ uuc_last_token_pos;                                      \
            uuc_last_token_pos %= 20;                                   \
        } while(0)

%}

%option noyywrap
%x LINE_COMMENT

%%

<INITIAL>{
    ";"            BEGIN(LINE_COMMENT);
    nil              return TOKEN_NIL;
    if               upd();  return TOKEN_IF            ;
    while            upd();  return TOKEN_WHILE         ;

    {L}({L}|{D})*            upd(); yylval.string_value = strdup(yytext); return TOKEN_IDENTIFIER;
    0[xX]{H}+{IS}?           upd(); yylval.int_value = strtol(yytext, NULL, 0); return TOKEN_INT_LITERAL; /*base 16 int*/
    0{D}+{IS}?               upd(); yylval.int_value = strtol(yytext, NULL, 0); return TOKEN_INT_LITERAL; /*base 8 int*/
    {D}+{IS}?                upd(); yylval.int_value = strtol(yytext, NULL, 0); return TOKEN_INT_LITERAL; /* int */
    {D}+{E}{FS}?             upd(); yylval.float_value = strtod(yytext, NULL); return TOKEN_FLOAT_LITERAL; /* floating point */
    {D}*"."{D}+({E})?{FS}?   upd(); yylval.float_value = strtod(yytext, NULL); return TOKEN_FLOAT_LITERAL; /* floating point dot after */
    {D}+"."{D}*({E})?{FS}?   upd(); yylval.float_value = strtod(yytext, NULL); return TOKEN_FLOAT_LITERAL; /* floating point dot before */
    L?\"(\\.|[^\\"])*\"      upd(); yylval.string_value = strdup(yytext); return TOKEN_STRING_LITERAL; /* string literal */


    "("            upd();  return '(';
    ")"            upd();  return ')';
    "'"            upd();  return '\'';
    "."            upd();  return '.';
    ","            upd();  return ',';
    "`"            upd();  return '`';

    [ \t\v\f]+     /* eat up whitespace */ upd();
    \n             upd(); ++uuc_parser_nb_row; uuc_parser_nb_col = 0;
    .              uuc_fatal_error("unexpected token '%s'", yytext);
}

<LINE_COMMENT>{
    [^\n]+      // eat in chunks
     \n        ++uuc_parser_nb_row; uuc_parser_nb_col = 0; BEGIN(INITIAL);
}

%%
