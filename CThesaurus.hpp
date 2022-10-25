#pragma once

class CThesaurus
{
public:
    CThesaurus();
    ~CThesaurus();

    bool AddSynonyms(const char* Synonyms);
    char* GetSynonyms(const char* Word);
    char** GetSynonyms(const char* Word, int* Count);
    char* GetAllWords();
};
