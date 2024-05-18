//
// Created by axel on 13/12/23.
//

#ifndef CHESSCPP_DAMA_H
#define CHESSCPP_DAMA_H
#include "Pieza.h"
#include "slidingPiece.h"
class Dama: public slidingPiece {

public:
    Dama();
    U64 generar_movimientos_legales(U64 dama, U64 piezasPropias, U64 piezasRivales, int turno) override;
    U64 generar_attack_mask(U64 bitboard_dama) override;
};


#endif //CHESSCPP_DAMA_H
