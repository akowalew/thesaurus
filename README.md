# Interview task - Thesaurus

![image](https://github.com/akowalew/thesaurus/assets/11333571/09100574-8e1d-4e07-8862-ebcadeb0199a)

![image](https://github.com/akowalew/thesaurus/assets/11333571/144412ef-6861-426a-aae2-6b4277aee33d)

In this directory you can find implementation of interview task, which was to write implementation for provided Thesaurus class, add some unit tests and example applications.

## Build and run

Open Visual Studio's developer's command prompt (x64), go to this directory and simply run `build.bat`. It will build win32 application, cli application and units tests, and after that it will run these tests.

If you want to compile using other compilers, you can also call `build_clang.bat` or `build_gcc.bat`.

Compilation is done using single-file-compilation-unit technique. In order to build an exe, all stuff are just #include-d into single .cpp file which is pushed right into compiler.

## CThesaurus class header

Initially the class provided had following body:

```c++
#pragma once

#include <vector>

using namespace std;

class CThesaurus
{
    public:

    CThesaurus();
    ~CThesaurus();

    //Adds the given synonyms to the thesaurus
    void AddSynonyms(vector<string>* pSynonyms);

    //Gets the synonyms for a given word
    vector<string>* GetSynonyms(string word);

    //Gets all words from the thesaurus
    vector<string>* GetAllWords();
};
```

Well, maybe it specifies how API should look like, but as you will take a look at final result, these methods are making final implementation much more difficult from the API users perspective. That's why I added other methods with 'Raw' suffix. But to be honest, I was thinking into going more deeper, by completely avoiding any memory allocation - because none is needed! But okay, client wanted to have std::vector, let he has...

## CThesaurus class implementation

There are many ways how to implement such a Thesaurus functionality. I decided to stick with rule "synonyms of my synonyms are not my synonyms". It simplified a lot whole implementation and speeded up loading an WordNet's database by orders of factors! I've decided to do it as simple as possible, by using `std::map` indexed by `char*` which has values as `std::set` of `char*`. I decided to avoid completely using internally std::string, because (1) for easier debugging and (2) for avoiding as much as possible memory allocations - I do not trust in any STL implementation about avoiding copies and move semantics.

How it works. We check if word is withing database. If not, then we malloc it, copy contents and assigns a **pointer** as synonym. This speedups further lookups massively (just compare 8-byte pointer instead of whole null-terminated words).

As mentioned, I downloaded from somewhere WordNet's database of most common English synonyms, in form of JSON file. I've implemented very extremely trivial parser of such file, which on my computer (mobile i9 11th gen) on bare debug msvc version (completely no optimizations) loads such JSON in 2 seconds. It can be optimized a lot, e.g. by not flushing data cache because of using null-terminated strings and by using SIMD to perform operations on such strings

## cli_thesaurus.exe

This is a simple command line application to present how thesaurus works. We can both lookup words but also add new synonyms. Very simple, strtok-based. By default app is running with empty dictionary, which we can manually fill or instead load JSON from WordNet

## win32_thesaurus.exe

Another very simple application but now Win32 based. I've glued couple of Windows Common Controls and some logic to make some really trivial Dictionary-like app. At startup it checks for JSON with database. If it exists then it loads it, if not, is asks for file location. After loading we have two lists boxes and one edit box. First contains list of all words, second list of found synonyms and third can be used as a word query. Also there is a simple status bar to present what is going on and some words counts info.

## Conclusion and improvement ideas

Well, everything can be improved, as always. About CThesaurus class I will for sure try to completely remove std::map and std::set and to have something locally allocated with local buffers, to be more cache-friendly. At the moment all these containers are mallocing everything which is crazy slow. I will stick to have like giant non-commited buffer and fail if somebody really will want to overflow. Also I will probably try to use hashing, instead of sorting by names, like std::map is doing now. This can be an interesting check. But yes, to find a good hash function for that can be not trivial.

Definitely SIMD is needed when working on strings. Also, as mentioned in TODO-s, there should be more bulletproof actions when searching for synonyms, like whitespace trimming, whitespace equalization etc.

By the way, that was a really nice excercise. I already forget, how ugly is STL and modern C++. ;-)