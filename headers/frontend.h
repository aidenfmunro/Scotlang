#ifndef FRONTEND_H
#define FRONTEND_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "tree.h"
#include "textfuncs.h"
#include "string.h"
#include "utils.h"

struct Token
{
    Node*      elem;

    size_t     linePos;

    size_t     lineNum;

    size_t     length;
};

struct Tokens
{
    Token* token;

    size_t curTokenNum  = 0;

    size_t curLinePos   = 0;

    size_t curLineNum   = 0;

    size_t curSymPos    = 0;

    Type curTokenStatus = NONE;
};

Tokens getTokens             (const char* fileIn);

bool skipSymbols             (Tokens* tkns, char* buffer);

bool getToken                (Tokens* tkns, char* buffer);

bool getKeywordToken         (Tokens* tkns, char* buffer);

bool getConstToken           (Tokens* tkns, char* buffer);

bool getNameToken            (Tokens* tkns, char* buffer);

ErrorCode createKeywordToken (Tokens* tkns, Keyword op);

ErrorCode createConstToken   (Tokens* tkns, double value);

ErrorCode createFuncToken    (Tokens* tkns, char* funcName);

ErrorCode createVarToken     (Tokens* tkns, char* varName);

ErrorCode updateToken        (Tokens* tkns, Node* node);

ErrorCode PrintTokens        (Tokens* tkns); 

Node* GetG();

Node* GetE();

Node* GetIfWhile();

Node* GetT();

Node* GetOp();

Node* GetA();

Node* GetName();

Node* GetP();




#endif