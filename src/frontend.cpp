#include "frontend.h"

ErrorCode CreateFrontend (Frontend* fe, const char* inFileName)
{
    AssertSoft (inFileName, NONE_EXIST);
    AssertSoft (fe,         NULL_PTR);

    Text frontEndSrc = {};

    CreateText (&frontEndSrc, inFileName);

    fe->frontEndSrc = frontEndSrc;

    fe->tkns        = getTokens  (&frontEndSrc);

    fe->astNode     = GetGrammar (&fe->tkns);

    DumpTreeGraph (fe->astNode);

    return OK;
}

ErrorCode DestroyFrontend (Frontend* fe)
{
    AssertSoft (fe, NULL_PTR);


    free (fe->tkns.token);

    ErrorCode err = deleteNode(fe->astNode);

    if (err)
    {
        return err;
    }

    err = DestroyText (&fe->frontEndSrc);

    return err;
}