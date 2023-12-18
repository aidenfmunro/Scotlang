#include "frontend.h"

struct Token
{
    NodeElem_t data;

    Type       type;

    size_t     linePos;

    size_t     lineNum;
};

struct Tokens
{
    Token* tokens;

    size_t size;
};

ErrorCode getTokens (const char* filename)
{
    Text text = {};

    CreateText(&text, filename, NONE);

    
}

bool requireOp (Operator op)
{
    ;
}