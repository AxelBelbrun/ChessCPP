//
// Created by axel on 12/01/24.
//

#ifndef CHESSCPP_TABLATRANS_H
#define CHESSCPP_TABLATRANS_H
#include <cstdlib>
#include <cstdio>
#include "Tablero.h"
typedef unsigned long long U64;

class TablaTrans {
public:
    TablaTrans();
    void asignarTablero(Tablero* tablero);
    void insertar(U64 clave, float valor, int profundidad, int tipo, u_short jugada);
    void limpiarTabla();


    U64 index();
    u_short obtenerMovimientoGuardado();
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

    entrada obtenerEntrada();


    Tablero* tablero;
    int tamanoTabla;
    entrada* tabla[20833333];



};


#endif //CHESSCPP_TABLATRANS_H
