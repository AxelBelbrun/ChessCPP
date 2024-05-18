#include "Torre.h"
#include <list>
using namespace constantes;

Torre:: Torre(){};


U64 Torre::generar_attack_mask(U64 torreBitboard) {
    U64 attack_mask = 0L;
    attack_mask |= attack_mask_arriba(torreBitboard)
                | attack_mask_abajo(torreBitboard)
                | attack_mask_izquierda(torreBitboard)
                | attack_mask_derecha(torreBitboard);
    return attack_mask;
}

U64 Torre::generar_movimientos_legales(U64 torres, U64 piezasPropias, U64 piezasRivales, int turno) {
    U64 movimientos = 0L;
    int casillaDeSalida = operaciones_bit::LSB(torres);
    movimientos |= movimientos_arriba(casillaDeSalida, piezasPropias, piezasRivales)
                | movimientos_abajo(casillaDeSalida, piezasPropias, piezasRivales)
                | movimientos_izquierda(casillaDeSalida, piezasPropias, piezasRivales)
                | movimientos_derecha(casillaDeSalida, piezasPropias, piezasRivales);

    return movimientos;
}

U64 Torre::movimientos_arriba(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales) {
    U64 movimientos = 0L;
    int casillaSiguiente= casillaDeSalida + 8;
    while (casillaSiguiente <= 64) {
        U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
        // Si en la casilla siguiente (hacia arriba) hay una pieza nuestra se corta el ciclo
        if ((casillaSiguienteBitboard & piezasPropias) > 0) {
            break;
        }
            // Si hay una pieza rival se agrega el move como captura y se corta el ciclo
        else if ((casillaSiguienteBitboard & piezasRivales) > 0) {
            movimientos = operaciones_bit::setBit(movimientos, casillaSiguiente, 1);
            break;
        }
            //Si no, se agre como quiet y ya. LSB es la posicion original de la torre y
            //pos es la casilla que estamos chequeando ahora mismo..
        else {
            movimientos = operaciones_bit::setBit(movimientos, casillaSiguiente, 1);

        }

        casillaSiguiente += 8;
    }
    return movimientos;

}

U64 Torre::movimientos_abajo(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales) {
    U64 movimientos = 0L;
    int casillaSiguiente = casillaDeSalida - 8;
    while (casillaSiguiente > 0) {
        U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
        // Si en la casilla siguiente (hacia abajo) hay una pieza nuestra se corta el ciclo
        if ((casillaSiguienteBitboard & piezasPropias) > 0) {
            break;
        }
            // Si hay una pieza rival se agrega el move como captura y se corta el ciclo
        else if ((casillaSiguienteBitboard & piezasRivales) > 0) {
            movimientos = operaciones_bit::setBit(movimientos, casillaSiguiente, 1);
            break;
        }
            //Si no, se agre como quiet y ya. LSB es la posicion original de la torre y
            //pos es la casilla que estamos chequeando ahora mismo..
        else {
            movimientos = operaciones_bit::setBit(movimientos, casillaSiguiente, 1);
        }
        casillaSiguiente -= 8;

    }
    return movimientos;

}

U64 Torre::movimientos_izquierda(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales) {
    U64 movimientos = 0L;
    int casillaSiguiente = casillaDeSalida + 1;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);

    while ((casillaSiguiente <= 64) && (casillaSiguienteBitboard & HFile) == 0) {
        // Si en la casilla siguiente (hacia abajo) hay una pieza nuestra se corta el ciclo
        if ((casillaSiguienteBitboard & piezasPropias) > 0) {
            break;
        }
            // Si hay una pieza rival se agrega el move como captura y se corta el ciclo
        else if ((casillaSiguienteBitboard & piezasRivales) > 0) {
            movimientos = operaciones_bit::setBit(movimientos, casillaSiguiente, 1);
            break;
        }
            //Si no, se agre como quiet y ya. LSB es la posicion original de la torre y
            //pos es la casilla que estamos chequeando ahora mismo..
        else {
            movimientos = operaciones_bit::setBit(movimientos, casillaSiguiente, 1);
        }
        casillaSiguiente += 1;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);

    }
    return movimientos;

}



U64 Torre::movimientos_derecha(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales) {
    U64 movimientos = 0L;
    int casillaSiguiente = casillaDeSalida - 1;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while (((casillaSiguienteBitboard & AFile) == 0) && (casillaSiguiente > 0)) {
        // Si en la casilla siguiente (hacia abajo) hay una pieza nuestra se corta el ciclo
        if ((casillaSiguienteBitboard & piezasPropias) > 0) {
            break;
        }
            // Si hay una pieza rival se agrega el move como captura y se corta el ciclo
        else if ((casillaSiguienteBitboard & piezasRivales) > 0) {
            movimientos = operaciones_bit::setBit(movimientos, casillaSiguiente, 1);
            break;
        }
            //Si no, se agre como quiet y ya. LSB es la posicion original de la torre y
            //pos es la casilla que estamos chequeando ahora mismo..
        else {
            movimientos = operaciones_bit::setBit(movimientos, casillaSiguiente, 1);
        }
        casillaSiguiente -= 1;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return movimientos;

}

U64 Torre::attack_mask_arriba(U64 torre) {
    U64 attack_mask = 0L;
    int casilla = operaciones_bit::LSB(torre);
    int casillaSiguiente = casilla + 8;
    while (casillaSiguiente <= 56) {
        attack_mask = operaciones_bit::setBit(attack_mask, casillaSiguiente, 1);
        casillaSiguiente += 8;
    }
    return attack_mask;
}

U64 Torre::attack_mask_abajo(U64 torre) {
    U64 attack_mask = 0L;
    int casilla = operaciones_bit::LSB(torre);
    int casillaSiguiente = casilla - 8;
    while (casillaSiguiente > 8) {
        attack_mask = operaciones_bit::setBit(attack_mask, casillaSiguiente, 1);
        casillaSiguiente -= 8;
    }
    return attack_mask;
}

U64 Torre::attack_mask_izquierda(U64 torre) {
    U64 attack_mask = 0L;
    int casilla = operaciones_bit::LSB(torre);
    int casillaSiguiente = casilla + 1;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente <= 64) && (casillaSiguienteBitboard & (HFile | AFile)) == 0) {
        attack_mask = operaciones_bit::setBit(attack_mask, casillaSiguiente, 1);
        casillaSiguiente += 1;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);

    }
    return attack_mask;
}

U64 Torre::attack_mask_derecha(U64 torre) {
    U64 attack_mask = 0L;
    int casilla = operaciones_bit::LSB(torre);
    int casillaSiguiente = casilla - 1;
    U64 casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    while ((casillaSiguiente > 0) && (casillaSiguienteBitboard & (HFile | AFile)) == 0) {
        attack_mask = operaciones_bit::setBit(attack_mask, casillaSiguiente, 1);
        casillaSiguiente -= 1;
        casillaSiguienteBitboard = operaciones_bit::setBit(0L, casillaSiguiente, 1);
    }
    return attack_mask;
}