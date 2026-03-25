#pragma once
// =============================================================================
//  Token.h
//  Define el enum TipoToken y la estructura Token.
//  Compartido por todos los módulos del compilador.
// =============================================================================

#include <string>
using namespace std;

// ---------------------------------------------------------------------------
//  Enumeración de todos los tipos de token reconocidos por el analizador léxico
// ---------------------------------------------------------------------------
enum TipoToken {
    T_PALABRA_RESERVADA,   // crear, mostrar, repetir, si, sino, mientras
    T_TIPO_DATO,           // entero, decimal
    T_IDENTIFICADOR,       // nombres de variable definidos por el usuario
    T_NUMERO_ENTERO,       // literales enteros: 0, 1, 42, ...
    T_NUMERO_DECIMAL,      // literales decimales: 3.14, 0.5, ...
    T_OPERADOR_ARIT,       // + - * /
    T_OPERADOR_REL,        // > <
    T_OPERADOR_IGUAL,      // == !=
    T_ASIGNACION,          // =
    T_FIN_LINEA,           // ;
    T_LLAVE_AB,            // {
    T_LLAVE_CI,            // }
    T_PAREN_AB,            // (
    T_PAREN_CI,            // )
    T_SIMBOLO_ESP,         // @ $ % ^ ~ , ...
    T_DESCONOCIDO          // cualquier carácter no reconocido → error léxico
};

// ---------------------------------------------------------------------------
//  Estructura que representa un token individual en el flujo de análisis
// ---------------------------------------------------------------------------
struct Token {
    TipoToken tipo;    // categoría del token
    string    valor;   // lexema (texto real extraído del código fuente)
    int       linea;   // número de línea donde aparece (para mensajes de error)
};

// ---------------------------------------------------------------------------
//  Devuelve una cadena legible para cada tipo de token (uso en tablas/reportes)
// ---------------------------------------------------------------------------
string tipoTokenStr(TipoToken t);