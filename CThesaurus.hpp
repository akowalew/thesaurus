#pragma once

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

    bool ImportFromWordNetJsonL(char* FileName);

    bool LoadFromBuffer(void* Data, size_t Size);
    bool SaveToBuffer(void* Data, size_t Size, size_t* BytesWritten);

    bool LoadFromFile(char* FileName);
    bool SaveToFile(char* FileName);

private:
    std::unordered_map<std::string, std::set<std::string>> mItems;
};

