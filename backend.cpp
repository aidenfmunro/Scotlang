#include "backend.h"

static size_t LABEL_NUM = 0;

static size_t RAM_SLOT_COUNTER = RAM_START_NUMBER;

#define LABEL(LABEL_COUNT, COMMENT) fprintf(be->outFile, "L%llu:   ; %s\n", LABEL_COUNT, COMMENT)

#define ASM(...) fprintf(be->outFile, __VA_ARGS__)

#define ASM_OP(OP) fprintf(be->outFile, "\t"#OP"\n") 

#define ASSEMBLE(node) Assemble (be, node)

ErrorCode BackendCreate (Backend* be)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR)
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    be->nameTables = (NameTable*) calloc (NUMBER_OF_NAMETABLES, sizeof (NameTable));

    be->capacity   = NUMBER_OF_NAMETABLES;

    be->size       = 0;

    if (! be->nameTables)
        return NO_MEMORY;
    
    for (size_t iterator = 0; iterator < NUMBER_OF_NAMETABLES; iterator++)
    {
        NameTable* curNameTable       = &be->nameTables[iterator];

        NameTableContainer* container = curNameTable->container;

        container = (NameTableContainer*) calloc (NUMBER_OF_CONTAINERS, sizeof (NameTableContainer));

        if (! container)
            return NO_MEMORY;

        curNameTable->capacity = NUMBER_OF_CONTAINERS;

        curNameTable->size     = 0;
    }

    return OK;
}

ErrorCode BackendDestroy (Backend* be)
{
    AssertSoft(be,                      NONE_EXIST);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR)
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);


    for (size_t iterator = 0; iterator < NUMBER_OF_NAMETABLES; iterator++)
    {
        NameTable* curNameTable = &be->nameTables[iterator];

        free (curNameTable);
    }

    free (be->nameTables);
}

ErrorCode Assemble (Backend* be, Node* node)
{
    AssertSoft(be,                      NONE_EXIST);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR)
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    if (! node)
        return NULL;

    switch (node->type)
    {
        switch (ID)
        {
            case FUNCDEC:
            {
                ASSEMBLE (node->right);

                assembleFunctionDeclaration(be, node->left);
            }

            case ENDLINE:
            {
                ASSEMBLE (node->left);

                ASSEMBLE (node->right);
            }

            case IF:
            {
                be->level++;

                assembleIf (be, node);

                be->level--;
            }

            case WHILE:
            {
                be->level++;

                assembleWhile (be, node);

                be->level--;
            }

            case ADD:
            case SUB:
            case MUL:
            case DIV:
            case SQRT:
            case SIN:
            case COS:
            case TG:
            case CTG:
            case LN:
            {
                ASSEMBLE (node->left);

                ASSEMBLE (node->right);

                ASM_OP   (node->data.id);
            }
            
            case ASSIGN:
            {
                
            }
            
        }

        switch (VAR)
        {
            assembleVariable (be, node);
        }
        
        switch (FUNC)
        {

        }
        
    }   

    return OK;
}

ErrorCode assembleVariable (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR)
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);
}

ErrorCode assembleFunctionDeclaration (Backend* be, Node* node)
{ 
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR)
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    ASM ("%.*s:\n", node->data.name, node->length);

    pushFunctionArgumentsToNameTable(be, node->left);

    ASSEMBLE (node->right);

    ASM("\n");

    return OK;
}

ErrorCode pushFunctionArgumentsToNameTable (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR)
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    Node* varNode = node->left;

    if (varNode)
        pushToNameTable(&be->nameTables[be->level], varNode->data.name, varNode->length);

    if (node->right)
        pushFunctionArgumentsToNameTable(be, node->right);

    return OK;   
}

ErrorCode assembleWhile (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR)
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);
    AssertSoft(node,                    NULL_PTR);

    LABEL (LABEL_NUM, "while");

    size_t WHILE_NUM = LABEL_NUM;

    LABEL_NUM++;

    ASSEMBLE (node->left);

    ASSEMBLE (node->right);

    ASM ("jmp L%llu", WHILE_NUM);

    LABEL (LABEL_NUM, "end while");

    return OK;
}

ErrorCode assembleIf (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR)
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);
    AssertSoft(node,                    NULL_PTR)

    ASSEMBLE (node->left);

    ASSEMBLE (node->right);

    LABEL (LABEL_NUM, "end if");

    LABEL_NUM++;

    return OK;
}

ErrorCode pushToNameTable (NameTable* nameTable, char* name, size_t nameLength)
{
    AssertSoft(nameTable,                             NULL_PTR);
    AssertSoft(nameTable->size < nameTable->capacity, INDEX_OUT_OF_RANGE);
    AssertSoft(name,                                  NULL_PTR);

    NameTableContainer* container = &nameTable->container[nameTable->size]; 

    container->name            = name;
    container->nameLength      = nameLength;
    container->addr.ramAddress = RAM_SLOT_COUNTER;
    container->type            = RAM; 

    nameTable->size++;

    return OK;
}

NameTableContainer* findNameTableContainer (NameTable* nameTable, char* name, size_t nameLength)
{
    AssertSoft(nameTable,                             NULL);
    AssertSoft(nameTable->size < nameTable->capacity, NULL)
    AssertSoft(name,                                  NULL);

    for (size_t iterator = 0; iterator < nameTable->size; iterator++)
    {
        NameTableContainer* container = &nameTable->container[iterator];

        if (! strncmp(container->name, name, nameLength))
        {
            return container;
        }
    }

    return NULL;
}