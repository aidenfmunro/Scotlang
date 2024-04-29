#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"

int main(void)
{
    Tokens tkns = getTokens("ex.sc");

    PrintTokens(&tkns);
    
    Node* root  = GetGrammar (&tkns);

    DumpTreeGraph(root);


    return 0;
}