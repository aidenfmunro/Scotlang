#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "utils.h"

//

enum MemoryType
{
    REG = 0,
    RAM = 1,
};

//

union MemoryAddress
{
    unsigned char regId;

    size_t        ramAddress;
};

//

struct NameTableContainer
{
    char*         name;

    size_t        nameLength;

    MemoryType    type;

    MemoryAddress addr;
};

struct NameTable
{
    NameTableContainer* container;

    char*               name;

    size_t              length;

    size_t              size;

    size_t              capacity;
};

struct Backend
{
    FILE* outFile;

    NameTable* nameTables;

    size_t size;

    size_t capacity;

    size_t curNameTable;

    size_t level = 0;
};

//

ErrorCode RunBackend                       (Node* astRoot, const char* outFileName);

ErrorCode BackendCreate                    (Backend* be);
               
ErrorCode BackendDestroy                   (Backend* be);
               
ErrorCode assemblePreamble                 (Backend* be);

ErrorCode Assemble                         (Backend* be, Node* node);

ErrorCode assembleConst                    (Backend* be, Node* node);

ErrorCode assembleOperation                (Backend* be, Node* node);

ErrorCode assembleReturn                   (Backend* be, Node* node);

ErrorCode assemblePrint                    (Backend* be, Node* node);

ErrorCode assembleInput                    (Backend* be, Node* node);

ErrorCode assembleFunctionCall             (Backend* be, Node* node);

ErrorCode stackFramePrologue               (Backend* be, NameTable* nameTable);

ErrorCode stackFrameEpilogue               (Backend* be, NameTable* nameTable);

ErrorCode assembleLogicalOperation         (Backend* be, Node* node);

ErrorCode assembleAssign                   (Backend* be, Node* node);
     
ErrorCode assembleVariable                 (Backend* be, Node* node);
     
ErrorCode assembleFunctionDeclaration      (Backend* be, Node* node);

ErrorCode pushFunctionArgumentsToNameTable (Backend* be, Node* node);

ErrorCode pushFunctionCallArguments        (Backend* be, Node* node);

ErrorCode assembleWhile                    (Backend* be, Node* node);

ErrorCode assembleIf                       (Backend* be, Node* node);

ErrorCode pushToNameTable                  (Backend* be, NameTable* nameTable, char* name, size_t nameLength);

ErrorCode resetNameTables                  (Backend* be);

ErrorCode resetNameTable                   (NameTable* nameTable);

ErrorCode downgradeNameTable               (Backend* be);

ErrorCode upgradeNameTable                 (Backend* be);

NameTableContainer* findNameTableContainer (Backend* be, char* name, size_t nameLength);
size_t              sumNameTableSizes      (Backend* be);

//

#endif