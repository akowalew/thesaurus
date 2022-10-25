#include "common.cpp"
#include "CThesaurus.hpp"
#include "CThesaurus.cpp"

static bool
IsAlpha(char C)
{
    if((C >= 'a' &&
        C <= 'z') ||
       (C >= 'A' &&
        C <= 'Z'))
    {
        return true;
    }
    else
    {
        return false;
    }
}

constexpr char* HelpString =
R"(
This program helps you finding synonyms for words
Syntax is as below:
    <WORD>                 - Gets synonyms for given WORD
    +<WORD1>,<WORD2>,...   - Adds new synonyms
    *                      - Retrieves all stored words
    ?                      - Prints this message
)";

int 
main(int Argc, char** Argv)
{
    puts("Welcome to Thesaurus!");
    puts(HelpString);

    CThesaurus Thesaurus;

    while(1)
    {
        printf("> ");

        static char Command[4096];
        if(!fgets(Command, sizeof(Command), stdin))
        {
            break;
        }

        char C = Command[0];
        switch(C)
        {
            case '?':
            {
                puts(HelpString);
            } break;

            case '*':
            {
                char* AllWords = Thesaurus.GetAllWords();
                puts(AllWords);
            } break;

            case '+':
            {
                char* Synonyms = &Command[1];
                if(Thesaurus.AddSynonyms(Synonyms))
                {
                    printf("OK\n");
                }
                else
                {
                    printf("FAIL\n");
                }
            } break;

            default:
            {
                if(IsAlpha(C))
                {
                    char* Word = Command;

                    int SynonymsCount;
                    char** Synonyms = Thesaurus.GetSynonyms(Word, &SynonymsCount);

                    if(SynonymsCount)
                    {
                        printf("%s", Synonyms[0]);
                        for(int Idx = 1;
                            Idx < SynonymsCount;
                            Idx++)
                        {
                            printf(", %s", Synonyms[Idx]);
                        }
                        putchar('\n');
                    }
                    else
                    {
                        printf("No synonyms for given word\n");
                    }
                }
                else if(C != '\n')
                {
                    printf("Error: invalid command\n");
                }
            } break;
        }
    }

    printf("Bye!\n");

    return EXIT_SUCCESS;
}
