#include "tokenizer.h"

Tokens getTokens (const char* fileIn)
{
    AssertSoft(fileIn, {});

    Text src = {};

    CreateText (&src, fileIn);

    SafeCalloc(tkn, src.size, Token, {});

    Tokens tkns = {.token = tkn};

    printf(""BOLD"->"COLOR_RESET" File size: "RED"%d\n"COLOR_RESET"", src.size);

    while (src.buffer)
    {
        skipSymbols (&tkns, src.buffer);

        if (*src.buffer == '\0')
            break;

        if ( ! getToken (&tkns, src.buffer) )
            break;
    }

    printf(""BOLD"->"COLOR_RESET" Amount of tokens eatten: %d!\n", tkns.curTokenNum);

    tkns.tokenCount = tkns.curTokenNum;

    tkns.curTokenNum = 0;

    return tkns; 
}

#define curSym   *(buffer + tkns->curSymPos)
#define curPtr    (buffer + tkns->curSymPos)
#define curToken  tkns->token[tkns->curTokenNum]

bool skipSymbols (Tokens* tkns, char* buffer)
{
    AssertSoft(tkns,    false);
    AssertSoft(buffer,  false);

    if (curSym == '\0')
    {
        return false;
    }

    while (isspace (curSym) || curSym == '\t' || curSym == '\n' || curSym == '\r')
    {
        if (curSym == '\n')
        {
            tkns->curLineNum++;

            tkns->curLinePos = 0;
        }
        else
        {
            tkns->curLinePos++;    
        }

        tkns->curSymPos++;
    }

    if (curSym == '\0')
    {
        return false;
    }

    return true;
}

bool getToken (Tokens* tkns, char* buffer)
{
    AssertSoft(tkns,   NULL_PTR);
    AssertSoft(buffer, NULL_PTR);

    if (! skipSymbols (tkns, buffer))
        return false;

    if (getKeywordToken (tkns, buffer))
        return true;
    
    if (getConstToken   (tkns, buffer))
        return true;
    
    if (getNameToken    (tkns, buffer))
        return true;

    return false;
}

bool getKeywordToken (Tokens* tkns, char* buffer)
{
    AssertSoft(tkns,   NULL_PTR);
    AssertSoft(buffer, NULL_PTR);

    #define DEF_KEYWORD(keyword, name)                              \
                                                                    \
    if (strncmp (keyword, curPtr, sizeof(keyword) - 1) == 0)        \
    {                                                               \
        tkns->curSymPos  += sizeof(keyword) - 1;                    \
        tkns->curLinePos += sizeof(keyword) - 1;                    \
                                                                    \
        createKeywordToken (tkns, name);                            \
                                                                    \
        return true;                                                \
    }

    #include "keywords.def"

    #undef DEF_KEYWORD

    return false;
}

bool getConstToken (Tokens* tkns, char* buffer)
{
    AssertSoft(tkns,   NULL_PTR);
    AssertSoft(buffer, NULL_PTR);

    if (isdigit (curSym))
    {
        char* endPtr = NULL;

        double value = strtod (curPtr, &endPtr);

        tkns->curSymPos  += endPtr - curPtr;

        tkns->curLinePos += endPtr - curPtr;

        createConstToken (tkns, value);

        return true;
    }

    return false;
}

bool getNameToken (Tokens* tkns, char* buffer)
{
    AssertSoft(tkns,   NULL_PTR);
    AssertSoft(buffer, NULL_PTR);

    int charsRead = 0;

    char* startPtr = curPtr;

    while ('a' <= curSym && curSym <= 'z' ||
           'A' <= curSym && curSym <= 'Z' )
    {
        tkns->curLinePos ++;

        tkns->curSymPos  ++;

        charsRead        ++;        
    } 

    if (charsRead > MAX_NAME_LENGTH)
    {
        return false;

        printf("(%s) the length of the word is too long!", startPtr);
    }

    curToken.length = charsRead;

    if (tkns->token[tkns->curTokenNum - 1].elem->data.op == FUNCDEC) // ???
    {
        createFuncToken (tkns, startPtr);

        printf(""BOLD"->"COLOR_RESET" Name: %.*s\n", charsRead, startPtr);

        return true;
    }

    createVarToken (tkns, startPtr);

    printf(""BOLD"->"COLOR_RESET" Name: %.*s\n", charsRead, startPtr);

    return true;
}

