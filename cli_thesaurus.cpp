#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <array>
#include <chrono>
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
    WORD                   - Gets synonyms for given WORD
    +WORD1,WORD2,...       - Adds new synonyms
    *                      - Retrieves all stored words
    <FILENAME              - Loads synonyms from FILENAME into program
    >FILENAME              - Saves synonyms from program into FILENAME
    %FILENAME              - Imports synonyms from WordNet's JSON file
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
                bool Error = 0;
                size_t Count = 0;
                char* Synonyms[256];

                if(Command[1])
                {
                    static const char Delimiters[] = " ,.-\t\v";
                    char* Word = strtok(&Command[1], Delimiters);
                    while(Word)
                    {
                        if(Count >= ArrayLength(Synonyms))
                        {
                            Error = 1;
                            puts("Error: Too much words");
                            break;
                        }

                        Synonyms[Count] = Word;
                        Count++;

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

                        if(Count >= ArrayLength(Synonyms))
                        {
                            Error = 1;
                            puts("Error: Too much words");
                            break;
                        }

                        Synonyms[Count] = Word;
                        Count++;
                    }
                }

                if(!Error)
                {
                    Thesaurus.AddSynonymsRaw(Synonyms, Count);
                }
            } break;

            case '>':
            {
                if(Command[1])
                {
                    char* FileName = &Command[1];
                    if(Thesaurus.SaveToFile(FileName))
                    {
                        puts("OK");
                    }
                    else
                    {
                        puts("FAIL");
                    }
                }
                else
                {
                    puts("Error: missing output filename");
                }
            } break;

            case '<':
            {
                if(Command[1])
                {
                    char* FileName = &Command[1];
                    if(Thesaurus.LoadFromFile(FileName))
                    {
                        puts("OK");
                    }
                    else
                    {
                        puts("FAIL");
                    }
                }
                else
                {
                    puts("Error: missing input filename");
                }
            } break;

            case '%':
            {
                if(Command[1])
                {
                    auto start = std::chrono::steady_clock::now();

                    char* FileName = &Command[1];
                    if(Thesaurus.ImportFromWordNetJson(FileName))
                    {
                        puts("OK");
                    }
                    else
                    {
                        puts("FAIL");
                    }

                    auto stop = std::chrono::steady_clock::now();
                    printf("Operation took %lld seconds\n", std::chrono::duration_cast<std::chrono::seconds>(stop - start).count());
                }
            }

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
