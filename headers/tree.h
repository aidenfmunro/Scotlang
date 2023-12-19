#ifndef TREE_H
#define TREE_H

#include "errors.h"

#include <cstddef>

#define SPECIFIER "%c"

enum Operator
{
    #define DEF_OP(keyword, name) name,

    #include "defop.h"

    #undef DEF_OP
};

enum Type
{
    FUNC,
    VAR,
    CONST,
    OP,
    NO_TYPE
};

union NodeElem_t
{
    double     constVal;

    Operator   op;

    char*      name;
};

struct Node
{
    NodeElem_t data       = {};

    Type       type       = NO_TYPE; // WARNING: CHANGED

    Node*      parent;

    Node*      left;

    Node*      right;
};

struct Tree
{
    Node*      root;
    
    size_t     size;  

    ErrorCode  error;  
};  

ErrorCode DestroyTree       (Tree* tree);

ErrorCode DumpTreeGraph     (Node* node);

ErrorCode DumpTreeTxt       (Tree* tree, const char* filename);

ErrorCode VerifyTree        (Tree* tree);

ErrorCode deleteNode        (Node* node);

ErrorCode connectNode       (Node* node, Node* leftChild, Node* rightChild);

Node*     copyNode          (Node* originalNode);

Node*     createNode        (NodeElem_t data, Type type, Node* left, Node* right);

char*     getOpName         (Operator op);

int       countMaxDepth     (Node* node);

bool      findVar           (Node* node);

#endif  