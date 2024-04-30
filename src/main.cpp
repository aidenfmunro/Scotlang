#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"

int main(void)
{
    Tokens tkns = getTokens("ex.sc");

    PrintTokens(&tkns);

    Node* root  = GetGrammar (&tkns);

    DumpTreeGraph(root);

    free(tkns.token);

    deleteNode(root);

    return 0;
}