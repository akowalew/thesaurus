@echo off

set FLAGS=-Wall -Wextra -Wpedantic -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Werror -Wfatal-errors -std=c++17 -luser32.lib -lgdi32.lib -lcomctl32.lib -lcomdlg32.lib

mkdir out >nul 2>nul
copy en_thesaurus.json out\file.json
pushd out
clang ..\win32_thesaurus.cpp %FLAGS%
clang ..\cli_thesaurus.cpp %FLAGS%
clang ..\CThesaurusTest.cpp %FLAGS%
CThesaurusTest.exe
popd
