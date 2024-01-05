#include "frontend.h"

const char* expression = " fa fit x mair 0 like?"
                         " ya" 
                         " x gie 1 !"
                         " ran fit x sma 5 like?"
                         " ya"
                         " x gie x + 1 !"
                         " gan?"
                         " fa fit x mair 1 like?"
                         " ya"
                         " x gie 1000 - 7 ! y gie 0 !"
                         " gan?"
                         " gan?";
                         

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

        if (*expression == '\0')
            break;

        printf("cur symb: %c\n", *expression);

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

    if ( getOpToken() )
        return true;
    
    if ( getConstToken() )
        return true;
    
    return getNameToken();
}

bool getOpToken()
{
    if (*expression == '\0')
        return false;

    #define DEF_OP(keyword, name)                                   \
                                                                    \
    if (strncmp(keyword, expression, sizeof(keyword) - 1) == 0)     \
    {                                                               \
        expression += sizeof(keyword) - 1;                          \
                                                                    \
        createOpToken(name);                                        \
                                                                    \
        return true;                                                \
    }

    #include "defop.h"

    #undef DEF_OP

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

bool getNameToken()
{
    if (*expression == '\0')
        return false;

    SafeCalloc(name, MAX_IDENTIFIER_NAME, char, false);

    sscanf(expression, "%[a-zA-Z_0-9]*", name);

    const char* endPtr = strchr(expression, ' ');

    int charsRead = endPtr - expression + 1; // func fit ... like?

    if (charsRead < 0 || charsRead > MAX_IDENTIFIER_NAME)
        return false;

    printf("-> %d\n", charsRead);

    expression += charsRead;

    if (tokens[curTokenNum - 1]->type == FUNC && tokens[curTokenNum - 1]->data.op == FUNCDEC)
    {
        createFuncNode(name);

        return true;
    }

    if (strncmp(expression, "fit", 3) == 0)
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
            printf ("OP    = %d\n", tokens[i]->data.op);

        if (tokens[i]->type == FUNC)
            printf ("FUNC  = %s\n", tokens[i]->data.name);

        if (tokens[i]->type == VAR)
            printf ("VAR   = %s\n", tokens[i]->data.name);

        if (tokens[i]->type == CONST)
            printf ("CONST = %lg\n", tokens[i]->data.constVal);
    }
}


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