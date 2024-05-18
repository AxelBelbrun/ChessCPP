//
// Created by axel on 13/12/23.
//

#include "Peon.h"
#include <array>
#include <list>
#include "operaciones_bit.h"
using namespace constantes;

Peon :: Peon(){};

void Peon ::movimientos_legales(Tablero* tablero, std::vector<u_short>* movimientos,U64 peones,
                                         U64 piezasPropias, U64 piezasRivales) {
    auto dama = new Dama();
    auto torre = new Torre();
    auto alfil = new Alfil();
    auto caballo = new Caballo();


    while (peones > 0) {
        int LSB = operaciones_bit::LSB(peones);
        U64 peon = operaciones_bit::setBit(0L, LSB, 1);
        // Si el peon blanco esta en la fila 2, puede moverse dos casillas hacia adelante
        if (((peon & fila2) > 0) && (tablero->_turno == 0)) {
            // Si la casilla siguiente esta vacia, se agrega como quiet
            if (((peon << 8) & (piezasPropias | piezasRivales)) == 0) {
                movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 8, QUIET));
                // Misma lógica para el movimiento doble
                if (((peon << 16) & (piezasPropias | piezasRivales)) == 0) {
                    movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 16, QUIET));
                }
            }
        }

            // Si el peon negro esta en la fila 7, puede moverse dos casillas hacia atras
        else if (((peon & fila7) > 0) && (tablero->_turno == 1)) {
            // Si la casilla siguiente esta vacia, se agrega como quiet
            if (((peon >> 8) & (piezasPropias | piezasRivales)) == 0) {
                movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 8, QUIET));
                if (((peon >> 16) & (piezasPropias | piezasRivales)) == 0) { // lo mismo que antes
                    movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 16, QUIET));
                }
            }

        }

            // Si el peon esta en cualquier otra fila, puede moverse una casilla hacia adelante
        else {
            // Si la casilla siguiente esta vacia, se agrega como quiet

            //Caso peón blanco
            if ((((peon << 8) & (piezasPropias | piezasRivales)) == 0) && (tablero->_turno == 0)) {
                if ((peon & fila7) > 0) {
                    U64 bitboardTemp = operaciones_bit::setBit(0L, LSB + 8, 1);
                    if(tablero->esJaque(
                            dama->generar_movimientos_legales(bitboardTemp, piezasPropias & (~peon), piezasRivales, 0), tablero->_turno)) {
                        movimientos->push_back(operaciones_bit::crearJugada(1, LSB + 8, PROMOTIONCHECK));
                    }
                    else{
                        movimientos->push_back(operaciones_bit::crearJugada(1, LSB + 8, PROMOTION));
                    }
                    if(tablero->esJaque(
                            torre->generar_movimientos_legales(bitboardTemp, piezasPropias & (~peon), piezasRivales, 0), tablero->_turno)) {
                        movimientos->push_back(operaciones_bit::crearJugada(2, LSB + 8, PROMOTIONCHECK));
                    }
                    else{
                        movimientos->push_back(operaciones_bit::crearJugada(2, LSB + 8, PROMOTION));
                    }

                    if(tablero->esJaque(
                            alfil -> generar_movimientos_legales(bitboardTemp, piezasPropias & (~peon), piezasRivales, 0), tablero->_turno)) {
                        movimientos->push_back(operaciones_bit::crearJugada(3, LSB + 8, PROMOTIONCHECK));
                    }
                    else{
                        movimientos->push_back(operaciones_bit::crearJugada(3, LSB + 8, PROMOTION));
                    }

                    if(tablero->esJaque(
                            caballo->generar_movimientos_legales(bitboardTemp, piezasPropias & (~peon), piezasRivales,
                                                                 0), tablero->_turno)) {
                        movimientos->push_back(operaciones_bit::crearJugada(4, LSB + 8, PROMOTIONCHECK));
                    }
                    else{
                        movimientos->push_back(operaciones_bit::crearJugada(4, LSB + 8, PROMOTION));
                    }

                } else {
                    movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 8, QUIET));
                }
            }

                //Caso peon negro
            else if ((((peon >> 8) & (piezasPropias | piezasRivales)) == 0) && (tablero->_turno == 1)) {
                if ((peon & fila2) > 0) {
                    U64 bitboardTemp = operaciones_bit::setBit(0L, LSB - 8, 1);
                    if(tablero->esJaque(
                            dama->generar_movimientos_legales(bitboardTemp, piezasPropias & (~peon), piezasRivales, 0), tablero->_turno)) {
                        movimientos->push_back(operaciones_bit::crearJugada(1, LSB - 8, PROMOTIONCHECK));
                    }
                    else{
                        movimientos->push_back(operaciones_bit::crearJugada(1, LSB - 8, PROMOTION));
                    }

                    if(tablero->esJaque(
                            torre->generar_movimientos_legales(bitboardTemp, piezasPropias & (~peon), piezasRivales, 0), tablero->_turno)) {
                        movimientos->push_back(operaciones_bit::crearJugada(2, LSB - 8, PROMOTIONCHECK));
                    }
                    else{
                        movimientos->push_back(operaciones_bit::crearJugada(2, LSB - 8, PROMOTION));
                    }

                    if(tablero->esJaque(
                            alfil->generar_movimientos_legales(bitboardTemp, piezasPropias & (~peon), piezasRivales, 0), tablero->_turno)) {
                        movimientos->push_back(operaciones_bit::crearJugada(3, LSB - 8, PROMOTIONCHECK));
                    }
                    else{
                        movimientos->push_back(operaciones_bit::crearJugada(3, LSB - 8, PROMOTION));
                    }

                    if(tablero->esJaque(
                            caballo->generar_movimientos_legales(bitboardTemp, piezasPropias & (~peon), piezasRivales,
                                                                 0), tablero->_turno)) {
                        movimientos->push_back(operaciones_bit::crearJugada(4, LSB - 8, PROMOTIONCHECK));
                    }
                    else{
                        movimientos->push_back(operaciones_bit::crearJugada(4, LSB - 8, PROMOTION));
                    }

                }
                else {
                    movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 8, QUIET));
                }
            }

        }
        // Si la casilla diagonal derecha esta ocupada por una pieza rival, se agrega como captura

        //Caso peón blanco
        if ((((peon & HFile) == 0) && (((peon << 7) & piezasRivales) > 0) && (tablero->_turno == 0))) {
            if((peon & fila7) > 0){
                movimientos->push_back(operaciones_bit::crearJugada(1, LSB + 7, PROMOTIONDER));
                movimientos->push_back(operaciones_bit::crearJugada(2, LSB + 7, PROMOTIONDER));
                movimientos->push_back(operaciones_bit::crearJugada(3, LSB + 7, PROMOTIONDER));
                movimientos->push_back(operaciones_bit::crearJugada(4, LSB + 7, PROMOTIONDER));


            }
            else {
                movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 7, CAPTURE));
            }
        }

            //Caso peon negro
        else if ((((peon & AFile) == 0) && (((peon >> 7) & piezasRivales) > 0) && (tablero->_turno == 1))) {
            if((peon & fila2) > 0){
                movimientos->push_back(operaciones_bit::crearJugada(1, LSB - 7, PROMOTIONDER));
                movimientos->push_back(operaciones_bit::crearJugada(2, LSB - 7, PROMOTIONDER));
                movimientos->push_back(operaciones_bit::crearJugada(3, LSB - 7, PROMOTIONDER));
                movimientos->push_back(operaciones_bit::crearJugada(4, LSB - 7, PROMOTIONDER));

            }
            else {
                movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 7, CAPTURE));
            }
        }



        // Si la casilla diagonal izquierda esta ocupada por una pieza rival, se agrega como captura

        //Caso peon blanco
        if ((((peon & AFile) == 0) && (((peon << 9) & piezasRivales) > 0) && (tablero->_turno == 0))) {
            if((peon & fila7) > 0){
                U64 bitboardTemp = operaciones_bit::setBit(0L, LSB + 9, 1);
                if(tablero->esJaque(dama -> generar_movimientos_legales(bitboardTemp, piezasPropias, piezasRivales, 0), tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(1, LSB + 9, CPIC));
                }
                else{
                    movimientos->push_back(operaciones_bit::crearJugada(1, LSB + 9, PROMOTIONIZQ));
                }
                if(tablero->esJaque(torre->generar_movimientos_legales(bitboardTemp, piezasPropias, piezasRivales, 0), tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(2, LSB + 9, CPIC));
                }
                else{
                    movimientos->push_back(operaciones_bit::crearJugada(2, LSB + 9, PROMOTIONIZQ));
                }

                if(tablero->esJaque(alfil->generar_movimientos_legales(bitboardTemp, piezasPropias, piezasRivales, 0), tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(3, LSB + 9, CPIC));
                }
                else{
                    movimientos->push_back(operaciones_bit::crearJugada(3, LSB + 9, PROMOTIONIZQ));
                }
                if(tablero->esJaque(caballo->generar_movimientos_legales(bitboardTemp, piezasPropias, piezasRivales, 0), tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(4, LSB + 9, CPIC));
                }
                else{
                    movimientos->push_back(operaciones_bit::crearJugada(4, LSB + 9, PROMOTIONIZQ));
                }

            }
            else {
                movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 9, CAPTURE));
            }
        }

            //Caso peon negro
        else if ((((peon & HFile) == 0) && (((peon >> 9) & piezasRivales) > 0) && (tablero->_turno == 1))) {
            if((peon & fila2) > 0){
                U64 bitboardTemp = operaciones_bit::setBit(0L, LSB - 9, 1);
                if(tablero->esJaque(dama->generar_movimientos_legales(bitboardTemp, piezasPropias, piezasRivales, 0), tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(1, LSB - 9, CPDC));
                }
                else{
                    movimientos->push_back(operaciones_bit::crearJugada(1, LSB - 9, PROMOTIONDER));
                }

                if(tablero->esJaque(torre->generar_movimientos_legales(bitboardTemp, piezasPropias, piezasRivales, 0), tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(2, LSB - 9, CPDC));
                }
                else{
                    movimientos->push_back(operaciones_bit::crearJugada(2, LSB - 9, PROMOTIONDER));
                }


                if(tablero->esJaque(alfil->generar_movimientos_legales(bitboardTemp, piezasPropias, piezasRivales, 0), tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(3, LSB - 9, CPDC));
                }
                else{
                    movimientos->push_back(operaciones_bit::crearJugada(3, LSB - 9, PROMOTIONDER));
                }
                if(tablero->esJaque(caballo->generar_movimientos_legales(bitboardTemp, piezasPropias, piezasRivales, 0), tablero->_turno)) {
                    movimientos->push_back(operaciones_bit::crearJugada(4, LSB - 9, CPDC));
                }
                else{
                    movimientos->push_back(operaciones_bit::crearJugada(4, LSB - 9, PROMOTIONDER));
                }

            }
            else {
                movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 9, CAPTURE));
            }
        }

        //Chequeo de captura al paso

        //Caso peón blanco
        if(((peon & fila5) > 0) && (tablero->_turno == 0)){
            int casillaDeCapturaAlPaso = hayCapturaAlPaso(tablero, peon, 0);
            if(casillaDeCapturaAlPaso == 0){
                continue;
            }
            else{
                movimientos->push_back(operaciones_bit::crearJugada(LSB,
                                                                    casillaDeCapturaAlPaso, ENPASSANT));
            }
        }

            //Caso peón negro

        else if (((peon & fila4) > 0) && (tablero->_turno) == 1){
            int casillaDeCapturaAlPaso = hayCapturaAlPaso(tablero, peon, 1);
            if(casillaDeCapturaAlPaso == 0){
                continue;
            }
            else{
                movimientos->push_back(operaciones_bit::crearJugada(LSB,
                                                                    casillaDeCapturaAlPaso, ENPASSANT));
            }

        }

    }

    delete dama;
    delete torre;
    delete caballo;
    delete alfil;


}

