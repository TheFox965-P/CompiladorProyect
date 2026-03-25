// =============================================================================
//  main.cpp
//  Punto de entrada de la aplicacion Windows.
// =============================================================================

#include <windows.h>
#include <commctrl.h>
#include "Ventana.h"

#pragma comment(lib, "comctl32.lib")

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);

    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "CompiladorWnd";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassExA(&wc);

    HWND hWnd = CreateWindowExA(
        WS_EX_APPWINDOW,
        "CompiladorWnd",
        "Compilador - Analizador Lexico - Sintactico - Semantico",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1100, 680,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}