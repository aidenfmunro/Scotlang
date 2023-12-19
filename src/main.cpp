#include <stdio.h>
#include "frontend.h"


int main(void)
{
    getTokens("ex.sc");

    PrintTokens();

    Node* root = GetG();

    DumpTreeGraph(root);
}