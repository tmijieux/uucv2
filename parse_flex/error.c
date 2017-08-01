#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "error.h"

extern int uuc_parser_nb_row;
extern int uuc_parser_nb_col;

extern int uuc_last_token_pos;
extern char *uuc_last_tokens[20];
extern char *uuc_parse_filename;

extern const char *yytext;


// \033[37;01m  WHITE
// \033[36;01m  NOTE
// \033[35;01m  WARNING
// \033[34;01m  blue
// \033[33;01m  yellow
// \033[32;01m  green
// \033[31;01m  ERROR

void uuc_verror(const char *msg, va_list ap)
{
    fprintf(stderr, "\033[37;01m%s:%d:%d:\033[0m \033[31;01merror: \033[0m",
            uuc_parse_filename, uuc_parser_nb_row, uuc_parser_nb_col);
    vfprintf(stderr, msg, ap);

    fprintf(stderr, " near\n");

    int i = uuc_last_token_pos%20;
    const int k =  (uuc_last_token_pos-1) < 0 ? 19 : (uuc_last_token_pos-1);

    for ( ;; ++i) {
        if (i == 20) {
            i = -1;
            continue;
        }
        if ( uuc_last_tokens[i] != NULL)
            fprintf(stderr, "%s", uuc_last_tokens[i]);
        if (i == k) {
            break;
        }
    }
    fprintf(stderr, "\n%*s\033[32;01m~~\033[31;01m^\033[32;01m~~\033[0m\n\n", uuc_parser_nb_col-3, "");

    va_end(ap);
}

void uuc_error(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    uuc_verror(msg, ap);
    va_end(ap);
}

void uuc_fatal_error(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    uuc_verror(msg, ap);
    va_end(ap);

    abort();
    exit(EXIT_FAILURE);
}
