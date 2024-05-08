#include "parser.h"

#define curNode tkns->token[tkns->curTokenNum].elem

#define curToken tkns->token[tkns->curTokenNum]

#define prevNode tkns->token[tkns->curTokenNum - 1].elem

#define nextNode tkns->token[tkns->curTokenNum + 1].elem

#define REQUIRE(keyword) (curNode->data.id == keyword)

#define curPos  tkns->curTokenNum

#define PrintCurrentParserState() printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In %s...\n Current token: ", __PRETTY_FUNCTION__); \
                                  PrintToken(&curToken) 


Node* GetGrammar (Tokens* tkns)
{
    Node* root = GetAll (tkns); 
    
    return root;
}

Node* GetAll (Tokens* tkns)
{
    Node* decNode = createKeywordNode(FUNCDEC);

    Node* curFuncNode = GetFunc(tkns);

    Node* curDecNode = decNode;

    while (curFuncNode != NULL)
    {
        Node* newDecNode = createKeywordNode(FUNCDEC);

        connectNode(curDecNode, curFuncNode, newDecNode);

        curDecNode = newDecNode;

        curFuncNode = GetFunc(tkns);
    }

    return decNode;
}

Node* GetFunc (Tokens* tkns)
{
    AssertSoft(curNode, NULL); // TODO: как сделать так, чтобы не выходило за рамки токены?

    PrintCurrentParserState();

    Node* funcNode = NULL;

    if (REQUIRE(FUNCDEC))
    {
        free(curNode);

        curPos++;

        funcNode = curNode;

        curPos++;

        Node* argsNode = GetFuncArguments(tkns);

        Node* blockNode = GetBlock(tkns);

        connectNode(funcNode, argsNode, blockNode);
    }

    return funcNode;
}

Node* GetBlock (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    Node* blockNode = NULL;

    if (REQUIRE(OPEN_SB))
    {
        PrintCurrentParserState();

        free(curNode);

        curPos++;

        blockNode = GetOp(tkns);

        Node* curBlockNode = blockNode;

        while (curBlockNode)
        {
            curBlockNode->right = GetOp(tkns);

            curBlockNode = curBlockNode->right;
        }
    }
    else
    {
        //
    }

    if (REQUIRE(CLOSE_SB))
    {
        free(curNode);

        curPos++;
    }

    return blockNode;
}

Node* GetFuncArguments (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    Node* argsNode = NULL;

    if (REQUIRE(OPEN_RB))
    {     
        free(curNode);

        curPos++;

        if (REQUIRE(CLOSE_RB))
        {
            free (curNode);

            curPos++;

            return NULL;
        }

        PrintCurrentParserState();

        argsNode = createKeywordNode(COMMA);

        Node* curArgNode = argsNode;

        while (! REQUIRE(CLOSE_RB))
        {
            PrintCurrentParserState();

            Node* argNode = GetAdditiveExpression (tkns);

            // curPos++;

            PrintCurrentParserState();

            Node* commaNode = curNode;

            if (commaNode->data.id == CLOSE_RB)
            {



                connectNode(curArgNode, argNode, NULL);

                break;
            }

            connectNode(curArgNode, argNode, commaNode);

            curArgNode = commaNode;

            curPos++;
        }

        free(curNode);

        curPos++;
    }

    return argsNode;
}

Node* GetAdditiveExpression (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    Node* firstNode = GetMultiplicativeExpression (tkns);

    while (REQUIRE(ADD) || REQUIRE(SUB))
    {
        Keyword id = curNode->data.id;
    
        free(curNode);

        curPos++;

        Node* secondNode = GetMultiplicativeExpression (tkns); 

        firstNode = createNode({.id = id}, ID, firstNode, secondNode);
    }

    return firstNode;
}

Node* GetIfWhile (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    if (REQUIRE(IF) || REQUIRE(WHILE))
    {
        Node* idNode = curNode;

        curPos++;

        if (REQUIRE(OPEN_RB))
        {
            free(curNode);

            curPos++;

            Node* firstNode = GetLogicalExpression (tkns);

            idNode->left = firstNode;

            if (REQUIRE(CLOSE_RB))
            {
                free(curNode);

                curPos++; 

                idNode->right = GetOp (tkns);

                Node* endNode = createKeywordNode(ENDLINE);

                return connectNode(endNode, idNode, NULL);
            }
        }
    }

    return NULL;
}

Node* GetLogicalExpression (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    Node* firstTerm = GetAdditiveExpression(tkns);

    PrintCurrentParserState();

    Node* logicalOpNode = NULL;

    if (REQUIRE(EQ) || REQUIRE(NEQ) || REQUIRE(LESS) || REQUIRE(LESS_OR_EQUAL) || REQUIRE(MORE) || REQUIRE(MORE_OR_EQUAL))
    {
        PrintCurrentParserState();

        logicalOpNode = curNode;

        curPos++;
    }

    Node* secondTerm = GetAdditiveExpression(tkns);

    return connectNode(logicalOpNode, firstTerm, secondTerm);
}

