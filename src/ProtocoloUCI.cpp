

#include "ProtocoloUCI.h"
#include <iostream>

using namespace std;



ProtocoloUCI:: ProtocoloUCI() {

}

void ProtocoloUCI:: comunicacionUCI() {
    while (true) {
        string input;
        getline(cin, input);
        if (input == "uci") {
            inputUCINewGame();
            cout << "id name RDI Chess" << endl;
            cout << "id author Axel Belbrun, Iñaki Zamudio" << endl;
            cout << "uciok" << endl;

        } else if (input == "isready") {
            cout << "readyok" << endl;


        } else if (input == "ucinewgame") {
            inputUCINewGame();

        } else if (input.substr(0, 8) == "position") {
            inputPosition(input);

        } else if (input.substr(0,2) ==  "go") {
            inputGo();
        } else if(input.substr(0,4) == "quit"){
            delete motor;
            delete tablero;
            delete motor->TT;
            exit(0);
        }
        else {
            cout << "No se reconoce el comando" << endl;
            continue;
        }
    }
}

void ProtocoloUCI:: inputUCINewGame() {
     motor = new Motor();
     delete tablero;
     tablero = new Tablero("position startpos");

     motor->TT->asignarTablero(tablero);
     motor->tablaInicializada = true;
     u_short jugada;
/*
     motor->negamax(tablero,6,-999999999,999999999,jugada);
*/

    posicionSeteada = false;
}

