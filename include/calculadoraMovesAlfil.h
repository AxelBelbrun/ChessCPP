//
// Created by axel on 20/12/23.
//

#ifndef CHESSCPP_CALCULADORAMOVESALFIL_H
#define CHESSCPP_CALCULADORAMOVESALFIL_H


#include <vector>
#include "operaciones_bit.h"
#include "Alfil.h"
typedef unsigned long long U64;

class calculadoraMovesAlfil {
public:


    static int BBits[64];
    std::vector<U64> masksTableroVacio();
    static std::vector<U64> masksPiezasBloqueando(U64 generar_attack_mask);
    U64 obtenerNumeroMagico(int casilla, int shifteo);


};

#endif //CHESSCPP_CALCULADORAMOVESALFIL_H
