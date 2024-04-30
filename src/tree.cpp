#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include "tree.h"
#include "utils.h"
#include "textfuncs.h"

const int MAX_COMMAND_LENGTH  = 256;

const int MAX_FILENAME_LENGTH = 256;

struct ParseRes
{
    char       type;

    NodeElem_t data;
};

static ErrorCode dumpTreeDot_    (Node* node, FILE* outFile);

static ErrorCode dumpTreeTxt_    (Node* node, FILE* outFile);

static ErrorCode checkTreeLinks_ (Tree* tree, Node* node, size_t* counter);

int countMaxDepth(Node* node)
{
    if (node == NULL)
    {
        return 0;
    }
    else
    {
        int leftDepth  = countMaxDepth(node->left);
        int rightDepth = countMaxDepth(node->right);

        if (leftDepth > rightDepth)
        {
            return leftDepth + 1;
        }
        else
        {
            return rightDepth + 1;
        }
    }    
}

bool findVar (Node* node)
{
    if (node == NULL)
    {
        return false;
    }

    if (node->type == VAR)
    {
        return true;
    }

    if (findVar (node->left) || findVar (node->right))
    {
        return true;
    }

    return false;
}

ErrorCode deleteNode (Node* node)
{
    AssertSoft(node, NULL_PTR);

    if (node->left  != NULL)
        deleteNode (node->left);
    
    if (node->right != NULL)
        deleteNode (node->right);

    node->right  = NULL;
    node->left   = NULL;
    node->parent = NULL;

    free(node); 

    return OK;
}

ErrorCode DestroyTree (Tree* tree)
{
    AssertSoft(tree, NULL_PTR);

    deleteNode(tree->root);

    tree->root  = NULL; // ?

    tree->error = 0;

    tree->size  = 0;

    return OK;
}

static ErrorCode checkTreeLinks_(Tree* tree, Node* node, size_t* counter)
{
    AssertSoft(tree, NULL_PTR);
    
    // TODO: counter check

    (*counter)++; // TODO: make it in 

    if ((node->left != NULL && node->left->parent != node) || (node->right != NULL && node->right->parent != node))
    {
        tree->error = TREE_LINKING_ERROR;

        return TREE_LINKING_ERROR;
    }

    if (node->left != NULL)
        checkTreeLinks_(tree, node->left, counter);
    
    if (node->right != NULL)
        checkTreeLinks_(tree, node->right, counter);

    return OK;
}

Node* createNode(NodeElem_t data, Type type, Node* left, Node* right)  
{
    SafeCalloc(newNode, 1, Node, NULL);

    if (left)
        left->parent  = newNode; 
        
    if (right)
        right->parent = newNode;
    
    newNode->left  = left;
    
    newNode->right = right;

    newNode->type = type;

    if      (type == CONST)
    {
        newNode->data.constVal = data.constVal;
    }
    else if (type == OP)
    {
        newNode->data.op  = data.op;
    }
    else if (type == VAR)
    {
        newNode->data.name = data.name;
    }
    else if (type == FUNC)
    {
        newNode->data.name = data.name;
    }
    else
    {
        return NULL; // TODO: return error message
    }


    return newNode;    
}

Node* copyNode (Node* originalNode)
{
    AssertSoft(originalNode, NULL);

    SafeCalloc(newNode, 1, Node, NULL); // check next line!

    newNode->type = originalNode->type;

    switch (originalNode->type)
    {
        case CONST:
        {
            newNode->data.constVal = originalNode->data.constVal;
            break;
        }

        case OP:
        {
            newNode->data.op     = originalNode->data.op;
            break;
        }    

        case VAR:
        {
            newNode->data.name      = originalNode->data.name;
            break;
        }

        default:
        {
            free(newNode);

            return NULL;   // TODO: return error message
        }
    }

    if (originalNode->left)
        newNode->left  = copyNode(originalNode->left);
    
    if (originalNode->right)
        newNode->right = copyNode(originalNode->right);

    return newNode;
}

Node* connectNode (Node* node, Node* leftChild, Node* rightChild)
{
    AssertSoft(node, NULL);

    node->left = leftChild;

    if (leftChild)
        leftChild->parent = node;

    node->right = rightChild;
    
    if (rightChild)
        rightChild->parent = node;

    return node;
}

Node* createConstNode (double value)
{
    NodeElem_t data = {};

    data.constVal = value;

    return createNode(data, CONST, nullptr, nullptr);
}

Node* createVarNode (char* varName, size_t length)
{
    NodeElem_t data = {};

    data.name   = varName;

    Node* node = createNode(data, VAR, nullptr, nullptr); 

    node->length = length;

    return node;   
}

Node* createFuncNode (char* funcName, size_t length)
{
    NodeElem_t data = {};

    data.name   = funcName;

    Node* node = createNode(data, FUNC, nullptr, nullptr);

    node->length = length;

    return node;   
}

