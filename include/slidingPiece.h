//
// Created by axel on 11/02/24.
//

#ifndef CHESSCPP_SLIDINGPIECE_H
#define CHESSCPP_SLIDINGPIECE_H
#include "Pieza.h"
#include "Tablero.h"
#include "Jugada.h"
#include "operaciones_bit.h"


class slidingPiece: public Pieza{
public:
    virtual U64 generar_attack_mask(U64 bitboard_alfil) = 0;
    virtual ~slidingPiece() {};
};


#endif //CHESSCPP_SLIDINGPIECE_H
