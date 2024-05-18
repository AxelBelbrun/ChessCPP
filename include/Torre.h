#ifndef CHESSCPP_TORRE_H
#define CHESSCPP_TORRE_H
#include "Pieza.h"
#include "Tablero.h"
#include "Jugada.h"
#include "operaciones_bit.h"
#include "slidingPiece.h"
class Torre: public slidingPiece {
public:
    Torre();
    U64 generar_attack_mask(U64 torres) override;
    U64 generar_movimientos_legales(U64 torres, U64 piezasPropias, U64 piezasRivales, int turno) override;

private:
    static U64 movimientos_arriba(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
    static U64 movimientos_abajo(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
    static U64 movimientos_izquierda(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
    static U64 movimientos_derecha(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
    static U64 attack_mask_arriba(U64 torre);
    static U64 attack_mask_abajo(U64 torre);
    static U64 attack_mask_izquierda(U64 torre);
    static U64 attack_mask_derecha(U64 torre);


};


#endif //CHESSCPP_TORRE_H