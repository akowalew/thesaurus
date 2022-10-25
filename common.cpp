#include <stdlib.h>
#include <stdio.h>

#define ArrayLength(x) (sizeof(x)/sizeof((x)[0]))

static void
AssertFailed(const char* File, int Line, const char* Expression)
{
    fprintf(stderr, "Assert failed on %s:%d) %s is false\n", File, Line, Expression);

    *(int*)(0) = 0;
}

#define Assert(x) if(!(x)) { AssertFailed(__FILE__, __LINE__, #x); }

