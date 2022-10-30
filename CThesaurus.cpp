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
    for(size_t Idx = 0;
        Idx < Count;
        Idx++)
    {
        // TODO: Trim
        // TODO: Normalize whitespace
        ConvertStringToLowerCase(Synonyms[Idx]);
    }

    for(size_t LeftIdx = 0;
        LeftIdx < Count;
        LeftIdx++)
    {
        char* LeftWord = FindOrCreateWord(Synonyms[LeftIdx]);
        std::set<char*>& LeftSynonymsSet = mItems[LeftWord];

        bool LeftSynonymsVectorFilled = false;
        std::vector<char*> LeftSynonymsVector;

        for(size_t RightIdx = 0;
            RightIdx < Count;
            RightIdx++)
        {
            if(LeftIdx != RightIdx)
            {
                char* RightWord = FindOrCreateWord(Synonyms[RightIdx]);
                if(strcmp(LeftWord, RightWord) != 0 &&
                   LeftSynonymsSet.count(RightWord) == 0)
                {
                    if(!LeftSynonymsVectorFilled)
                    {
                         LeftSynonymsVector = {LeftSynonymsSet.begin(), LeftSynonymsSet.end()};
                         LeftSynonymsVectorFilled = true;
                    }

                    std::set<char*>& RightSynonymsSet = mItems[RightWord];
                    std::vector<char*> RightSynonymsVector = {RightSynonymsSet.begin(), RightSynonymsSet.end()};

                    LeftSynonymsSet.insert(RightWord);
                    for(char* LeftSynonym : LeftSynonymsVector)
                    {
                        std::set<char*>& LeftSynonymSynonymsSet = mItems[LeftSynonym];

                        if(LeftSynonym != RightWord)
                        {
                            LeftSynonymSynonymsSet.insert(RightWord);

                            RightSynonymsSet.insert(LeftSynonym);
                        }

                        for(char* RightSynonym : RightSynonymsVector)
                        {
                            if(LeftSynonym != RightSynonym)
                            {
                                LeftSynonymSynonymsSet.insert(RightSynonym);
                            }
                        }
                    }

                    RightSynonymsSet.insert(LeftWord);
                    for(char* RightSynonym : RightSynonymsVector)
                    {
                        std::set<char*>& RightSynonymSynonymsSet = mItems[RightSynonym];

                        if(RightSynonym != LeftWord)
                        {
                            RightSynonymSynonymsSet.insert(LeftWord);

                            LeftSynonymsSet.insert(RightSynonym);
                        }

                        for(char* LeftSynonym : LeftSynonymsVector)
                        {
                            if(RightSynonym != LeftSynonym)
                            {
                                RightSynonymSynonymsSet.insert(LeftSynonym);
                            }
                        }
                    }
                }
            }
        }
    }
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

            printf("%s ", SynonymBegin);
        }

        AddSynonymsRaw(Synonyms, SynonymsCount);

        printf("\n");

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
        if(Elapsed < sizeof(uint8_t))
        {
            // TODO: Logging
            return false;
        }

        uint8_t M = *(uint8_t*)(At);
        At += sizeof(uint8_t);
        Elapsed -= sizeof(uint8_t);

        char* Synonyms[256];

        for(size_t Col = 0;
            Col < M;
            Col++)
        {
            Synonyms[Col] = At;

            char* WordEnd = (char*) memchr(At, '\0', Elapsed);
            if(!WordEnd)
            {
                // TODO: Logging
                return false;
            }

            Elapsed -= ((WordEnd + 1) - At);
            At = WordEnd + 1;
        }

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
        std::set<char*>& Synonyms = ItemAt->second;

        size_t M = Synonyms.size() + 1;
        if(M > 0xFF)
        {
            // TODO: Logging
            return false;
        }

        if(Elapsed < sizeof(uint8_t))
        {
            // TODO: Logging
            return false;
        }

        *(uint8_t*)(At) = (uint8_t) M;
        At += sizeof(uint8_t);
        Elapsed -= sizeof(uint8_t);

        size_t WordLength = strlen(Word);
        if(Elapsed < WordLength)
        {
            // TODO: Logging
            return false;
        }

        memcpy(At, Word, WordLength);
        At += WordLength;
        Elapsed -= WordLength;

        for(char* Synonym : Synonyms)
        {
            size_t SynonymLength = strlen(Synonym);
            if(Elapsed < SynonymLength)
            {
                // TODO: Logging
                return false;
            }

            memcpy(At, Synonym, SynonymLength);
            At += SynonymLength;
            Elapsed -= SynonymLength;
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
