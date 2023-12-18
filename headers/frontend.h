#ifndef FRONTEND_H
#define FRONTEND_H

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "textfuncs.h"
#include "string.h"
#include "utils.h"

Tokens readTokens (const char* filename);

Token readToken (char* buffer, size_t* curPos, size_t* curLinePos, size_t* curLineNum);

ErrorCode skipSpaces (char* buffer, size_t* curPos, size_t* curLinePos, size_t* curLineNum);

bool requireOp (Tokens* tokens, Operator op);

#endif