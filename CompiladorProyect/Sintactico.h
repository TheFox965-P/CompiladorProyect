#pragma once

#include <string>
#include <vector>
#include "Token.h"
using namespace std;

//  Resultado del analizador sintáctico

struct ResultadoSint {
    bool   exitoso;   // true si no hubo errores sintácticos
    string mensaje;   // descripción de errores o mensaje de éxito
};

//  Función principal del analizador sintáctico.
//  Recibe la lista de tokens del análisis léxico.
ResultadoSint analizarSintactico(const vector<Token>& tokens);