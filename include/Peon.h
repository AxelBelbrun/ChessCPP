//
// Created by axel on 13/12/23.
//

#ifndef CHESSCPP_PEON_H
#define CHESSCPP_PEON_H
#include "Pieza.h"
#include "Dama.h"
#include "Torre.h"
#include "Alfil.h"
#include "Caballo.h"
class Peon: public Pieza{
public:
    Peon();
     void movimientos_legales(Tablero* tablero, int ply, U64 peones, U64 piezasPropias, U64 piezasRivales);
    static int hayCapturaAlPaso(Tablero* tablero, U64 peonBitboard, int turno);
    U64 generar_movimientos_legales(U64 peon, U64 piezasPropias, U64 piezasRivales, int turno) override;
    static U64 avances(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales, int turno);
    static U64 capturas(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales, int turno);
    static void agregarAvances(Tablero* tablero, int ply, U64 bitboardMovimientos,
                        int desplazamiento);
    static void agregarCapturas(Tablero* tablero, int ply, U64 bitboardMovimientos,
                         int desplazamiento);
    static void agregarPromociones(Tablero* tablero, int ply, U64 bitboardMovimientos);

    static void agregarPromIzq(Tablero* tablero, int ply, U64 bitboardMovimientos);

    static void agregarPromDer(Tablero* tablero, int ply, U64 bitboardMovimientos);

    static void agregarCapturasAlPaso(Tablero* tablero, int ply, U64 peones,
                                      int turno);

    U64 todos_los_movimientos(U64 peon, U64 piezasPropias, U64 piezasRivales, int turno);

};


#endif //CHESSCPP_PEON_H
