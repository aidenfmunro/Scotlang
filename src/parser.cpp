#include "parser.h"

#define curNode tkns->token[tkns->curTokenNum].elem

#define curToken tkns->token[tkns->curTokenNum]

#define prevNode tkns->token[tkns->curTokenNum - 1].elem

#define nextNode tkns->token[tkns->curTokenNum + 1].elem

#define REQUIRE(keyword) (curNode->data.id == keyword)

#define curPos  tkns->curTokenNum


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

    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetFunc...\n Current token: ");

    PrintToken(&curToken);

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
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetBlock...\n Current token: ");

    PrintToken(&curToken);

    Node* blockNode = NULL;

    if (REQUIRE(OPEN_SB))
    {
        printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetBlock...\n Function has open brackets... \n");

        free(curNode);

        curPos++;

        blockNode = GetOp(tkns);
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
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetFuncArguments...\n Current token: ");

    PrintToken(&curToken);

    Node* argsNode = createKeywordNode(COMMA);

    if (REQUIRE(OPEN_RB))
    {
        Node* curArgNode = argsNode;

        free(curNode);

        curPos++;

        while (! REQUIRE(CLOSE_RB))
        {
            Node* argNode = curNode;

            curPos++;

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

Node* GetE (Tokens* tkns)
{
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetE...\n Current token: ");

    PrintToken(&curToken);

    Node* firstNode = GetT (tkns);

    while (EQ <= curNode->data.id && curNode->data.id <= SUB)
    {
        Keyword id = curNode->data.id;
    
        free(curNode);

        curPos++;

        Node* secondNode = GetT (tkns); 

        firstNode = createNode({.id = id}, ID, firstNode, secondNode);
    }

    return firstNode;
}

Node* GetIfWhile (Tokens* tkns)
{
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetIfWhile...\n Current token: ");

    PrintToken(&curToken);

    if (REQUIRE(IF) || REQUIRE(WHILE))
    {
        Node* idNode = curNode;

        curPos++;

        if (REQUIRE(OPEN_RB))
        {
            free(curNode);

            curPos++;

            Node* firstNode = GetE (tkns);

            idNode->left = firstNode;

            if (REQUIRE(CLOSE_RB))
            {
                free(curNode);

                curPos++; 

                idNode->right = GetOp (tkns);

                return idNode;
            }
        }
    }

    return NULL;
}

Node* GetT (Tokens* tkns)
{
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetT...\n Current token: ");

    PrintToken(&curToken);

    Node* firstNode = GetP (tkns);

    while (curNode->data.id >= MUL && curNode->data.id <= POW)
    {
        Keyword id = curNode->data.id;

        free(curNode);

        curPos++;

        Node* secondNode = GetP (tkns);

        firstNode = createNode({.id = id}, ID, firstNode, secondNode);
    }

    return firstNode;
}

Node* GetOp (Tokens* tkns)
{
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetOp...\n Current token: ");

    PrintToken(&curToken);

    if (curNode->type == ID && (REQUIRE(IF) || REQUIRE(WHILE)))
    {
        return GetIfWhile (tkns);
    }
    else if (curNode->type == VAR)
    {
        return GetA (tkns);
    }
    else if (REQUIRE(OPEN_SB))
    {
        free(curNode);

        curPos++;

        Node* firstNode = GetOp (tkns);

        // printf("node: %p\n", firstNode);

        Node* secondNode = {};

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

Node* GetA (Tokens* tkns)
{
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetA...\n Current token: ");

    PrintToken(&curToken);

    if (curNode->type == VAR)
    {
        Node* varNode = curNode;

        curPos++;

        if (REQUIRE(ASSIGN))
        {
            printf("in assign if\n");

            Node* assignNode = curNode;

            curPos++;
            
            Node* valueNode  = GetE (tkns);

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

Node* GetName (Tokens* tkns)
{
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetName...\n Current token: ");

    PrintToken(&curToken);

    if (curNode->data.id >= SIN)
    {
        Keyword id = curNode->data.id; // curPos++

        curPos++;

        if (curNode->data.id == OPEN_RB)
        {
            free(curNode);

            curPos++;

            Node* firstNode = GetE (tkns);

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

Node* GetP (Tokens* tkns)
{
    printf(""BOLD"[PARSER]"COLOR_RESET""RED" =>"COLOR_RESET" In GetP...\n Current token: ");

    PrintToken(&curToken);

    if (curNode->type == ID && curNode->data.id == OPEN_RB)
    {
        free(curNode);

        curPos++;

        Node* firstNode = GetE (tkns);

        if (curNode->type == ID && curNode->data.id == CLOSE_RB)
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
    else
    {
        return GetName (tkns);
    }
}