void ProtocoloUCI:: inputPosition(string input) {


    if (input.std::string::find("moves") != std::string::npos) {
        //Si aún no existe un tablero con una posición inicial
        if(!posicionSeteada){
            //Si nos pasan un fen, se recorta la parte de "moves" en adelante para no entrar en conflicto
            // con el constructor de Tablero
            if(input.std::string::find("fen") != std::string::npos){
                string inputSinMoves = input.substr(0, input.size() - 11);
                delete tablero;
                tablero = new Tablero(inputSinMoves);
                motor->TT->asignarTablero(tablero);

                posicionSeteada = true;
            }

            //Si no es un fen, se setea el tablero con la posición inicial
            else {
                delete tablero;
                tablero = new Tablero(input);
                motor->TT->asignarTablero(tablero);
                posicionSeteada = true;
                return;
            }
        }

        //se realiza la ultima jugada que aparece en el string. Primero se chequea si es una promoción..
        int i = input.size() - 5;
        string jugada = input.substr(i, 5);
        int salida, llegada;

        if (jugada.substr(4, 1) == "q") {
            salida = constantes::casillaANumero[jugada.substr(0, 2)];
            llegada = constantes::casillaANumero[jugada.substr(2, 2)];
            if (salida == llegada + 8 || salida == llegada - 8) {
                tablero->moverPieza(1, llegada, PROMOTION);
            } else if (salida == llegada + 7 || salida == llegada - 7) {
                tablero->moverPieza(1, llegada, PROMOTIONDER);
            } else if (salida == llegada + 9 || salida == llegada - 9) {
                tablero->moverPieza(1, llegada, PROMOTIONIZQ);
            }
            if(tablero->ganoNegro){
                cout << "Han ganado las negras!" << endl;
                exit(0);
            }
            else if(tablero->ganoBlanco){
                cout << "Han ganado las blancas!" << endl;
                exit(0);
            }
            return;

        } else if (jugada.substr(4, 1) == "r") {
            salida = constantes::casillaANumero[jugada.substr(0, 2)];
            llegada = constantes::casillaANumero[jugada.substr(2, 2)];
            if (salida == llegada + 8 || salida == llegada - 8) {
                tablero->moverPieza(2, llegada, PROMOTION);
            } else if (salida == llegada + 7 || salida == llegada - 7) {
                tablero->moverPieza(2, llegada, PROMOTIONDER);
            } else if (salida == llegada + 9 || salida == llegada - 9) {
                tablero->moverPieza(2, llegada, PROMOTIONIZQ);
            }
            if(tablero->ganoNegro){
                cout << "Han ganado las negras!" << endl;
                exit(0);
            }
            else if(tablero->ganoBlanco){
                cout << "Han ganado las blancas!" << endl;
                exit(0);
            }
            return;
        } else if (jugada.substr(4, 1) == "b") {
            salida = constantes::casillaANumero[jugada.substr(0, 2)];
            llegada = constantes::casillaANumero[jugada.substr(2, 2)];
            if (salida == llegada + 8 || salida == llegada - 8) {
                tablero->moverPieza(3, llegada, PROMOTION);
            } else if (salida == llegada + 7 || salida == llegada - 7) {
                tablero->moverPieza(3, llegada, PROMOTIONDER);
            } else if (salida == llegada + 9 || salida == llegada - 9) {
                tablero->moverPieza(3, llegada, PROMOTIONIZQ);
            }
            if(tablero->ganoNegro){
                cout << "Han ganado las negras!" << endl;
                exit(0);
            }
            else if(tablero->ganoBlanco){
                cout << "Han ganado las blancas!" << endl;
                exit(0);
            }
            return;
        } else if (jugada.substr(4, 1) == "n") {
            salida = constantes::casillaANumero[jugada.substr(0, 2)];
            llegada = constantes::casillaANumero[jugada.substr(2, 2)];
            if (salida == llegada + 8 || salida == llegada - 8) {
                tablero->moverPieza(4, llegada, PROMOTION);
            } else if (salida == llegada + 7 || salida == llegada - 7) {
                tablero->moverPieza(4, llegada, PROMOTIONDER);
            } else if (salida == llegada + 9 || salida == llegada - 9) {
                tablero->moverPieza(4, llegada, PROMOTIONIZQ);
            }
            if(tablero->ganoNegro){
                cout << "Han ganado las negras!" << endl;
                exit(0);
            }
            else if(tablero->ganoBlanco){
                cout << "Han ganado las blancas!" << endl;
                exit(0);
            }
        }

        //Si no es una promoción, podría ser enpassant
        else {

            //Se toman los caracteres desde la posición 1 porque, como no entró en ningún if de arriba,
            //entonces en la primera posición va a haber un espacio

            /* Por ejemplo, si se tuviera una promoción, los últimos 5 caractéres del string serían del
             * estilo: "e7e8q". Si no es una promoción, entonces los últimos 5 caracteres serían del estilo
             * " e7e5". Por eso, para tomar la jugada, se toman los caracteres desde la posición 1.
             */
            salida = constantes::casillaANumero[jugada.substr(1, 2)];
            llegada = constantes::casillaANumero[jugada.substr(3, 2)];
            int tipoDePieza = tablero->obtenerTipoDePieza(salida);
            if (tipoDePieza == PEON) {
                //Si nos movimos en diagonal (hay 4 posibilidades, contempladas con las restas
                // entre las casillas de salida y llegada) y la casilla de llegada está vacía,
                // entonces es un enpassant
                if ((tablero->obtenerTipoDePieza(llegada) == VACIO) &&
                    ((salida - llegada == 7) || (salida - llegada == -7)
                     || (salida - llegada == 9) || (salida - llegada == -9))) {
                    tablero->moverPieza(salida, llegada, ENPASSANT);
                    if(tablero->ganoNegro){
                        cout << "Han ganado las negras!" << endl;
                        delete tablero;
                        delete motor->TT;
                        delete motor;

                        exit(0);
                    }
                    else if(tablero->ganoBlanco){
                        cout << "Han ganado las blancas!" << endl;
                        delete tablero;
                        delete motor->TT;
                        delete motor;
                        exit(0);
                    }
                    return;
                }
            }

            //Si no es ninguno de los "movimientos especiales" entonces simplemente se realiza sin chequear
            // nada más
            u_short jugadaUshort = operaciones_bit::crearJugada(salida, llegada, 0);
            int tipoDeJugada;
            if(tablero->obtenerTipoDePieza(llegada) != VACIO){
                tipoDeJugada = CAPTURE;
            }
            else{
                tipoDeJugada = QUIET;
            }
            //Se usa la función que asume que el movimiento que se pasa es legal
            tablero->moverPiezaTrusted(operaciones_bit::getSalida(jugadaUshort),
                                       operaciones_bit::getLlegada(jugadaUshort), tipoDeJugada);
            if(tablero->ganoNegro){
                cout << "Han ganado las negras!" << endl;
                delete tablero;
                delete motor->TT;
                delete motor;
                exit(0);
            }
            else if(tablero->ganoBlanco){
                cout << "Han ganado las blancas!" << endl;
                delete tablero;
                delete motor->TT;
                delete motor;
                exit(0);
            }

        }
        
    }

    //Si no hay "moves" en el input, entonces se setea el tablero con la posición inicial o el fen que nos pasan
    else {
        //se crea el tablero basado en el fen
        if(input.std::string::find("fen") != std::string::npos){
            delete tablero;
            tablero = new Tablero(input);
            motor->TT->asignarTablero(tablero);
            posicionSeteada = true;
            return;
        }
        else{
            delete tablero;
            tablero = new Tablero(input);
            motor->TT->asignarTablero(tablero);
            posicionSeteada = true;
        }

    }
}

