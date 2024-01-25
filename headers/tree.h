#ifndef TREE_H
#define TREE_H

#include "errors.h"

#include <cstddef>

#define SPECIFIER "%c"

enum Keyword
{
    #define DEF_KEYWORD(keyword, name) name,

    #include "keywords.def"

    #undef DEF_KEYWORD
};

enum Type
{
    FUNC,
    VAR,
    CONST,
    OP,
    NONE
};

union NodeElem_t
{
    double     constVal;

    Keyword    op;

    char*      name;
};

struct Node
{
    NodeElem_t data       = {};

    Type       type       = NONE; // WARNING: CHANGED

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

Node*     connectNode       (Node* node, Node* leftChild, Node* rightChild);

Node*     copyNode          (Node* originalNode);

Node*     createNode        (NodeElem_t data, Type type, Node* left, Node* right);

Node*     createConstNode   (double value);

Node*     createVarNode     (char* varName);

Node*     createFuncNode    (char* funcName);

Node*     createKeywordNode (Keyword op);

char*     getKeywordName         (Keyword op);

int       countMaxDepth     (Node* node);

bool      findVar           (Node* node);

#endif  