Node* createKeywordNode (Keyword op)
{
    NodeElem_t data = {};
    
    data.op = op;

    return createNode(data, OP, nullptr, nullptr);
}

ErrorCode VerifyTree (Tree* tree) // TODO: make proper verify 
{
    size_t count = 0;

    checkTreeLinks_(tree, tree->root, &count);

    return OK;
} 

ErrorCode DumpTreeTxt (Tree* tree, const char* filename)
{
    myOpen(filename, "w+", outFile);

    dumpTreeTxt_(tree->root, outFile);

    myClose(outFile);

    return OK;
}

#define dumpText(...) fprintf(outFile, __VA_ARGS__);

static ErrorCode dumpTreeTxt_ (Node* node, FILE* outFile) // TODO: create tree with txt file
{
    AssertSoft(outFile, UNABLE_TO_OPEN_FILE);

    if (node == NULL)
    {
        // dumpText("(");

        return OK;
    }

    dumpText("(");

    dumpTreeTxt_ (node->left, outFile);

    switch (node->type)
    {
        case CONST:
        {
            dumpText ("%lg", node->data.constVal);
            break;
        }

        case FUNC:
        {
            dumpText ("%*.s", node->length, node->data.name); 
            break;
        }

        case VAR:
        {
            dumpText ("%*.s", node->length, node->data.name);
            break;
        }

        default:
        {
            printf ("Unknown type: %d!\n", node->type);
            break;
        }
    }
 
    dumpTreeTxt_ (node->right, outFile);

    dumpText(")");

    return OK;
}

#undef dumpText

#include "graph.h"

#define dumpGraph(...) fprintf(outFile, __VA_ARGS__);

ErrorCode DumpTreeGraph (Node* node)
{
    AssertSoft(node, NULL_PTR);

    static int DUMP_NUM = 0;

    char filename[MAX_FILENAME_LENGTH] = "";

    sprintf(filename, "log/dot/treegraph_%d.dot", DUMP_NUM); // NOTE: check for existence of folder

    myOpen(filename, "w", outFile);

    dumpGraph("  digraph tree"
              "  {\n"
              "  node[ "
              "  shape     = "SHAPE"," 
              "  style     = "STYLE"," 
              "  fillcolor = "BACKGROUND_PARENT_COLOR"," 
              "  fontcolor = "FONT_COLOR","
              "  margin    = "SPACE_BETWEEN_CONTENTS"];\n"
              "  rankdir   = "RANK";\n\n"
              "  label     = "DUMP_NAME";\n");

    dumpTreeDot_(node, outFile);

    dumpGraph("  }");

    myClose(outFile);

    char command[MAX_COMMAND_LENGTH] = "";

    sprintf(command, "dot -Tpng log/dot/treegraph_%d.dot -o log/img/treeimg_%d.png", DUMP_NUM, DUMP_NUM);

    system(command);

    DUMP_NUM++;

    return OK;
}

static ErrorCode dumpTreeDot_ (Node*  node, FILE* outFile)
{
    AssertSoft(outFile, UNABLE_TO_OPEN_FILE);
    AssertSoft(node,    NULL_PTR);

    if (! node)
        return OK;

    static size_t nodeNum = 0;

    size_t curNodeNum = nodeNum;
    
    if (node->type == OP)
    {
        dumpGraph("\"node%zu\" [shape = \"record\", label = \"{%s}\", fillcolor = \"coral\"]\n",
                                                                             nodeNum, getKeywordName(node->data.op));
    }

    if (node->type == CONST)
    {
        dumpGraph("\"node%zu\" [shape = \"record\", label = \"{%lg}\", fillcolor = \"aqua\"]\n",
                                                                             nodeNum, node->data.constVal);
    }

    if (node->type == VAR)
    {
        dumpGraph("\"node%zu\" [shape = \"record\", label = \"{%.*s}\", fillcolor = \"aquamarine\"]\n",
                                                                             nodeNum, node->length, node->data.name);
    }

    if (node->type == FUNC)
    {
        dumpGraph("\"node%zu\" [shape = \"record\", label = \"{%.*s}\", fillcolor = \"azure\"]\n",
                                                                             nodeNum, node->length, node->data.name);
    }

    nodeNum++;

    if (node->left)
    {
        size_t childNum = nodeNum;

        dumpTreeDot_ (node->left, outFile);

        dumpGraph("\"node%zu\" -> \"node%zu\"\n",
                            curNodeNum, childNum);
    }

    if (node->right)
    {
        size_t childNum = nodeNum;

        dumpTreeDot_ (node->right, outFile);

        dumpGraph("\"node%zu\" -> \"node%zu\"\n",
                            curNodeNum, childNum);
    }
    
    return OK;
}

#undef dumpGraph

const char* getKeywordName (Keyword op)
{
    #define DEF_KEYWORD(keyword, name) case name: return #name;

    switch (op)
    {
        #include "keywords.def"

        default:
            return "UNKNOWN";
    }

    #undef DEF_KEYWORD
}