#undef curSym   
#undef curPtr    

ErrorCode updateToken (Tokens* tkns, Node* node)
{
    AssertSoft(tkns, NULL_PTR);
    AssertSoft(node, NULL_PTR);

    curToken.elem    = node;

    curToken.linePos = tkns->curLinePos;

    curToken.lineNum = tkns->curLineNum;

    tkns->curTokenNum++;

    printf (""BOLD"->"COLOR_RESET" "
            ""BOLD"Updated token"COLOR_RESET" "
            "%d! in line: "BOLD"%d"COLOR_RESET","
            " pos: "BOLD"%d"COLOR_RESET"\n", tkns->curTokenNum, tkns->curLineNum, tkns->curLinePos);

    return OK;
}

#undef curToken

ErrorCode createKeywordToken (Tokens* tkns, Keyword op)
{
    AssertSoft(tkns, NULL_PTR);

    updateToken (tkns, createKeywordNode (op));

    tkns->curTokenStatus = OP;

    return OK;
}

ErrorCode createConstToken (Tokens* tkns, double value)
{
    AssertSoft(tkns,    NULL_PTR);

    updateToken (tkns, createConstNode (value));

    tkns->curTokenStatus = CONST;

    return OK; 
}

ErrorCode createFuncToken (Tokens* tkns, char* funcName)
{
    AssertSoft(tkns,     NULL_PTR);
    AssertSoft(funcName, NULL_PTR);

    updateToken (tkns, createFuncNode (funcName));

    tkns->curTokenStatus = FUNC;

    return OK;
}

ErrorCode createVarToken (Tokens* tkns, char* varName)
{
    AssertSoft(tkns,    NULL_PTR);
    AssertSoft(varName, NULL_PTR);

    updateToken (tkns, createVarNode (varName));

    tkns->curTokenStatus = VAR;

    return OK;
}

ErrorCode PrintTokens (Tokens* tkns) 
{
    AssertSoft(tkns, NULL_PTR);

    for (size_t i = 0; i < tkns->tokenCount; i++) 
    {
        if (tkns->token[i].elem->type == OP)
            printf ("OP    = %s\n",   getKeywordName(tkns->token[i].elem->data.op));

        if (tkns->token[i].elem->type == FUNC)
            printf ("FUNC  = %.*s\n", tkns->token[i].length, tkns->token[i].elem->data.name);

        if (tkns->token[i].elem->type == VAR)
            printf ("VAR   = %.*s\n", tkns->token[i].length, tkns->token[i].elem->data.name);

        if (tkns->token[i].elem->type == CONST)
            printf ("CONST = %lg\n",  tkns->token[i].elem->data.constVal);
    }

    return OK;
}

ErrorCode PrintToken (Token* tkn) 
{
    AssertSoft(tkn, NULL_PTR);

    if (tkn->elem->type == OP)
        printf (""BOLD"OP:"COLOR_RESET" %s\n",   getKeywordName(tkn->elem->data.op));

    if (tkn->elem->type == FUNC)
        printf (""BOLD"FUNC:"COLOR_RESET" %.*s\n", tkn->length, tkn->elem->data.name);

    if (tkn->elem->type == VAR)
        printf (""BOLD"VAR:"COLOR_RESET" %.*s\n", tkn->length, tkn->elem->data.name);

    if (tkn->elem->type == CONST)
        printf (""BOLD"CONST:"COLOR_RESET" %lg\n",  tkn->elem->data.constVal);

    return OK;
}



