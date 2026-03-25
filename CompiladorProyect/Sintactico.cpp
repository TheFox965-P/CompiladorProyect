// =============================================================================
//  Sintactico.cpp
//  Implementación del analizador sintáctico mediante descenso recursivo.
//
//  El parser avanza por la lista de tokens verificando que la estructura
//  del programa cumpla con la gramática del lenguaje definido.
// =============================================================================

#include "Sintactico.h"
#include <string>
using namespace std;

// ---------------------------------------------------------------------------
//  Clase Parser (interna a este módulo)
// ---------------------------------------------------------------------------
class Parser {
public:
    const vector<Token>& toks;
    int    pos;
    string errores;
    bool   ok;

    Parser(const vector<Token>& t) : toks(t), pos(0), ok(true) {}

    // -----------------------------------------------------------------------
    //  Acceso al token actual y consumo
    // -----------------------------------------------------------------------
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

    // -----------------------------------------------------------------------
    //  Espera el fin de línea ;
    // -----------------------------------------------------------------------
    void esperarFin(int linea) {
        if (!match(T_FIN_LINEA)) {
            errores += "  [Linea " + to_string(linea) +
                "] Error Sintáctico: Se esperaba ';' al final de la instruccion.\r\n";
            ok = false;
        }
    }

    // -----------------------------------------------------------------------
    //  programa → sentencia*
    // -----------------------------------------------------------------------
    void parsePrograma() {
        while (pos < (int)toks.size()) {
            parseSentencia();
        }
    }

    // -----------------------------------------------------------------------
    //  sentencia → declaracion | mostrar | si | mientras | repetir | asignacion
    // -----------------------------------------------------------------------
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
        else if (c.tipo == T_IDENTIFICADOR) {
            parseAsignacion();
        }
        else if (c.tipo == T_LLAVE_CI) {
            return; // fin de bloque, el llamador consume }
        }
        else {
            errores += "  [Linea " + to_string(c.linea) +
                "] Error Sintactico: Sentencia inesperada '" + c.valor + "'.\r\n";
            ok = false;
            consume();
        }
    }

    // -----------------------------------------------------------------------
    //  declaracion → "crear" tipo_dato ID ["=" expr] ";"
    // -----------------------------------------------------------------------
    void parseDeclaracion() {
        int lin = current().linea;
        consume(); // "crear"

        if (current().tipo != T_TIPO_DATO) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba tipo de dato (entero|decimal) despues de 'crear'.\r\n";
            ok = false; return;
        }
        consume(); // tipo

        if (current().tipo != T_IDENTIFICADOR) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba nombre de variable en la declaracion.\r\n";
            ok = false; return;
        }
        consume(); // id

        // Inicialización opcional
        if (current().tipo == T_ASIGNACION) {
            consume(); // =
            parseExpr(lin);
        }

        esperarFin(lin);
    }

    // -----------------------------------------------------------------------
    //  mostrar → "mostrar" expr ";"
    // -----------------------------------------------------------------------
    void parseMostrar() {
        int lin = current().linea;
        consume(); // "mostrar"
        parseExpr(lin);
        esperarFin(lin);
    }

    // -----------------------------------------------------------------------
    //  si_stmt → "si" "(" expr ")" bloque ["sino" bloque]
    // -----------------------------------------------------------------------
    void parseSi() {
        int lin = current().linea;
        consume(); // "si"

        if (!match(T_PAREN_AB)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '(' después de 'si'.\r\n";
            ok = false; return;
        }
        parseExpr(lin);
        if (!match(T_PAREN_CI)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba ')' para cerrar condición de 'si'.\r\n";
            ok = false; return;
        }
        parseBloque(lin);

        // Parte opcional: sino
        if (current().tipo == T_PALABRA_RESERVADA && current().valor == "sino") {
            consume();
            parseBloque(lin);
        }
    }

    // -----------------------------------------------------------------------
    //  mientras_stmt → "mientras" "(" expr ")" bloque
    // -----------------------------------------------------------------------
    void parseMientras() {
        int lin = current().linea;
        consume(); // "mientras"

        if (!match(T_PAREN_AB)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '(' después de 'mientras'.\r\n";
            ok = false; return;
        }
        parseExpr(lin);
        if (!match(T_PAREN_CI)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba ')' para cerrar condición de 'mientras'.\r\n";
            ok = false; return;
        }
        parseBloque(lin);
    }

    // -----------------------------------------------------------------------
    //  repetir_stmt → "repetir" "(" NUM_ENTERO ")" bloque
    // -----------------------------------------------------------------------
    void parseRepetir() {
        int lin = current().linea;
        consume(); // "repetir"

        if (!match(T_PAREN_AB)) {
            errores += "  [Línea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '(' despues de 'repetir'.\r\n";
            ok = false; return;
        }
        if (current().tipo != T_NUMERO_ENTERO) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintáctico: 'repetir' requiere un numero entero como contador.\r\n";
            ok = false;
        }
        else {
            consume(); // número de iteraciones
        }
        if (!match(T_PAREN_CI)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba ')' para cerrar 'repetir'.\r\n";
            ok = false; return;
        }
        parseBloque(lin);
    }

    // -----------------------------------------------------------------------
    //  asignacion → ID "=" expr ";"
    // -----------------------------------------------------------------------
    void parseAsignacion() {
        int lin = current().linea;
        consume(); // ID

        if (!match(T_ASIGNACION)) {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba '=' en la asignacion.\r\n";
            ok = false; return;
        }
        parseExpr(lin);
        esperarFin(lin);
    }

    // -----------------------------------------------------------------------
    //  bloque → "{" sentencia* "}"
    // -----------------------------------------------------------------------
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

    // -----------------------------------------------------------------------
    //  expr → term ((OP_ARIT | OP_REL | OP_IGUAL) term)*
    // -----------------------------------------------------------------------
    void parseExpr(int lin) {
        parseTerm(lin);
        while (current().tipo == T_OPERADOR_ARIT ||
            current().tipo == T_OPERADOR_REL ||
            current().tipo == T_OPERADOR_IGUAL) {
            consume();
            parseTerm(lin);
        }
    }

    // -----------------------------------------------------------------------
    //  term → ID | NUM_ENTERO | NUM_DECIMAL
    // -----------------------------------------------------------------------
    void parseTerm(int lin) {
        Token c = current();
        if (c.tipo == T_NUMERO_ENTERO ||
            c.tipo == T_NUMERO_DECIMAL ||
            c.tipo == T_IDENTIFICADOR) {
            consume();
        }
        else {
            errores += "  [Linea " + to_string(lin) +
                "] Error Sintactico: Se esperaba un valor o identificador, se encontró '" +
                c.valor + "'.\r\n";
            ok = false;
            consume();
        }
    }
};

// ---------------------------------------------------------------------------
//  Función pública del módulo
// ---------------------------------------------------------------------------
ResultadoSint analizarSintactico(const vector<Token>& tokens) {
    Parser p(tokens);
    p.parsePrograma();

    ResultadoSint res;
    res.exitoso = p.ok;
    res.mensaje = p.errores;
    return res;
}