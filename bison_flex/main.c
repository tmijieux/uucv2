#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "grammar.h"
#include "scanner.h"
#include "symbol_table.h"

extern void scanner_init(void);
extern void scanner_exit(void);

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    extern char *uuc_parse_filename;
    uuc_parse_filename = strdup("<stdin>");

    scanner_init();

    symbol_table_init();
    yyparse();
    symbol_table_exit();

    scanner_exit();

    return 0;
}
