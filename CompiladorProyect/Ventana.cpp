// =============================================================================
//  Ventana.cpp
//  Implementacion de la ventana principal del compilador.
//  Paleta visual: morado profundo + blanco perla + violeta brillante
// =============================================================================

#include "Ventana.h"
#include "Lexico.h"
#include "Sintactico.h"
#include "Semantico.h"
#include <string>
using namespace std;

// ---------------------------------------------------------------------------
//  Definicion de variables globales
// ---------------------------------------------------------------------------
HWND hWndMain = NULL;
HWND hEditIn = NULL;
HWND hEditOut = NULL;
HWND hLblLex = NULL;
HWND hLblSint = NULL;
HWND hLblSem = NULL;
HWND hBtnAnalizar = NULL;

HFONT  hFontTitle = NULL;
HFONT  hFontMono = NULL;
HFONT  hFontBtn = NULL;
HFONT  hFontLabel = NULL;
HBRUSH hBrushBg = NULL;
HBRUSH hBrushPanel = NULL;

EstadoEtapa estadoLex = INACTIVO;
EstadoEtapa estadoSint = INACTIVO;
EstadoEtapa estadoSem = INACTIVO;

// ---------------------------------------------------------------------------
//  ejecutarAnalisis
// ---------------------------------------------------------------------------
void ejecutarAnalisis() {
    int len = GetWindowTextLengthA(hEditIn);
    if (len == 0) {
        SetWindowTextA(hEditOut,
            "  [!] El area de codigo esta vacia.\r\n"
            "      Ingrese codigo para analizar.");
        return;
    }

    string codigo(len + 1, '\0');
    GetWindowTextA(hEditIn, &codigo[0], len + 1);
    codigo.resize(len);

    string resultado;
    estadoLex = estadoSint = estadoSem = INACTIVO;

    // =========================================================================
    //  FASE 1: ANALISIS LEXICO
    // =========================================================================
    resultado += "  ANALISIS LEXICO\r\n";

    ResultadoLexico rLex = analizarLexico(codigo);

    if (rLex.exitoso) {
        estadoLex = OK_STAGE;
        resultado += "   Analisis lexico completado sin errores.\r\n\r\n";
        resultado += rLex.tabla;
    }
    else {
        estadoLex = ERROR_STAGE;
        resultado += "   Errores lexicos encontrados:\r\n";
        resultado += rLex.errores;
        resultado += "\r\n  [!] Corrija los errores lexicos para continuar.\r\n";
        SetWindowTextA(hEditOut, resultado.c_str());
        InvalidateRect(hWndMain, NULL, TRUE);
        return;
    }

    // =========================================================================
    //  FASE 2: ANALISIS SINTACTICO
    // =========================================================================
    resultado += "  ANALISIS SINTACTICO\r\n";

    ResultadoSint rSint = analizarSintactico(rLex.tokens);

    if (rSint.exitoso) {
        estadoSint = OK_STAGE;
        resultado += "   Analisis sintactico completado sin errores.\r\n";
        resultado += "    La estructura del programa es correcta.\r\n";
    }
    else {
        estadoSint = ERROR_STAGE;
        resultado += "   Errores sintacticos encontrados:\r\n";
        resultado += rSint.mensaje;
        resultado += "\r\n  [!] Corrija los errores sintacticos para continuar.\r\n";
        SetWindowTextA(hEditOut, resultado.c_str());
        InvalidateRect(hWndMain, NULL, TRUE);
        return;
    }

    // =========================================================================
    //  FASE 3: ANALISIS SEMANTICO
    // =========================================================================
    resultado += "  ANALISIS SEMANTICO\r\n";

    ResultadoSem rSem = analizarSemantico(rLex.tokens);

    if (rSem.exitoso) {
        estadoSem = OK_STAGE;
        resultado += "Analisis semantico completado sin errores.\r\n";
    }
    else {
        estadoSem = ERROR_STAGE;
        resultado += "Errores semanticos encontrados:\r\n";
    }
    resultado += rSem.mensaje;

    // =========================================================================
    //  VEREDICTO FINAL
    // =========================================================================
    if (estadoLex == OK_STAGE && estadoSint == OK_STAGE && estadoSem == OK_STAGE)
        resultado += "==========  COMPILACION EXITOSA  =========\r\n";
    else
        resultado += "    COMPILACION FALLIDA  \r\n";

    SetWindowTextA(hEditOut, resultado.c_str());
    InvalidateRect(hWndMain, NULL, TRUE);
}