U64
Peon::capturas(int casillaDelPeon, U64 piezasPropias, U64 piezasRivales, int turno) {// Si la casilla diagonal derecha esta ocupada por una pieza rival, se agrega como captura
    U64 movimientos = 0ULL;
    U64 peonBitboard = operaciones_bit::setBit(0ULL, casillaDelPeon, 1);
//Capturas hacia la derecha

    //Caso peon blanco
    if ((((peonBitboard & HFile) == 0) && (turno == 0))) {

        movimientos = operaciones_bit::setBit(movimientos, casillaDelPeon + 7, 1);

    }

    //Caso peon negro
    else if ((((peonBitboard & AFile) == 0) && (turno == 1))) {

        movimientos = operaciones_bit::setBit(movimientos, casillaDelPeon - 7, 1);

    }



//Capturas hacia la izquierda

    //Caso peon blanco
    if ((((peonBitboard & AFile) == 0) && ((turno == 0)))) {

        movimientos = operaciones_bit::setBit(movimientos, casillaDelPeon + 9, 1);

    }

    //Caso peon negro
    else if ((((peonBitboard & HFile) == 0) && turno == 1)) {

        movimientos = operaciones_bit::setBit(movimientos, casillaDelPeon - 9, 1);;

    }

    return movimientos;
};

