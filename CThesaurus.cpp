CThesaurus::CThesaurus()
{

}

CThesaurus::~CThesaurus()
{

}

bool
CThesaurus::AddSynonyms(const char* Synonyms)
{
    return true;
}

// char*
// CThesaurus::GetSynonyms(const char* Word)
// {
//     return "great,fabolous,good";
// }

char**
CThesaurus::GetSynonyms(const char* Word, int* Count)
{
    static char* Words0[] = {"great", "fabolous", "good"};
    static char* Words1[] = {"great", "good", "fabolous", "great", "good", "fabolous", "great", "good", "fabolous", "great", "good", "fabolous"};
    static char* Words2[] = {"fabolous", "great", "good"};
    static char* Words3[] = {"good", "fabolous", "great"};
    static int WordsIdx = 0;

    WordsIdx = (WordsIdx+1) & 3;
    switch(WordsIdx)
    {
        case 0: *Count = ArrayLength(Words0); return Words0;
        case 1: *Count = ArrayLength(Words1); return Words1;
        case 2: *Count = ArrayLength(Words2); return Words2;
        default: *Count = ArrayLength(Words3); return Words3;
    }
}

char*
CThesaurus::GetAllWords()
{
    return "great,fabolous,good";
}

