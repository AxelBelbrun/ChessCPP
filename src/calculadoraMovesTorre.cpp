//
// Created by axel on 20/12/23.
//

#include <vector>
#include "calculadoraMovesTorre.h"
#include <math.h>
#include <random>
#include <bitset>
typedef unsigned long long U64;

std::vector<U64> calculadoraMovesTorre:: masksTableroVacio(){
        std::vector<U64> masks;
        for(int i = 1; i < 65; i++){
            U64 mask = 0;
            mask = operaciones_bit::setBit(mask, i, 1);
            Torre* torre = new Torre();
            U64 bitBoardDeMovimientosPosibles = torre->generar_attack_mask(mask);

            masks.push_back(bitBoardDeMovimientosPosibles);
            delete torre;

        }
        return masks;
    }
    std::vector<U64> calculadoraMovesTorre::masksPiezasBloqueando(U64 attack_mask){
        std::vector<int> posiciones;
        int contador = 0;
        while(attack_mask > 0){
            int LSB = operaciones_bit::LSB(attack_mask);
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

U64 calculadoraMovesTorre::obtenerNumeroMagico(int casilla, int shifteo) {
    vector<U64> masksTableroVacio = calculadoraMovesTorre::masksTableroVacio();
    vector<U64> masksPiezasBloqueando = calculadoraMovesTorre::masksPiezasBloqueando(masksTableroVacio[casilla - 1]);
    U64 bitboardTorre = operaciones_bit::setBit(0L, casilla, 1);
    U64 tablaDeHash[4096];
    Torre *torre = new Torre();
    //Tablero *tablero = new Tablero();

    // Generate a random number
    std::random_device rd;
    std::mt19937_64 mt(rd());
/*
    std::uniform_int_distribution<U64> dist((U64) pow(2,60), (U64) pow(2,64));
*/


    /*numeroMagico = numeroMagico << 6;*/
    /*while(operaciones_bit::contadorDe1s(numeroMagico) < 55){
        numeroMagico = mt() & mt() & mt();
        numeroMagico = numeroMagico << 6;

    }*/
    U64 numeroMagico = 0;
    bool fail = false;
    int w;
    for ( w = 0; w < 10000000; w++) {

        numeroMagico = mt() & mt() & mt();

        for (int i = 0; i < masksPiezasBloqueando.size(); i++) {

            int indice = (int) ((masksPiezasBloqueando[i] * numeroMagico) >> (64 - 12));
            U64 movimientosEnEsaPosicion =
                    torre->generar_movimientos_legales(bitboardTorre, 0,
                                                       masksPiezasBloqueando[i], 0);
            if (tablaDeHash[indice] == 0) {
                tablaDeHash[indice] = movimientosEnEsaPosicion;
            } else if (movimientosEnEsaPosicion != tablaDeHash[indice]) {


                for (int j = 0; j < 4096; j++) {
                    tablaDeHash[j] = 0;
                }
                fail = true;

                break;


            }
        }
        if(!fail){
            delete torre;
            //delete tablero;
            return numeroMagico;
        }
        fail = false;
    }
    if(w == 10000000){
        numeroMagico = 0;
    }
    delete torre;
    //delete tablero;
    return numeroMagico;
}




