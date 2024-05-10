#include "backend.h"

size_t NUMBER_OF_CONTAINERS = 32;   // amount of variables

size_t NUMBER_OF_NAMETABLES = 8;    // scope levels

//

size_t LABEL_NUM = 0;

//

#define LABEL(LABEL_COUNT, COMMENT) fprintf(be->outFile, "L%llu:   ; %s\n", LABEL_COUNT, COMMENT)

#define WRITE_ASM(...) fprintf(be->outFile, __VA_ARGS__)

#define ASSEMBLE(node) Assemble (be, node)

ErrorCode RunBackend ()
{
}

ErrorCode BackendCreate (Backend* be)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);

    be->nameTables = (NameTable*) calloc (NUMBER_OF_NAMETABLES, sizeof (NameTable));

    be->capacity   = NUMBER_OF_NAMETABLES;

    be->size       = 0;

    if (! be->nameTables)
        return NO_MEMORY;
    
    for (size_t iterator = 0; iterator < NUMBER_OF_NAMETABLES; iterator++)
    {
        NameTable* curNameTable       = &be->nameTables[iterator];

        curNameTable->container = (NameTableContainer*) calloc (NUMBER_OF_CONTAINERS, sizeof (NameTableContainer));

        if (! curNameTable->container)
            return NO_MEMORY;

        curNameTable->capacity = NUMBER_OF_CONTAINERS;

        resetNameTable (curNameTable);
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
        NameTableContainer* curContainer = be->nameTables[iterator].container;

        free (curContainer);
    }

    free (be->nameTables);

    return OK;
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
        case ID:
        {
        switch (node->data.id)
        {
            case FUNCDEC:
            {
                resetNameTables (be);

                assembleFunctionDeclaration(be, node->left);

                ASSEMBLE (node->right);

                return OK;
            }

            case ENDLINE:
            {
                ASSEMBLE (node->left);

                ASSEMBLE (node->right);

                return OK;
            }

            case IF:
            {
                return assembleIf (be, node);
            }

            case WHILE:
            {
                return assembleWhile (be, node);
            }

            case ADD:
            case SUB:
            case MUL:
            case DIV:
            case POW:
            case SQRT:
            case SIN:
            case COS:
            case TG:
            case CTG:
            case LN:
            {
                return assembleOperation (be, node);
            }

            case NEQ:
            case EQ:
            case MORE_OR_EQUAL:
            case LESS_OR_EQUAL:
            case LESS:
            case MORE:
            {
                return assembleLogicalOperation (be, node);
            }
            
            case ASSIGN:
            {
                return assembleAssign (be, node);
            }

            case PRINT:
            {
            }

            case INPUT:
            {
            }

            case RETURN:
            {
                return assembleReturn  (be, node);
            }
            
            default:
            {
                WRITE_ASM ("[ERROR]: unkown node id:%d!\n", node->data.id);

                return INCORRECT_ID;
            }

        }
        }

        case CONST:
        {
            return assembleConst (be, node);
        }

        case VAR:
        {
            return assembleVariable (be, node);
        }
        
        case FUNC:
        {
            return assembleFunctionCall (be, node);
        }

        default:
        {
            WRITE_ASM ("[ERROR]: unknown node type:%d!\n", node->type);

            return INCORRECT_TYPE;
        }
    }   

    return OK;
}

ErrorCode assembleConst (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    WRITE_ASM ("\tpush %lg\n", node->data.constVal);

    return OK;
}

