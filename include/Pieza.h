//
// Created by axel on 09/12/23.
//

#ifndef CHESSCPP_PIEZA_H
#define CHESSCPP_PIEZA_H
#include "Tablero.h"
#include <string>
#include <list>
#include "Jugada.h"
#include "constantes.h"
using namespace std;

typedef unsigned long long U64;

class Pieza {

public:

    virtual U64 generar_movimientos_legales(U64 piezas, U64 piezasPropias, U64 piezasRivales, int turno) = 0;
    virtual ~Pieza() = default;


};

#endif //CHESSCPP_PIEZA_H
