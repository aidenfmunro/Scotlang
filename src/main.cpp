#include "frontend.h"
#include "backend.h"

int main (int argc, char* argv[])
{
    Frontend fe = {};

    if (argv[1])
    {
        CreateFrontend (&fe, argv[1]);
    }
    else
    {
        printf ("You might of forgot to type the Scotlang filename... \n");

        return NONE_EXIST;
    }

    if (argv[2])
    {
        RunBackend (fe.astNode, argv[2]);
    }
    else
    {
        printf ("You might of forgot to type the asm output filename...\n");
        
        return NONE_EXIST;
    }

    DestroyFrontend (&fe);

    return OK;
}