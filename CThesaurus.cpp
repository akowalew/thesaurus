static char ToLowerCase(char C)
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

static void ConvertStringToLowerCase(char* String)
{
    char* At = String;
    while(*At)
    {
        // TODO: SIMD this
        *At = ToLowerCase(*At);
        At++;
    }
}

CThesaurus::CThesaurus()
{
    // Do nothing
}

CThesaurus::~CThesaurus()
{
    // Do nothing
}

void
CThesaurus::AddSynonyms(std::vector<std::string>* Synonyms)
{
    for(std::string& Synonym : *Synonyms)
    {
        ConvertStringToLowerCase(&Synonym[0]);
    }

    for(size_t LeftIdx = 0;
        LeftIdx < Synonyms->size();
        LeftIdx++)
    {
        std::string& LeftWord = (*Synonyms)[LeftIdx];
        std::set<std::string>& LeftSynonymsSet = mItems[LeftWord];
        std::vector<std::string> LeftSynonymsVector = {LeftSynonymsSet.begin(), LeftSynonymsSet.end()};

        for(size_t RightIdx = 0;
            RightIdx < Synonyms->size();
            RightIdx++)
        {
            if(LeftIdx != RightIdx)
            {
                std::string& RightWord = (*Synonyms)[RightIdx];
                if(LeftWord != RightWord)
                {
                    std::set<std::string>& RightSynonymsSet = mItems[RightWord];
                    std::vector<std::string> RightSynonymsVector = {RightSynonymsSet.begin(), RightSynonymsSet.end()};

                    LeftSynonymsSet.insert(RightWord);
                    for(const std::string& LeftSynonym : LeftSynonymsVector)
                    {
                        std::set<std::string>& LeftSynonymSynonymsSet = mItems[LeftSynonym];

                        if(LeftSynonym != RightWord)
                        {
                            LeftSynonymSynonymsSet.insert(RightWord);

                            RightSynonymsSet.insert(LeftSynonym);
                        }

                        for(const std::string& RightSynonym : RightSynonymsVector)
                        {
                            if(LeftSynonym != RightSynonym)
                            {
                                LeftSynonymSynonymsSet.insert(RightSynonym);
                            }
                        }
                    }

                    RightSynonymsSet.insert(LeftWord);
                    for(const std::string& RightSynonym : RightSynonymsVector)
                    {
                        std::set<std::string>& RightSynonymSynonymsSet = mItems[RightSynonym];

                        if(RightSynonym != LeftWord)
                        {
                            RightSynonymSynonymsSet.insert(LeftWord);

                            LeftSynonymsSet.insert(RightSynonym);
                        }

                        for(const std::string& LeftSynonym : LeftSynonymsVector)
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

std::vector<std::string>*
CThesaurus::GetSynonyms(std::string Word)
{
    ConvertStringToLowerCase(&Word[0]);

    auto At = mItems.find(Word);
    if(At != mItems.end())
    {
        auto Result = new std::vector<std::string>();
        Result->reserve(At->second.size());
        for(const std::string& Synonym : At->second)
        {
            Result->push_back(Synonym);
        }

        return Result;
    }

    return 0;
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

