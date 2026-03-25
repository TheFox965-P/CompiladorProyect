// =============================================================================
//  Token.cpp
//  Implementación de la función auxiliar tipoTokenStr.
// =============================================================================

#include "Token.h"

string tipoTokenStr(TipoToken t) {
    switch (t) {
    case T_PALABRA_RESERVADA: return "Palabra Reservada";
    case T_TIPO_DATO:         return "Tipo de Dato";
    case T_IDENTIFICADOR:     return "Identificador";
    case T_NUMERO_ENTERO:     return "Numero Entero";
    case T_NUMERO_DECIMAL:    return "Numero Decimal";
    case T_OPERADOR_ARIT:     return "Operador Aritmético";
    case T_OPERADOR_REL:      return "Operador Relacional";
    case T_OPERADOR_IGUAL:    return "Operador de Igualdad";
    case T_ASIGNACION:        return "Asignacion";
    case T_FIN_LINEA:         return "Fin de Linea (;)";
    case T_LLAVE_AB:          return "Llave Abre ({)";
    case T_LLAVE_CI:          return "Llave Cierra (})";
    case T_PAREN_AB:          return "Parentesis Abre";
    case T_PAREN_CI:          return "Parentesis Cierra";
    case T_SIMBOLO_ESP:       return "Simbolo Especial";
    default:                  return "Desconocido";
    }
}