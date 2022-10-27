#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            char* FileData = malloc(FileSize+1);
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

#define Max(a, b) ((a) > (b)) ? (a) : (b)
#define Min(a, b) ((a) < (b)) ? (a) : (b)

int
main(int Argc, char** Argv)
{
    if(Argc < 2)
    {
        fprintf(stderr, "Syntax: %s <thesaurus_json>\n", Argv[0]);
        return EXIT_FAILURE;
    }

    char* FileName = Argv[1];

    size_t FileSize;
    char* FileData = ReadEntireFileAndNullTerminate(FileName, &FileSize);
    if(!FileData)
    {
        fprintf(stderr, "Failed to read file\n");
        return EXIT_FAILURE;
    }

    char* OutputData = malloc(FileSize);
    if(!OutputData)
    {
        fprintf(stderr, "Failed to alloc output data\n");
        return EXIT_FAILURE;
    }

    char* OutputAt = OutputData;

    size_t WordsCount = 0;
    size_t SynonymsCountMax = 0;
    size_t WordLengthMax = 0;
    size_t SynonymLengthMax = 0;

    char* At = FileData;
    while(*At)
    {
        static const char WORD_MARKER[] = "\"word\": \"";
        char* WordMarkerBegin = strstr(At, WORD_MARKER);
        if(!WordMarkerBegin)
        {
            fprintf(stderr, "Word marker not found\n");
            return EXIT_FAILURE;
        }

        char* WordBegin = WordMarkerBegin + sizeof(WORD_MARKER) - 1;
        char* WordEnd = strchr(WordBegin, '"');
        if(!WordEnd)
        {
            fprintf(stderr, "Word end not found\n");
            return EXIT_FAILURE;
        }

        size_t WordLength = WordEnd - WordBegin;
        memcpy(OutputAt, WordBegin, WordLength);
        OutputAt += WordLength;

        WordLengthMax = Max(WordLengthMax, WordLength);

        static const char SYNONYMS_MARKER[] = "\"synonyms\": [";
        char* SynonymsMarkerBegin = strstr(WordEnd, SYNONYMS_MARKER);
        if(!SynonymsMarkerBegin)
        {
            fprintf(stderr, "Synonyms marker not found\n");
            return EXIT_FAILURE;
        }

        At = SynonymsMarkerBegin + sizeof(SYNONYMS_MARKER) - 1;

        size_t SynonymsCount = 0;

        while(*At != ']')
        {
            char* SynonymStringBegin = strchr(At, '"');
            if(!SynonymStringBegin)
            {
                fprintf(stderr, "Synonym string begin not found\n");
                return EXIT_FAILURE;
            }

            char* SynonymBegin = SynonymStringBegin + 1;
            char* SynonymEnd = strchr(SynonymBegin, '"');
            if(!SynonymEnd)
            {
                fprintf(stderr, "Synonym end not found\n");
                return EXIT_FAILURE;
            }

            size_t SynonymLength = SynonymEnd - SynonymBegin;
            *(OutputAt++) = ',';
            memcpy(OutputAt, SynonymBegin, SynonymLength);
            OutputAt += SynonymLength;

            SynonymLengthMax = Max(SynonymLengthMax, SynonymLength);

            At = SynonymEnd + 1;

            SynonymsCount++;
        }

        SynonymsCountMax = Max(SynonymsCountMax, SynonymsCount);

        char* EndOfLineMarker = strchr(At, '\n');
        if(!EndOfLineMarker)
        {
            fprintf(stderr, "End of line marker not found\n");
            return EXIT_FAILURE;
        }

        At = EndOfLineMarker + 1;

        *(OutputAt++) = '\n';

        WordsCount++;
    }

    printf("%d\n", (int)WordsCount);
    printf("%d\n", (int)SynonymsCountMax);
    printf("%d\n", (int)WordLengthMax);
    printf("%d\n", (int)SynonymLengthMax);

    fwrite(OutputData, 1, OutputAt-OutputData, stdout);

    return EXIT_SUCCESS;
}
