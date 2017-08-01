#ifndef UUC_ENUMERATOR_H
#define UUC_ENUMERATOR_H

struct uuc_enumerator;
typedef struct uuc_enumerator uuc_enumerator_t;

#include "expression.h"

uuc_enumerator_t *uuc_enumerator(const char *identifier, uuc_expression_t *value);

#endif // UUC_ENUMERATOR_H
