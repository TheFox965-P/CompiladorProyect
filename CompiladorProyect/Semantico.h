#pragma once
// =============================================================================
//  Semantico.h
//  Declaración del analizador semántico.
//
//  Responsabilidades:
//    - Mantener la tabla de símbolos (variables declaradas con "crear")
//    - Detectar variables no declaradas
//    - Detectar variables ya declaradas (redeclaración)
//    - Detectar asignación de tipo incompatible (ej: decimal a entero)
//    - Advertir sobre variables usadas sin inicializar
//    - Imprimir la tabla de símbolos al finalizar
//
//  REQUISITO: Solo se ejecuta si el análisis sintáctico fue exitoso.
// =============================================================================

#include <string>
#include <vector>
#include <map>
#include "Token.h"
using namespace std;

// ---------------------------------------------------------------------------
//  Entrada de la tabla de símbolos
// ---------------------------------------------------------------------------
struct Simbolo {
    string tipo;          // "entero" o "decimal"
    string valor;         // valor asignado (si existe)
    bool   inicializado;  // true si se asignó un valor
};

// ---------------------------------------------------------------------------
//  Tabla de símbolos global del programa analizado.
//  Se expone para que la ventana pueda consultarla si es necesario.
// ---------------------------------------------------------------------------
extern map<string, Simbolo> tablaSimbolos;

// ---------------------------------------------------------------------------
//  Resultado del analizador semántico
// ---------------------------------------------------------------------------
struct ResultadoSem {
    bool   exitoso;  // true si no hubo errores semánticos
    string mensaje;  // errores, advertencias y tabla de símbolos
};

// ---------------------------------------------------------------------------
//  Función principal del analizador semántico.
//  Recibe la lista de tokens del análisis léxico.
// ---------------------------------------------------------------------------
ResultadoSem analizarSemantico(const vector<Token>& tokens);