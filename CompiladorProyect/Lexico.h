#pragma once
// =============================================================================
//  Lexico.h
//  Declaración del analizador léxico.
//
//  Palabras reservadas propias del lenguaje:
//    crear   → declaración de variable
//    mostrar → instrucción de salida (print)
//    repetir → bucle for simplificado
//
//  Tipos de dato soportados: entero, decimal
//  Símbolos especiales: { } ( ) @ $ % ^ ~ ,
// =============================================================================

#include <string>
#include <vector>
#include "Token.h"
using namespace std;

// ---------------------------------------------------------------------------
//  Resultado que devuelve el analizador léxico tras procesar el código fuente
// ---------------------------------------------------------------------------
struct ResultadoLexico {
    bool          exitoso;  // true si no se encontró ningún error léxico
    string        errores;  // descripción de errores (vacío si exitoso)
    vector<Token> tokens;   // lista de tokens reconocidos
    string        tabla;    // tabla formateada lista para mostrar en la UI
};

// ---------------------------------------------------------------------------
//  Función principal del analizador léxico.
//  Recibe el código fuente como string y devuelve un ResultadoLexico.
//  REQUISITO: debe estar libre de errores para que avance el análisis sintáctico.
// ---------------------------------------------------------------------------
ResultadoLexico analizarLexico(const string& codigo);