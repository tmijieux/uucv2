D                       [0-9]
L                       [a-zA-Z_]
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

    #include "token.h"
    #include "error.h"
    #include "symbol_table.h"

    extern void yyerror(const char* val);

    int uuc_parser_nb_row = 1;
    int uuc_parser_nb_col = 0;
    int uuc_last_token_pos;
    char *uuc_last_tokens[20] = {NULL};
    char *uuc_parse_filename = NULL;

    void scanner_init(void)
    {
    }
    void scanner_exit(void)
    {
    }

    #define count()         uuc_parser_nb_col += strlen(yytext)

    #define upd()  do {                                          \
            count();                                                   \
            free(uuc_last_tokens[uuc_last_token_pos]);                  \
            uuc_last_tokens[uuc_last_token_pos] = strdup(yytext);       \
            ++ uuc_last_token_pos;                                      \
            uuc_last_token_pos %= 20;                                   \
        } while(0)

    void parse_cpp_macro(char *str);

%}

%option noyywrap
%x IN_CPP_MACRO IN_COMMENT IN_FILENAME LINE_COMMENT

%%

<INITIAL>{
    "#"             BEGIN(IN_CPP_MACRO);
    "/*"            BEGIN(IN_COMMENT);
    "//"            BEGIN(LINE_COMMENT);

    _Alignas         upd();  return TOKEN_ALIGNAS       ;
    _Thread_local    upd();  return TOKEN_THREAD_LOCAL  ;
    _Atomic          upd();  return TOKEN_ATOMIC        ;
    static           upd();  return TOKEN_STATIC        ;
    register         upd();  return TOKEN_REGISTER      ;
    auto             upd();  return TOKEN_AUTO          ;
    extern           upd();  return TOKEN_EXTERN        ;
    typedef          upd();  return TOKEN_TYPEDEF       ;
    restrict         upd();  return TOKEN_REGISTER      ;
    inline           upd();  return TOKEN_INLINE        ;
    _Noreturn        upd();  return TOKEN_NORETURN      ;
    const            upd();  return TOKEN_CONST         ;
    volatile         upd();  return TOKEN_VOLATILE      ;
    sizeof           upd();  return TOKEN_SIZEOF        ;
    _Alignof         upd();  return TOKEN_ALIGNOF       ;
    struct           upd();  return TOKEN_STRUCT        ;
    union            upd();  return TOKEN_UNION         ;
    enum             upd();  return TOKEN_ENUM          ;
    unsigned         upd();  return TOKEN_UNSIGNED      ;
    signed           upd();  return TOKEN_SIGNED        ;
    char             upd();  return TOKEN_CHAR          ;
    short            upd();  return TOKEN_SHORT         ;
    int              upd();  return TOKEN_INT           ;
    long             upd();  return TOKEN_LONG          ;
    float            upd();  return TOKEN_FLOAT         ;
    double           upd();  return TOKEN_DOUBLE        ;
    void             upd();  return TOKEN_VOID          ;
    _Complex         upd();  return TOKEN_COMPLEX       ;
    if               upd();  return TOKEN_IF            ;
    else             upd();  return TOKEN_ELSE          ;
    while            upd();  return TOKEN_WHILE         ;
    do               upd();  return TOKEN_DO            ;
    for              upd();  return TOKEN_FOR           ;
    continue         upd();  return TOKEN_CONTINUE      ;
    switch           upd();  return TOKEN_SWITCH        ;
    case             upd();  return TOKEN_CASE          ;
    default          upd();  return TOKEN_DEFAULT       ;
    break            upd();  return TOKEN_BREAK         ;
    goto             upd();  return TOKEN_GOTO          ;
    return           upd();  return TOKEN_RETURN        ;
    _Static_assert   upd();  return TOKEN_STATIC_ASSERT ;
    _Generic         upd();  return TOKEN_GENERIC       ;

    {L}({L}|{D})*            upd(); return TOKEN_IDENTIFIER;
    0[xX]{H}+{IS}?           upd(); return TOKEN_INT_LITERAL; /*base 16 int*/
    0{D}+{IS}?               upd(); return TOKEN_INT_LITERAL; /*base 8 int*/
    {D}+{IS}?                upd(); return TOKEN_INT_LITERAL; /* int */
    L?'(\\.|[^\\'])+'        upd(); return TOKEN_CHAR_LITERAL; /* char constant */
    {D}+{E}{FS}?             upd(); return TOKEN_FLOAT_LITERAL; /* floating point */
    {D}*"."{D}+({E})?{FS}?   upd(); return TOKEN_FLOAT_LITERAL; /* floating point dot after */
    {D}+"."{D}*({E})?{FS}?   upd(); return TOKEN_FLOAT_LITERAL; /* floating point dot before */
    L?\"(\\.|[^\\"])*\"      upd(); return TOKEN_STRING_LITERAL; /* string literal */

    "<<="          upd();  return TOKEN_ASSIGN_SHIFT_LEFT;
    ">>="          upd();  return TOKEN_ASSIGN_SHIFT_RIGHT;
    "..."          upd();  return TOKEN_ELLIPSIS;

    "->"           upd();  return TOKEN_ARROW;
    "&&"           upd();  return TOKEN_AND;
    "||"           upd();  return TOKEN_OR;
    ">="           upd();  return TOKEN_GREATER_EQUAL;
    "<="           upd();  return TOKEN_LOWER_EQUAL;
    "=="           upd();  return TOKEN_EQUAL;
    "!="           upd();  return TOKEN_NOT_EQUAL;
    "<<"           upd();  return TOKEN_SHIFT_LEFT;
    ">>"           upd();  return TOKEN_SHIFT_RIGHT;
    "+="           upd();  return TOKEN_ASSIGN_ADD;
    "-="           upd();  return TOKEN_ASSIGN_SUB;
    "*="           upd();  return TOKEN_ASSIGN_MUL;
    "/="           upd();  return TOKEN_ASSIGN_DIV;
    "%="           upd();  return TOKEN_ASSIGN_MOD;
    "&="           upd();  return TOKEN_ASSIGN_BITAND;
    "|="           upd();  return TOKEN_ASSIGN_BITOR;
    "^="           upd();  return TOKEN_ASSIGN_BITXOR;
    "++"           upd();  return TOKEN_INCREMENT;
    "--"           upd();  return TOKEN_DECREMENT;

    "<%"           upd();  return '{';
    "%>"           upd();  return '}';
    "<:"           upd();  return '[';
    ":>"           upd();  return ']';

    "("            upd();  return '(';
    ")"            upd();  return ')';
    "{"            upd();  return '{';
    "}"            upd();  return '}';
    "["            upd();  return '[';
    "]"            upd();  return ']';
    ","            upd();  return ',';
    ";"            upd();  return ';';
    "="            upd();  return '=';
    "+"            upd();  return '+';
    "-"            upd();  return '-';
    "*"            upd();  return '*';
    "/"            upd();  return '/';
    "%"            upd();  return '%';
    "<"            upd();  return '<';
    ">"            upd();  return '>';
    "."            upd();  return '.';
    "?"            upd();  return '?';
    ":"            upd();  return ':';
    "!"            upd();  return '!';
    "&"            upd();  return '&';
    "|"            upd();  return '|';
    "^"            upd();  return '^';
    "~"            upd();  return '~';

    [ \t\v\f]+     /* eat up whitespace */ upd();
    \n             upd(); ++uuc_parser_nb_row; uuc_parser_nb_col = 0;
    .              uuc_error("unexpected token");
}

