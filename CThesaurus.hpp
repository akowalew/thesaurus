#pragma once

class CThesaurus
{
public:
    CThesaurus();
    ~CThesaurus();

    void AddSynonyms(std::vector<std::string>* Synonyms);
    std::vector<std::string>* GetSynonyms(std::string Word);
    std::vector<std::string>* GetAllWords();

private:
    std::unordered_map<std::string, std::set<std::string>> mItems;
};
