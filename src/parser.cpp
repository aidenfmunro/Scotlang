#include "parser.h"

#define curNode tkns->token[tkns->curTokenNum].elem

#define curToken tkns->token[tkns->curTokenNum]

#define prevNode tkns->token[tkns->curTokenNum - 1].elem

#define nextNode tkns->token[tkns->curTokenNum + 1].elem

#define REQUIRE(keyword) (curNode->data.op == keyword)

#define curPos  tkns->curTokenNum


Node* GetGrammar (Tokens* tkns)
{
    Node* root = GetOp (tkns); 
    
    return root;
}


Node* GetE (Tokens* tkns)
{
    printf("In GetE\n");

    PrintToken(&curToken);

    Node* firstNode = GetT (tkns);

    while (curNode->data.op >= EQ && curNode->data.op <= SUB)
    {
        Keyword op = curNode->data.op;

        Node* secondNode = GetT (tkns); 

        firstNode = createNode({.op = op}, OP, firstNode, secondNode);
    }

    return firstNode;
}

Node* GetIfWhile (Tokens* tkns)
{
    printf("In GetIfWhile\n");

    PrintToken(&curToken);

    if (REQUIRE(IF) || REQUIRE(WHILE))
    {
        Node* opNode = curNode;

        curPos++;

        if (REQUIRE(OPEN_RB))
        {
            free(curNode);

            curPos++;

            Node* firstNode = GetE (tkns);

            opNode->left = firstNode;

            if (REQUIRE(CLOSE_RB))
            {
                // printf("IM HERE!!!\n");

                free(curNode);

                curPos++; // TODO: Free and move on, maybe?

                opNode->right = GetOp (tkns);

                return opNode;
            }
        }
    }

    return NULL;
}

Node* GetT (Tokens* tkns)
{
    printf("In GetT\n");

    PrintToken(&curToken);

    Node* firstNode = GetP (tkns);

    while (curNode->data.op >= MUL && curNode->data.constVal <= POW)
    {
        Keyword op = curNode->data.op;

        free(curNode);

        curPos++;

        Node* secondNode = GetP (tkns);

        firstNode = createNode({.op = op}, OP, firstNode, secondNode);
    }

    return firstNode;
}

Node* GetOp (Tokens* tkns)
{
    printf("In GetOp\n");

    PrintToken(&curToken);

    if (curNode->type == OP && (REQUIRE(IF) || REQUIRE(WHILE)))
    {
        return GetIfWhile (tkns);
    }
    else if (curNode->type == VAR)
    {
        // printf("bebra\n");        

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
    printf("In GetA\n");

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
    printf("In GetName\n");

    PrintToken(&curToken);

    if (curNode->data.op >= SIN)
    {
        Keyword op = curNode->data.op; // curPos++

        curPos++;

        if (curNode->data.op == OPEN_RB)
        {
            free(curNode);

            curPos++;

            Node* firstNode = GetE (tkns);

            firstNode = createNode({.op = op}, OP, NULL, firstNode);

            if (curNode->data.op == CLOSE_RB)
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
    printf("In GetP\n");

    PrintToken(&curToken);

    if (curNode->type == OP && curNode->data.op == OPEN_RB)
    {
        free(curNode);

        curPos++;

        Node* firstNode = GetE (tkns);

        if (curNode->type == OP && curNode->data.op == CLOSE_RB)
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


