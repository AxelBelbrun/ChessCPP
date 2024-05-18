//
// Created by axel on 09/12/23.
//

#include "Caballo.h"
#include <array>
#include <list>

using namespace constantes;
Caballo :: Caballo(){};

// Bitboards de movimientos legales para un caballo en una posicion dada. 
// Si fuese ilegal, el bitboard es 0.
static U64 noNoEa(U64 b) {return (b << 17) & ~HFile ;}
static U64 noEaEa(U64 b) {return (b << 10) & ~HFile & ~GFile;}
static U64 soEaEa(U64 b) {return (b >>  6) & ~HFile & ~GFile;}
static U64 soSoEa(U64 b) {return (b >> 15) & ~HFile ;}
static U64 noNoWe(U64 b) {return (b << 15) & ~AFile ;}
static U64 noWeWe(U64 b) {return (b <<  6) & ~BFile & ~AFile;}
static U64 soWeWe(U64 b) {return (b >> 10) & ~BFile & ~AFile;}
static U64 soSoWe(U64 b) {return (b >> 17) & ~AFile ;}


void Caballo:: capturas(Tablero* tablero,  std::vector<u_short>* movimientos,U64 bitboard_caballos,
                        U64 piezasPropias, U64 piezasRivales) {
    while (bitboard_caballos > 0) {

        //Se obtiene la casilla del caballo más cercano, empezando desde la derecha del bitboard.
        int casilla = operaciones_bit::LSB(bitboard_caballos);

        //Se crea un bitboard con la casilla actual en 1 y el resto en 0.
        U64 caballo = operaciones_bit::setBit(0L, casilla, 1);

        //Se crea un bitboard con los 8 movimientos posibles para la posición actual del caballo
        U64 movimientos_generados = 0L;
        movimientos_generados = movimientos_generados | noNoEa(caballo) | noEaEa(caballo) | soEaEa(caballo) |
                                soSoEa(caballo) | noNoWe(caballo) | noWeWe(caballo) |
                                soWeWe(caballo) | soSoWe(caballo);

        while (movimientos_generados > 0) {
            int llegada = operaciones_bit::LSB(movimientos_generados);
            U64 llegada_bitboard = operaciones_bit::setBit(0L, llegada, 1);
            if ((llegada_bitboard & piezasRivales) > 0) {
                movimientos->push_back(operaciones_bit::crearJugada(casilla, llegada, CAPTURE));
                continue;

            }
        }
    }
}
void Caballo:: obtener_movimientos(Tablero* tablero, std::vector<u_short>* movimientos,U64 bitboard_caballos,
                                           U64 piezasPropias, U64 piezasRivales) {



    //Se calculan los 8 movimientos posibles y se chequea que sean legales,
    // Se repite esto hasta que ya no haya caballos en el bitboard
    
    while(bitboard_caballos > 0) {

        //Se obtiene la casilla del caballo más cercano, empezando desde la derecha del bitboard.
        int casilla = operaciones_bit::LSB(bitboard_caballos);

        //Se crea un bitboard con la casilla actual en 1 y el resto en 0.
        U64 caballo = operaciones_bit::setBit(0L, casilla, 1);

        //Se crea un bitboard con los 8 movimientos posibles para la posición actual del caballo
        U64 movimientos_generados = generar_movimientos_legales(caballo, piezasPropias, piezasRivales, 0);

        //Se separan en distintos bitboards los movimientos que son capturas de los que no lo son

            while(movimientos_generados > 0) {
                int llegada = operaciones_bit::LSB(movimientos_generados);
                U64 llegada_bitboard = operaciones_bit::setBit(0L, llegada, 1);
                if ((llegada_bitboard & piezasPropias) > 0) {
                    continue;

                } else if (tablero->esJaque(
                        this-> generar_movimientos_legales(llegada_bitboard, piezasPropias, piezasRivales, 0),
                        tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(casilla, llegada, CHECK));
                    continue;
                } else if ((llegada_bitboard & piezasRivales) > 0) {
                    movimientos->push_back(operaciones_bit::crearJugada(casilla, llegada, CAPTURE));
                    continue;
                } else {
                    movimientos->push_back(operaciones_bit::crearJugada(casilla, llegada, QUIET));

                }
            }




    }


};

U64 Caballo::generar_movimientos_legales(U64 bitboard_caballo, U64 piezasPropias, U64 piezasRivales, int turno) {
    U64  movimientos_generados = 0L;
    movimientos_generados = movimientos_generados | noNoEa(bitboard_caballo) | noEaEa(bitboard_caballo) | soEaEa(bitboard_caballo) |
                            soSoEa(bitboard_caballo) | noNoWe(bitboard_caballo) | noWeWe(bitboard_caballo) |
                            soWeWe(bitboard_caballo) | soSoWe(bitboard_caballo);
    movimientos_generados = movimientos_generados & (~piezasPropias);
    return movimientos_generados;
}
