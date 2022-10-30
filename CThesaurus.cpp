CThesaurus::CThesaurus()
{
    // Do nothing
}

CThesaurus::~CThesaurus()
{
    // Do nothing
}

char*
CThesaurus::FindOrCreateWord(char* Word)
{
    auto WordAt = mItems.find(Word);
    if(WordAt == mItems.end())
    {
        size_t WordLength = strlen(Word);
        char* Result = (char*) malloc(WordLength + 1);
        memcpy(Result, Word, WordLength+1);

        mItems[Result] = {};

        return Result;
    }
    else
    {
        return WordAt->first;
    }
}

void
CThesaurus::AddSynonymsRaw(char** Synonyms, size_t Count)
{
    if(Count == 0)
    {
        return;
    }

    for(size_t Idx = 0;
        Idx < Count;
        Idx++)
    {
        // TODO: Trim
        // TODO: Normalize whitespace
        ConvertStringToLowerCase(Synonyms[Idx]);
    }

#ifdef SYNONYM_OF_ANOTHER_SYNONYM_IS_ALSO_A_SYNONYM_NOOOOO
    char* LeftWord = FindOrCreateWord(Synonyms[0]);

    for(size_t Idx = 1;
        Idx < Count;
        Idx++)
    {
        char* RightQuery = Synonyms[Idx];

        if(strcmp(LeftWord, RightQuery) == 0)
        {
            continue;
        }

        char* RightWord = FindOrCreateWord(RightQuery);

        std::vector<char*>& RightSynonyms = mItems[RightWord];
        auto RightSize = RightSynonyms.size();

        if(std::count(RightSynonyms.begin(), RightSynonyms.end(), LeftWord))
        {
            // Already synonyms
            continue;
        }

        std::vector<char*>& LeftSynonyms = mItems[LeftWord];
        auto LeftSize = LeftSynonyms.size();

        for(size_t LeftIdx = 0;
            LeftIdx < LeftSize;
            LeftIdx++)
        {
            char* LeftSynonym = LeftSynonyms[LeftIdx];

            std::vector<char*>& LeftSynonymSynonyms = mItems[LeftSynonym];

            size_t LeftSynonymSynonymsSize = LeftSynonymSynonyms.size();
            LeftSynonymSynonyms.resize(LeftSynonymSynonymsSize+1+RightSize);
            LeftSynonymSynonyms[LeftSynonymSynonymsSize] = RightWord;
            memcpy(&LeftSynonymSynonyms[LeftSynonymSynonymsSize+1], &RightSynonyms[0], RightSize*sizeof(char*));
        }

        RightSynonyms.resize(RightSize+1+LeftSize);
        RightSynonyms[RightSize] = LeftWord;
        memcpy(&RightSynonyms[RightSize+1], &LeftSynonyms[0], LeftSize*sizeof(char*));

        for(size_t RightIdx = 0;
            RightIdx < RightSize;
            RightIdx++)
        {
            char* RightSynonym = RightSynonyms[RightIdx];

            std::vector<char*>& RightSynonymSynonyms = mItems[RightSynonym];

            size_t RightSynonymSynonymsSize = RightSynonymSynonyms.size();
            RightSynonymSynonyms.resize(RightSynonymSynonymsSize+1+LeftSize);
            RightSynonymSynonyms[RightSynonymSynonymsSize] = LeftWord;
            memcpy(&RightSynonymSynonyms[RightSynonymSynonymsSize+1], &LeftSynonyms[0], LeftSize*sizeof(char*));
        }

        LeftSynonyms.resize(LeftSize+1+RightSize);
        LeftSynonyms[LeftSize] = RightWord;
        memcpy(&LeftSynonyms[LeftSize+1], &RightSynonyms[0], RightSize*sizeof(char*));
    }
#else
    for(size_t LeftIdx = 0;
        LeftIdx < Count;
        LeftIdx++)
    {
        char* LeftQuery = Synonyms[LeftIdx];
        char* LeftWord = FindOrCreateWord(LeftQuery);

        for(size_t RightIdx = 0;
            RightIdx < Count;
            RightIdx++)
        {
            if(LeftIdx == RightIdx)
            {
                continue;
            }

            char* RightQuery = Synonyms[RightIdx];

            if(strcmp(LeftQuery, RightQuery) == 0)
            {
                continue;
            }

            char* RightWord = FindOrCreateWord(RightQuery);

            std::vector<char*>& LeftSynonyms = mItems[LeftWord];

            if(std::count(LeftSynonyms.begin(), LeftSynonyms.end(), RightWord))
            {
                continue;
            }

            std::vector<char*>& RightSynonyms = mItems[RightWord];

            RightSynonyms.push_back(LeftWord);
            LeftSynonyms.push_back(RightWord);
        }
    }
#endif
}

