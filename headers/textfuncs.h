//! @file
//! @author Aiden Munro

#ifndef TEXTFUNCS_H
#define TEXTFUNCS_H

#include "errors.h"

/**
 * 
 * Enum for file correction codes.
 * 
 */

enum FileApproval
{
    INCORRECT = 0,
    CORRECT   = 1
};

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
 * \brief Creates a text.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * 
 */

ErrorCode CreateText (Text* text, const char* filename);

/**
 * 
 * \brief Appends the text to a file.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * 
 */

ErrorCode DestroyText (Text* text);

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

size_t getSize (const char* filename);

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

char* parseBuf (Text* text, const char* filename);

#endif