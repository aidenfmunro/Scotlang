#include <stdio.h>
#include "frontend.h"

int main(void)
{
    Tokens tkns = getTokens("ex.sc");

    PrintTokens(&tkns);
}