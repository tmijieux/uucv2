#include <stdio.h>
#include <stdlib.h>

int main()
{
    long long *some_variable;
    printf("sizeof(*some_variable)=%zu\n", sizeof*some_variable);

    {
        char *some_variable = ( printf("sizeof(*some_variable)=%zu\n", sizeof*some_variable),
                                calloc(1, sizeof*some_variable)  );
    }
    return 0;
}
