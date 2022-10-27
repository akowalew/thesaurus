#include "common.cpp"

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

    static char Buffer[4096];

    while(1)
    {
        printf("> ");

        if(!fgets(Buffer, sizeof(Buffer), stdin))
        {
            break;
        }

        char* Command = strtok(Buffer, "\r\n");
        if(!Command)
        {
            continue;
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
                std::vector<std::string>* AllWords = Thesaurus.GetAllWords();
                if(AllWords->empty())
                {
                    puts("No words yet");
                }
                else
                {
                    for(std::string& Word : *AllWords)
                    {
                        puts(Word.c_str());
                    }
                }

                delete AllWords;
            } break;

            case '+':
            {
                std::vector<std::string> Synonyms;

                if(Command[1])
                {
                    static const char Delimiters[] = " ,.-\t\v";
                    char* Word = strtok(&Command[1], Delimiters);
                    while(Word)
                    {
                        Synonyms.push_back(Word);

                        Word = strtok(0, Delimiters);
                    }
                }
                else
                {
                    while(1)
                    {
                        printf("+");

                        if(!fgets(Buffer, sizeof(Buffer), stdin))
                        {
                            break;
                        }

                        char* Word = strtok(Buffer, "\r\n");
                        if(!Word)
                        {
                            break;
                        }

                        Synonyms.push_back(Word);
                    }
                }

                Thesaurus.AddSynonyms(&Synonyms);
            } break;

            default:
            {
                if(IsAlpha(C))
                {
                    std::vector<std::string>* Synonyms = Thesaurus.GetSynonyms(Command);
                    if(Synonyms)
                    {
                        if(Synonyms->empty())
                        {
                            puts("There are no synonyms for given word");
                        }
                        else
                        {
                            for(auto& Synonym : *Synonyms)
                            {
                                puts(Synonym.c_str());
                            }
                        }

                        delete Synonyms;
                    }
                    else
                    {
                        puts("Word not found");
                    }
                }
                else
                {
                    puts("Error: invalid command");
                }
            } break;
        }
    }

    puts("Bye!");

    return EXIT_SUCCESS;
}