<IN_COMMENT>{
    "*/"      BEGIN(INITIAL);
    [^*\n]+   // eat comment in chunks
    "*"       // eat the lone star
    \n        ++uuc_parser_nb_row; uuc_parser_nb_col=0;
}

<LINE_COMMENT>{
    [^\n]+      // eat in chunks
     \n        ++uuc_parser_nb_row; uuc_parser_nb_col = 0; BEGIN(INITIAL);
}

<IN_CPP_MACRO>{
    \n        BEGIN(INITIAL);
    /*  "\""  BEGIN(IN_FILENAME); */

    [^\n]+    parse_cpp_macro(yytext);
}

<IN_FILENAME>{
    "\""      BEGIN(LINE_COMMENT);
    [^\"]+    free(uuc_parse_filename); uuc_parse_filename = strdup(yytext);
}

%%


void parse_cpp_macro(char *str)
{
     char *quote_ptr;

    quote_ptr = strchr(str, '"');
    if (quote_ptr == NULL) {
         uuc_error("cannot parse unpreprocessed content");
         return;
    }
    *quote_ptr = '\0';

    char *quote_end_ptr;
    quote_end_ptr = strchr(quote_ptr+1, '"');
    if (quote_end_ptr == NULL) {
         uuc_error("cannot parse unpreprocessed content");
         return;
    }
    *quote_end_ptr = '\0';

    free(uuc_parse_filename);
    uuc_parse_filename = strdup(quote_ptr+1);

    int nb_row = 0;
    if ( sscanf(yytext, " %d ", &nb_row) != 1) {
         uuc_error("FATAL: cannot parse unpreprocessed content");
         exit(EXIT_FAILURE);
         return;
    }
    uuc_parser_nb_row = nb_row-1;
    uuc_parser_nb_col = 0;

    // fprintf(stderr, "XXL = '%d'\n", uuc_parser_nb_row);
    // fprintf(stderr, "XX = '%s'\n", uuc_parse_filename);
}
