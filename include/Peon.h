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
     void movimientos_legales(Tablero* tablero,std::vector<u_short>* movimientos, U64 peones, U64 piezasPropias, U64 piezasRivales);
    static int hayCapturaAlPaso(Tablero* tablero, U64 peonBitboard, int turno);
    U64 generar_movimientos_legales(U64 peon, U64 piezasPropias, U64 piezasRivales, int turno) override;
    static U64 avances(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales, int turno);
    static U64 capturas(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales, int turno);

};


#endif //CHESSCPP_PEON_H
