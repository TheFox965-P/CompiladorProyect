#pragma once
#include <string>
#include <vector>
#include <map>
#include "Token.h"
using namespace std;

struct Simbolo {
    string tipo;
    string valor;
    bool   inicializado;
};

extern map<string, Simbolo> tablaSimbolos;

struct ResultadoSem {
    bool   exitoso;
    string mensaje;
};

ResultadoSem analizarSemantico(const vector<Token>& tokens);
