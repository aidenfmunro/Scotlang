//! @file
//! @author Aiden Munro

#ifndef TEXTFUNCS_H
#define TEXTFUNCS_H

#include <stdlib.h>

/**
 * 
 * Enum for sort modes.
 * 
 */

enum SORT
{
    BACKWARDS = 0,
    FORWARDS  = 1,
    NONE      = 2
};

/**
 * 
 * Enum for file correction codes.
 * 
 */

enum CORRECTFILE
{
    INCORRECT = 0,
    CORRECT   = 1
};

/**
 * 
 * SWAP
 * macro.
 * 
 */

#define SWAP(a, b, size)						                                           \
    do									                                                       \
      {                                                                        \
        char *_a = (char*)a, *_b = (char*)b;					                         \
        for (size_t i = 0; i < size; i++)								                       \
	        {								                                                     \
	          char _temp = _a[i];						                                     \
	          _a[i]= _b[i];						                                           \
	          _b[i] = _temp;						                                         \
	        } 						                                                       \
      } while (0)

/**
 * 
 * Assert macro.
 * 
 */


#define myAssert(EXPRESSION, ERROR_CODE)                                       \
    if (!(EXPRESSION))                                                         \
      {                                                                        \
        fprintf (stderr,"%s, failed at %s:%d\n", #ERROR_CODE                   \
        , __FILE__, __LINE__);                                                 \
        exit(ERROR_CODE);                                                      \
      }

/**
 * 
 * Text structure that contains line pointers, buffer, size & amount of tokens.
 * 
 */

struct Text 
{
    char*  buffer;
    size_t size;
};


/**
 * 
 * \brief Creates a text with the chosen sortmode.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * @param[in] sortmode - sort mode.
 * 
 */

void CreateText(Text* text, const char* filename, size_t sortmode);

/**
 * 
 * \brief Appends the text to a file.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * @param[in] sortmode - sort mode.
 * 
 */



void DestroyText(Text* text);

/**
 * 
 * \brief Validates the file name.
 * 
 * @param[in] filename - name of file.
 * 
 * \return file correction code.
 * 
 */

size_t CheckFile (const char* filename);

/**
 * 
 * \brief Gets the length of the file.
 * 
 * @param[in] filename - name of file.
 * @param[out] length - size of file.
 * 
 * \return size of file.
 * 
 */

size_t getSize(const char* filename);

/**
 * 
 * \brief Buffer parser.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * @param[out] buffer - parsed buffer.
 * 
 * \return buffer.
 * 
 */

char* parseBuf(Text* text, const char* filename);

#endif