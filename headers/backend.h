#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "utils.h"

//

size_t NUMBER_OF_CONTAINERS = 32;

size_t NUMBER_OF_NAMETABLES = 32;

size_t RAM_START_NUMBER     = 1000;

//

enum MemoryType
{
    REG = 0,
    RAM = 1
};

union MemoryAddress
{
    unsigned char regId;

    size_t        ramAddress;
};

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

    size_t size;

    size_t capacity;

    size_t hash;
};

struct Backend
{
    FILE* outFile;

    NameTable* nameTables;

    size_t size;

    size_t capacity;

};

#endif