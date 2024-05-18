//
// Created by axel on 13/12/23.
//

#include "Alfil.h"
#include <array>
#include <list>
#include "operaciones_bit.h"


using namespace constantes;

Alfil :: Alfil(){};



//Esta función genera los movimientos de un alfil en un tablero vacío, obviando las casillas de los extremos,
//detalle que elimina configuraciones de piezas "en el camino" del alfil redundantes.
U64 Alfil::generar_attack_mask(U64 bitboard_alfil) {
        U64 bitboardMovimientos = 0L;
        bitboardMovimientos |= attack_mask_nordeste(bitboard_alfil) | attack_mask_noroeste(bitboard_alfil) |
                             attack_mask_sudeste(bitboard_alfil) | attack_mask_sudoeste(bitboard_alfil);
        return bitboardMovimientos;

}

U64 Alfil::generar_movimientos_legales(U64 alfil, U64 piezasPropias, U64 piezasRivales, int turno) {
    U64 bitboardMovimientos = 0L;
    int casillaDeSalida = operaciones_bit::LSB(alfil);
    bitboardMovimientos |= movimientos_legales_nordeste(casillaDeSalida, piezasPropias, piezasRivales)
                           | movimientos_legales_noroeste(casillaDeSalida, piezasPropias, piezasRivales)
                           | movimientos_legales_sudeste(casillaDeSalida, piezasPropias, piezasRivales)
                           | movimientos_legales_sudoeste(casillaDeSalida, piezasPropias, piezasRivales);
    return bitboardMovimientos;
}

U64 Alfil::movimientos_legales_nordeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales) {
    U64 bitboardMovimientosAux = 0L;
    int casillaSiguiente = casillaDeSalida + 7;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente <= 64) && ((casillaSiguienteBitboard & AFile) == 0)) {
        if ((casillaSiguienteBitboard & piezasPropias) > 0) {
            break;
        } else if ((casillaSiguienteBitboard & piezasRivales) > 0) {
            bitboardMovimientosAux = operaciones_bit::setBit(bitboardMovimientosAux, casillaSiguiente, 1);
            break;
        } else {
            bitboardMovimientosAux = operaciones_bit::setBit(bitboardMovimientosAux, casillaSiguiente, 1);
        }
        casillaSiguiente += 7;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return bitboardMovimientosAux;
}

U64 Alfil::movimientos_legales_noroeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales) {
    U64 bitboardMovimientosAux = 0L;
    int casillaSiguiente = casillaDeSalida + 9;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente <= 64) && ((casillaSiguienteBitboard & HFile) == 0)) {
        if ((casillaSiguienteBitboard & piezasPropias) > 0) {
            break;
        }
        else if ((casillaSiguienteBitboard & piezasRivales) > 0) {
            bitboardMovimientosAux = operaciones_bit::setBit(bitboardMovimientosAux, casillaSiguiente, 1);
            break;
        } else {
            bitboardMovimientosAux = operaciones_bit::setBit(bitboardMovimientosAux, casillaSiguiente, 1);
        }
        casillaSiguiente += 9;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return bitboardMovimientosAux;
}

U64 Alfil::movimientos_legales_sudeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales) {
    U64 bitboardMovimientosAux = 0L;
    int casillaSiguiente = casillaDeSalida - 9;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente > 0) && ((casillaSiguienteBitboard & AFile) == 0)) {
        if ((casillaSiguienteBitboard & piezasPropias) > 0) {
            break;
        } else if ((casillaSiguienteBitboard & piezasRivales) > 0) {
            bitboardMovimientosAux = operaciones_bit::setBit(bitboardMovimientosAux, casillaSiguiente, 1);
            break;
        } else {
            bitboardMovimientosAux = operaciones_bit::setBit(bitboardMovimientosAux, casillaSiguiente, 1);
        }
        casillaSiguiente -= 9;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return bitboardMovimientosAux;
}

U64 Alfil::movimientos_legales_sudoeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales) {
    U64 bitboardMovimientosAux = 0L;
    int casillaSiguiente = casillaDeSalida - 7;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente > 0) && ((casillaSiguienteBitboard & HFile) == 0)) {
        if ((casillaSiguienteBitboard & piezasPropias) > 0) {
            break;
        } else if ((casillaSiguienteBitboard & piezasRivales) > 0) {
            bitboardMovimientosAux = operaciones_bit::setBit(bitboardMovimientosAux, casillaSiguiente, 1);
            break;
        } else {
            bitboardMovimientosAux = operaciones_bit::setBit(bitboardMovimientosAux, casillaSiguiente, 1);
        }
        casillaSiguiente -= 7;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return bitboardMovimientosAux;
}

U64 Alfil::attack_mask_nordeste(U64 bitboard_alfil) {
    U64 bitboardMovimientos = 0L;
    int casillaActual = operaciones_bit::LSB(bitboard_alfil);
    int casillaSiguiente = casillaActual + 7;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente <= 56) && ((casillaSiguienteBitboard & (HFile | AFile)) == 0)) {
        bitboardMovimientos = operaciones_bit::setBit(bitboardMovimientos, casillaSiguiente, 1);
        casillaSiguiente += 7;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return bitboardMovimientos;
}

U64 Alfil::attack_mask_noroeste(U64 bitboard_alfil) {
    U64 bitboardMovimientos = 0L;
    int casillaActual = operaciones_bit::LSB(bitboard_alfil);
    int casillaSiguiente = casillaActual + 9;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente <= 56) && ((casillaSiguienteBitboard & (HFile | AFile)) == 0)) {
        bitboardMovimientos = operaciones_bit::setBit(bitboardMovimientos, casillaSiguiente, 1);
        casillaSiguiente += 9;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return bitboardMovimientos;
}

U64 Alfil::attack_mask_sudeste(U64 bitboard_alfil) {
    U64 bitboardMovimientos = 0L;
    int casillaActual = operaciones_bit::LSB(bitboard_alfil);
    int casillaSiguiente = casillaActual - 9;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente > 8) && ((casillaSiguienteBitboard & (HFile | AFile)) == 0)) {
        bitboardMovimientos = operaciones_bit::setBit(bitboardMovimientos, casillaSiguiente, 1);
        casillaSiguiente -= 9;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return bitboardMovimientos;
}

U64 Alfil::attack_mask_sudoeste(U64 bitboard_alfil) {
    U64 bitboardMovimientos = 0L;
    int casillaActual = operaciones_bit::LSB(bitboard_alfil);
    int casillaSiguiente = casillaActual - 7;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente > 8) && ((casillaSiguienteBitboard & (HFile | AFile)) == 0)) {
        bitboardMovimientos = operaciones_bit::setBit(bitboardMovimientos, casillaSiguiente, 1);
        casillaSiguiente -= 7;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return bitboardMovimientos;
}