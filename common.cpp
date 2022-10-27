#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <set>
#include "CThesaurus.hpp"
#include "CThesaurus.cpp"

#define ArrayLength(x) (sizeof(x)/sizeof((x)[0]))

static void
AssertFailed(const char* File, int Line, const char* Expression)
{
    fprintf(stderr, "Assert failed on %s:%d) %s is false\n", File, Line, Expression);

    *(int*)(0) = 0;
}

static void
AssertCheck(bool Result, const char* File, int Line, const char* Expression)
{
    if(!Result)
    {
        AssertFailed(File, Line, Expression);
    }
}

#define Assert(x) AssertCheck((x), __FILE__, __LINE__, #x)

