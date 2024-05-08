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
    ID,
    NONE
};

union NodeElem_t
{
    double     constVal;

    Keyword    id;

    char*      name;
};

struct Node
{
    NodeElem_t data       = {};

    size_t     length;            // TODO: if you change type and length what be careful!

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

Node*     createVarNode     (char* varName, size_t length);

Node*     createFuncNode    (char* funcName, size_t length);

Node*     createKeywordNode (Keyword id);

const char*     getKeywordColor        (Keyword id);

const char*     getKeywordName         (Keyword id);

int       countMaxDepth     (Node* node);

bool      findVar           (Node* node);

#endif  