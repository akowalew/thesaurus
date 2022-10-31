#include "common.cpp"

static void
AssertVectorsEqualCheck(std::vector<std::string>* A, std::vector<std::string> B, const char* File, int Line, const char* AExpr, const char* BExpr)
{
    std::set<std::string> AA = {A->begin(), A->end()};
    std::set<std::string> BB = {B.begin(), B.end()};
    if(A->size() != B.size() ||
       AA != BB)
    {
        fprintf(stderr, "Assert failed at %s:%d - Vectors '%s' and '%s' are not equal\n", File, Line, AExpr, BExpr);

        for(std::string& Item : *A)
        {
            fprintf(stderr, "\"%s\", ", Item.c_str());
        }

        fprintf(stderr, "\n... comparing to ...\n");

        for(std::string& Item : B)
        {
            fprintf(stderr, "\"%s\", ", Item.c_str());
        }

        fprintf(stderr, "\n");

        exit(-1);
    }
}

#define AssertVectorsEqual(A, B) AssertVectorsEqualCheck(A, B, __FILE__, __LINE__, #A, #B)

int main(int Argc, char** Argv)
{
    {
        CThesaurus Thesaurus;
        Assert(Thesaurus.GetAllWords()->empty());
        Assert(!Thesaurus.GetSynonyms("good"));
        Assert(!Thesaurus.GetSynonyms("great"));
        Assert(!Thesaurus.GetSynonyms("fabolous"));
    }

    {
        CThesaurus Thesaurus;

        std::vector<std::string> Synonyms = {"good", "great", "fabolous"};

        Thesaurus.AddSynonyms(&Synonyms);

        AssertVectorsEqual(Thesaurus.GetAllWords(), std::vector<std::string>({"fabolous", "good", "great"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("good"), std::vector<std::string>({"fabolous", "great"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("great"), std::vector<std::string>({"fabolous", "good"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("fabolous"), std::vector<std::string>({"good", "great"}));
    }

    {
        CThesaurus Thesaurus;

        std::vector<std::string> Synonyms = {"good", "Good", "gOoD", "GOOD"};

        Thesaurus.AddSynonyms(&Synonyms);

        AssertVectorsEqual(Thesaurus.GetAllWords(), std::vector<std::string>({"good"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("good"), std::vector<std::string>());
        AssertVectorsEqual(Thesaurus.GetSynonyms("Good"), std::vector<std::string>());
        AssertVectorsEqual(Thesaurus.GetSynonyms("gOoD"), std::vector<std::string>());
        AssertVectorsEqual(Thesaurus.GetSynonyms("GOOD"), std::vector<std::string>());
    }

    {
        CThesaurus Thesaurus;

        std::vector<std::string> Synonyms = {"good", "great", "fabolous", "good", "fabolous", "great"};

        Thesaurus.AddSynonyms(&Synonyms);

        AssertVectorsEqual(Thesaurus.GetAllWords(), std::vector<std::string>({"fabolous", "good", "great"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("good"), std::vector<std::string>({"fabolous", "great"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("great"), std::vector<std::string>({"fabolous", "good"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("fabolous"), std::vector<std::string>({"good", "great"}));
    }

    {
        CThesaurus Thesaurus;

        std::vector<std::string> SynonymsA = {"good", "great", "fabolous"};
        std::vector<std::string> SynonymsB = {"good", "great", "fabolous"};

        Thesaurus.AddSynonyms(&SynonymsA);
        Thesaurus.AddSynonyms(&SynonymsB);

        AssertVectorsEqual(Thesaurus.GetAllWords(), std::vector<std::string>({"fabolous", "good", "great"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("good"), std::vector<std::string>({"fabolous", "great"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("great"), std::vector<std::string>({"fabolous", "good"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("fabolous"), std::vector<std::string>({"good", "great"}));
    }

    {
        CThesaurus Thesaurus;

        std::vector<std::string> SynonymsA = {"good", "great", "fabolous"};
        std::vector<std::string> SynonymsB = {"nice", "extra", "excellent"};
        std::vector<std::string> SynonymsC = {"good", "nice"};

        Thesaurus.AddSynonyms(&SynonymsA);
        Thesaurus.AddSynonyms(&SynonymsB);
        Thesaurus.AddSynonyms(&SynonymsC);

        AssertVectorsEqual(Thesaurus.GetAllWords(), std::vector<std::string>({"excellent", "extra", "fabolous", "good", "great", "nice"}));

        AssertVectorsEqual(Thesaurus.GetSynonyms("good"), std::vector<std::string>({"great", "fabolous", "nice"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("great"), std::vector<std::string>({"fabolous", "good"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("fabolous"), std::vector<std::string>({"good", "great"}));

        AssertVectorsEqual(Thesaurus.GetSynonyms("nice"), std::vector<std::string>({"extra", "excellent", "good"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("extra"), std::vector<std::string>({"excellent", "nice"}));
        AssertVectorsEqual(Thesaurus.GetSynonyms("excellent"), std::vector<std::string>({"nice", "extra"}));
    }

    puts("Test passed");
}
