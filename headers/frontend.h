#ifndef FRONTEND_H
#define FRONTEND_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "tree.h"
#include "textfuncs.h"
#include "string.h"
#include "utils.h"

struct Token
{
    NodeElem_t data;

    Type       type;

    size_t     linePos;

    size_t     lineNum;

    size_t     length;
};

struct Tokens
{
    Token* tokens;

    size_t curTokenNum = 0;

    size_t size        = 0;
};

Tokens      readTokens (const char* filename);

Token       readToken  (char* buffer, size_t* curPos, size_t* curLinePos, size_t* curLineNum);

ErrorCode   skipSpaces (char* buffer, size_t* curPos, size_t* curLinePos, size_t* curLineNum);

bool        requireOp  (Tokens* tokens, Operator op);

size_t      countChars (char* buffer, size_t* curPos);

#endif