//Chequear que exista la chance de hacer una captura al paso. SI la hay, devuelve la casilla donde podríamos
//tomar, y si no la hay, devuelve 0.
int Peon::hayCapturaAlPaso(Tablero* tablero, U64 peonBitboard, int turno) {
    int casillaDelPeon = operaciones_bit::LSB(peonBitboard);
    U64 bitboardPeon = operaciones_bit::setBit(0ULL, casillaDelPeon, 1);
    if(tablero->_jugadas.empty()){
        return 0;
    }
    int casilla_salida_ultima_jugada = operaciones_bit::getSalida(tablero->_jugadas.back());
    int casilla_llegada_ultima_jugada = operaciones_bit::getLlegada(tablero->_jugadas.back());
    //Caso para las blancas:
    if(turno == 0){

        //Si el último movimiento llega hacia alguna de las casillas de interés:
        if((casilla_llegada_ultima_jugada == casillaDelPeon+1) && ((bitboardPeon & AFile) == 0)) { // chequea si el peon está en la casilla de la izquierda
            //Si sale de la séptima fila, columna de la izquierda
            if (casilla_salida_ultima_jugada == casillaDelPeon + 17) {
                U64 bitboardPosiblePeonRival =
                        operaciones_bit::setBit(0L, casilla_llegada_ultima_jugada, 1);
                if (tablero->obtenerTipoDePieza(casilla_llegada_ultima_jugada) == PEON) { //chequea si hay un peon ahí
                    return casillaDelPeon + 9;
                }

            }
        }
        else if((casilla_llegada_ultima_jugada == casillaDelPeon-1) && (bitboardPeon & HFile) == 0){ // chequea si el peón está en la casilla de la derecha
            //Si sale de la séptima fila, columna de la derecha
            if(casilla_salida_ultima_jugada == casillaDelPeon+15){
                U64 bitboardPosiblePeonRival =
                        operaciones_bit::setBit(0L, casilla_llegada_ultima_jugada , 1);
                if(tablero->obtenerTipoDePieza(casilla_llegada_ultima_jugada) == PEON) {
                    return casillaDelPeon + 7;
                }

            }
        }
    }

    else{
        if((casilla_llegada_ultima_jugada == casillaDelPeon-1) && ((bitboardPeon & HFile) == 0)){
            //Si sale de la segunda fila, columna de la derecha
            if(casilla_salida_ultima_jugada == casillaDelPeon-17){
                U64 bitboardPosiblePeonRival =
                        operaciones_bit::setBit(0L, casilla_llegada_ultima_jugada , 1);
                if(tablero->obtenerTipoDePieza(casilla_llegada_ultima_jugada) == PEON) {
                    return casillaDelPeon - 9;
                }
            }
        }
        else if((casilla_llegada_ultima_jugada == casillaDelPeon+1) && ((bitboardPeon & AFile) == 0)){
            //Si sale de la segunda fila, columna de la izquierda
            if(casilla_salida_ultima_jugada == casillaDelPeon-15){
                U64 bitboardPosiblePeonRival =
                        operaciones_bit::setBit(0L, casilla_llegada_ultima_jugada , 1);
                if(tablero->obtenerTipoDePieza(casilla_llegada_ultima_jugada) == PEON) {
                    return casillaDelPeon - 7;
                }
            }
        }
    }

    return 0;
}

