#pragma once
#include <windows.h>
#include <commctrl.h>

#define ID_BTN_ANALIZAR   100
#define ID_EDIT_INPUT     101
#define ID_EDIT_OUTPUT    102
#define ID_LABEL_LEXICO   103
#define ID_LABEL_SINT     104
#define ID_LABEL_SEM      105


#define CLR_BG        RGB(18,  12,  38)    
#define CLR_PANEL     RGB(30,  20,  58)    
#define CLR_BORDER    RGB(90,  60,  160)   
#define CLR_ACCENT    RGB(160, 110, 255)   
#define CLR_ACCENT2   RGB(200, 170, 255)   
#define CLR_TEXT      RGB(240, 235, 255)   
#define CLR_TEXT_DIM  RGB(170, 155, 210)   
#define CLR_OK        RGB(120, 230, 160)   
#define CLR_ERR       RGB(255, 100, 120)   
#define CLR_MUTED     RGB(120, 100, 170)   
#define CLR_BTN       RGB(100, 60,  200)   
#define CLR_BTN_HOV   RGB(130, 90,  240)   


enum EstadoEtapa { INACTIVO, OK_STAGE, ERROR_STAGE };


extern HWND hWndMain;
extern HWND hEditIn, hEditOut;
extern HWND hLblLex, hLblSint, hLblSem;
extern HWND hBtnAnalizar;
extern HFONT hFontTitle, hFontMono, hFontBtn, hFontLabel;
extern HBRUSH hBrushBg, hBrushPanel;
extern EstadoEtapa estadoLex, estadoSint, estadoSem;


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wP, LPARAM lP);


void dibujarBoton(HWND hBtn, HDC hdc);
void dibujarEtiqueta(HWND hLbl, HDC hdc, EstadoEtapa estado, const char* nombre);

void ejecutarAnalisis();