//
// Created by axel on 14/12/23.
//

#ifndef CHESSCPP_REY_H
#define CHESSCPP_REY_H
#include "Pieza.h"

class Rey: public Pieza {
public:
    Rey();
    void movimientos_legales(Tablero* tablero, std::vector<u_short>* movimientos,U64 rey, U64 piezasPropias, U64 piezasRivales);
    U64 generar_movimientos_legales(U64 rey, U64 piezasPropias, U64 piezasRivales, int turno) override;
    static void capturas(Tablero* tablero,  std::vector<u_short>* movimientos,U64 rey, U64 piezasPropias, U64 piezasRivales);



};


#endif //CHESSCPP_REY_H
