//
// Created by axel on 20/12/23.
//

#ifndef CALCULADORAMOVESTORRES_CALCULADORAMOVES_H
#define CALCULADORAMOVESTORRES_CALCULADORAMOVES_H

#include <vector>
#include "operaciones_bit.h"
#include "Torre.h"
typedef unsigned long long U64;

class calculadoraMovesTorre {
public:
    std::vector<U64> masksTableroVacio();
    static std::vector<U64> masksPiezasBloqueando(U64 generar_attack_mask);
    U64 obtenerNumeroMagico(int casilla, int shifteo);


};


#endif //CALCULADORAMOVESTORRES_CALCULADORAMOVES_H
