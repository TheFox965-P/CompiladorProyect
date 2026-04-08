#pragma once

#include <string>
#include <vector>
#include "Token.h"
using namespace std;

struct ResultadoLexico {
    bool          exitoso;  
    string        errores;  
    vector<Token> tokens;   
    string        tabla;    
};


ResultadoLexico analizarLexico(const string& codigo);