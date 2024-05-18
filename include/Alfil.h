//
// Created by axel on 13/12/23.
//

#ifndef CHESSCPP_ALFIL_H
#define CHESSCPP_ALFIL_H
#include "Pieza.h"
#include "slidingPiece.h"

class Alfil: public slidingPiece {
public:
    Alfil();
    U64 generar_attack_mask(U64 bitboard_alfil) override;
    U64 generar_movimientos_legales(U64 alfil, U64 piezasPropias, U64 piezasRivales, int turno) override;
private:
    static U64 movimientos_legales_nordeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
    static U64 movimientos_legales_noroeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
    static U64 movimientos_legales_sudeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
    static U64 movimientos_legales_sudoeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
    static U64 attack_mask_nordeste(U64 alfil);
    static U64 attack_mask_noroeste(U64 alfil);
    static U64 attack_mask_sudeste(U64 alfil);
    static U64 attack_mask_sudoeste(U64 alfil);
};


#endif //CHESSCPP_ALFIL_H
