//
// Created by axel on 13/12/23.
//

#include "Dama.h"
#include "Alfil.h"
#include "Torre.h"
#include "operaciones_bit.h"
using namespace constantes;
Dama :: Dama(){};


//Genera los movimientos
U64 Dama::generar_movimientos_legales(U64 pieza, U64 piezasPropias, U64 piezasRivales, int turno) {

    static Alfil alfil;
    static Torre torre;
    U64 movimientos = 0L;
    movimientos = movimientos | alfil.generar_movimientos_legales(pieza, piezasPropias, piezasRivales, 0);
    movimientos = movimientos | torre.generar_movimientos_legales(pieza, piezasPropias, piezasRivales, 0);
    return movimientos;
}

//Genera la mascara de ataque
U64 Dama::generar_attack_mask(U64 bitboard_dama) {
    static Alfil alfil;
    static Torre torre;
    U64 movimientos = 0L;
    movimientos = movimientos | alfil.generar_attack_mask(bitboard_dama);
    movimientos = movimientos | torre.generar_attack_mask(bitboard_dama);
    return movimientos;
}

//
