#ifndef UUC_PARSER_H
#define UUC_PARSER_H

#include <stdbool.h>

struct uuc_parser {
    bool finished;
};
typedef struct uuc_parser uuc_parser_t;


void uuc_parser_init(uuc_parser_t *parser);
void uuc_parser_free(uuc_parser_t *parser);
void uuc_parser_parse(uuc_parser_t *parser);

#endif // UUC_PARSER_H
