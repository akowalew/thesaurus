char*
CThesaurus::FindOrCreateWord(char* Word)
{
    auto WordAt = mItems.find(Word);
    if(WordAt == mItems.end())
    {
        size_t WordLength = strlen(Word);
        char* Result = new char[WordLength + 1];
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

            auto& LeftSynonyms = mItems[LeftWord];

            if(LeftSynonyms.count(RightWord))
            {
                continue;
            }

            auto& RightSynonyms = mItems[RightWord];

            RightSynonyms.insert(LeftWord);
            LeftSynonyms.insert(RightWord);
        }
    }
}

void
CThesaurus::AddSynonyms(std::vector<std::string>* Synonyms)
{
    // NOTE: Just a helper - normally from app we use AddSynonymsRaw
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
        auto& Synonyms = At->second;

        auto Result = new std::vector<std::string>();
        Result->reserve(Synonyms.size());
        for(char* Synonym : Synonyms)
        {
            Result->push_back(Synonym);
        }

        std::sort(Result->begin(), Result->end());

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

bool ImportThesaurusFromWordNetJsonBuffer(CThesaurus* Thesaurus, char* Buffer)
{
    char* At = Buffer;

    while(*At)
    {
        char* Synonyms[512];
        size_t SynonymsCount = 0;

        static const char WORD_MARKER[] = "\"word\": \"";
        char* WordMarkerBegin = strstr(At, WORD_MARKER);
        if(!WordMarkerBegin)
        {
            // TODO: Logging
            return false;
        }

        char* WordBegin = WordMarkerBegin + sizeof(WORD_MARKER) - 1;
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

        Thesaurus->AddSynonymsRaw(Synonyms, SynonymsCount);

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

bool ImportThesaurusFromWordNetJsonFile(CThesaurus* Thesaurus, char* FileName)
{
    size_t FileSize;
    char* FileData = (char*) ReadEntireFileAndNullTerminate(FileName, &FileSize);
    if(!FileData)
    {
        // TODO: Logging
        return false;
    }

    bool Result = ImportThesaurusFromWordNetJsonBuffer(Thesaurus, FileData);

    delete[] FileData;

    return Result;
}
