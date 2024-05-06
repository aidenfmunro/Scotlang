#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"

int main(void)
{
    Tokens tkns = getTokens("ex.sc");

    PrintTokens(&tkns);

    Tree tree = {};

    tree.root  = GetGrammar (&tkns);

    DumpTreeGraph(tree.root);

    free(tkns.token);

    deleteNode(tree.root);

    return 0;
}