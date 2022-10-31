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
    std::vector<std::string>* GetSynonymsRaw(char* Word);

    void AddSynonyms(std::vector<std::string>* Synonyms);
    std::vector<std::string>* GetSynonyms(std::string Word);
    std::vector<std::string>* GetAllWords();

private:
    char* FindOrCreateWord(char* Word);

    std::map<char*, std::set<char*>, string_less> mItems;
};

bool ImportThesaurusFromWordNetJsonBuffer(CThesaurus* Thesaurus, char* Buffer);
bool ImportThesaurusFromWordNetJsonFile(CThesaurus* Thesaurus, char* FileName);