U64 Peon:: avances(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales, int turno){
    U64 movimientos = 0L;
    U64 casillaDeSalidaBitboard = operaciones_bit::setBit(0L, casillaDeSalida, 1);
    // Si el peon blanco esta en la fila 2, puede moverse dos casillas hacia adelante
    if (((casillaDeSalidaBitboard & fila2) > 0) && (turno == 0)) {
        // Si la casilla siguiente esta vacia, se agrega como quiet
        if (((casillaDeSalidaBitboard << 8) & (piezasPropias | piezasRivales)) == 0) {
            movimientos = operaciones_bit::setBit(movimientos, casillaDeSalida + 8, 1);
            // Misma lógica para el movimiento doble
            if (((casillaDeSalidaBitboard << 16) & (piezasPropias | piezasRivales)) == 0) {
                movimientos = operaciones_bit::setBit(movimientos, casillaDeSalida + 16, 1);
            }
        }
    }

        // Si el peon negro esta en la fila 7, puede moverse dos casillas hacia atras
    else if (((casillaDeSalidaBitboard & fila7) > 0) && (turno == 1)) {
        // Si la casilla siguiente esta vacia, se agrega como quiet
        if (((casillaDeSalidaBitboard >> 8) & (piezasPropias | piezasRivales)) == 0) {
            movimientos = operaciones_bit::setBit(movimientos, casillaDeSalida - 8, 1);
            if (((casillaDeSalidaBitboard >> 16) & (piezasPropias | piezasRivales)) == 0) { // lo mismo que antes
                movimientos = operaciones_bit::setBit(movimientos, casillaDeSalida -16, 1);
            }
        }

    }

        // Si el peon esta en cualquier otra fila, puede moverse una casilla hacia adelante
    else {
        // Si la casilla siguiente esta vacia, se agrega como quiet

        //Caso peón blanco
        if ((((casillaDeSalidaBitboard << 8) & (piezasPropias | piezasRivales)) == 0) && (turno == 0)
         && (casillaDeSalida + 8) <= 64) {
            movimientos = operaciones_bit::setBit(movimientos, casillaDeSalida + 8, 1);
        }


            //Caso peon negro
        else if ((((casillaDeSalidaBitboard >> 8) & (piezasPropias | piezasRivales)) == 0) && (turno == 1)
        && (casillaDeSalida - 8) > 0) {
            movimientos = operaciones_bit::setBit(movimientos, casillaDeSalida - 8, 1);
            }
        }

    return movimientos;

}

U64 Peon::generar_movimientos_legales(U64 peon, U64 piezasPropias, U64 piezasRivales, int turno) {
    int casillaDePeon = operaciones_bit::LSB(peon);
    return avances(casillaDePeon, piezasPropias, piezasRivales, turno)
            | capturas(casillaDePeon,piezasPropias,piezasRivales,turno);
}