#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"
#include "backend.h"

int main (int argc, char* argv[])
{
    Text src = {};

    CreateText(&src, "factorial.sc");

    Tokens tkns = getTokens(&src);

    PrintTokens(&tkns);

    Tree tree = {};

    tree.root  = GetGrammar (&tkns);

    DumpTreeGraph(tree.root);


        FILE * fileOut = fopen("langAsm.txt", "w");

    Backend be = {};

    be.outFile = fileOut;

    BackendCreate (&be);

    Assemble (&be, tree.root);

        fclose (fileOut);

    BackendDestroy (&be);


    free(tkns.token);

    deleteNode(tree.root);

    DestroyText(&src);

    return 0;
}