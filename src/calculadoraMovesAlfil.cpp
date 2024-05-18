//
// Created by axel on 20/12/23.
//

#include "calculadoraMovesAlfil.h"
#include <vector>
#include <math.h>
#include <random>
typedef unsigned long long U64;
int calculadoraMovesAlfil:: BBits[64] {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
};
std::vector<U64> calculadoraMovesAlfil:: masksTableroVacio(){
    std::vector<U64> masks;
    auto alfil = new Alfil();
    for(int i = 1; i < 65; i++){
        U64 mask = 0;
        mask = operaciones_bit::setBit(mask, i, 1);

        U64 bitBoardDeMovimientosPosibles = alfil->generar_attack_mask(mask);

        masks.push_back(bitBoardDeMovimientosPosibles);
    }
    delete alfil;
    return masks;
}
std::vector<U64> calculadoraMovesAlfil::masksPiezasBloqueando(U64 generar_attack_mask){
    std::vector<int> posiciones;
    int contador = 0;
    while(generar_attack_mask > 0){
        int LSB = operaciones_bit::LSB(generar_attack_mask);
        posiciones.push_back(LSB);
        contador++;
    }
    vector<U64> masks;
    for(int j = 0; j < pow(2, contador); j++){
        masks.push_back(0L);
        for(int k = 0; k < contador; k++){
            int bit = (j >> k) & 1;
            masks[j] = operaciones_bit::setBit(masks[j], posiciones[k], bit);
        }

    }

    return masks;
}

U64 calculadoraMovesAlfil:: obtenerNumeroMagico(int casilla, int shifteo) {
    vector<U64> masksTableroVacio = calculadoraMovesAlfil::masksTableroVacio();
    vector<U64> masksPiezasBloqueando = calculadoraMovesAlfil::masksPiezasBloqueando(masksTableroVacio[casilla - 1]);
    U64 bitboardAlfil = operaciones_bit::setBit(0L, casilla, 1);
    U64 tablaDeHash[512];
    Alfil* alfil = new Alfil();
    //Tablero* tablero =T new Tablero("startpos");
    std::random_device rd;
    std::mt19937_64 mt(rd());
    /*std::uniform_int_distribution<U64> dist((U64) pow(2,60), (U64) pow(2,63));*/

    U64 numeroMagico = 0;
/*
    numeroMagico = numeroMagico << 6;
*/
    bool fail = false;
    int w = 0;
    for(w; w < 100000; w++){
        numeroMagico = mt() ;
        /*if (operaciones_bit::contadorDe1s((masksTableroVacio[casilla] * numeroMagico) & 0xFF00000000000000)
            < 1) {
            continue;
        }*/
        for(int i = 0; i < masksPiezasBloqueando.size(); i++){

            int indice = (int) ((masksPiezasBloqueando[i] * numeroMagico) >> (64 - 9));
            vector<u_short> a;
            U64 movimientosEnEsaPosicion =
                    alfil->generar_movimientos_legales(bitboardAlfil, 0, masksPiezasBloqueando[i], 0);
            if(tablaDeHash[indice] == 0){
                tablaDeHash[indice] = movimientosEnEsaPosicion;
            }
            else if
                    (movimientosEnEsaPosicion != tablaDeHash[indice]) {



                for (int j = 0; j < 512; j++) {
                    tablaDeHash[j] = 0;
                }
                fail = true;
                break;


            }

            }

        if(!fail){
            return numeroMagico;
        }
        fail = false;

    }
    if(w == 100000){
        numeroMagico = 0;
    }

    return numeroMagico;


}
