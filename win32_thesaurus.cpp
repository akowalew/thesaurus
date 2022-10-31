#pragma warning(push, 0)
#include <windowsx.h>
#include <windows.h>
#include <commctrl.h>
#pragma warning(pop)

#include "common.cpp"

static void
DebugPrintf(const char* Format, ...)
{
    char Buffer[256];

    va_list Args;
    va_start(Args, Format);
    vsnprintf(Buffer, sizeof(Buffer), Format, Args);
    va_end(Args);

    OutputDebugStringA(Buffer);
}

static bool
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);
    return(dwAttrib != INVALID_FILE_ATTRIBUTES &&
           !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#define ListBox_GetCurSel(Handle) SendMessage(Handle, LB_GETCURSEL, 0, 0)
#define ListBox_SelectString(Handle, Index, String) SendMessage(Handle, LB_SELECTSTRING, (WPARAM)Index, (LPARAM)String)

#define StatusBar_SetText(Handle, Index, Text) SendMessage(Handle, SB_SETTEXT, Index, (LPARAM)Text)
#define StatusBar_SetParts(Handle, Count, Coordinates) SendMessage(Handle, SB_SETPARTS, Count, (LPARAM)Coordinates)

#define STATUSBAR_OVERALL_ID 0
#define STATUSBAR_MISC_ID 1

static CThesaurus Thesaurus;

static HWND MainHandle;
static HWND StatusBarHandle;
static HWND WordEditHandle;
static HWND WordsListBoxHandle;
static HWND SynonymsListBoxHandle;

static void
ChangeCurrentWord(char* Word)
{
    std::vector<std::string>* Synonyms = Thesaurus.GetSynonymsRaw(Word);

    ListBox_ResetContent(SynonymsListBoxHandle);

    if(Synonyms)
    {
        for(auto& Synonym : *Synonyms)
        {
            ListBox_AddString(SynonymsListBoxHandle, Synonym.c_str());
        }

        char SynonymsCountText[256];
        snprintf(SynonymsCountText, sizeof(SynonymsCountText), "Synonyms count: %d", (int)Synonyms->size());
        StatusBar_SetText(StatusBarHandle, STATUSBAR_MISC_ID, SynonymsCountText);
        StatusBar_SetText(StatusBarHandle, STATUSBAR_OVERALL_ID, "Word found");

        delete Synonyms;
    }
    else
    {
        StatusBar_SetText(StatusBarHandle, STATUSBAR_OVERALL_ID, "Word not found");
    }
}

static LRESULT CALLBACK
WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_COMMAND:
        {
            HWND CommandFrom = (HWND)LParam;
            if(CommandFrom == WordsListBoxHandle)
            {
                unsigned NotificationCode = HIWORD(WParam);
                if(NotificationCode == LBN_SELCHANGE)
                {
                    LRESULT WordIndex = ListBox_GetCurSel(WordsListBoxHandle);

                    char Word[256];

                    int WordLength = ListBox_GetTextLen(WordsListBoxHandle, WordIndex);

                    Assert(WordLength < sizeof(Word));

                    ListBox_GetText(WordsListBoxHandle, WordIndex, Word);

                    ChangeCurrentWord(Word);
                }
            }
            else if(CommandFrom == WordEditHandle)
            {
                unsigned NotificationCode = HIWORD(WParam);
                if(NotificationCode == EN_CHANGE)
                {
                    char Word[256];

                    int WordLength = Edit_GetTextLength(WordEditHandle);

                    Assert(WordLength < sizeof(Word));

                    Edit_GetText(WordEditHandle, Word, sizeof(Word));

                    LRESULT Index = ListBox_SelectString(WordsListBoxHandle, 0, Word);

                    if(Index != LB_ERR)
                    {
                        WordLength = ListBox_GetTextLen(WordsListBoxHandle, Index);

                        Assert(WordLength < sizeof(Word));

                        ListBox_GetText(WordsListBoxHandle, Index, Word);

                        ChangeCurrentWord(Word);
                    }
                    else
                    {
                        StatusBar_SetText(StatusBarHandle, STATUSBAR_OVERALL_ID, "Word not found");
                    }
                }
            }
        } break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;

        case WM_CLOSE:
        {
            DestroyWindow(Window);
        } break;

        default:
        {
            DebugPrintf("Unknown message: %d\n", Message);
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

INT WINAPI
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CommandLine, INT CommandShow)
{
    const char* WindowClassName = "ThesaurusWindowClass";

    INITCOMMONCONTROLSEX InitCommonControlsExStruct;
    InitCommonControlsExStruct.dwSize = sizeof(InitCommonControlsExStruct);
    InitCommonControlsExStruct.dwICC = ICC_TAB_CLASSES;
    Assert(InitCommonControlsEx(&InitCommonControlsExStruct));

    WNDCLASSA WindowsClass = {0};
    WindowsClass.lpfnWndProc = WindowProc;
    WindowsClass.hInstance = Instance;
    WindowsClass.lpszClassName = WindowClassName;
    WindowsClass.style = CS_HREDRAW | CS_VREDRAW;
    Assert(RegisterClassA(&WindowsClass));

    int WindowWidth = 500;
    int WindowHeight = 500;

    const char* WindowTitle = "Thesaurus";
    DWORD WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
    HWND WindowHandle = CreateWindow(WindowClassName, WindowTitle, WindowStyle,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     WindowWidth, WindowHeight,
                                     0, 0, Instance, 0);
    Assert(WindowHandle);

    RECT ClientRect;
    Assert(GetClientRect(WindowHandle, &ClientRect));
    Assert(ClientRect.right > ClientRect.left);
    Assert(ClientRect.bottom > ClientRect.top);
    int ClientWidth = ClientRect.right - ClientRect.left;
    int ClientHeight = ClientRect.bottom - ClientRect.top;

    long DialogBaseUnits = GetDialogBaseUnits();
    long HorizontalUnits = DialogBaseUnits & 0x0000FFFF;
    long VerticalUnits = DialogBaseUnits >> 16;

    int Margin = 10;

    const int ImageListID    = 0;
    const int bitmapSize     = 16;

    MainHandle = CreateWindow(WC_STATIC, "",
                              WS_CHILD | WS_VISIBLE,
                              0, 0, ClientWidth, ClientHeight,
                              WindowHandle, 0, Instance, 0);

    RECT MainRect;
    Assert(GetClientRect(MainHandle, &MainRect));
    Assert(MainRect.right > MainRect.left);
    Assert(MainRect.bottom > MainRect.top);
    int MainWidth = MainRect.right - MainRect.left;
    int MainHeight = MainRect.bottom - MainRect.top;

    int ControlWidth = (MainWidth - 3 * Margin) / 2;
    int ControlHeight = (int)(10 * (float)(VerticalUnits) / 8);

    StatusBarHandle = CreateWindow(STATUSCLASSNAME, "",
                                   WS_CHILD | WS_VISIBLE,
                                   0, 0, 0, 0,
                                   MainHandle, 0, Instance, 0);
    Assert(StatusBarHandle);
    int StatusBarPartsCoordinates[] = { MainWidth/2, -1 };
    Assert(StatusBar_SetParts(StatusBarHandle, ArrayLength(StatusBarPartsCoordinates), StatusBarPartsCoordinates));

    RECT StatusBarRect;
    Assert(GetWindowRect(StatusBarHandle, &StatusBarRect));
    int StatusBarHeight = StatusBarRect.bottom - StatusBarRect.top;

    int RemainingHeight = MainHeight - StatusBarHeight;

    int Top = Margin;

    {
        int Y = Top;

        int Length = HorizontalUnits*6;

        CreateWindow(WC_STATIC, "Query: ",
                     WS_CHILD | WS_VISIBLE,
                     Margin, Y, Length, ControlHeight,
                     WindowHandle, NULL, Instance, NULL);

        WordEditHandle = CreateWindow(WC_EDIT, "",
                                      WS_CHILD | WS_VISIBLE | WS_BORDER,
                                      Margin+Length, Y, ControlWidth-Length, ControlHeight,
                                      WindowHandle, NULL, Instance, NULL);
        Assert(WordEditHandle);

        Y += ControlHeight + Margin;

        WordsListBoxHandle = CreateWindow(WC_LISTBOX, "",
                                          WS_CHILD | WS_BORDER | WS_VISIBLE | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY,
                                          Margin, Y, ControlWidth, RemainingHeight - (Y - Top) - 2*Margin,
                                          WindowHandle, NULL, Instance, NULL);
        Assert(WordsListBoxHandle);
    }

    {
        SynonymsListBoxHandle = CreateWindow(WC_LISTBOX, "",
                                             WS_CHILD | WS_BORDER | WS_VISIBLE | LBS_NOINTEGRALHEIGHT,
                                             ControlWidth + 2*Margin, Top, ControlWidth, RemainingHeight - 2*Margin,
                                             WindowHandle, NULL, Instance, NULL);
        Assert(WordsListBoxHandle);
    }

    char FileName[256] = "file.json";

    if(!FileExists(FileName))
    {
        int UserChoice = MessageBoxA(WindowHandle, "Synonyms database file does not exist.\nWould you like to select other file?", "Thesaurus", MB_YESNO|MB_ICONQUESTION);
        if(UserChoice == IDYES)
        {
            FileName[0] = '\0';

            OPENFILENAMEA OpenFileName = {0};
            OpenFileName.lStructSize = sizeof(OpenFileName);
            OpenFileName.hwndOwner = WindowHandle;
            OpenFileName.hInstance = GetModuleHandleA(0);
            OpenFileName.lpstrFilter = "JSON files\0*.json\0All files\0*.*\0";
            OpenFileName.lpstrFile = FileName;
            OpenFileName.nMaxFile = sizeof(FileName);

            GetOpenFileNameA(&OpenFileName);
        }
    }

    StatusBar_SetText(StatusBarHandle, STATUSBAR_OVERALL_ID, "Loading...");
    StatusBar_SetText(StatusBarHandle, STATUSBAR_MISC_ID, "");

    if(ImportThesaurusFromWordNetJsonFile(&Thesaurus, FileName))
    {
        StatusBar_SetText(StatusBarHandle, STATUSBAR_OVERALL_ID, "Data loaded");

        ListBox_ResetContent(WordsListBoxHandle);

        std::vector<std::string>* AllWords = Thesaurus.GetAllWords();

        char WordsCountText[256];
        snprintf(WordsCountText, sizeof(WordsCountText), "Words count: %d", (int)AllWords->size());

        StatusBar_SetText(StatusBarHandle, STATUSBAR_MISC_ID, WordsCountText);

        for(auto& Word : *AllWords)
        {
            ListBox_AddString(WordsListBoxHandle, Word.c_str());
        }

        delete AllWords;

        Assert(StatusBar_SetText(StatusBarHandle, STATUSBAR_OVERALL_ID, "Data loaded"));
    }
    else
    {
        StatusBar_SetText(StatusBarHandle, STATUSBAR_OVERALL_ID, "Failed to load data");

        MessageBoxA(WindowHandle, "Failed to import thesaurus database", "Error", MB_ICONERROR|MB_OK);
    }

    MSG Message;
    BOOL GetMessageResult;
    while((GetMessageResult = GetMessage(&Message, 0, 0, 0)) != 0)
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return (INT) Message.wParam;
}
