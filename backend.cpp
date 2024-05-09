#include "backend.h"

static size_t LABEL_NUM = 0;

#define LABEL(LABEL_COUNT, COMMENT) fprintf(be->outFile, "L%llu:   ; %s\n", LABEL_COUNT, COMMENT)

#define ASM(...) fprintf(be->outFile, __VA_ARGS__)

#define ASM_OP(OP) fprintf(be->outFile, #OP"\n") 

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
    AssertSoft(node, NULL_PTR);

    switch (node->type)
    {
        switch (ID)
        {
            case ENDLINE:
            {
                ASSEMBLE (node->left);

                ASSEMBLE (node->right);
            }

            case IF:
            {
                assembleIf (node);
            }

            case WHILE:
            {
                assembleWhile (node, outFile);
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

        }
        
        switch (FUNC)
        {

        }
        
    }   

    return OK;
}

ErrorCode assembleWhile (Backend* be, Node* node)
{
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
    ASSEMBLE (node->left);

    ASSEMBLE (node->right);

    LABEL (LABEL_NUM, "end if");

    LABEL_NUM++;

    return OK;
}