#define CASE(OP) case OP:                          \
                 {                                 \
                     ASSEMBLE (node->left);        \
                                                   \
                     ASSEMBLE (node->right);       \
                                                   \
                     WRITE_ASM ("\t"#OP"\n");      \
                                                   \
                     return OK;                    \
                 }

ErrorCode assembleOperation (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    switch (node->data.id)
    {
        CASE(ADD)
        CASE(SUB)
        CASE(MUL)
        CASE(DIV)
        CASE(POW)
        CASE(SQRT)
        CASE(COS)
        CASE(SIN)
        CASE(TG)
        CASE(CTG)
        CASE(LN)

        default:
        {
            WRITE_ASM ("[ERROR]: unknown math operation id:%llu\n", node->data.id);

            return NO_OPERATION;
        }
    }

    return OK;
}

#undef CASE(OP)

ErrorCode assembleReturn (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);


    if (node->right)
    {
        ASSEMBLE (node->right);

        WRITE_ASM ("\tret\n\n");
    }
    else
    {
        WRITE_ASM ("\n\thlt\n");
    }    

    return OK;
}
ErrorCode assembleFunctionCall (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    NameTable* curNameTable = &be->nameTables[be->level];

    if (node->left)
        pushFunctionCallArguments (be, node->left);

    stackFramePrologue (be, curNameTable);
    
    WRITE_ASM ("\n");
    WRITE_ASM ("\tcall %.*s\n", node->length, node->data.name);
    WRITE_ASM ("\n");

    stackFrameEpilogue (be, curNameTable);

    return OK;
}

ErrorCode stackFramePrologue (Backend* be, NameTable* nameTable)
{
    AssertSoft (nameTable, NULL_PTR);

    WRITE_ASM ("\n");
    WRITE_ASM ("\tpush rax ; stack frame prologue\n");
    WRITE_ASM ("\tpush %llu\n", nameTable->size);
    WRITE_ASM ("\tadd\n");
    WRITE_ASM ("\tpop rax\n");
    WRITE_ASM ("\n");

    return OK;
}

ErrorCode stackFrameEpilogue (Backend* be, NameTable* nameTable)
{
    AssertSoft (nameTable, NULL_PTR);

    WRITE_ASM ("\n");
    WRITE_ASM ("\tpush rax ; stack frame epilogue\n");
    WRITE_ASM ("\tpush %llu\n", nameTable->size);
    WRITE_ASM ("\tsub\n");
    WRITE_ASM ("\tpop rax\n");
    WRITE_ASM ("\n");

    return OK;
}

ErrorCode pushFunctionCallArguments (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    Node* varNode = node->left;

    NameTable* curNameTable = &be->nameTables[be->level];

    if (node->left)
        ASSEMBLE (node->left);

    if (node->right)
    {
       pushFunctionCallArguments (be, node); // TODO: add return to functions??? 
    }

    return OK;
}

ErrorCode assembleLogicalOperation (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    ASSEMBLE (node->left);

    ASSEMBLE (node->right);

    switch (node->data.id)
    {
        case NEQ:
        {

            WRITE_ASM ("\tjne L%llu\n", LABEL_NUM);

            return OK;
        }

        case EQ:
        {
            WRITE_ASM ("\tje L%llu\n",  LABEL_NUM);

            return OK;
        }

        case MORE:
        {
            WRITE_ASM ("\tja L%llu\n",  LABEL_NUM);

            return OK;
        }

        case LESS:
        {
            WRITE_ASM ("\tjb L%llu\n",  LABEL_NUM);

            return OK;
        }

        case MORE_OR_EQUAL:
        {
            WRITE_ASM ("\tjae L%llu\n", LABEL_NUM);

            return OK;
        }

        case LESS_OR_EQUAL:
        {
            WRITE_ASM ("\tjbe L%llu\n", LABEL_NUM);

            return OK;
        }

        default:
        {
            WRITE_ASM ("[ERROR]: unknown logical operation id:%d!\n", node->data.id);

            return INCORRECT_LOGICAL_OPERATION;
        }
    }

    return OK;
}

ErrorCode assembleAssign (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    ASSEMBLE (node->right);

    NameTable*          curNameTable = &be->nameTables[be->level];

    char* lVar        = node->left->data.name;

    size_t lVarLength = node->left->length;

    NameTableContainer* container    = findNameTableContainer(be, lVar, lVarLength);

    if (! container)
    {
        pushToNameTable (curNameTable, lVar, lVarLength);

        container = findNameTableContainer(be, lVar, lVarLength);        
    }

    WRITE_ASM ("\tpop [rax + %llu] ; %.*s assign\n", container->addr.ramAddress, container->nameLength, container->name);

    return OK;
}

