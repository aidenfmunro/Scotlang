#include "frontend.h"

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

    size_t curTokenNum;

    size_t size = 0;
};

Tokens readTokens (const char* filename)
{
    AssertSoft(filename, {});

    Text text = {};

    CreateText(&text, filename, NONE);

    size_t curLineNum = 0;

    size_t curLinePos = 1;

    size_t curPos = 0;

    Tokens tokens = {};

    SafeCalloc(temptokens, text.size, Token, {});

    tokens.tokens = temptokens;

    while (text.buffer[curPos] != '\0') // TOKENTYPE
    {
        skipSpaces(text.buffer, &curPos, &curLinePos, &curLineNum);

        tokens.tokens[tokens.curTokenNum] = readToken(text.buffer, &curPos, &curLinePos, &curLineNum);
    }

    tokens.curTokenNum = 0;

    return tokens;
}

static const int MAX_STR_SIZE = 64;

Token readToken (char* buffer, size_t* curPos, size_t* curLinePos, size_t* curLineNum)
{
    AssertSoft(buffer, {});

    Token token = {};

    #define DEF_OP(keyword, name)                             \
                                                              \
    if (strncasecmp(keyword, buffer, sizeof(keyword) - 1))    \
    {                                                         \
        token.data.op = name;                                 \
                                                              \
        token.lineNum = *curLineNum;                          \
                                                              \
        token.linePos = *curLinePos;                          \
                                                              \
        token.length = sizeof(keyword) - 1;                   \
                                                              \
        *curPos     += token.length;                          \
                                                              \
        *curLinePos += token.length;                          \
                                                              \
    }

    #include "defop.h"

    #undef DEF_OP

    char tempstr[MAX_STR_SIZE] = {};

    int charsRead = sscanf(buffer, "%[A-Za-z0-9_]", tempstr); // переменные и числа

    token.data.name = buffer;

    *curPos     += charsRead;

    *curLinePos += charsRead;

    return token;
}

ErrorCode skipSpaces (char* buffer, size_t* curPos, size_t* curLinePos, size_t* curLineNum)
{
    AssertSoft(buffer, NULL_PTR);

    while (*buffer == ' ' || *buffer == '\t' || *buffer == '\n' || *buffer == '\r') // TODO: isspace
    {
        if (*buffer == '\n')
        {
            *curLineNum += 1;
            *curLinePos  = 0;
        }

        *curPos += 1;
    }

    return OK;
}

// ---------------------------------> #define curToken tokens->tokens[tokens->curTokenNum]

bool requireOp (Tokens* tokens, Operator op)
{
    if (tokens->tokens[tokens->curTokenNum].data.op == op)
    {
        return true;
    }

    return false;   
}

// "if"'('P')''{'E'}'

// func main(a, b)
//{
//  var x = 5;
//  
//  if (x < 5)
//  {
//      x = x * 2;
//  }
//
//  return;
//}
//
//
//
//
//
//
//
//
//

// как организовать локальные переменные в функции ???