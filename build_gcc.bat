@echo off

set FLAGS=-Wall -Wextra -Wpedantic -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Wno-unknown-pragmas -Werror -Wfatal-errors -std=c++17 -luser32 -lgdi32 -lcomctl32 -lcomdlg32

mkdir out >nul 2>nul
copy en_thesaurus.json out\file.json
pushd out
g++ ..\win32_thesaurus.cpp %FLAGS% -o win32_thesaurus.exe
g++ ..\cli_thesaurus.cpp %FLAGS% -o cli_thesaurus.exe
g++ ..\CThesaurusTest.cpp %FLAGS% -o CThesaurusTest.exe
CThesaurusTest.exe
popd
