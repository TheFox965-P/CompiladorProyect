#pragma once
// =============================================================================
//  Sintactico.h
//  Declaración del analizador sintáctico (descenso recursivo).
//
//  Gramática soportada (simplificada):
//
//    programa     → sentencia*
//    sentencia    → declaracion
//                 | mostrar
//                 | si_stmt
//                 | mientras_stmt
//                 | repetir_stmt
//                 | asignacion
//
//    declaracion  → "crear" tipo_dato ID ["=" expr] ";"
//    asignacion   → ID "=" expr ";"
//    mostrar      → "mostrar" expr ";"
//    si_stmt      → "si" "(" expr ")" bloque ["sino" bloque]
//    mientras_stmt→ "mientras" "(" expr ")" bloque
//    repetir_stmt → "repetir" "(" NUM_ENTERO ")" bloque
//    bloque       → "{" sentencia* "}"
//    expr         → term ((OP_ARIT | OP_REL | OP_IGUAL) term)*
//    term         → ID | NUM_ENTERO | NUM_DECIMAL
//
//  REQUISITO: Solo se ejecuta si el análisis léxico fue exitoso.
//             Solo avanza al análisis semántico si este es exitoso.
// =============================================================================

#include <string>
#include <vector>
#include "Token.h"
using namespace std;

// ---------------------------------------------------------------------------
//  Resultado del analizador sintáctico
// ---------------------------------------------------------------------------
struct ResultadoSint {
    bool   exitoso;   // true si no hubo errores sintácticos
    string mensaje;   // descripción de errores o mensaje de éxito
};

// ---------------------------------------------------------------------------
//  Función principal del analizador sintáctico.
//  Recibe la lista de tokens del análisis léxico.
// ---------------------------------------------------------------------------
ResultadoSint analizarSintactico(const vector<Token>& tokens);