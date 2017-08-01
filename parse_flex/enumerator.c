
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "enumerator.h"

struct uuc_enumerator {
    char *identifier;
    uuc_expression_t *value;
};

uuc_enumerator_t *uuc_enumerator(const char *identifier, uuc_expression_t *value)
{
    uuc_enumerator_t *enumerator = calloc(1, sizeof*enumerator);
    enumerator->identifier = strdup(identifier);
    enumerator->value = value;
    return enumerator;
}
