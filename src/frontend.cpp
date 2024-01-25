#include "frontend.h"

Tokens getTokens (const char* fileIn)
{
    Text src = {};

    CreateText (&src, fileIn);

    SafeCalloc(tkn, src.size, Token, {});

    Tokens tkns = {.token = tkn};

    printf("file size: %d\n", src.size);

    while (src.buffer)
    {
        skipSymbols (&tkns, src.buffer);

        if (*src.buffer == '\0')
            break;

        if ( ! getToken (&tkns, src.buffer) )
            break;
    }

    printf("Amount of tokens eatten: %d!\n", tkns.curTokenNum);

    return tkns; 
}

#define curSym   *(buffer + tkns->curSymPos)
#define curPtr    (buffer + tkns->curSymPos)
#define curToken  tkns->token[tkns->curTokenNum]

ErrorCode skipSymbols (Tokens* tkns, char* buffer)
{
    AssertSoft(tkns,    NULL);
    AssertSoft(buffer,  NULL);

    size_t charsSkipped = 0;

    if (*buffer == '\0')
    {
        return 0;
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

    return OK;
}

bool getToken (Tokens* tkns, char* buffer)
{
    skipSymbols (tkns, buffer);

    if (getKeywordToken (tkns, buffer))
        return true;
    
    if (getConstToken (tkns, buffer))
        return true;
    
    if (getNameToken (tkns, buffer))
        return true;

    return false;
}

bool getKeywordToken (Tokens* tkns, char* buffer)
{
    if (curSym == '\0')
        return false;

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

const int MAX_IDENTIFIER_NAME = 32;

bool getNameToken (Tokens* tkns, char* buffer)
{
    SafeCalloc(name, MAX_IDENTIFIER_NAME, char, false);

    sscanf (curPtr, "%[a-zA-Z_0-9]*", name);

    const char* endPtr = strchr (curPtr, ' ');

    int charsRead = endPtr - curPtr;

    if (charsRead < 0 || charsRead > MAX_IDENTIFIER_NAME)
        return false;

    printf ("-> %d\n", charsRead);

    tkns->curLinePos += charsRead;

    tkns->curSymPos  += charsRead; 

    if (tkns->token[tkns->curTokenNum - 1].elem->data.op == FUNCDEC) // ???
    {
        createFuncToken (tkns, name);

        printf("name: %s\n", name);

        return true;
    }

    createVarToken (tkns, name);

    printf("name: %s\n", name);

    return true;
}

ErrorCode updateToken (Tokens* tkns, Node* node)
{
    AssertSoft(tkns, NULL_PTR);

    curToken.elem    = node;

    curToken.linePos = tkns->curLinePos;

    curToken.lineNum = tkns->curLineNum;

    tkns->curTokenNum++;

    printf ("updated token %d! in line: %d, pos: %d\n", tkns->curTokenNum, tkns->curLineNum, tkns->curLinePos);

    return OK;
}

ErrorCode createKeywordToken (Tokens* tkns, Keyword op)
{
    AssertSoft(tkns, NULL_PTR);

    updateToken (tkns, createKeywordNode (op));

    tkns->curTokenStatus = OP;

    return OK;
}

ErrorCode createConstToken (Tokens* tkns, double value)
{
    AssertSoft(tkns, NULL_PTR);

    updateToken (tkns, createConstNode (value));

    tkns->curTokenStatus = CONST;

    return OK; 
}

ErrorCode createFuncToken (Tokens* tkns, char* funcName)
{
    AssertSoft(tkns, NULL_PTR);

    updateToken (tkns, createFuncNode (funcName));

    tkns->curTokenStatus = FUNC;

    return OK;
}

ErrorCode createVarToken (Tokens* tkns, char* varName)
{
    AssertSoft(tkns, NULL_PTR);

    updateToken (tkns, createVarNode (varName));

    tkns->curTokenStatus = VAR;

    return OK;
}

ErrorCode PrintTokens (Tokens* tkns) 
{
    for (size_t i = 0; i < tkns->curTokenNum; i++) 
    {
        if (tkns->token[i].elem->type == OP)
            printf ("OP    = %d\n", tkns->token[i].elem->data.op);

        if (tkns->token[i].elem->type == FUNC)
            printf ("FUNC  = %s\n", tkns->token[i].elem->data.name);

        if (tkns->token[i].elem->type == VAR)
            printf ("VAR   = %s\n", tkns->token[i].elem->data.name);

        if (tkns->token[i].elem->type == CONST)
            printf ("CONST = %lg\n", tkns->token[i].elem->data.constVal);
    }

    return OK;
}

/*

#define CHECKOPTOKEN(oper) (tokens[curPos]->data.op == oper)

size_t curPos = 0;

Node* GetG()
{
    Node* root = GetOp();
    
    return root;
}

Node* GetE()
{
    Node* firstNode = GetT();

    while (tokens[curPos]->data.op >= EQ && tokens[curPos]->data.op <= SUB)
    {
        Operator op = tokens[curPos]->data.op;

        free(tokens[curPos++]);

        Node* secondNode = GetT();

        firstNode = createNode({.op = op}, OP, firstNode, secondNode);
    }

    return firstNode;
}

Node* GetIfWhile()
{
    if (CHECKOPTOKEN(IF) || CHECKOPTOKEN(WHILE))
    {
        Node* opNode = tokens[curPos++];

        if (CHECKOPTOKEN(OPENRB))
        {
            free(tokens[curPos++]);

            Node* firstNode = GetE();

            opNode->left = firstNode;

            if (CHECKOPTOKEN(CLOSERB))
            {
                // printf("IM HERE!!!\n");

                free(tokens[curPos++]);

                opNode->right = GetOp();

                return opNode;
            }
        }
    }

    return NULL;
}

Node* GetT()
{
    Node* firstNode = GetP();

    while (tokens[curPos]->data.op >= MUL && tokens[curPos]->data.constVal <= POW)
    {
        Operator op = tokens[curPos]->data.op;

        free(tokens[curPos++]);

        Node* secondNode = GetP();

        firstNode = createNode({.op = op}, OP, firstNode, secondNode);
    }

    return firstNode;
}

Node* GetOp()
{
    if (tokens[curPos]->type == OP && (CHECKOPTOKEN(IF) || CHECKOPTOKEN(WHILE)))
    {
        return GetIfWhile();
    }
    else if (tokens[curPos]->type == VAR)
    {
        // printf("bebra\n");        

        return GetA();
    }
    else if (CHECKOPTOKEN(OPENSB))
    {
        free(tokens[curPos++]);

        Node* firstNode = GetOp();

        // printf("node: %p\n", firstNode);

        Node* secondNode = {};

        Node* root = firstNode;

        while (!CHECKOPTOKEN(CLOSESB))
        {
            secondNode = GetOp();

            firstNode->right = secondNode;

            firstNode = secondNode;
        }

        free(tokens[curPos++]);

        return root;
    }
    else
    {
        return NULL;
    }
}

Node* GetA()
{
    if (tokens[curPos]->type == VAR)
    {
        Node* varNode = tokens[curPos++];

        if (CHECKOPTOKEN(ASSIGN))
        {
            // printf("in assign if\n");

            Node* assignNode = tokens[curPos++];
            
            Node* valueNode  = GetE();

            connectNode(assignNode, varNode, valueNode);

            if (CHECKOPTOKEN(ENDLINE))
            {
                // printf("in end if\n");

                Node* endNode = tokens[curPos++];
    
                return connectNode(endNode, assignNode, NULL);
            }
        }
    }

    return NULL;
}

Node* GetName()
{
    if (tokens[curPos]->data.op >= SIN)
    {
        Operator op = tokens[curPos++]->data.op; // curPos++

        if (tokens[curPos]->data.op == OPENRB)
        {
            free(tokens[curPos++]);

            Node* firstNode = GetE();

            firstNode = createNode({.op = op}, OP, NULL, firstNode);

            if (tokens[curPos]->data.op == CLOSERB)
            {
                free(tokens[curPos++]);

                return firstNode;
            }
        }
    }
    
    return NULL;
}

Node* GetP()
{
    if (tokens[curPos]->type == OP && tokens[curPos]->data.op == OPENRB)
    {
        free(tokens[curPos++]);

        Node* firstNode = GetE();

        if (tokens[curPos]->type == OP && tokens[curPos]->data.op == CLOSERB)
        {
            free(tokens[curPos++]);

            return firstNode;
        }

        return NULL;
    }
    else if (tokens[curPos]->type == CONST || tokens[curPos]->type == VAR)
    {
        curPos++;

        return tokens[curPos - 1];      
    }
    else
    {
        return GetName();
    }
}
*/



