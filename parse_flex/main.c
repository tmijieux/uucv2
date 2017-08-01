#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "parser.h"
#include "scanner.h"
#include "symbol_table.h"

extern void scanner_init(void);
extern void scanner_exit(void);

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;


    uuc_parser_t parser;
    uuc_parser_init(&parser);

    extern char *uuc_parse_filename;
    uuc_parse_filename = strdup("<stdin>");

    scanner_init();

    symbol_table_init();
    uuc_parser_parse(&parser);
    symbol_table_exit();

    scanner_exit();
    uuc_parser_free(&parser);

    return 0;
}