void
CThesaurus::AddSynonyms(std::vector<std::string>* Synonyms)
{
    std::vector<char*> Vector(Synonyms->size());
    for(size_t Idx = 0;
        Idx < Synonyms->size();
        Idx++)
    {
        Vector[Idx] = &((*Synonyms)[Idx])[0];
    }

    AddSynonymsRaw(&Vector[0], Synonyms->size());
}

std::vector<std::string>*
CThesaurus::GetSynonymsRaw(char* Word)
{
    ConvertStringToLowerCase(Word);

    auto At = mItems.find(Word);
    if(At != mItems.end())
    {
        auto Result = new std::vector<std::string>();
        Result->reserve(At->second.size());
        for(char* Synonym : At->second)
        {
            Result->push_back(Synonym);
        }

        return Result;
    }

    return 0;
}

std::vector<std::string>*
CThesaurus::GetSynonyms(std::string Word)
{
    return GetSynonymsRaw(&Word[0]);
}

std::vector<std::string>*
CThesaurus::GetAllWords()
{
    auto Result = new std::vector<std::string>();
    Result->reserve(mItems.size());
    for(auto& [Key, _] : mItems)
    {
        Result->push_back(Key);
    }

    return Result;
}

bool CThesaurus::ImportFromWordNetJson(char* FileName)
{
    size_t FileSize;
    char* FileData = (char*) ReadEntireFileAndNullTerminate(FileName, &FileSize);
    if(!FileData)
    {
        // TODO: Logging
        return false;
    }

    char* At = FileData;

    while(*At)
    {
        char* Synonyms[512];
        size_t SynonymsCount = 0;

        static const char WORD_MARKER[] = "\"word\": ";
        char* WordMarkerBegin = strstr(At, WORD_MARKER);
        if(!WordMarkerBegin)
        {
            // TODO: Logging
            return false;
        }

        char* WordBegin = WordMarkerBegin + 1;
        char* WordEnd = strchr(WordBegin, '"');
        if(!WordEnd)
        {
            // TODO: Logging
            return false;
        }

        *WordEnd = '\0';

        Synonyms[SynonymsCount] = WordBegin;
        SynonymsCount++;

        At = WordEnd + 1;

        static const char SYNONYMS_MARKER[] = "\"synonyms\": [";
        char* SynonymsMarkerBegin = strstr(At, SYNONYMS_MARKER);
        if(!SynonymsMarkerBegin)
        {
            // TODO: Logging
            return false;
        }

        At = SynonymsMarkerBegin + sizeof(SYNONYMS_MARKER) - 1;

        while(*At != ']')
        {
            char* SynonymStringBegin = strchr(At, '"');
            if(!SynonymStringBegin)
            {
                // TODO: Logging
                return false;
            }

            char* SynonymBegin = SynonymStringBegin + 1;
            char* SynonymEnd = strchr(SynonymBegin, '"');
            if(!SynonymEnd)
            {
                // TODO: Logging
                return false;
            }

            *SynonymEnd = '\0';

            Synonyms[SynonymsCount] = SynonymBegin;
            SynonymsCount++;
            if(SynonymsCount >= ArrayLength(Synonyms))
            {
                // TODO: Logging
                return false;
            }

            At = SynonymEnd + 1;
        }

        AddSynonymsRaw(Synonyms, SynonymsCount);

        char* EndOfLineMarker = strchr(At, '\n');
        if(!EndOfLineMarker)
        {
            // TODO: Logging
            return false;
        }

        At = EndOfLineMarker + 1;
    }

    return true;
}

