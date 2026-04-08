#include "Lexico.h"
#include <set>
#include <cctype>

using namespace std;

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

//  Helpers
static bool esLetra(char c) { return isalpha((unsigned char)c) || c == '_'; }
static bool esDigito(char c) { return isdigit((unsigned char)c) != 0; }

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

        // Comentarios 
        if (c == '\n') { linea++; i++; continue; }

        if (isspace((unsigned char)c)) { i++; continue; }

        if (c == '/' && i + 1 < n && codigo[i + 1] == '/') {
            while (i < n && codigo[i] != '\n') i++;
            continue;
        }

        Token tok;
        tok.linea = linea;

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

        else if (c == '=' && i + 1 < n && codigo[i + 1] == '=') {
            tok.valor = "=="; tok.tipo = T_OPERADOR_IGUAL; i += 2;
        }

        else if (c == '!' && i + 1 < n && codigo[i + 1] == '=') {
            tok.valor = "!="; tok.tipo = T_OPERADOR_IGUAL; i += 2;
        }

        else if (c == '=') {
            tok.valor = "="; tok.tipo = T_ASIGNACION; i++;
        }

        else if (c == '>' || c == '<') {
            tok.valor = string(1, c); tok.tipo = T_OPERADOR_REL; i++;
        }

        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            tok.valor = string(1, c); tok.tipo = T_OPERADOR_ARIT; i++;
        }

        else if (c == ';') {
            tok.valor = ";"; tok.tipo = T_FIN_LINEA; i++;
        }

        else if (c == '{') { tok.valor = "{"; tok.tipo = T_LLAVE_AB; i++; }
        else if (c == '}') { tok.valor = "}"; tok.tipo = T_LLAVE_CI; i++; }

        else if (c == '(') { tok.valor = "("; tok.tipo = T_PAREN_AB; i++; }
        else if (c == ')') { tok.valor = ")"; tok.tipo = T_PAREN_CI; i++; }

        else if (simbolosEspeciales.count(c)) {
            tok.valor = string(1, c); tok.tipo = T_SIMBOLO_ESP; i++;
        }

        // error lexico 
        else {
            tok.valor = string(1, c); tok.tipo = T_DESCONOCIDO;
            errores += "  [Linea " + to_string(linea) +
                "] Error Lexico: Caracter desconocido '" +
                string(1, c) + "'\r\n";
            res.exitoso = false;
            i++;
        }

        res.tokens.push_back(tok);

        // Agregar fila 
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