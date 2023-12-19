#include "frontend.h"

Tokens readTokens (const char* filename)
{
    AssertSoft(filename, {});

    Text text = {};

    CreateText(&text, filename, NONE);
    printf("%d", text.size);

    size_t curLineNum = 0;

    size_t curLinePos = 1;

    size_t curPos = 0;

    Tokens tokens = {};

    SafeCalloc(temptokens, text.size, Token, {});

    tokens.tokens = temptokens;

    skipSpaces(text.buffer, &curPos, &curLinePos, &curLineNum);

    while (text.buffer[curPos] != '\0') // TOKENTYPE
    {
        printf("%d\n", curPos);

        tokens.tokens[tokens.curTokenNum] = readToken(text.buffer, &curPos, &curLinePos, &curLineNum);

        printf("token [%d](%d): %s in line: %zu, pos: %zu\n", tokens.curTokenNum, tokens.tokens[tokens.curTokenNum].data.op,
                                    tokens.tokens[tokens.curTokenNum].lineNum,
                                    tokens.tokens[tokens.curTokenNum].linePos);

        printf("%d\n", curPos);

        tokens.size        += 1;

        tokens.curTokenNum += 1;

        skipSpaces(text.buffer, &curPos, &curLinePos, &curLineNum);
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
        return token;                                         \
    }

    #include "defop.h"

    #undef DEF_OP

    token.length = countChars(buffer, curPos);

    printf("sdsd %zu", countChars(buffer, curPos));

    token.data.name = buffer + *curPos;

    *curPos     += token.length;

    *curLinePos += token.length;

    return token;
}

size_t countChars (char* buffer, size_t* curPos)
{
    size_t charsRead = 0;

    while (isalpha(*(buffer + charsRead + *curPos)) || *(buffer + charsRead + *curPos) == '?')
    {
        charsRead += 1;
        printf("%d\n", charsRead);
    }

    printf("%d\n", charsRead);

    return charsRead;
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