#include <stdio.h>
#include "frontend.h"

int main(void)
{
    Tokens tkns = getTokens("ex.sc");

    // PrintTokens(&tkns);

    printf ("FUNC  = %d\n", tkns.token[0].elem->data.op);
}