@echo off

mkdir out >nul 2>nul
pushd out
rem cl ..\convert_thesaurus_json_to_txt.c /MT /EHa- /EHs- /EHc- /Zf /Zi /FC /W4 /Wall /wd4505 /wd4101 /wd4820 /wd5045 /wd4100 /wd4189 /nologo /link user32.lib gdi32.lib comctl32.lib comdlg32.lib /INCREMENTAL:no /OPT:REF
cl ..\CThesaurusTest.cpp /std:c++17 /MT /EHsc /Zf /Zi /FC /W4 /Wall /wd4505 /wd4101 /wd4820 /wd5045 /wd4100 /wd4189 /nologo /link user32.lib gdi32.lib comctl32.lib comdlg32.lib /INCREMENTAL:no /OPT:REF
cl ..\cli_thesaurus.cpp /std:c++17 /MT /EHsc /Zf /Zi /FC /W4 /Wall /wd4505 /wd4101 /wd4820 /wd5045 /wd4100 /wd4189 /nologo /link user32.lib gdi32.lib comctl32.lib comdlg32.lib /INCREMENTAL:no /OPT:REF
CThesaurusTest.exe
popd
