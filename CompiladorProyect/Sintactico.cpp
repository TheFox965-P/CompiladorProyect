#include "Sintactico.h"
#include <string>
using namespace std;

class Parser {
public:
    const vector<Token>& toks;
    int    pos;
    string errores;
    bool   ok;

    Parser(const vector<Token>& t) : toks(t), pos(0), ok(true) {}

    Token current() const {
        if (pos < (int)toks.size()) return toks[pos];
        Token eof; eof.tipo = T_DESCONOCIDO; eof.valor = "EOF"; eof.linea = -1;
        return eof;
    }

    Token consume() {
        Token t = current();
        pos++;
        return t;
    }

    bool match(TipoToken tipo, const string& val = "") {
        Token c = current();
        if (c.tipo != tipo) return false;
        if (!val.empty() && c.valor != val) return false;
        consume();
        return true;
    }

    void esperarFin(int linea) {
        if (!match(T_FIN_LINEA)) {
            errores += "  [Linea " + to_string(linea) +
                "] Error Sintactico: Se esperaba ';' al final de la instruccion.\r\n";
            ok = false;
        }
    }

    void parsePrograma() {
        while (pos < (int)toks.size()) {
            int posAnterior = pos;
            parseSentencia();

            if (pos == posAnterior) {
                Token c = current();
                errores += "  [Linea " + to_string(c.linea) +
                    "] Error Sintactico: No se pudo continuar el analisis desde '" +
                    c.valor + "'.\r\n";
                ok = false;
                consume();
            }
        }
    }

    void parseSentencia() {
        Token c = current();
        if (c.tipo == T_DESCONOCIDO) { consume(); return; }

        if (c.tipo == T_PALABRA_RESERVADA && c.valor == "crear") {
            parseDeclaracion();
        }
        else if (c.tipo == T_PALABRA_RESERVADA && c.valor == "mostrar") {
            parseMostrar();
        }
        else if (c.tipo == T_PALABRA_RESERVADA && c.valor == "si") {
            parseSi();
        }
        else if (c.tipo == T_PALABRA_RESERVADA && c.valor == "mientras") {
            parseMientras();
        }
        else if (c.tipo == T_PALABRA_RESERVADA && c.valor == "repetir") {
            parseRepetir();
        }
        else if (c.tipo == T_PALABRA_RESERVADA && c.valor == "duplicar") {
            parseOperacionVariable("duplicar", linMensaje("duplicar"));
        }
        else if (c.tipo == T_PALABRA_RESERVADA && c.valor == "reiniciar") {
            parseOperacionVariable("reiniciar", linMensaje("reiniciar"));
        }
        else if (c.tipo == T_IDENTIFICADOR) {
            parseAsignacion();
        }
        else if (c.tipo == T_LLAVE_CI) {
            errores += "  [Linea " + to_string(c.linea) +
                "] Error Sintactico: Llave de cierre '}' inesperada.\r\n";
            ok = false;
            consume();
        }
        else {
            errores += "  [Linea " + to_string(c.linea) +
                "] Error Sintactico: Sentencia inesperada '" + c.valor + "'.\r\n";
            ok = false;
            consume();
        }
    }

    string linMensaje(const string& palabra) {
        return "Se esperaba un identificador despues de '" + palabra + "'.";
    }

    void parseDeclaracion() {
        int lin = current().linea;
        consume();

        if (current().tipo != T_TIPO_DATO) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba tipo de dato (entero|decimal) despues de 'crear'.\r\n";
            ok = false; return;
        }
        consume();

        if (current().tipo != T_IDENTIFICADOR) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba nombre de variable en la declaracion.\r\n";
            ok = false; return;
        }
        consume();

        if (current().tipo == T_ASIGNACION) {
            consume();
            parseExpr(lin);
        }

        esperarFin(lin);
    }

    void parseMostrar() {
        int lin = current().linea;
        consume();
        parseExpr(lin);
        esperarFin(lin);
    }

    void parseSi() {
        int lin = current().linea;
        consume();

        if (!match(T_PAREN_AB)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '(' después de 'si'.\r\n";
            ok = false; return;
        }
        parseExpr(lin);
        if (!match(T_PAREN_CI)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba ')' para cerrar condicion de 'si'.\r\n";
            ok = false; return;
        }
        parseBloque(lin);

        if (current().tipo == T_PALABRA_RESERVADA && current().valor == "sino") {
            consume();
            parseBloque(lin);
        }
    }

    void parseMientras() {
        int lin = current().linea;
        consume();

        if (!match(T_PAREN_AB)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '(' después de 'mientras'.\r\n";
            ok = false; return;
        }
        parseExpr(lin);
        if (!match(T_PAREN_CI)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba ')' para cerrar condicion de 'mientras'.\r\n";
            ok = false; return;
        }
        parseBloque(lin);
    }

    void parseRepetir() {
        int lin = current().linea;
        consume();

        if (!match(T_PAREN_AB)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '(' despues de 'repetir'.\r\n";
            ok = false; return;
        }
        if (current().tipo != T_NUMERO_ENTERO) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintáctico: 'repetir' requiere un numero entero como contador.\r\n";
            ok = false;
        }
        else {
            consume();
        }
        if (!match(T_PAREN_CI)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba ')' para cerrar 'repetir'.\r\n";
            ok = false; return;
        }
        parseBloque(lin);
    }

    void parseOperacionVariable(const string& palabra, const string& mensajeError) {
        int lin = current().linea;
        consume();

        if (current().tipo != T_IDENTIFICADOR) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: " + mensajeError + "\r\n";
            ok = false;
            return;
        }

        consume();
        esperarFin(lin);
    }

    void parseAsignacion() {
        int lin = current().linea;
        consume();

        if (!match(T_ASIGNACION)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '=' en la asignacion.\r\n";
            ok = false; return;
        }
        parseExpr(lin);
        esperarFin(lin);
    }

    void parseBloque(int lin) {
        if (!match(T_LLAVE_AB)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '{' para abrir bloque.\r\n";
            ok = false; return;
        }
        while (pos < (int)toks.size() && current().tipo != T_LLAVE_CI) {
            parseSentencia();
        }
        if (!match(T_LLAVE_CI)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '}' para cerrar bloque.\r\n";
            ok = false;
        }
    }

    void parseExpr(int lin) {
        parseTerm(lin);
        while (current().tipo == T_OPERADOR_ARIT ||
            current().tipo == T_OPERADOR_REL ||
            current().tipo == T_OPERADOR_IGUAL) {
            consume();
            parseTerm(lin);
        }
    }

    void parseTerm(int lin) {
        Token c = current();
        if (c.tipo == T_NUMERO_ENTERO ||
            c.tipo == T_NUMERO_DECIMAL ||
            c.tipo == T_IDENTIFICADOR) {
            consume();
        }
        else {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba un valor o identificador, se encontro '" +
                c.valor + "'.\r\n";
            ok = false;
            consume();
        }
    }
};

ResultadoSint analizarSintactico(const vector<Token>& tokens) {
    Parser p(tokens);
    p.parsePrograma();

    ResultadoSint res;
    res.exitoso = p.ok;
    res.mensaje = p.errores;
    return res;
}
