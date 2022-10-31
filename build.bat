@echo off

set FLAGS=/std:c++17 /MT /EHsc /Zf /Zi /FC /W4 /Wall /wd4505 /wd4101 /wd4820 /wd5045 /wd4100 /wd4189 /nologo /link user32.lib gdi32.lib comctl32.lib comdlg32.lib /INCREMENTAL:no /OPT:REF

mkdir out >nul 2>nul
copy en_thesaurus.json out\file.json
pushd out
cl ..\win32_thesaurus.cpp %FLAGS%
cl ..\cli_thesaurus.cpp %FLAGS%
cl ..\CThesaurusTest.cpp %FLAGS%
CThesaurusTest.exe
popd
