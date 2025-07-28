//
// Created by axel on 10/12/23.
//

#ifndef CHESSCPP_JUGADA_H
#define CHESSCPP_JUGADA_H
#include <map>
#include <string>
#include "constantes.h"


struct Jugada{
    u_short movimiento;
    static u_short stringToUShort(std::string jugada);

    Jugada(int salida, int llegada, int tipoDeJugada){
        movimiento = salida | (llegada << 6) | (tipoDeJugada << 12);
    }
    int getSalida(){
        return (movimiento & 0b111111);
    }
    int getLlegada(){
        return ((movimiento >> 6) & 0b111111);
    }
    int getTipoDeJugada(){
        return ((movimiento >> 12) & 0b111111);
    }


/*
    Jugada( int salida, int llegada, int tipoDeJugada);
*/

    int getCasillaDestino();
    bool operator==(const Jugada& j) const{
        return (movimiento == j.movimiento);
    }
/*    bool operator==(const Jugada& j) const{
        return (salida == j.salida && llegada == j.llegada && tipoDeJugada == j.tipoDeJugada);
    }
    bool operator<(const Jugada& j) const{
        return (tipoDeJugada < j.tipoDeJugada);
    }
    bool operator>(const Jugada& j) const{
        return (tipoDeJugada > j.tipoDeJugada);
    }
    bool operator<=(const Jugada& j) const{
        return (tipoDeJugada <= j.tipoDeJugada);
    }
    bool operator>=(const Jugada& j) const{
        return (tipoDeJugada >= j.tipoDeJugada);
    }
    bool operator!=(const Jugada& j) const{
        return (salida != j.salida || llegada != j.llegada || tipoDeJugada != j.tipoDeJugada);
    }*/


};
enum tipoDeJugada {
    QUIET, ENPASSANT, CASTLING, PROMOTIONDER,PROMOTIONIZQ,PROMOTION,CAPTURE,CPIC,
    CPDC,PROMOTIONCHECK,CAPTURECHECK,CHECK,CHECKMATE, ENPASSANTCHECK, CASTLINGCHECK

};


#endif //CHESSCPP_JUGADA_H
