#include <windows.h>
#include <tchar.h>
#include "id.h"

HWND hMainWindow;
static OPENFILENAME file_name;
static WCHAR link[260] = { 0 };
HINSTANCE hInstance;
//static HWND box;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnCreateWindow(HWND hWnd);
void HotKeysUsing(HWND hWnd, WPARAM wParam);
void CommandUsing(HWND hWnd, WPARAM wParam);
void AddMenus(HWND hWnd);
void OpenFile(HWND hWnd);
void SaveFile(HWND hWnd);
void HotKeyRegister(HWND hWnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = NULL;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("SystemCall Application");
    wcex.hIconSm = NULL;
    RegisterClassEx(&wcex);

    HWND hMainWindow = CreateWindow(
        _T("SystemCall Application"),
        _T("SystemCall Application"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0, 800, 500,
        NULL, NULL, NULL, NULL);
    if (!hMainWindow)
    {
        return 0;
    }
    ShowWindow(hMainWindow, SW_SHOW);
    UpdateWindow(hMainWindow);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SYSTEMSCALL));
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            if (msg.message == WM_KEYDOWN)
                SendMessage(hMainWindow, WM_KEYDOWN, msg.wParam, msg.lParam);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

    file_name.lpstrFile = link;

    switch (message){

        case WM_CREATE:
            OnCreateWindow(hWnd);
            break;

        case WM_SIZE:
            MoveWindow(box, 0, 0, LOWORD(lParam), HIWORD(lParam), 1);
            break;
    
        case WM_HOTKEY: 
            HotKeysUsing(hWnd, wParam);
            break;

        case WM_COMMAND:
            CommandUsing(hWnd, wParam);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);

    }
    return 0;
}

void OnCreateWindow(HWND hWnd) {
    hMainWindow = hWnd;
    box = CreateWindow(_T("EDIT"), _T(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL,
        0, 0, 800, 500, hWnd, NULL, hInstance, NULL);
    AddMenus(hWnd);
    HotKeyRegister(hWnd);
}

void AddMenus(HWND hWnd){

    HMENU hMenu, hSubMenu;
    hMenu = CreateMenu();
    hSubMenu = CreatePopupMenu();
   
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, _T("Edit"));
    AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, _T("Open"));
    AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVE, _T("Save"));

    SetMenu(hWnd, hMenu);
}
void OpenFile(HWND hWnd){

    SetWindowText(hWnd, L"");

    file_name.lStructSize = sizeof(OPENFILENAME);
    file_name.hwndOwner = hWnd;
    file_name.nMaxFile = sizeof(link);
    file_name.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    file_name.nFilterIndex = 1;
    file_name.lpstrFileTitle = NULL;
    file_name.nMaxFileTitle = 0;
    file_name.lpstrInitialDir = NULL;
    file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&file_name)){

        HANDLE hFile = CreateFile(file_name.lpstrFile,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hFile == INVALID_HANDLE_VALUE){

            MessageBox(NULL, L"Cannot open file", L"Error", MB_OK);
            return;
        }
        DWORD dwFileSize = GetFileSize(hFile, NULL);
        if (dwFileSize == INVALID_FILE_SIZE){

            MessageBox(NULL, L"Cannot get file size", L"Error", MB_OK);
            return;
        }

        char* buffer = new char[dwFileSize + 1];
        DWORD dwBytesRead;
        if (!ReadFile(hFile, buffer, dwFileSize, &dwBytesRead, NULL)){

            MessageBox(NULL, L"Cannot read file", L"Error", MB_OK);
            return;

        }
        buffer[dwFileSize] = 0;
        SetWindowTextA(box, buffer);
        delete[] buffer;
        CloseHandle(hFile);
    }
}
void SaveFile(HWND hWnd)
{
    if (link[0] != 0){

        HANDLE hFile = CreateFile(file_name.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE){

            MessageBox(NULL, _T("Cannot create file"), _T("Error"), MB_OK);
            return;
        
        }
        DWORD dwBytesWritten = 0;
        int nLength = GetWindowTextLength(box);
        char* buffer = new char[nLength + 1];
        GetWindowTextA(box, buffer, nLength + 1);
        WriteFile(hFile, buffer, nLength, &dwBytesWritten, NULL);
        CloseHandle(hFile);
        MessageBox(NULL, _T("Save file successfully"), _T("Save"), MB_OK);
        delete[] buffer;

    }
    else{

        long size = GetWindowTextLength(box);
        char* buffer = new char[size + 1];
        GetWindowTextA(box, buffer, size + 1);
        ZeroMemory(&file_name, sizeof(file_name));
        file_name.lStructSize = sizeof(file_name);
        file_name.hwndOwner = hWnd;
        file_name.nMaxFile = 260;
        file_name.lpstrFilter = L"Text document (*.txt) \0*.txt\0All file (*.*)\0*.*\0";
        file_name.nFilterIndex = 2;
        file_name.lpstrInitialDir = NULL;
        file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (GetSaveFileName(&file_name) == TRUE){

            HANDLE hFile = CreateFile(file_name.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            DWORD dwBytesWritten = 0;
            WriteFile(hFile, buffer, size, &dwBytesWritten, NULL);
            CloseHandle(hFile);
        
        }
    }
}

void HotKeyRegister(HWND hWnd) {
    //регистрация hotKey (сочитание клавишь типа ctrl+s)
    RegisterHotKey(hWnd, COMBINATION_SAVE, MOD_CONTROL, 'S');
    RegisterHotKey(hWnd, COMBINATION_OPEN, MOD_CONTROL, 'O');

}

void HotKeysUsing(HWND hWnd, WPARAM wParam) {
    switch (wParam){
        
        case COMBINATION_OPEN:
            OpenFile(hWnd);
            break;

        case COMBINATION_SAVE:
            SaveFile(hWnd);
            break;
    }
}
void CommandUsing(HWND hWnd, WPARAM wParam) {
    int wmId = LOWORD(wParam);

    switch (wmId){

        case ID_FILE_OPEN:
            OpenFile(hWnd);
            break;
        case ID_FILE_SAVE:
            SaveFile(hWnd);
            break;
    }
}