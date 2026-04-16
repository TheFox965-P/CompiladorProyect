#pragma once

#include <string>
#include <vector>
#include "Token.h"
using namespace std;

struct ResultadoSint {
    bool   exitoso;
    string mensaje;
};

ResultadoSint analizarSintactico(const vector<Token>& tokens);
