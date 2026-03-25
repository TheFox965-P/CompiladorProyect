#pragma once
// =============================================================================
//  Ventana.h
//  Declaracion de la ventana principal y sus componentes visuales.
//
//  Paleta: morado profundo + blanco perla + acentos violeta brillante
// =============================================================================

#include <windows.h>
#include <commctrl.h>

// ---------------------------------------------------------------------------
//  IDs de controles
// ---------------------------------------------------------------------------
#define ID_BTN_ANALIZAR   100
#define ID_EDIT_INPUT     101
#define ID_EDIT_OUTPUT    102
#define ID_LABEL_LEXICO   103
#define ID_LABEL_SINT     104
#define ID_LABEL_SEM      105

// ---------------------------------------------------------------------------
//  Paleta de colores (tema morado/blanco)
// ---------------------------------------------------------------------------
#define CLR_BG        RGB(18,  12,  38)    // Morado muy oscuro (fondo principal)
#define CLR_PANEL     RGB(30,  20,  58)    // Morado oscuro (paneles)
#define CLR_BORDER    RGB(90,  60,  160)   // Morado medio (bordes)
#define CLR_ACCENT    RGB(160, 110, 255)   // Violeta brillante (acentos)
#define CLR_ACCENT2   RGB(200, 170, 255)   // Violeta claro (subtitulos)
#define CLR_TEXT      RGB(240, 235, 255)   // Blanco perla (texto principal)
#define CLR_TEXT_DIM  RGB(170, 155, 210)   // Blanco apagado (texto secundario)
#define CLR_OK        RGB(120, 230, 160)   // Verde menta (exito)
#define CLR_ERR       RGB(255, 100, 120)   // Rojo coral (error)
#define CLR_MUTED     RGB(120, 100, 170)   // Morado grisaceo (inactivo)
#define CLR_BTN       RGB(100, 60,  200)   // Morado boton normal
#define CLR_BTN_HOV   RGB(130, 90,  240)   // Morado boton hover

// ---------------------------------------------------------------------------
//  Estado visual de cada etapa del compilador
// ---------------------------------------------------------------------------
enum EstadoEtapa { INACTIVO, OK_STAGE, ERROR_STAGE };

// ---------------------------------------------------------------------------
//  Variables globales de la ventana (definidas en Ventana.cpp)
// ---------------------------------------------------------------------------
extern HWND hWndMain;
extern HWND hEditIn, hEditOut;
extern HWND hLblLex, hLblSint, hLblSem;
extern HWND hBtnAnalizar;
extern HFONT hFontTitle, hFontMono, hFontBtn, hFontLabel;
extern HBRUSH hBrushBg, hBrushPanel;
extern EstadoEtapa estadoLex, estadoSint, estadoSem;

// ---------------------------------------------------------------------------
//  Window Procedure
// ---------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wP, LPARAM lP);

// ---------------------------------------------------------------------------
//  Funciones de dibujo personalizado (owner-draw)
// ---------------------------------------------------------------------------
void dibujarBoton(HWND hBtn, HDC hdc);
void dibujarEtiqueta(HWND hLbl, HDC hdc, EstadoEtapa estado, const char* nombre);

// ---------------------------------------------------------------------------
//  Funcion que orquesta los tres analizadores al presionar ANALIZAR
// ---------------------------------------------------------------------------
void ejecutarAnalisis();