Node* GetMultiplicativeExpression (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    Node* firstNode = GetPrimaryExpression (tkns);

    while (REQUIRE(MUL) || REQUIRE(POW) || REQUIRE(DIV))
    {
        Keyword id = curNode->data.id;

        free(curNode);

        curPos++;

        Node* secondNode = GetPrimaryExpression (tkns);

        firstNode = createNode({.id = id}, ID, firstNode, secondNode);
    }
    
    return firstNode;
}

Node* GetOp (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    if (curNode->type == ID && (REQUIRE(IF) || REQUIRE(WHILE)))
    {
        return GetIfWhile (tkns);
    }
    else if (curNode->type == VAR)
    {
        return GetAssign (tkns);
    }
    else if (curNode->data.id == RETURN)
    {
        return GetReturn (tkns);
    }
    else if (curNode->data.id == INPUT || curNode->data.id == PRINT)
    {
        return GetInputPrint (tkns);
    }
    else if (curNode->type == FUNC)
    {
        return GetFuncExpression (tkns);
    }
    else if (REQUIRE(OPEN_SB))
    {
        free(curNode);

        curPos++;

        Node* firstNode = GetOp (tkns);

        // printf("node: %p\n", firstNode);

        Node* secondNode = NULL;

        Node* root = firstNode;

        while (! REQUIRE(CLOSE_SB))
        {
            secondNode = GetOp (tkns);

            firstNode->right = secondNode;

            firstNode = secondNode;
        }

        free(curNode);

        curPos++;

        return root;
    }
    else
    {
        return NULL;
    }
}

Node* GetInputPrint (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    if (REQUIRE(INPUT) || REQUIRE(PRINT))
    {
        Node* inputNode = curNode;

        curPos++;

        PrintCurrentParserState();

        if (REQUIRE(OPEN_RB))
        {
            free(curNode);

            curPos++;

            PrintCurrentParserState();

            if (curNode->type == VAR || curNode->type == CONST)
            {
                Node* varNode = curNode;

                curPos++;

                PrintCurrentParserState();

                if (REQUIRE(CLOSE_RB))
                {
                    free(curNode);

                    curPos++;

                    if (REQUIRE(ENDLINE))
                    {
                        Node* endNode = curNode;

                        curPos++;

                        return connectNode(endNode, connectNode(inputNode, NULL, varNode), NULL);
                    }
                }
            }
        }
    }

    return NULL;
}

Node* GetReturn (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    Node* retNode = NULL;

    if (REQUIRE(RETURN))
    {
        PrintCurrentParserState();

        retNode = curNode;

        curPos++;

        Node* retArgNode = GetAdditiveExpression (tkns);

        PrintCurrentParserState();

        if (REQUIRE(ENDLINE))
        {
            PrintCurrentParserState();

            Node* endNode = curNode;

            curPos++;

            return connectNode(endNode, connectNode(retNode, NULL, retArgNode), NULL);
        }
    }

    return NULL;
}

Node* GetAssign (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    if (curNode->type == VAR)
    {
        Node* varNode = curNode;

        curPos++;

        if (REQUIRE(ASSIGN))
        {
            printf("in assign if\n");

            Node* assignNode = curNode;

            curPos++;
            
            Node* valueNode  = GetAdditiveExpression (tkns);

            connectNode(assignNode, varNode, valueNode);

            if (REQUIRE(ENDLINE))
            {
                printf("in end if\n");

                Node* endNode = curNode;

                curPos++;
    
                return connectNode(endNode, assignNode, NULL);
            }
        }
    }

    return NULL;
}

Node* GetUnaryOperation (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    if (curNode->data.id >= SIN && curNode->data.id <= LN)
    {
        Keyword id = curNode->data.id; // curPos++

        curPos++;

        if (curNode->data.id == OPEN_RB)
        {
            free(curNode);

            curPos++;

            Node* firstNode = GetAdditiveExpression (tkns);

            firstNode = createNode({.id = id}, ID, NULL, firstNode);

            if (curNode->data.id == CLOSE_RB)
            {
                free(curNode);

                curPos++;

                return firstNode;
            }
        }
    }
    
    return NULL;
}

Node* GetPrimaryExpression (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    if (REQUIRE(OPEN_RB))
    {
        free(curNode);

        curPos++;

        Node* firstNode = GetAdditiveExpression (tkns);

        if (REQUIRE(CLOSE_RB))
        {
            free(curNode);

            curPos++;

            return firstNode;
        }

        return NULL;
    }
    else if (curNode->type == CONST || curNode->type == VAR)
    {
        curPos++;

        return prevNode;      
    }
    else if (curNode->type == FUNC)
    {
        return GetFuncExpression (tkns);
    }
    else
    {
        return GetUnaryOperation (tkns);
    }
}

Node* GetFuncExpression (Tokens* tkns)
{
    AssertSoft(tkns, NULL);

    PrintCurrentParserState();

    if (curNode->type == FUNC)
    {
        Node* funcNode = curNode;
        
        curPos++;

        PrintCurrentParserState();

        Node* funcArgsNode = GetFuncArguments (tkns);

        PrintCurrentParserState();

        return connectNode(funcNode, funcArgsNode, NULL);
    }

    return NULL;
}


