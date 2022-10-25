@echo off

mkdir out >nul 2>nul
pushd out
cl ..\cli_thesaurus.cpp /MT /EHa- /EHs- /EHc- /Zf /Zi /FC /W4 /Wall /wd4505 /wd4101 /wd4820 /wd5045 /wd4100 /wd4189 /nologo /link user32.lib gdi32.lib comctl32.lib comdlg32.lib /INCREMENTAL:no /OPT:REF
popd
