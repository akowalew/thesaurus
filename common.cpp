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

static void*
ReadEntireFile(char* FileName, size_t* BytesRead)
{
    char* Result = 0;

    FILE* File;
    if(!fopen_s(&File, FileName, "rb"))
    {
        fseek(File, 0, SEEK_END);
        int OffsetToEnd = ftell(File);
        fseek(File, 0, SEEK_SET);

        if(OffsetToEnd > 0)
        {
            size_t FileSize = (size_t) OffsetToEnd;
            char* FileData = (char*)malloc(FileSize);
            if(FileData)
            {
                *BytesRead = fread(FileData, 1, FileSize, File);
                Result = FileData;
            }
        }

        fclose(File);
    }

    return Result;
}

static char*
ReadEntireFileAndNullTerminate(char* FileName, size_t* BytesRead)
{
    char* Result = 0;

    FILE* File;
    if(!fopen_s(&File, FileName, "r"))
    {
        fseek(File, 0, SEEK_END);
        int OffsetToEnd = ftell(File);
        fseek(File, 0, SEEK_SET);

        if(OffsetToEnd > 0)
        {
            size_t FileSize = (size_t) OffsetToEnd;
            char* FileData = (char*) malloc(FileSize+1);
            if(FileData)
            {
                size_t Count = fread(FileData, 1, FileSize, File);
                FileData[Count] = '\0';
                *BytesRead = Count;
                Result = FileData;
            }
        }

        fclose(File);
    }

    return Result;
}

static bool
WriteEntireFile(char* FileName, void* FileData, size_t FileSize)
{
    bool Result = 0;

    FILE* File;
    if(!fopen_s(&File, FileName, "wb"))
    {
        size_t BytesWritten = fwrite(FileData, 1, FileSize, File);
        if(BytesWritten == FileSize)
        {
            Result = 1;
        }

        fclose(File);
    }

    return Result;
}

static char
ToLowerCase(char C)
{
    if(C >= 'A' &&
       C <= 'Z')
    {
        return C - 'A' + 'a';
    }
    else
    {
        return C;
    }
}

static void
ConvertStringToLowerCase(char* String)
{
    char* At = String;
    while(*At)
    {
        // TODO: SIMD this

        *At = (*At >= 'A' && *At <= 'Z') ? (*At - 'A' + 'a') : *At;

        At++;
    }
}
