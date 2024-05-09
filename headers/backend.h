#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "utils.h"

//

size_t NUMBER_OF_CONTAINERS = 32;   // amount of variables

size_t NUMBER_OF_NAMETABLES = 8;    // scope levels

size_t RAM_START_NUMBER     = 1000; // RAM starting point 

//

enum MemoryType
{
    REG = 0,
    RAM = 1
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

ErrorCode BackendCreate                    (Backend* be);
               
ErrorCode BackendDestroy                   (Backend* be);
               
ErrorCode Assemble                         (Backend* be, Node* node);

ErrorCode assembleLogicalOperation         (Backend* be, Node* node);

ErrorCode assembleAssign                   (Backend* be, Node* node);
     
ErrorCode assembleVariable                 (Backend* be, Node* node);
     
ErrorCode assembleFunctionDeclaration      (Backend* be, Node* node);

ErrorCode pushFunctionArgumentsToNameTable (Backend* be, Node* node);

ErrorCode assembleWhile                    (Backend* be, Node* node);

ErrorCode assembleIf                       (Backend* be, Node* node);

ErrorCode pushToNameTable                  (NameTable* nameTable, char* name, size_t nameLength);

NameTableContainer* findNameTableContainer (NameTable* nameTable, char* name, size_t nameLength)







#endif