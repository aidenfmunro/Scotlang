//! @file
//! @author Aiden Munro

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include "textfuncs.h"
#include "errors.h"

void CreateText(Text* text, const char* filename, size_t sortmode)
{
    myAssert(text, NULL_PTR);

    text->size     = getSize(filename);

    text->buffer   = parseBuf(text, filename);
}

void DestroyText(Text* text)
{
    free((void*)text->buffer);
}

char* parseBuf(Text* text, const char* filename)
{   
    myAssert(text, NULL_PTR);

    FILE* fp = fopen(filename, "rb");

    char* buffer = (char*)calloc(text->size + 1, sizeof(char));

    fread(buffer, sizeof(char), text->size, fp);

    buffer[text->size] = '\0';
  
    fclose(fp);

    return buffer;
}

size_t getSize(const char* filename)
{
    myAssert(filename, NULL_PTR);

    struct stat stats = {};
    stat(filename, &stats);

    return stats.st_size;
}

size_t CheckFile(const char* filename)
{
    myAssert(filename, NULL_PTR);

    FILE* fp = fopen(filename, "rb");
        if (fp == NULL)
          {
            printf("Unable to open file: \"%s\"\n", filename);
            return INCORRECT;
          }

    fclose(fp);

    const char* dotptr = strchr(filename, (int)('.')) + 1;

    if(strcmp("txt", dotptr) || strcmp("rtf", dotptr))
        return CORRECT;
      
    return INCORRECT;
}