bool CThesaurus::LoadFromBuffer(void* Data, size_t Size)
{
    size_t Elapsed = Size;
    char* At = (char*)Data;

    if(Elapsed < sizeof(uint32_t))
    {
        // TODO: Logging
        return false;
    }

    uint32_t N = *(uint32_t*)(At);
    At += sizeof(uint32_t);
    Elapsed -= sizeof(uint32_t);

    for(size_t Row = 0;
        Row < N;
        Row++)
    {
        if(Elapsed < sizeof(uint32_t))
        {
            // TODO: Logging
            return false;
        }

        uint32_t M = *(uint32_t*)(At);
        At += sizeof(uint32_t);
        Elapsed -= sizeof(uint32_t);

        char* Synonyms[4096];

        if(M > ArrayLength(Synonyms))
        {
            // TODO: Logging
            return false;
        }

        for(size_t Col = 0;
            Col < M;
            Col++)
        {
            Synonyms[Col] = At;

            printf("%s ", At);

            char* WordEnd = (char*) memchr(At, '\0', Elapsed);
            if(!WordEnd)
            {
                // TODO: Logging
                return false;
            }

            Elapsed -= ((WordEnd + 1) - At);
            At = WordEnd + 1;
        }

        printf("\n");

        AddSynonymsRaw(Synonyms, M);
    }

    return true;
}

bool CThesaurus::LoadFromFile(char* FileName)
{
    bool Result = 0;

    size_t FileSize;
    void* FileData = ReadEntireFile(FileName, &FileSize);
    if(FileData)
    {
        if(LoadFromBuffer(FileData, FileSize))
        {
            Result = 1;
        }

        free(FileData);
    }

    return Result;
}

bool CThesaurus::SaveToBuffer(void* Data, size_t Size, size_t* BytesWritten)
{
    char* At = (char*) Data;
    size_t Elapsed = Size;

    size_t N = mItems.size();
    if(N > 0xFFFFFFFF)
    {
        return false;
    }

    if(Elapsed < sizeof(uint32_t))
    {
        // TODO: Logging
        return false;
    }

    *(uint32_t*)(At) = (uint32_t) N;
    At += sizeof(uint32_t);
    Elapsed -= sizeof(uint32_t);

    auto ItemsEnd = mItems.end();
    for(auto ItemAt = mItems.begin();
        ItemAt != ItemsEnd;
        ItemAt++)
    {
        char* Word = ItemAt->first;
        std::vector<char*>& Synonyms = ItemAt->second;

        size_t M = Synonyms.size() + 1;
        if(M > 0xFFFFFFFF)
        {
            // TODO: Logging
            return false;
        }

        if(Elapsed < sizeof(uint32_t))
        {
            // TODO: Logging
            return false;
        }

        *(uint32_t*)(At) = (uint32_t) M;
        At += sizeof(uint32_t);
        Elapsed -= sizeof(uint32_t);

        size_t WordLength = strlen(Word);
        if(Elapsed < WordLength)
        {
            // TODO: Logging
            return false;
        }

        memcpy(At, Word, (WordLength+1));
        At += (WordLength+1);
        Elapsed -= (WordLength+1);

        for(char* Synonym : Synonyms)
        {
            size_t SynonymLength = strlen(Synonym);
            if(Elapsed < SynonymLength)
            {
                // TODO: Logging
                return false;
            }

            memcpy(At, Synonym, (SynonymLength+1));
            At += (SynonymLength+1);
            Elapsed -= (SynonymLength+1);
        }
    }

    *BytesWritten = (Size - Elapsed);

    return true;
}

bool CThesaurus::SaveToFile(char* FileName)
{
    bool Result = 0;

    size_t MaxFileSize = 1 * 1024 * 1024 * 1024;
    void* FileData = malloc(MaxFileSize);
    if(FileData)
    {
        size_t FileSize;
        if(SaveToBuffer(FileData, MaxFileSize, &FileSize))
        {
            if(WriteEntireFile(FileName, FileData, FileSize))
            {
                Result = 1;
            }
        }
        else
        {
            // TODO: Logging
        }

        free(FileData);
    }
    else
    {
        // TODO: Logging
    }

    return Result;
}