ErrorCode assembleVariable (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    NameTable*          curNameTable = &be->nameTables[be->level];

    NameTableContainer* container    = findNameTableContainer(be, node->data.name, node->length);

    if (! container)
    {
        pushToNameTable(curNameTable, node->data.name, node->length);
    }
    else
    {
        WRITE_ASM ("\tpush [rax + %llu] ; %.*s\n", curNameTable->size, node->length, node->data.name);
        WRITE_ASM ("\n");
    }

    // TODO: Maybe think on what's need to be added here? IDK (2:00 AM) 

    return OK;
}

ErrorCode assembleFunctionDeclaration (Backend* be, Node* node)
{ 
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    WRITE_ASM ("%.*s:\n", node->length, node->data.name);

    pushFunctionArgumentsToNameTable(be, node->left);

    ASSEMBLE (node->right);

    WRITE_ASM ("\n");

    return OK;
}

ErrorCode pushFunctionArgumentsToNameTable (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);
    AssertSoft(node,                    NULL_PTR);

    Node* varNode = node->left;

    if (varNode)
    {
        // function arguments are held in the stack

        WRITE_ASM ("\tpop [rax + %llu] ; %.*s\n", be->nameTables[be->level].size, varNode->length, varNode->data.name);
        WRITE_ASM ("\n");

        pushToNameTable(&be->nameTables[be->level], varNode->data.name, varNode->length);
    }

    if (node->right)
        pushFunctionArgumentsToNameTable(be, node->right);

    return OK;   
}

ErrorCode assembleWhile (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);
    AssertSoft(node,                    NULL_PTR);

    downgradeNameTable (be);

    LABEL (LABEL_NUM, "while");

    size_t WHILE_NUM = LABEL_NUM;

    LABEL_NUM++;

    ASSEMBLE (node->left);

    ASSEMBLE (node->right);

    WRITE_ASM ("jmp L%llu", WHILE_NUM);

    LABEL (LABEL_NUM, "end while");

    upgradeNameTable (be);

    return OK;
}

ErrorCode assembleIf (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);
    AssertSoft(node,                    NULL_PTR);

    downgradeNameTable (be);

    ASSEMBLE (node->left);

    ASSEMBLE (node->right);

    LABEL (LABEL_NUM, "end if");

    LABEL_NUM++;

    upgradeNameTable (be);

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
    container->addr.ramAddress = nameTable->size;
    container->type            = RAM; 

    nameTable->size++;

    return OK;
}

ErrorCode resetNameTables (Backend* be)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    for (size_t lvlDepth = 0; lvlDepth <= be->level; lvlDepth++)
    {
        resetNameTable (&be->nameTables[lvlDepth]);
    }

    return OK;    
}

ErrorCode resetNameTable (NameTable* nameTable)
{
    AssertSoft (nameTable,                             NULL_PTR);
    AssertSoft (nameTable->size < nameTable->capacity, NULL_PTR);

    for (size_t curContainerNum = 0; curContainerNum < NUMBER_OF_CONTAINERS; curContainerNum++)
    {
        NameTableContainer* container = &nameTable->container[curContainerNum];

        container->addr.ramAddress = curContainerNum;

        container->name            = NULL;

        container->nameLength      = 0;
        
        container->type            = RAM;
    }

    nameTable->size = 0;

    return OK;
}

ErrorCode downgradeNameTable (Backend* be)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    be->level++;

    return OK;
}

ErrorCode upgradeNameTable (Backend* be)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    resetNameTable (&be->nameTables[be->level]);

    be->level--;

    return OK;
}

NameTableContainer* findNameTableContainer (Backend* be, char* name, size_t nameLength)
{
    AssertSoft(be,                      NULL);
    AssertSoft(be->outFile,             NULL);
    AssertSoft(be->nameTables,          NULL);
    AssertSoft(be->size < be->capacity, NULL);
    AssertSoft(name,                    NULL);

    for (size_t lvlDepth = 0; lvlDepth <= be->level; lvlDepth++)
    {
        for (size_t containerNum = 0; containerNum < be->nameTables[lvlDepth].size; containerNum++)
        {
            NameTableContainer* container = &be->nameTables[lvlDepth].container[containerNum];

            if (! strncmp(container->name, name, nameLength))
            {
                return container;
            }
        }
    }

    return NULL;
}