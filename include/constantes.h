//
// Created by axel on 12/12/23.
//

#ifndef CHESSCPP_CONSTANTES_H
#define CHESSCPP_CONSTANTES_H


#include <map>
#include <string>
#include <vector>
#include <cstdint>
typedef unsigned long long U64;
typedef u_int8_t bitmask;

namespace constantes {
    extern U64 AFile;
    extern U64 BFile;
    extern U64 HFile;
    extern U64 GFile;
    extern U64 fila2;
    extern U64 fila7;
    extern U64 fila1;
    extern U64 fila8;
    extern U64 fila5;
    extern U64 fila4;
    extern U64 fila3;
    extern U64 fila6;
    extern float pesoMovilidad;
    extern float casillasCentralesCaballoBlanco[64];
    extern float casillasCentralesCaballoNegro[64];
    extern float casillasCentralesPeonBlanco[64];
    extern float casillasCentralesPeonNegro[64];
    extern float casillasCentralesAlfilBlanco[64];
    extern float casillasCentralesAlfilNegro[64];
    extern float premioPorEnrocar;
    extern float  castigoPorSacarDama;
    extern float  castigoMultiplesMovimientos;
    extern int premioPeonPasado[6];
    extern float  castigoPeonBloqueado;
    extern float  premioDesarrolloPiezaMenor;
    extern float  premioEscudoDePeones;
    extern float  castigoEnroqueSinEscudo;
    extern std::map<std::string,int> casillaANumero;
    extern std::map<int,std::string> NumeroACasilla;
    extern U64 magicsParaTorre[64];
    extern U64 magicsParaAlfil[64];
    extern float valorPieza[12];
    extern int casillasEspejadas[64];
    extern float ocupacionPeon[64];
    extern float ocupacionCaballo[64];
    extern float ocupacionAlfil[64];
    extern float ocupacionTorre[64];
    extern float ocupacionReina[64];
    extern float ocupacionReyMedioJuego[64];
    extern float ocupacionReyFinal[64];
    extern U64 zobristKeys[12][64];
    extern U64 mueveElNegro;
    extern U64 enroqueLargoNegro;
    extern U64 enroqueCortoNegro;
    extern U64 enroqueLargoBlanco;
    extern U64 enroqueCortoBlanco;
    extern U64 enPassant[8];



    const int bishop_relevant_bits[64] = {
            6, 5, 5, 5, 5, 5, 5, 6,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
            6, 5, 5, 5, 5, 5, 5, 6
    };

// rook relevant occupancy bit count for every square on board
    const int rook_relevant_bits[64] = {
            12, 11, 11, 11, 11, 11, 11, 12,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            12, 11, 11, 11, 11, 11, 11, 12
    };
    extern std::vector<U64> attackMasksTorre;
    extern std::vector<U64> attackMasksAlfil;


};


#endif //CHESSCPP_CONSTANTES_H
