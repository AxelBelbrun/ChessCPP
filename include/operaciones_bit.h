//
// Created by axel on 10/12/23.
//

#ifndef CHESSCPP_OPERACIONES_BIT_H
#define CHESSCPP_OPERACIONES_BIT_H

#include <cstdlib>
#include <cstdio>

typedef unsigned long long U64;

namespace operaciones_bit {
    static U64 semilla = 1804289383;
    // Pone un bit en la posición indicada ..
    U64 setBit(U64 num, int pos, int bit);
    int LSB(U64 &num);
    u_short crearJugada(int salida, int llegada, int tipoDeJugada);
    int getLlegada(u_short movimiento);
    int getSalida(u_short movimiento);
    int getTipoDeJugada(u_short movimiento);
    int contadorDe1s(U64 num);
    U64 generador_numeros_random();
    U64 random_custom();
    int espejarCasilla(int casilla);

};


#endif //CHESSCPP_OPERACIONES_BIT_H
