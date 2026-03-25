// =============================================================================
//  Lexico.cpp
//  Implementación del analizador léxico.
//
//  Recorre el código fuente carácter a carácter construyendo tokens.
//  Reconoce:
//    - Identificadores y palabras reservadas (crear, mostrar, repetir, si, sino, mientras)
//    - Tipos de dato: entero, decimal
//    - Números enteros y decimales
//    - Operadores: + - * /  >  <  ==  !=  =
//    - Fin de línea: ;
//    - Llaves: { }
//    - Paréntesis: ( )
//    - Símbolos especiales: @ $ % ^ ~ ,
//    - Comentarios de línea: //   (ignorados)
// =============================================================================

#include "Lexico.h"
#include <set>
#include <cctype>

using namespace std;

// ---------------------------------------------------------------------------
//  Conjuntos estáticos de clasificación
// ---------------------------------------------------------------------------
static const set<string> palabrasReservadas = {
    "crear", "mostrar", "repetir",
    "si", "sino", "mientras"
};

static const set<string> tiposDato = {
    "entero", "decimal"
};

static const set<char> simbolosEspeciales = {
    '@', '$', '%', '^', '~', ','
};

// ---------------------------------------------------------------------------
//  Helpers
// ---------------------------------------------------------------------------
static bool esLetra(char c) { return isalpha((unsigned char)c) || c == '_'; }
static bool esDigito(char c) { return isdigit((unsigned char)c) != 0; }

// ---------------------------------------------------------------------------
//  analizarLexico
// ---------------------------------------------------------------------------
ResultadoLexico analizarLexico(const string& codigo) {
    ResultadoLexico res;
    res.exitoso = true;

    string errores;
    string tabla = "TABLA DE TOKENS\r\n";
    tabla += string(56, '-') + "\r\n";
    tabla += " Linea | Token               | Tipo\r\n";
    tabla += string(56, '-') + "\r\n";

    int i = 0;
    int n = (int)codigo.size();
    int linea = 1;

    while (i < n) {
        char c = codigo[i];

        // --- Salto de línea ---
        if (c == '\n') { linea++; i++; continue; }

        // --- Espacios y tabuladores ---
        if (isspace((unsigned char)c)) { i++; continue; }

        // --- Comentario de una línea: // ---
        if (c == '/' && i + 1 < n && codigo[i + 1] == '/') {
            while (i < n && codigo[i] != '\n') i++;
            continue;
        }

        Token tok;
        tok.linea = linea;

        // --- Identificador o palabra reservada ---
        if (esLetra(c)) {
            string word;
            while (i < n && (esLetra(codigo[i]) || esDigito(codigo[i])))
                word += codigo[i++];
            tok.valor = word;

            if (palabrasReservadas.count(word))
                tok.tipo = T_PALABRA_RESERVADA;
            else if (tiposDato.count(word))
                tok.tipo = T_TIPO_DATO;
            else
                tok.tipo = T_IDENTIFICADOR;
        }

        // --- Número entero o decimal ---
        else if (esDigito(c)) {
            string num;
            bool esFloat = false;
            while (i < n && esDigito(codigo[i]))
                num += codigo[i++];
            if (i < n && codigo[i] == '.' && i + 1 < n && esDigito(codigo[i + 1])) {
                esFloat = true;
                num += codigo[i++];
                while (i < n && esDigito(codigo[i]))
                    num += codigo[i++];
            }
            tok.valor = num;
            tok.tipo = esFloat ? T_NUMERO_DECIMAL : T_NUMERO_ENTERO;
        }

        // --- Operador de igualdad == ---
        else if (c == '=' && i + 1 < n && codigo[i + 1] == '=') {
            tok.valor = "=="; tok.tipo = T_OPERADOR_IGUAL; i += 2;
        }

        // --- Operador de desigualdad != ---
        else if (c == '!' && i + 1 < n && codigo[i + 1] == '=') {
            tok.valor = "!="; tok.tipo = T_OPERADOR_IGUAL; i += 2;
        }

        // --- Asignación = ---
        else if (c == '=') {
            tok.valor = "="; tok.tipo = T_ASIGNACION; i++;
        }

        // --- Operadores relacionales > < ---
        else if (c == '>' || c == '<') {
            tok.valor = string(1, c); tok.tipo = T_OPERADOR_REL; i++;
        }

        // --- Operadores aritméticos + - * / ---
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            tok.valor = string(1, c); tok.tipo = T_OPERADOR_ARIT; i++;
        }

        // --- Fin de línea ; ---
        else if (c == ';') {
            tok.valor = ";"; tok.tipo = T_FIN_LINEA; i++;
        }

        // --- Llaves ---
        else if (c == '{') { tok.valor = "{"; tok.tipo = T_LLAVE_AB; i++; }
        else if (c == '}') { tok.valor = "}"; tok.tipo = T_LLAVE_CI; i++; }

        // --- Paréntesis ---
        else if (c == '(') { tok.valor = "("; tok.tipo = T_PAREN_AB; i++; }
        else if (c == ')') { tok.valor = ")"; tok.tipo = T_PAREN_CI; i++; }

        // --- Símbolos especiales ---
        else if (simbolosEspeciales.count(c)) {
            tok.valor = string(1, c); tok.tipo = T_SIMBOLO_ESP; i++;
        }

        // --- Carácter desconocido → error léxico ---
        else {
            tok.valor = string(1, c); tok.tipo = T_DESCONOCIDO;
            errores += "  [Linea " + to_string(linea) +
                "] Error Lexico: Caracter desconocido '" +
                string(1, c) + "'\r\n";
            res.exitoso = false;
            i++;
        }

        res.tokens.push_back(tok);

        // Agregar fila a la tabla de tokens
        char buf[128];
        sprintf_s(buf, sizeof(buf), "  %4d  | %-20s| %s\r\n",
            tok.linea,
            tok.valor.c_str(),
            tipoTokenStr(tok.tipo).c_str());
        tabla += buf;
    }

    tabla += string(56, '-') + "\r\n";
    tabla += "  Total de tokens: " + to_string(res.tokens.size()) + "\r\n";

    res.errores = errores;
    res.tabla = tabla;
    return res;
}