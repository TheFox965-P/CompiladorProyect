// =============================================================================
//  Semantico.cpp
//  Implementación del analizador semántico.
//
//  Recorre la lista de tokens y aplica reglas semánticas:
//    1. Registra declaraciones (crear tipo id) en la tabla de símbolos.
//    2. Verifica que no se redeclare la misma variable.
//    3. Verifica compatibilidad de tipos en asignaciones.
//    4. Verifica que los identificadores usados estén declarados.
//    5. Advierte cuando se usa una variable sin inicializar.
//    6. Al finalizar imprime la tabla de símbolos.
// =============================================================================

#include "Semantico.h"
#include <string>
using namespace std;

// ---------------------------------------------------------------------------
//  Definición de la tabla de símbolos (declarada extern en el .h)
// ---------------------------------------------------------------------------
map<string, Simbolo> tablaSimbolos;

// ---------------------------------------------------------------------------
//  analizarSemantico
// ---------------------------------------------------------------------------
ResultadoSem analizarSemantico(const vector<Token>& tokens) {
    ResultadoSem res;
    res.exitoso = true;
    tablaSimbolos.clear();

    string errores;
    int i = 0;
    int n = (int)tokens.size();

    // Función lambda para avanzar hasta el próximo ;
    auto saltarHastaFin = [&]() {
        while (i < n && tokens[i].tipo != T_FIN_LINEA) i++;
        if (i < n) i++; // consume ;
        };

    while (i < n) {
        Token c = tokens[i];

        // ===================================================================
        //  DECLARACIÓN: crear tipo id [= valor] ;
        // ===================================================================
        if (c.tipo == T_PALABRA_RESERVADA && c.valor == "crear") {
            i++;
            if (i >= n) break;

            string tipo = tokens[i].valor;  i++; // tipo de dato
            if (i >= n) break;

            string nombre = tokens[i].valor;
            int    linDec = tokens[i].linea;   i++; // identificador

            // ¿Ya fue declarada?
            if (tablaSimbolos.count(nombre)) {
                errores += "  [Linea " + to_string(linDec) +
                    "] Error Semantico: La variable '" + nombre +
                    "' ya fue declarada anteriormente.\r\n";
                res.exitoso = false;
                saltarHastaFin();
                continue;
            }

            Simbolo s;
            s.tipo = tipo;
            s.inicializado = false;

            // ¿Tiene asignación inicial?
            if (i < n && tokens[i].tipo == T_ASIGNACION) {
                i++; // consume =
                if (i < n) {
                    TipoToken tipoTok = tokens[i].tipo;

                    // Verificar compatibilidad de tipo
                    if (tipo == "entero" && tipoTok == T_NUMERO_DECIMAL) {
                        errores += "  [Linea " + to_string(linDec) +
                            "] Error Semantico: No se puede asignar un valor decimal"
                            " a la variable 'entero' '" + nombre + "'.\r\n";
                        res.exitoso = false;
                    }
                    s.valor = tokens[i].valor;
                    s.inicializado = true;
                    i++;
                }
            }

            tablaSimbolos[nombre] = s;
            saltarHastaFin();
        }

        // ===================================================================
        //  ASIGNACIÓN: id = expr ;
        // ===================================================================
        else if (c.tipo == T_IDENTIFICADOR &&
            i + 1 < n && tokens[i + 1].tipo == T_ASIGNACION) {

            string nombre = c.valor;
            int    linA = c.linea;
            i += 2; // consume id y =

            if (!tablaSimbolos.count(nombre)) {
                errores += "  [Linea " + to_string(linA) +
                    "] Error Semantico: La variable '" + nombre +
                    "' no ha sido declarada.\r\n";
                res.exitoso = false;
            }
            else {
                // Verificar tipo del valor asignado
                if (i < n) {
                    TipoToken tipoTok = tokens[i].tipo;
                    if (tablaSimbolos[nombre].tipo == "entero" &&
                        tipoTok == T_NUMERO_DECIMAL) {
                        errores += "  [Linea " + to_string(linA) +
                            "] Error Semantico: No se puede asignar un valor decimal"
                            " a la variable 'entero' '" + nombre + "'.\r\n";
                        res.exitoso = false;
                    }
                    tablaSimbolos[nombre].inicializado = true;
                    tablaSimbolos[nombre].valor = tokens[i].valor;
                }
            }
            saltarHastaFin();
        }

        // ===================================================================
        //  MOSTRAR: mostrar expr ;
        //  Verifica que la variable exista y esté inicializada
        // ===================================================================
        else if (c.tipo == T_PALABRA_RESERVADA && c.valor == "mostrar") {
            i++;
            if (i < n && tokens[i].tipo == T_IDENTIFICADOR) {
                string nombre = tokens[i].valor;
                int    linM = tokens[i].linea;

                if (!tablaSimbolos.count(nombre)) {
                    errores += "  [Linea " + to_string(linM) +
                        "] Error Semantico: La variable '" + nombre +
                        "' no ha sido declarada.\r\n";
                    res.exitoso = false;
                }
                else if (!tablaSimbolos[nombre].inicializado) {
                    errores += "  [Linea " + to_string(linM) +
                        "] Advertencia Semantica: La variable '" + nombre +
                        "' se usa sin haber sido inicializada.\r\n";
                }
            }
            saltarHastaFin();
        }

        // ===================================================================
        //  USO DE IDENTIFICADOR EN EXPRESIONES (condiciones, etc.)
        // ===================================================================
        else if (c.tipo == T_IDENTIFICADOR) {
            if (!tablaSimbolos.count(c.valor)) {
                errores += "  [Linea " + to_string(c.linea) +
                    "] Error Semantico: La variable '" + c.valor +
                    "' no ha sido declarada.\r\n";
                res.exitoso = false;
            }
            i++;
        }

        else { i++; }
    }

    // -----------------------------------------------------------------------
    //  Tabla de símbolos al final del reporte
    // -----------------------------------------------------------------------
    if (!tablaSimbolos.empty()) {
        errores += "\r\nTABLA DE SIMBOLOS:\r\n";
        errores += string(44, '-') + "\r\n";
        errores += " Variable         | Tipo    | Inicializado\r\n";
        errores += string(44, '-') + "\r\n";

        for (auto& kv : tablaSimbolos) {
            char buf[128];
            sprintf_s(buf, sizeof(buf), " %-17s| %-8s| %s\r\n",
                kv.first.c_str(),
                kv.second.tipo.c_str(),
                kv.second.inicializado ? "Sí" : "No");
            errores += buf;
        }
        errores += string(44, '-') + "\r\n";
    }

    res.mensaje = errores;
    return res;
}