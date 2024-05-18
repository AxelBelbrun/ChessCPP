//
// Created by axel on 10/12/23.
//

#include "Jugada.h"
#include "Tablero.h"
#include "constantes.h"
#include <string>
using namespace constantes;
/*Jugada:: Jugada(int salida, int llegada, int tipoDeJugada){
    u_short _salida = salida;
    u_short _llegada = llegada << 6;
    u_short _tipoDeJugada = tipoDeJugada << 12;



};*/
int Jugada::getCasillaDestino(){
    return (movimiento >> 6);

};
u_short Jugada::stringToUShort(std::string jugada) {
    int salida = casillaANumero.at(jugada.substr(0,2));
    int llegada = casillaANumero.at(jugada.substr(2,2));
    int tipoDeJugada = jugada[4] - '0';
    U64 res = 0ULL;
    res = res | salida;
    res = res | (llegada << 6);
    res = res | (tipoDeJugada << 12);
    return res;

}
