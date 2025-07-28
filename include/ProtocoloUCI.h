//
// Created by inaki on 21/12/23.
//

#ifndef CHESSCPP_PROTOCOLOUCI_H
#define CHESSCPP_PROTOCOLOUCI_H

#include <string>
#include "Tablero.h"
#include "Motor.h"
#include "operaciones_bit.h"
#include "constantes.h"
class ProtocoloUCI {
public:
    ProtocoloUCI();
    bool posicionSeteada = false;
    bool ready = false;
    void comunicacionUCI();
    std::string formatearJugada(u_short jugada);
    void inputSetOption(string inputString);
    void inputUCINewGame();
    void inputPosition(std::string inputString);
    void inputGo(string input);
    std:: string generarJugadaString(int salida, int llegada, int llegadaPromocion);
    Tablero* tablero;
    Motor* motor;
};

#endif //CHESSCPP_PROTOCOLOUCI_H
