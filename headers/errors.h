#ifndef ERRORS_H
#define ERRORS_H

typedef int ErrorCode;

enum Error
{
    OK,
    NULL_PTR,
    NO_MEMORY,
    REPEAT_INSERT_LEFT,
    REPEAT_INSERT_RIGHT,
    TREE_LINKING_ERROR,
    UNKNOWN_POSITION,
    UNABLE_TO_OPEN_FILE,
    UNRECOGNISED_TOKEN,
    SYNTAX_ERROR,
    UNKNOWN_NAME,
    UNKNOWN_MODE,
    INDEX_OUT_OF_RANGE
};

#endif