// ---------------------------------------------------------------------------
//  dibujarBoton  (owner-draw del boton ANALIZAR)
//  Efecto: fondo degradado simulado + borde redondeado + brillo superior
// ---------------------------------------------------------------------------
void dibujarBoton(HWND hBtn, HDC hdc) {
    RECT rc;
    GetClientRect(hBtn, &rc);

    bool hover = (GetCapture() == hBtn || GetFocus() == hBtn);
    COLORREF clrTop = hover ? RGB(150, 100, 255) : RGB(120, 70, 220);
    COLORREF clrBottom = hover ? RGB(100, 60, 200) : RGB(70, 35, 160);

    // Degradado vertical simulado (bandas horizontales)
    int h = rc.bottom - rc.top;
    for (int y = 0; y < h; y++) {
        float t = (float)y / (float)(h - 1);
        int r = (int)(GetRValue(clrTop) + t * (GetRValue(clrBottom) - GetRValue(clrTop)));
        int g = (int)(GetGValue(clrTop) + t * (GetGValue(clrBottom) - GetGValue(clrTop)));
        int b = (int)(GetBValue(clrTop) + t * (GetBValue(clrBottom) - GetBValue(clrTop)));
        HPEN linePen = CreatePen(PS_SOLID, 1, RGB(r, g, b));
        HPEN old = (HPEN)SelectObject(hdc, linePen);
        MoveToEx(hdc, rc.left, rc.top + y, NULL);
        LineTo(hdc, rc.right, rc.top + y);
        SelectObject(hdc, old);
        DeleteObject(linePen);
    }

    // Borde exterior violeta brillante
    HPEN penOuter = CreatePen(PS_SOLID, 2, RGB(160, 110, 255));
    SelectObject(hdc, penOuter);
    MoveToEx(hdc, rc.left, rc.top, NULL);
    LineTo(hdc, rc.right - 1, rc.top);
    LineTo(hdc, rc.right - 1, rc.bottom - 1);
    LineTo(hdc, rc.left, rc.bottom - 1);
    LineTo(hdc, rc.left, rc.top);
    DeleteObject(penOuter);

    // Brillo sutil en borde superior
    HPEN penShine = CreatePen(PS_SOLID, 1, RGB(200, 170, 255));
    SelectObject(hdc, penShine);
    MoveToEx(hdc, rc.left + 2, rc.top + 1, NULL);
    LineTo(hdc, rc.right - 2, rc.top + 1);
    DeleteObject(penShine);

    // Texto centrado
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(240, 235, 255));
    SelectObject(hdc, hFontBtn);
    DrawTextA(hdc, "ANALIZAR", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// ---------------------------------------------------------------------------
//  dibujarEtiqueta  (owner-draw de los indicadores de etapa)
//  Efecto: fondo con borde izquierdo coloreado + indicador de estado
// ---------------------------------------------------------------------------
void dibujarEtiqueta(HWND hLbl, HDC hdc, EstadoEtapa estado, const char* nombre) {
    RECT rc;
    GetClientRect(hLbl, &rc);

    COLORREF color = CLR_MUTED;
    if (estado == OK_STAGE)    color = CLR_OK;
    if (estado == ERROR_STAGE) color = CLR_ERR;

    // Fondo del panel
    HBRUSH br = CreateSolidBrush(CLR_PANEL);
    FillRect(hdc, &rc, br);
    DeleteObject(br);

    // Borde exterior sutil
    HPEN penBorder = CreatePen(PS_SOLID, 1, CLR_BORDER);
    SelectObject(hdc, penBorder);
    MoveToEx(hdc, rc.left, rc.top, NULL);
    LineTo(hdc, rc.right - 1, rc.top);
    LineTo(hdc, rc.right - 1, rc.bottom - 1);
    LineTo(hdc, rc.left, rc.bottom - 1);
    LineTo(hdc, rc.left, rc.top);
    DeleteObject(penBorder);

    // Barra lateral izquierda de color (indicador de estado)
    HPEN penBar = CreatePen(PS_SOLID, 3, color);
    SelectObject(hdc, penBar);
    MoveToEx(hdc, rc.left + 1, rc.top + 4, NULL);
    LineTo(hdc, rc.left + 1, rc.bottom - 4);
    DeleteObject(penBar);

    // Subrayado inferior de color
    HPEN penLine = CreatePen(PS_SOLID, 2, color);
    SelectObject(hdc, penLine);
    MoveToEx(hdc, rc.left + 4, rc.bottom - 2, NULL);
    LineTo(hdc, rc.right - 4, rc.bottom - 2);
    DeleteObject(penLine);

    // Punto de estado + nombre
    const char* prefix = "  o  ";
    if (estado == OK_STAGE)    prefix = "  *  ";
    if (estado == ERROR_STAGE) prefix = "  x  ";

    string label = string(prefix) + nombre;
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    SelectObject(hdc, hFontLabel);

    RECT rcText = { rc.left + 8, rc.top, rc.right, rc.bottom };
    DrawTextA(hdc, label.c_str(), -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// ---------------------------------------------------------------------------
//  WndProc  -  procedimiento de ventana principal
// ---------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wP, LPARAM lP) {
    switch (msg) {

        // -----------------------------------------------------------------------
    case WM_CREATE: {
        hWndMain = hWnd;
        hBrushBg = CreateSolidBrush(CLR_BG);
        hBrushPanel = CreateSolidBrush(CLR_PANEL);

        // Fuente titulo: negrita, ligeramente mas grande
        hFontTitle = CreateFontA(15, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH, "Consolas");
        // Fuente monoespacio para editores
        hFontMono = CreateFontA(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH, "Consolas");
        // Fuente boton: negrita
        hFontBtn = CreateFontA(14, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH, "Consolas");
        // Fuente etiquetas: semibold, ligeramente mas pequeña
        hFontLabel = CreateFontA(13, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH, "Consolas");

        RECT rc; GetClientRect(hWnd, &rc);
        int W = rc.right, H = rc.bottom;
        int PAD = 14;
        int TOP = 80;   // Un poco mas de espacio para la barra superior
        int BTNH = 46;
        int midX = W / 2;
        int areaH = H - TOP - BTNH - PAD * 3;
        int lblW = (W - PAD * 4) / 3;

        // Indicadores de etapa
        hLblLex = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
            PAD, PAD + 2, lblW, 44, hWnd, (HMENU)ID_LABEL_LEXICO, NULL, NULL);
        hLblSint = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
            PAD * 2 + lblW, PAD + 2, lblW, 44, hWnd, (HMENU)ID_LABEL_SINT, NULL, NULL);
        hLblSem = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
            PAD * 3 + lblW * 2, PAD + 2, lblW, 44, hWnd, (HMENU)ID_LABEL_SEM, NULL, NULL);

        // Editor de codigo fuente (entrada)
        hEditIn = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
            PAD, TOP, midX - PAD - PAD / 2, areaH,
            hWnd, (HMENU)ID_EDIT_INPUT, NULL, NULL);
        SendMessage(hEditIn, WM_SETFONT, (WPARAM)hFontMono, TRUE);
        SetWindowTextA(hEditIn,
            "// Ingrese su codigo aqui\r\n"
            "// Palabras reservadas: crear, mostrar, repetir\r\n"
            "// Tipos de dato: entero, decimal\r\n"
            "//\r\n"
            "crear entero x = 10;\r\n"
            "crear decimal pi = 3.14;\r\n"
            "si (x > 5) {\r\n"
            "    mostrar x;\r\n"
            "} sino {\r\n"
            "    mostrar pi;\r\n"
            "}\r\n"
            "mientras (x > 0) {\r\n"
            "    x = x - 1;\r\n"
            "}\r\n"
            "repetir (3) {\r\n"
            "    mostrar x;\r\n"
            "}\r\n");

        // Area de resultados (solo lectura)
        hEditOut = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            midX + PAD / 2, TOP, W - midX - PAD - PAD / 2, areaH,
            hWnd, (HMENU)ID_EDIT_OUTPUT, NULL, NULL);
        SendMessage(hEditOut, WM_SETFONT, (WPARAM)hFontMono, TRUE);
        SetWindowTextA(hEditOut, "  Presione ANALIZAR para iniciar el analisis.");

        // Boton ANALIZAR (mas ancho y con mas padding visual)
        int btnW = 220;
        hBtnAnalizar = CreateWindowExA(0, "BUTTON", "ANALIZAR",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            W / 2 - btnW / 2, H - BTNH - PAD, btnW, BTNH,
            hWnd, (HMENU)ID_BTN_ANALIZAR, NULL, NULL);
        SendMessage(hBtnAnalizar, WM_SETFONT, (WPARAM)hFontBtn, TRUE);

        return 0;
    }

                  // -----------------------------------------------------------------------
    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wP;
        SetBkColor(hdc, CLR_PANEL);
        SetTextColor(hdc, CLR_TEXT);
        return (LRESULT)hBrushPanel;
    }

                        // -----------------------------------------------------------------------
    case WM_CTLCOLORSTATIC: {
        HDC  hdc = (HDC)wP;
        HWND hCtrl = (HWND)lP;
        if (hCtrl == hLblLex || hCtrl == hLblSint || hCtrl == hLblSem) {
            SetBkMode(hdc, TRANSPARENT);
            return (LRESULT)hBrushPanel;
        }
        SetBkColor(hdc, CLR_BG);
        SetTextColor(hdc, CLR_TEXT);
        return (LRESULT)hBrushBg;
    }

                          // -----------------------------------------------------------------------
    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lP;
        switch (dis->CtlID) {
        case ID_BTN_ANALIZAR:
            dibujarBoton(dis->hwndItem, dis->hDC);
            break;
        case ID_LABEL_LEXICO:
            dibujarEtiqueta(dis->hwndItem, dis->hDC, estadoLex, "LEXICO");
            break;
        case ID_LABEL_SINT:
            dibujarEtiqueta(dis->hwndItem, dis->hDC, estadoSint, "SINTACTICO");
            break;
        case ID_LABEL_SEM:
            dibujarEtiqueta(dis->hwndItem, dis->hDC, estadoSem, "SEMANTICO");
            break;
        }
        return TRUE;
    }

                    // -----------------------------------------------------------------------
    case WM_ERASEBKGND: {
        HDC  hdc = (HDC)wP;
        RECT rc; GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, hBrushBg);
        return 1;
    }

                      // -----------------------------------------------------------------------
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rc; GetClientRect(hWnd, &rc);
        int W = rc.right, H = rc.bottom;
        int PAD = 14;
        int TOP = 80;
        int BTNH = 46;
        int midX = W / 2;
        int areaH = H - TOP - BTNH - PAD * 3;

        // ----- Barra superior con fondo ligeramente diferente -----
        RECT rcBar = { 0, 0, W, TOP - 4 };
        HBRUSH brBar = CreateSolidBrush(RGB(24, 16, 50));
        FillRect(hdc, &rcBar, brBar);
        DeleteObject(brBar);

        // Linea inferior de la barra superior (violeta brillante)
        HPEN penAccent = CreatePen(PS_SOLID, 2, CLR_ACCENT);
        SelectObject(hdc, penAccent);
        MoveToEx(hdc, 0, TOP - 5, NULL);
        LineTo(hdc, W, TOP - 5);
        DeleteObject(penAccent);

        // Linea separadora vertical (entre editor y resultado)
        HPEN penDiv = CreatePen(PS_SOLID, 1, CLR_BORDER);
        SelectObject(hdc, penDiv);
        MoveToEx(hdc, midX, TOP, NULL);
        LineTo(hdc, midX, TOP + areaH);
        DeleteObject(penDiv);

        // ----- Etiquetas de seccion -----
        SetBkMode(hdc, TRANSPARENT);
        SelectObject(hdc, hFontTitle);

        // "CODIGO FUENTE"
        SetTextColor(hdc, CLR_ACCENT);
        TextOutA(hdc, PAD + 2, TOP - 22, "[ CODIGO FUENTE ]", 17);

        // "RESULTADO DEL ANALISIS"
        TextOutA(hdc, midX + PAD, TOP - 22, "[ RESULTADO DEL ANALISIS ]", 26);

        // ----- Barra inferior (zona del boton) -----
        RECT rcFoot = { 0, H - BTNH - PAD * 2, W, H };
        HBRUSH brFoot = CreateSolidBrush(RGB(24, 16, 50));
        FillRect(hdc, &rcFoot, brFoot);
        DeleteObject(brFoot);

        // Linea superior de la barra inferior
        HPEN penFoot = CreatePen(PS_SOLID, 1, CLR_BORDER);
        SelectObject(hdc, penFoot);
        MoveToEx(hdc, 0, H - BTNH - PAD * 2, NULL);
        LineTo(hdc, W, H - BTNH - PAD * 2);
        DeleteObject(penFoot);

        EndPaint(hWnd, &ps);
        return 0;
    }

                 // -----------------------------------------------------------------------
    case WM_SIZE: {
        int W = LOWORD(lP), H = HIWORD(lP);
        int PAD = 14;
        int TOP = 80;
        int BTNH = 46;
        int areaH = H - TOP - BTNH - PAD * 3;
        int midX = W / 2;
        int lblW = (W - PAD * 4) / 3;

        MoveWindow(hLblLex, PAD, PAD + 2, lblW, 44, TRUE);
        MoveWindow(hLblSint, PAD * 2 + lblW, PAD + 2, lblW, 44, TRUE);
        MoveWindow(hLblSem, PAD * 3 + lblW * 2, PAD + 2, lblW, 44, TRUE);

        MoveWindow(hEditIn, PAD, TOP, midX - PAD - PAD / 2, areaH, TRUE);
        MoveWindow(hEditOut, midX + PAD / 2, TOP, W - midX - PAD - PAD / 2, areaH, TRUE);

        int btnW = 220;
        MoveWindow(hBtnAnalizar, W / 2 - btnW / 2, H - BTNH - PAD, btnW, BTNH, TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }

                // -----------------------------------------------------------------------
    case WM_COMMAND:
        if (LOWORD(wP) == ID_BTN_ANALIZAR && HIWORD(wP) == BN_CLICKED)
            ejecutarAnalisis();
        return 0;

        // -----------------------------------------------------------------------
    case WM_DESTROY:
        DeleteObject(hBrushBg);
        DeleteObject(hBrushPanel);
        DeleteObject(hFontTitle);
        DeleteObject(hFontMono);
        DeleteObject(hFontBtn);
        DeleteObject(hFontLabel);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hWnd, msg, wP, lP);
}