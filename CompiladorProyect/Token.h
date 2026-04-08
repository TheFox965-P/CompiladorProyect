#pragma once


#include <string>
using namespace std;


enum TipoToken {
    T_PALABRA_RESERVADA,   
    T_TIPO_DATO,           
    T_IDENTIFICADOR,       
    T_NUMERO_ENTERO,       
    T_NUMERO_DECIMAL,      
    T_OPERADOR_ARIT,       
    T_OPERADOR_REL,        
    T_OPERADOR_IGUAL,      
    T_ASIGNACION,          
    T_FIN_LINEA,           
    T_LLAVE_AB,            
    T_LLAVE_CI,            
    T_PAREN_AB,            
    T_PAREN_CI,            
    T_SIMBOLO_ESP,         
    T_DESCONOCIDO          
};

struct Token {
    TipoToken tipo;    
    string    valor;   
    int       linea;   
};

string tipoTokenStr(TipoToken t);