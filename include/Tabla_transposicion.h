//
// Created by axel on 12/01/24.
//

#ifndef CHESSCPP_TABLA_TRANSPOSICION_H
#define CHESSCPP_TABLA_TRANSPOSICION_H
#include <cstdlib>
#include <cstdio>
#include "Tablero.h"
typedef unsigned long long U64;

class Tabla_transposicion {
public:
    Tabla_transposicion();
    void asignarTablero(Tablero* tablero);
    void insertar(U64 clave, float valor, int profundidad, int tipo, u_short jugada);
    void limpiarTabla();


    U64 index(U64 clave);
    u_short obtenerMovimientoGuardado(U64 clave);
    U64  count;
    typedef struct entrada {
        entrada() {
            clave = 0;
            valor = 0;
            profundidad = 0;
            tipo = 0;
            jugada = 0;
        }
        U64 clave;
        float valor;
        int profundidad;
        int tipo;
        u_short jugada = 0;
    };

    entrada obtenerEntrada(U64 clave);


    Tablero* tablero;
    int tamanoTabla;
    entrada tabla[4*419430];



};


#endif //CHESSCPP_TABLA_TRANSPOSICION_H
