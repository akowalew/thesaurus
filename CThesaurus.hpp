#pragma once

struct string_less
{
    bool operator()(char* Left, char* Right) const
    {
        return (strcmp(Left, Right) < 0);
    }
};

class CThesaurus
{
public:
    CThesaurus();
    ~CThesaurus();

    void AddSynonymsRaw(char** Synonyms, size_t Count);
    void AddSynonymsVector(std::vector<char*>* Synonyms);
    std::vector<std::string>* GetSynonymsRaw(char* Word);

    void AddSynonyms(std::vector<std::string>* Synonyms);
    std::vector<std::string>* GetSynonyms(std::string Word);
    std::vector<std::string>* GetAllWords();

    bool ImportFromWordNetJson(char* FileName);

    bool LoadFromBuffer(void* Data, size_t Size);
    bool SaveToBuffer(void* Data, size_t Size, size_t* BytesWritten);

    bool LoadFromFile(char* FileName);
    bool SaveToFile(char* FileName);

private:
    char* FindOrCreateWord(char* Word);

    std::map<char*, std::set<char*>, string_less> mItems;
};
