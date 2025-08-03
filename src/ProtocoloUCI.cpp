

#include "ProtocoloUCI.h"
#include <iostream>
#include <chrono>

using namespace std;



ProtocoloUCI:: ProtocoloUCI() {

}

std::string ProtocoloUCI:: formatearJugada(u_short jugada){
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
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);

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
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);

    }
    else if(tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == CPIC) {
        llegada = operaciones_bit::getLlegada(jugada);
        int piezaAPromover = operaciones_bit::getSalida(jugada);
        if (llegada > 32) {
            salida = llegada - 9;
        } else {
            salida = llegada + 9;
        }
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);

    }
    else {
        salida = operaciones_bit::getSalida(jugada);
        llegada = operaciones_bit::getLlegada(jugada);

        jugadaString += constantes::NumeroACasilla[salida];
        jugadaString += constantes::NumeroACasilla[llegada];
    }
    return jugadaString;
}


std::string ProtocoloUCI:: generarJugadaString(int piezaAPromover, int salida, int llegada) {
    std::string jugadaString = "";
    jugadaString += constantes::NumeroACasilla[salida];
    jugadaString += constantes::NumeroACasilla[llegada];

    switch(piezaAPromover) {
        case DAMA:
            jugadaString += "q";
            break;
        case TORRE:
            jugadaString += "r";
            break;
        case ALFIL:
            jugadaString += "b";
            break;
        case CABALLO:
            jugadaString += "n";
            break;
    }

    return jugadaString;
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
            inputGo(input);
        } else if(input.substr(0,4) == "quit"){
            delete motor->TT;

            delete motor;
            delete tablero;
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
                tablero->imprimirTablero();
                motor->TT->asignarTablero(tablero);
                posicionSeteada = true;
                return;
            }
        }

        //se realiza la ultima jugada que aparece en el string.
        int i = input.size() - 5;
        string jugada = input.substr(i, 5);
        int salida, llegada;
        if(jugada[0] == ' '){
            jugada = jugada.substr(1, 4);
        }
        tablero->generar_movimientos(tablero->_turno, 0);
        for(int j = 0; j <= tablero->cantMovesGenerados[0]; j++){
            u_short move = tablero->movimientos_generados[0][j];
            if(jugada == formatearJugada(move)){
                tablero->moverPieza(operaciones_bit::getSalida(move), operaciones_bit::getLlegada(move), operaciones_bit::getTipoDeJugada(move));
/*
                motor->TT->limpiarTabla();
*/
/*
                tablero->imprimirTablero();
*/
                tablero->cantMovesGenerados[0] = -1;

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

void ProtocoloUCI:: inputGo(string input) {
    // Tiempo e incrementos (en milisegundos)
    long long wt = 0, bt = 0, winc = 0, binc = 0;
    size_t pos;
    int prof = 0;

    // Extraer el tiempo y el incremento del input
    if (tablero->_turno == 0) { // Blanco
        if ((pos = input.find("wtime")) != string::npos) {
            wt = static_cast<long long>(stod(input.substr(pos + 6)));
        }
        if ((pos = input.find("winc")) != string::npos) {
            winc = static_cast<long long>(stod(input.substr(pos + 5)));
        }
    } else { // Negro
        if ((pos = input.find("btime")) != string::npos) {
            bt = static_cast<long long>(stod(input.substr(pos + 6)));
        }
        if ((pos = input.find("binc")) != string::npos) {
            binc = static_cast<long long>(stod(input.substr(pos + 5)));
        }
    }

    // Determinar el tiempo disponible para esta jugada
    long long tiempo = (tablero->_turno == 0) ? wt : bt;
    long long incremento = (tablero->_turno == 0) ? winc : binc;

    if(incremento > 0 && (tiempo <= 5000)){
        tiempo = (incremento*75)/100;
    }

    // Heurística de gestión del tiempo (20 jugadas restantes como estimación)
    motor->tiempoDisponible = tiempo / 20 + incremento / 2;

    // Iniciar búsqueda
    motor->timeStart = chrono::steady_clock::now();
    motor->stopSearch = false;

    int i = 1;
    u_short mejorJugadaTemporal;
    int plySeguidosComoMejorJugada = 0;

    while (i < 10) {
        if (i == 1) {
            motor->bestMove = 0;

        }


        motor->ply = -1;
        motor->nodosBusqueda = 0;
        motor->incrementos = 0;
        motor->nodos = 0;


        float eval = motor->negamax(tablero, i, -999999999, 999999999, true);

        bool romper = false;
        bool esFinal = (4*tablero->damasBlancas + 4*tablero->damasNegras +
                        2*tablero->torresBlancas + 2*tablero->torresNegras +
                        tablero->caballosBlancos + tablero->caballosNegros +
                        tablero->alfilesBlancos + tablero->alfilesNegros) <= 6 ? true : false;

        int plySegunEtapa;
        esFinal ? plySegunEtapa = 5 : plySegunEtapa = 10;
        if( i >= 5 && !esFinal) {
            if (tablero->formatearJugada(mejorJugadaTemporal) == tablero->formatearJugada(motor->bestMove)) {
                plySeguidosComoMejorJugada++;
            }
            else {
                plySeguidosComoMejorJugada = 0;
            }
            if ((plySeguidosComoMejorJugada == plySegunEtapa) && (eval - motor->maxEval2 >= 100)) {
                romper = true;
            }
        }
        cout << "Nodos en profundidad " << i << ": " << motor->nodos << endl;
        if (romper) {
            break;
        }
        mejorJugadaTemporal = motor->bestMove;





/*
        cout << "Porcentaje de hits:" << (motor->hashHits*100)/(motor->misses) << endl;
*/
/*        motor->hashHits = 0;
        motor->misses = 0;*/

        // Verificar si se agotó el tiempo
        auto now = chrono::steady_clock::now();
        long long tiempoTranscurrido = chrono::duration_cast<chrono::milliseconds>(now - motor->timeStart).count();
/*        std::cout << "Profundidad: " << i
                  << ", Tiempo transcurrido: " << tiempoTranscurrido
                  << " / " << motor->tiempoDisponible << " ms" << std::endl;*/

        if (tiempoTranscurrido >= motor->tiempoDisponible) {
            break;
        }

        ++i;
        prof = i;
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();


    cout << motor -> exitosNull << endl;

    string jugadaString;
    int salida, llegada;
    u_short jugada = motor->bestMove;

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
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);
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
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);
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
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);
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
/*
    tablero->imprimirTablero();
*/
    cout << "bestmove " << jugadaString << endl;
    cout << "profundidad alcanzada: " << prof << endl;
   cout << "cantidad de nodos buscados: " << motor->nodos << endl;
    /*cout << "cantidad de hits: " << motor->hashHits << endl;*/
    //cout << "Porcentaje de tabla de transposicion llena:" <<
         //motor->porcentajeTabla() << endl;
/*
    motor->TT->limpiarTabla();
*/
    motor->index_repeticion = tablero->contadorJugadas;
    U64 claveActual = tablero->zobrist;
    motor->tabla_de_repeticiones[motor->index_repeticion] = claveActual;
    for(int i = 0; i < 256; i++){
        tablero->cantMovesGenerados[i] = -1;
    }
    if(tablero->ganoNegro){
        cout << "Han ganado las negras!" << endl;
        exit(0);
    }
    else if(tablero->ganoBlanco){
        cout << "Han ganado las blancas!" << endl;
        exit(0);
    }



/*
    tablero->imprimirTablero();
*/

}

void ProtocoloUCI:: inputSetOption(string inputString) {
}