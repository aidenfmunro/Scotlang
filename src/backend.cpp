#include "backend.h"

size_t NUMBER_OF_CONTAINERS = 32;   // amount of variables

size_t NUMBER_OF_NAMETABLES = 8;    // scope levels

size_t RAM_START_NUMBER     = 1000; // RAM starting point 

//

size_t LABEL_NUM = 0;

//

static size_t RAM_SLOT_COUNTER = RAM_START_NUMBER;

#define LABEL(LABEL_COUNT, COMMENT) fprintf(be->outFile, "L%llu:   ; %s\n", LABEL_COUNT, COMMENT)

#define ASM(...) fprintf(be->outFile, __VA_ARGS__)

#define ASM_OP(OP) fprintf(be->outFile, "\t"#OP"\n") 

#define ASSEMBLE(node) Assemble (be, node)

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

    // free (be->nameTables);

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
                ASM ("[ERROR]: unkown node id:%d!\n", node->data.id);

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
            ASM ("[ERROR]: unknown node type:%d!\n", node->type);

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

    ASM ("\tpush %lg\n", node->data.constVal);

    return OK;
}

#define CASE(OP) case OP:                   \
                 {                           \
                     ASSEMBLE (node->left);  \
                                             \
                     ASSEMBLE (node->right); \
                                             \
                     ASM ("\t"#OP"\n");      \
                                             \
                     return OK;              \
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
            ASM ("[ERROR]: unknown math operation id:%llu\n", node->data.id);

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

    ASSEMBLE (node->right);

    ASM ("\tret\n");    

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
    
    ASM ("\n");
    ASM ("\tcall %.*s\n", node->length, node->data.name);
    ASM ("\n");

    stackFrameEpilogue (be, curNameTable);

    return OK;
}

ErrorCode stackFramePrologue (Backend* be, NameTable* nameTable)
{
    AssertSoft (nameTable, NULL_PTR);

    ASM ("\tpush rax ; stack frame prologue\n");
    ASM ("\tpush %llu\n", nameTable->size);
    ASM ("\tadd\n");
    ASM ("\tpop rax\n");

    return OK;
}

ErrorCode stackFrameEpilogue (Backend* be, NameTable* nameTable)
{
    AssertSoft (nameTable, NULL_PTR);

    ASM ("\tpush rax ; stack frame epilogue\n");
    ASM ("\tpush %llu\n", nameTable->size);
    ASM ("\tsub\n");
    ASM ("\tpop rax\n");

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

    if (varNode)
    {
        NameTableContainer* container = findNameTableContainer(curNameTable, varNode->data.name, varNode->length);

        if (! container)
        {
            ASM ("[ERROR]: variable (%.*s) isn't declared!\n", varNode->length, varNode->data.name);

            return NO_VARIABLE_IN_NAMETABLE;            
        }

        ASM ("\tpush [rax + %llu] ; push var %.*s\n", container->addr.ramAddress, varNode->length, varNode->data.name);
    }

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

            ASM ("\tjne L%llu\n", LABEL_NUM);

            return OK;
        }

        case EQ:
        {
            ASM ("\tje L%llu\n",  LABEL_NUM);

            return OK;
        }

        case MORE:
        {
            ASM ("\tja L%llu\n",  LABEL_NUM);

            return OK;
        }

        case LESS:
        {
            ASM ("\tjb L%llu\n",  LABEL_NUM);

            return OK;
        }

        case MORE_OR_EQUAL:
        {
            ASM ("\tjae L%llu\n", LABEL_NUM);

            return OK;
        }

        case LESS_OR_EQUAL:
        {
            ASM ("\tjbe L%llu\n", LABEL_NUM);

            return OK;
        }

        default:
        {
            ASM ("[ERROR]: unknown logical operation id:%d!\n", node->data.id);

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

    NameTableContainer* container    = findNameTableContainer(curNameTable, lVar, lVarLength);

    if (! container)
    {
        pushToNameTable (curNameTable, lVar, lVarLength);

        container = findNameTableContainer(curNameTable, lVar, lVarLength);        
    }

    ASM ("\tpop [rax + %llu] ; %.*s assign\n", container->addr.ramAddress, container->nameLength, container->name);

    return OK;
}

ErrorCode assembleVariable (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);

    NameTable*          curNameTable = &be->nameTables[be->level];

    NameTableContainer* container    = findNameTableContainer(curNameTable, node->data.name, node->length);

    if (! container)
    {
        pushToNameTable(curNameTable, node->data.name, node->length);
    }
    else
    {
        ASM ("\tpush [rax + %llu] ; %.*s\n", curNameTable->size, node->length, node->data.name);
        ASM ("\n");
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

    ASM ("%.*s:\n", node->length, node->data.name);

    pushFunctionArgumentsToNameTable(be, node->left);

    ASSEMBLE (node->right);

    ASM ("\n");

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

        ASM ("\tpop [rax + %llu] ; %.*s\n", be->nameTables[be->level].size, varNode->length, varNode->data.name);
        ASM ("\n");

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

    be->level++;

    LABEL (LABEL_NUM, "while");

    size_t WHILE_NUM = LABEL_NUM;

    LABEL_NUM++;

    ASSEMBLE (node->left);

    ASSEMBLE (node->right);

    ASM ("jmp L%llu", WHILE_NUM);

    LABEL (LABEL_NUM, "end while");

    be->level--;

    return OK;
}

ErrorCode assembleIf (Backend* be, Node* node)
{
    AssertSoft(be,                      NULL_PTR);
    AssertSoft(be->outFile,             NULL_PTR);
    AssertSoft(be->nameTables,          NULL_PTR);
    AssertSoft(be->size < be->capacity, INDEX_OUT_OF_RANGE);
    AssertSoft(node,                    NULL_PTR)

    be->level++;

    ASSEMBLE (node->left);

    ASSEMBLE (node->right);

    LABEL (LABEL_NUM, "end if");

    LABEL_NUM++;

    be->level--;

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

NameTableContainer* findNameTableContainer (NameTable* nameTable, char* name, size_t nameLength)
{
    AssertSoft(nameTable,                             NULL);
    AssertSoft(nameTable->size < nameTable->capacity, NULL);
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