void ProtocoloUCI:: inputGo() {
    u_short jugada;
    //Iterative deepening
    int a, b;
    for(int i = 1; i < 8; i++){
        if(i == 1){
            jugada = 0;
        }

        motor -> ply = -1;
        motor -> incrementos = 0;
        if(tablero->endgame){
            motor->negamax(tablero, i+8, -999999999, 999999999, jugada);

        }
        else {
            motor->negamax(tablero, i, -999999999, 999999999, jugada);
        }
    }





    string jugadaString;
    int salida, llegada;

    //Chequear el tipo de jugada para imprimir la jugada en el formato correcto
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    if(tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK){
        llegada = operaciones_bit::getLlegada(jugada);
        int piezaAPromover = operaciones_bit::getSalida(jugada);

        if(llegada > 32) {
            salida = llegada - 8;
        }
        else{
            salida = llegada + 8;
        }
        if(piezaAPromover == DAMA){
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "q";
        }
        else if(piezaAPromover == TORRE){
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "r";
        }
        else if(piezaAPromover == ALFIL){
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "b";
        }
        else if(piezaAPromover == CABALLO){
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "n";
        }
        salida = operaciones_bit::getSalida(jugada);

    }
    else if(tipoDeJugada == PROMOTIONDER || tipoDeJugada == CPDC){
        llegada = operaciones_bit::getLlegada(jugada);
        int piezaAPromover = operaciones_bit::getSalida(jugada);
        if(llegada > 32) {
            salida = llegada - 7;
        }
        else{
            salida = llegada + 7;
        }
        if(piezaAPromover == DAMA){
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "q";
        }
        else if(piezaAPromover == TORRE){
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "r";
        }
        else if(piezaAPromover == ALFIL){
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "b";
        }
        else if(piezaAPromover == CABALLO){
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "n";
        }
        salida = operaciones_bit::getSalida(jugada);

    }
    else if(tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == CPIC) {
        llegada = operaciones_bit::getLlegada(jugada);
        int piezaAPromover = operaciones_bit::getSalida(jugada);
        if (llegada > 32) {
            salida = llegada - 9;
        } else {
            salida = llegada + 9;
        }
        if (piezaAPromover == DAMA) {
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "q";
        } else if (piezaAPromover == TORRE) {
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "r";
        } else if (piezaAPromover == ALFIL) {
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "b";
        } else if (piezaAPromover == CABALLO) {
            jugadaString += constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            jugadaString += "n";
        }
        salida = operaciones_bit::getSalida(jugada);

    }
    else {
        salida = operaciones_bit::getSalida(jugada);
        llegada = operaciones_bit::getLlegada(jugada);

        jugadaString += constantes::NumeroACasilla[salida];
        jugadaString += constantes::NumeroACasilla[llegada];
    }

    //Antes de imprimir la jugada, se realiza en el tablero interno
    tablero->moverPieza(salida, llegada, tipoDeJugada);
    motor->TT->limpiarTabla();
    tablero->imprimirTablero();
    if(tablero->ganoNegro){
        cout << "Han ganado las negras!" << endl;
        exit(0);
    }
    else if(tablero->ganoBlanco){
        cout << "Han ganado las blancas!" << endl;
        exit(0);
    }

    cout << "bestmove " << jugadaString << endl;
    cout << "cantidad de nodos" << motor->nodos << endl;
/*
    tablero->imprimirTablero();
*/

}

void ProtocoloUCI:: inputSetOption(string inputString) {
}