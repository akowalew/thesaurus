#pragma warning(push, 0)
#include <windowsx.h>
#include <windows.h>
#include <commctrl.h>
#pragma warning(pop)

#include <stdint.h>
#include <stdio.h>

#include "common.hpp"
#include "CThesaurus.hpp"
#include "CThesaurus.cpp"

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

#define StatusBar_SetText(Handle, Index, Text) SendMessage(Handle, SB_SETTEXT, Index, (LPARAM)Text)
#define StatusBar_SetParts(Handle, Count, Coordinates) SendMessage(Handle, SB_SETPARTS, Count, (LPARAM)Coordinates)

static LRESULT CALLBACK
ThesaurusWindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
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
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

static CThesaurus Thesaurus;
static WNDPROC OldEditProc;
static HWND SynonymsListBoxHandle;
static HWND StatusBarHandle;

static LRESULT CALLBACK
EditProc(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_KEYDOWN:
        {
            switch (WParam)
            {
                case VK_RETURN:
                {
                    char Buffer[256];

                    Edit_GetText(Handle, Buffer, sizeof(Buffer));

                    ListBox_ResetContent(SynonymsListBoxHandle);
                    Assert(StatusBar_SetText(StatusBarHandle, 0, "Searching..."));
                    Assert(StatusBar_SetText(StatusBarHandle, 1, ""));

                    Sleep(1000);

                    int SynonymsCount;
                    char** Synonyms = Thesaurus.GetSynonyms(Buffer, &SynonymsCount);

                    for(int Idx = 0;
                        Idx < SynonymsCount;
                        Idx++)
                    {
                        char* Synonym = Synonyms[Idx];

                        ListBox_AddString(SynonymsListBoxHandle, Synonym);
                    }

                    Assert(StatusBar_SetText(StatusBarHandle, 0, "Ready"));
                    Assert(StatusBar_SetText(StatusBarHandle, 1, "Words found: 123"));
                } break;
            }
        } break;

        default:
        {
            Result = CallWindowProc(OldEditProc, Handle, Message, WParam, LParam);
        } break;
    }

    return Result;
}

INT WINAPI
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CommandLine, INT CommandShow)
{
    const char* WindowClassName = "ThesaurusWindowClass";

    WNDCLASSA WindowsClass = {0};
    WindowsClass.lpfnWndProc = ThesaurusWindowProc;
    WindowsClass.hInstance = Instance;
    WindowsClass.lpszClassName = WindowClassName;
    WindowsClass.style = CS_HREDRAW | CS_VREDRAW;
    Assert(RegisterClassA(&WindowsClass));

    int WindowWidth = 240;
    int WindowHeight = 320;

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

    int ControlWidth = ClientWidth - 2 * Margin;
    int ControlHeight = (int)(10 * (float)(VerticalUnits) / 8);

    int X = Margin;
    int Y = Margin;

    HWND EnterWordLabelHandle = CreateWindow(WC_EDIT, "Enter word:",
                                             WS_CHILD | WS_VISIBLE,
                                             X, Y, ControlWidth, ControlHeight,
                                             WindowHandle, 0, Instance, 0);
    Assert(EnterWordLabelHandle);
    Y += ControlHeight + Margin;

    HWND EditHandle = CreateWindow(WC_EDIT, "",
                                   WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   X, Y, ControlWidth, ControlHeight,
                                   WindowHandle, 0, Instance, 0);
    Assert(EditHandle);
    OldEditProc = (WNDPROC) SetWindowLongPtr(EditHandle, GWLP_WNDPROC, (LONG_PTR)EditProc);
    Y += ControlHeight + Margin;


    HWND SynonymsListLabelHandle = CreateWindow(WC_EDIT, "Synonyms list:",
                                                WS_CHILD | WS_VISIBLE,
                                                X, Y, ControlWidth, ControlHeight,
                                                WindowHandle, 0, Instance, 0);
    Assert(SynonymsListLabelHandle);
    Y += ControlHeight + Margin;

    int SynonymsListBoxHeight = ClientHeight - Y - Margin - ControlHeight - 2;
    SynonymsListBoxHandle = CreateWindow(WC_LISTBOX, "",
                                         WS_CHILD | WS_BORDER | WS_VISIBLE | LBS_EXTENDEDSEL | LBS_NOINTEGRALHEIGHT,
                                         X, Y, ControlWidth, SynonymsListBoxHeight,
                                         WindowHandle, 0, Instance, 0);
    Assert(SynonymsListBoxHandle);
    Y += ControlHeight + Margin;

    StatusBarHandle = CreateWindow(STATUSCLASSNAME, "",
                                   WS_CHILD | WS_VISIBLE,
                                   X, Y, ControlWidth, ControlHeight,
                                   WindowHandle, 0, Instance, 0);
    Assert(StatusBarHandle);

    int StatusBarPartsCoordinates[] = { WindowWidth/2-Margin, -1 };
    Assert(StatusBar_SetParts(StatusBarHandle, ArrayLength(StatusBarPartsCoordinates), StatusBarPartsCoordinates));
    Assert(StatusBar_SetText(StatusBarHandle, 0, "Ready"));

    MSG Message;
    BOOL GetMessageResult;
    while((GetMessageResult = GetMessage(&Message, 0, 0, 0)) != 0)
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return (INT) Message.wParam;
}
