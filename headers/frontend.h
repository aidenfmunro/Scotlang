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


ErrorCode getTokens(const char* fileIn);

size_t skipSymbols();

bool getToken();

bool getOpToken();

bool getConstToken();

bool getNameToken();

Node* createOpToken(Operator op);

Node* createConstToken(double value);

Node* createFuncToken(char* funcName);

Node* createVarToken(char* varName);

void PrintTokens ();

Node* GetG();

Node* GetE();

Node* GetIfWhile();

Node* GetT();

Node* GetOp();

Node* GetA();

Node* GetName();

Node* GetP();




#endif