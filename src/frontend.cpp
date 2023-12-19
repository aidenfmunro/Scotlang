#include "frontend.h"

const char* expression = "ya div 1 3 4";

Type curTokenStatus = NO_TYPE;

Node* tokens[100] = {};

size_t curTokenNum = 0;

ErrorCode getTokens(const char* fileIn)
{
    Text source = {};

    CreateText(&source, fileIn, NONE);

    while (*expression)
    {
        skipSymbols();

        printf("cur symb: %c\n", *expression);

        if (*expression == '\0')
            break;
        
        if (! getToken())
            break;
    }

    for (size_t i = 0; i < 4; i++)
    {
        printf("%p\n", tokens[i]);
    }

    return OK; 
}

size_t skipSymbols()
{
    size_t charsSkipped = 0;

    if (*expression == '\0')
        return 0;

    while (isspace(*expression) || *expression == '\t' || *expression == '\n' || *expression == '\r')
    {
        expression++;
        charsSkipped++;
    }

    return charsSkipped;
}

bool getToken()
{
    skipSymbols();

    if (getOpToken() )
        return true;
    
    if (getConstToken() )
        return true;
    
    /* if (getNametToken() )
        return true; */

    return false;
}

bool getOpToken()
{
    /*
    #define DEF_OP(keyword, name)                                   \
    if (strncmp(keyword, expression, sizeof(keyword) - 1) == 0)     \
    {                                                               \
        expression += sizeof(keyword) - 1;                          \
        createOpToken(name);                                        \
        return true;                                                \
    }

    #include "defop.h"

    #undef DEF_OP
    */

    Node* node = NULL;

    if (strncmp("ya", expression, sizeof("ya") - 1) == 0)     
    {                                                               
        expression += sizeof("ya") - 1;                          
        node = createOpToken(OPENSB);   
        printf("node: %p\n", node);                                     
        return true;                                                
    }

    if (strncmp("div", expression, sizeof("div") - 1) == 0)     
    {                                                               
        expression += sizeof("div") - 1;                          
        createOpToken(EQ);                                        
        return true;                                               
    }



    return false;
}

bool getConstToken()
{
    if (isdigit(*expression))
    {
        char* endPtr = NULL;

        double value = strtod(expression, &endPtr);

        expression += endPtr - expression;

        createConstToken(value);

        return true;
    }

    return false;
}

const int MAX_IDENTIFIER_NAME = 32;

bool getNametToken()
{
    SafeCalloc(name, MAX_IDENTIFIER_NAME, char, false);

    sscanf(expression, "%[a-zA-Z]", name);

    if (tokens[curTokenNum - 1]->type == OP && tokens[curTokenNum - 1]->data.op == FUNCDEC)
    {
        createFuncNode(name);

        return true;
    }

    if (*expression == '(')
    {
        createFuncToken(name);

        return true;
    }

    createVarToken(name);

    return true;
}

Node* createOpToken(Operator op)
{
    Node* node = createOpNode(op);

    tokens[curTokenNum++] = node;

    curTokenStatus = OP;

    return node;
}

Node* createConstToken(double value)
{
    Node* node = createConstNode(value);

    tokens[curTokenNum++] = node;

    curTokenStatus = CONST;

    return node; 
}

Node* createFuncToken(char* funcName)
{
    Node* node = createFuncNode(funcName);

    tokens[curTokenNum++] = node;

    curTokenStatus = FUNC;

    return node;
}

Node* createVarToken(char* varName)
{
    Node* node = createVarNode(varName);

    tokens[curTokenNum++] = node;

    curTokenStatus = VAR;

    return node;
}

void PrintTokens () 
{
    for (size_t i = 0; i < curTokenNum; i++) 
    {
        if (tokens[i]->type == OP)
            printf ("Sys oper = %d\n", tokens[i]->data.op);

        if (tokens[i]->type == FUNC)
            printf ("Func = %d\n", tokens[i]->data.op);

        if (tokens[i]->type == VAR)
            printf ("Var = %d\n", tokens[i]->data.op);

        if (tokens[i]->type == CONST)
            printf ("Const = %lg\n", tokens[i]->data.constVal);
    }
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