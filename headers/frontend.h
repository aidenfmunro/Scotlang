#ifndef FRONTEND_H
#define FRONTEND_H

#include "tokenizer.h"
#include "parser.h"

//

struct Frontend
{
    Tokens  tkns;

    Text    frontEndSrc;

    Node*   astNode;
};

//

ErrorCode CreateFrontend  (Frontend* fe, const char* inFileName);

ErrorCode DestroyFrontend (Frontend* fe);

#endif