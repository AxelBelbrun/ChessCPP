#include <iostream>
#include <fstream>
#include "Pieza.h"
#include "Caballo.h"
#include "Tablero.h"
#include "Motor.h"
#include "Jugada.h"
#include "calculadoraMovesTorre.h"
#include "calculadoraMovesAlfil.h"
#include <chrono>
#include <string>
#include "constantes.h"
#include "ProtocoloUCI.h"
#include "TablaTrans.h"
#include <random>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>


Tablero* t = new Tablero("position startpos");
Motor* m = new Motor();
std::vector<double> velocidades(459, 0.0);
double velocity = 0.0;


double meanSquareError(double k){
    ifstream file("/home/axel/Documentos/positions.txt");
    std::vector<string> positions;
    string line;
    k = 0.278;
    while( getline(file, line) ){
        positions.push_back(line);
    }

    Motor* m = new Motor();
    double error = 0.0;
    Tablero* t = new Tablero("position startpos");
    for (int i = 0; i < 5000000; i++) {
        string res = positions[i].substr(positions[i].size()-4,3);
        double resultado = stof(res);
        string fen = positions[i].substr(0,positions[i].size()-5);
        t->configurarFen(fen);

        double score;
        if(t->_turno == 0){
            score = m->quiescence(t, -500000, 500000);
        }
        else{
            score = -m->quiescence(t, -500000, 500000);
        }
/*
        score = m->quiescence(t, -500000, 500000);
*/

        double sigmoid = 1.0/(double) (1+pow(10, -1.13*score/400));
        error = error + pow(resultado - sigmoid,2);


    }

    return error/(double)5000000;
}

void actualizarParametros(std::vector<double> x, double rate, double epsilon){
    double n = 1;

    for(int i = 1; i < 6; i++){
        constantes::valorPieza[i] += x[i-1]*epsilon*rate;
        constantes::valorPieza[i+6] -= x[i-1]*epsilon*rate;
    }

    for(int i = 5, j = 0; i < 69; i++, j++){

        constantes::ocupacionPeonBlanco[j] += x[i]*epsilon*rate;
    }
    for(int i = 69, j = 0; i < 133; i++, j++){
        constantes::ocupacionCaballoBlanco[j] += x[i]*epsilon*rate;
    }
    for(int i = 133, j = 0; i < 197; i++, j++){
        constantes::ocupacionAlfilBlanco[j] += x[i]*epsilon*rate;
    }
    for(int i = 197, j = 0; i < 261; i++, j++){
        constantes::ocupacionTorreBlanco[j] += x[i]*epsilon*rate;
    }
    for(int i = 261, j = 0; i < 325; i++, j++){
        constantes::ocupacionReinaBlanco[j] += x[i]*epsilon*rate;
    }
    for(int i = 325, j = 0; i < 389; i++, j++){
        constantes::ocupacionReyBlancoMedioJuego[j] += x[i]*epsilon*rate;
    }
    for(int i = 389, j = 0; i < 453; i++, j++){
        constantes::ocupacionReyBlancoFinal[j] += x[i]*epsilon*rate;
    }
    constantes::premioPorEnrocar += rate * epsilon * x[453];
    constantes::castigoPorSacarDama += rate * epsilon * x[454];
    constantes::castigoMultiplesMovimientos += rate * epsilon * x[455];
    constantes::castigoPeonBloqueado += rate * epsilon * x[456];
    constantes::premioDesarrolloPiezaMenor += rate * epsilon * x[457];
    constantes::premioEscudoDePeones += rate * epsilon * x[458];
    constantes::castigoEnroqueSinEscudo += rate * epsilon * x[459];



}

void guardarParametros(){
    std::ofstream file;
    file.open("/home/axel/Documentos/parametros.txt");
    file << "Pesos de las piezas: " << std::endl;
    for(int i = 0; i < 5; i++){
        file << constantes::valorPieza[i+1] << ", " << std::endl;
    }
    file << "Ocupacion peon blanco: " << std::endl;

    for(int i = 5; i < 69; i++){
        file << constantes::ocupacionPeonBlanco[i-5] << ", " << std::endl;
    }
    file << "Ocupacion caballo blanco: " << std::endl;
    for(int i = 69; i < 133; i++){
        file << constantes::ocupacionCaballoBlanco[i-69] << ", " << std::endl;
    }
    file << "Ocupacion alfil blanco: " << std::endl;
    for(int i = 133; i < 197; i++) {
        file << constantes::ocupacionAlfilBlanco[i - 133] << ", " << std::endl;
    }
    file << "Ocupacion torre blanco: " << std::endl;
    for(int i = 197; i < 261; i++){
        file << constantes::ocupacionTorreBlanco[i-197] << ", " << std::endl;
    }
    file << "Ocupacion reina blanco: " << std::endl;
    for(int i = 261; i < 325; i++){
        file << constantes::ocupacionReinaBlanco[i-261] << ", " << std::endl;
    }
    file << "Ocupacion rey blanco medio juego: " << std::endl;
    for(int i = 325; i < 389; i++){
        file << constantes::ocupacionReyBlancoMedioJuego[i-325] << ", " << std::endl;
    }
    file << "Ocupacion rey blanco final: " << std::endl;
    for(int i = 389; i < 453; i++){
        file << constantes::ocupacionReyBlancoFinal[i-389] << ", " << std::endl;
    }
    file << "Premio por enrocar: " << constantes::premioPorEnrocar << std::endl;
    file << "Castigo por sacar dama: " << constantes::castigoPorSacarDama << std::endl;
    file << "Castigo por multiples movimientos: " << constantes::castigoMultiplesMovimientos << std::endl;
    file << "Castigo por peon bloqueado: " << constantes::castigoPeonBloqueado << std::endl;
    file << "Premio desarrollo pieza menor: " << constantes::premioDesarrolloPiezaMenor << std::endl;
    file << "Premio escudo de peones: " << constantes::premioEscudoDePeones << std::endl;
    file << "Castigo enroque sin escudo: " << constantes::castigoEnroqueSinEscudo << std::endl;


    file.close();
}

std::vector<double> obtenerParametros(){
    std:vector<double> parametros;
    for(int i = 1; i < 6; i++){
        parametros.push_back(constantes::valorPieza[i]);
    }
    for(int i = 5; i < 69; i++){
        parametros.push_back(constantes::ocupacionPeonBlanco[i-5]);
    }
    for(int i = 69; i < 133; i++){
        parametros.push_back(constantes::ocupacionCaballoBlanco[i-69]);
    }
    for(int i = 133; i < 197; i++){
        parametros.push_back(constantes::ocupacionAlfilBlanco[i-133]);
    }
    for(int i = 197; i < 261; i++){
        parametros.push_back(constantes::ocupacionTorreBlanco[i-197]);
    }
    for(int i = 261; i < 325; i++){
        parametros.push_back(constantes::ocupacionReinaBlanco[i-261]);
    }
    for(int i = 325; i < 389; i++){
        parametros.push_back(constantes::ocupacionReyBlancoMedioJuego[i-325]);
    }
    for(int i = 389; i < 453; i++){
        parametros.push_back(constantes::ocupacionReyBlancoFinal[i-389]);
    }
    parametros.push_back(constantes::premioPorEnrocar);
    parametros.push_back(constantes::castigoPorSacarDama);
    parametros.push_back(constantes::castigoMultiplesMovimientos);
    parametros.push_back(constantes::castigoPeonBloqueado);
    parametros.push_back(constantes::premioDesarrolloPiezaMenor);
    parametros.push_back(constantes::premioEscudoDePeones);
    parametros.push_back(constantes::castigoEnroqueSinEscudo);


    return parametros;
}
std::vector<double> calcularGradiente(std::string fen, double epsilon) {
   t->configurarFen(fen);
    std::vector<double> gradiente;
    std::vector<double> parametros = obtenerParametros();
    int k;
    if(t->_turno == 0){
        k = 1;
    }
    else{
        k = -1;
    }
    for (int i = 0; i < parametros.size(); i++) {

            if (i < 5) {
                constantes::valorPieza[i + 1] += epsilon;
                constantes::valorPieza[i + 7] -= epsilon;
            } else if (i < 69 && i >= 5) {
                constantes::ocupacionPeonBlanco[i - 5] += epsilon;
            } else if (i < 133 && i >= 69) {
                constantes::ocupacionCaballoBlanco[i - 69] += epsilon;
            } else if (i < 197 && i >= 133) {
                constantes::ocupacionAlfilBlanco[i - 133] += epsilon;
            } else if (i < 261 && i >= 197) {
                constantes::ocupacionTorreBlanco[i - 197] += epsilon;
            } else if (i < 325 && i >= 261) {
                constantes::ocupacionReinaBlanco[i - 261] += epsilon;
            } else if (i < 389 && i >= 325) {
                constantes::ocupacionReyBlancoMedioJuego[i - 325] += epsilon;
            } else if (i < 453 && i >= 389) {
                constantes::ocupacionReyBlancoFinal[i - 389] += epsilon;
            } /*else if (i == 453) {
                constantes::premioPorEnrocar += epsilon;
            } else if (i == 454) {
                constantes::castigoPorSacarDama += epsilon;
            } else if (i == 455) {
                constantes::castigoMultiplesMovimientos += epsilon;
            } else if (i == 456) {
                constantes::castigoPeonBloqueado += epsilon;
            } else if (i == 457) {
                constantes::premioDesarrolloPiezaMenor += epsilon;
            } else if (i == 458) {
                constantes::premioEscudoDePeones += epsilon;
            } else if (i == 459) {
                constantes::castigoEnroqueSinEscudo += epsilon;
            }*/

        double val1 = k * m->quiescence(t, -500000, 500000);


            if (i < 5) {
                constantes::valorPieza[i + 1] -= 2 * epsilon;
                constantes::valorPieza[i + 7] += 2 * epsilon;
            } else if (i < 69 && i >= 5) {
                constantes::ocupacionPeonBlanco[i - 5] -= 2 * epsilon;
            } else if (i < 133 && i >= 69) {
                constantes::ocupacionCaballoBlanco[i - 69] -= 2 * epsilon;
            } else if (i < 197 && i >= 133) {
                constantes::ocupacionAlfilBlanco[i - 133] -= 2 * epsilon;
            } else if (i < 261 && i >= 197) {
                constantes::ocupacionTorreBlanco[i - 197] -= 2 * epsilon;
            } else if (i < 325 && i >= 261) {
                constantes::ocupacionReinaBlanco[i - 261] -= 2 * epsilon;
            } else if (i < 389 && i >= 325) {
                constantes::ocupacionReyBlancoMedioJuego[i - 325] -= 2 * epsilon;
            } else if (i < 453 && i >= 389) {
                constantes::ocupacionReyBlancoFinal[i - 389] -= 2 * epsilon;
            } /*else if (i == 453) {
                constantes::premioPorEnrocar -= 2 * epsilon;
            } else if (i == 454) {
                constantes::castigoPorSacarDama -= 2 * epsilon;
            } else if (i == 455) {
                constantes::castigoMultiplesMovimientos -= 2 * epsilon;
            } else if (i == 456) {
                constantes::castigoPeonBloqueado -= 2 * epsilon;
            } else if (i == 457) {
                constantes::premioDesarrolloPiezaMenor -= 2 * epsilon;
            } else if (i == 458) {
                constantes::premioEscudoDePeones -= 2 * epsilon;
            } else if (i == 459) {
                constantes::castigoEnroqueSinEscudo -= 2 * epsilon;
            }*/

        double val2 = k * m->quiescence(t, -500000, 500000);
            double v = (val2 - val1) / (epsilon * 2);
        gradiente.push_back(v);
        if (i < 5) {
            constantes::valorPieza[i + 1] += epsilon;
            constantes::valorPieza[i + 7] -= epsilon;
        } else if (i < 69 && i >= 5) {
            constantes::ocupacionPeonBlanco[i - 5] += epsilon;
        } else if (i < 133 && i >= 69) {
            constantes::ocupacionCaballoBlanco[i - 69] += epsilon;
        } else if (i < 197 && i >= 133) {
            constantes::ocupacionAlfilBlanco[i - 133] += epsilon;
        } else if (i < 261 && i >= 197) {
            constantes::ocupacionTorreBlanco[i - 197] += epsilon;
        } else if (i < 325 && i >= 261) {
            constantes::ocupacionReinaBlanco[i - 261] += epsilon;
        } else if (i < 389 && i >= 325) {
            constantes::ocupacionReyBlancoMedioJuego[i - 325] += epsilon;
        } else if (i < 453 && i >= 389) {
            constantes::ocupacionReyBlancoFinal[i - 389] += epsilon;
        } /*else if (i == 453) {
            constantes::premioPorEnrocar += epsilon;
        } else if (i == 454) {
            constantes::castigoPorSacarDama += epsilon;
        } else if (i == 455) {
            constantes::castigoMultiplesMovimientos += epsilon;
        } else if (i == 456) {
            constantes::castigoPeonBloqueado += epsilon;
        } else if (i == 457) {
            constantes::premioDesarrolloPiezaMenor += epsilon;
        } else if (i == 458) {
            constantes::premioEscudoDePeones += epsilon;
        } else if (i == 459) {
            constantes::castigoEnroqueSinEscudo += epsilon;
        }*/
    }


    return gradiente;
}

std::vector<double> SGD(int epochs, double learningRate){
    ifstream file("/home/axel/Documentos/positions.txt");
    std::vector<string> positions;
    string line;
    while( getline(file, line) ){
        positions.push_back(line);
    }
    Tablero* t = new Tablero("position startpos");

    std::vector<double> parametros =  obtenerParametros();
    double l1_rate = 0.1;
    vector<double> gradientes(460, 0.0);
    for(int d = 0; d < 100000 ; d++) {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, positions.size() - 1);
        std::uniform_int_distribution<> dis2(1,6);

        int x = dis(gen);
        string fen = positions[x].substr(0, positions[x].size() - 5);
        std::vector<double> variacionesRandom;
        for (int i = 0; i < 460; i++) {
            double t = dis2(gen);
            variacionesRandom.push_back(t);
        }
        actualizarParametros(variacionesRandom, 1, 1);
        t->configurarFen(fen);
        int k;
        if(t->_turno == 0){
            k = 1;
        }
        else{
            k = -1;
        }
        double quiesPositivo = k*m->quiescence(t, -50000, 50000);
        actualizarParametros(variacionesRandom, 1, -2*1);
        t->configurarFen(fen);
        double quiesNegativo = k*m->quiescence(t, -50000, 50000);
        for (int i = 0; i < 460; i++) {
            gradientes[i] += (quiesPositivo - quiesNegativo) / (2 * variacionesRandom[i]);

        }
        actualizarParametros(variacionesRandom, 1, 1);

    }
    for(int i = 0; i < 460; i++){
        gradientes[i] = gradientes[i]/100000;
    }

    return gradientes;
}
int main() {
    ProtocoloUCI *x = new ProtocoloUCI();
    x->comunicacionUCI();

    return 0;
}

/*    Tablero *tablero = new Tablero("position startpos");
    tablero->moverPieza(13,21,QUIET);
    tablero->moverPieza(50,42,QUIET);
    tablero->moverPieza(6,41,QUIET);
    tablero->moverPieza(58,43,QUIET);
    tablero->moverPieza(41,59,CAPTURE);
    tablero->imprimirTablero();
    Motor *motor = new Motor();
    std::ofstream resultados("resultados.txt");
    vector<u_short> moves = tablero->generar_movimientos(1);
    for (int i = 0; i < moves.size(); i++) {
        if (tablero->moverPieza(operaciones_bit::getSalida(moves[i]), operaciones_bit::getLlegada(moves[i]),
                                operaciones_bit::getTipoDeJugada(moves[i]))) {
            int salida = operaciones_bit::getSalida(moves[i]);
            int llegada = operaciones_bit::getLlegada(moves[i]);

            string jugadaString = constantes::NumeroACasilla[salida];
            jugadaString += constantes::NumeroACasilla[llegada];
            U64 resultadoPropio = motor->perft(0, tablero);
            resultados << jugadaString << " " << resultadoPropio << std::endl;
            tablero->deshacerMovimiento();
        }
    }
}*/
/*    Tablero *t = new Tablero("position fen 6k1/8/8/6p1/2p5/8/K4p2/4q3 w - -");
    t->imprimirTablero();

*//*
    t->moverPieza(31,4,CHECK);
    t->moverPieza(7,16,QUIET);*//*

    Motor *m = new Motor();
    vector<u_short> moves = t->generar_movimientos(1);
    U64 zobrist1 = t->zobrist;
    t->moverPieza(3,3,PROMOTION);
    t->deshacerMovimiento();
    cout << t->zobrist << endl;
    u_short posBestMove;
    m->negamax(t, 9, -999999999, 999999999, posBestMove);*/
/*    for (auto movimiento: moves) {
        if (t->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento),
                          operaciones_bit::getTipoDeJugada(movimiento))) {
            u_short posBestMove = 0;
            double eval = -m->negamax(t, 8, -999999999, 999999999, posBestMove);
            t->deshacerMovimiento();
            cout << constantes::NumeroACasilla[operaciones_bit::getSalida(movimiento)] <<
                 constantes::NumeroACasilla[operaciones_bit::getLlegada(movimiento)] << " " <<
                 eval << endl;
        }
    }*/
/*    cout << t->zobrist << endl;
}*/
    /* double eval = m->valoracion(t);
     u_short bestMove;
     m->ply = -1;
     m->negamax(t, 5, -1000000, 1000000, bestMove);
     u_short jugada = bestMove ;
     cout << eval;
     cout << constantes::NumeroACasilla[operaciones_bit::getSalida(bestMove)] << constantes::NumeroACasilla[operaciones_bit::getLlegada(bestMove)] << endl;*/
//}

/*
    Tablero *t = new Tablero("position startpos");
    Motor *m = new Motor();
    for (int i = 0; i < 16; i++) {
        u_short jugada;
        for (int i = 1; i < 4; i++) {
            if (i == 1) {
                jugada = 0;
            }

            m->ply = -1;
            if (t->endgame) {
                m->negamax(t, i + 2, -999999999, 999999999, jugada);

            } else {
                m->negamax(t, i, -999999999, 999999999, jugada);
            }
        }
        t->moverPieza(operaciones_bit::getSalida(jugada), operaciones_bit::getLlegada(jugada),
                      operaciones_bit::getTipoDeJugada(jugada));
*/




// Tuner
    /*double error = meanSquareError(100);
    cout << "Error: " << error << endl;*/
/*
vector<double> random;
for(int i = 0; i < 453; i++){
    double x = rand() %  500;
    random.push_back(x);
}
    actualizarParametros(random, 1, 0);
*/
   /* std::vector<double> parametros = obtenerParametros();
    int i = 0;
    bool mejora = true;
    while (i < 1000) {
        actualizarParametros(SGD(1, -0.1), -0.01, 1);
        cout << "SGD calculado" << endl;

        double error2 = meanSquareError(9);

        i++;
        cout << "Iteracion: " << i << endl;

        *//*cout << "Error: " << error2 << endl;
        guardarParametros();*//*

        if (error2 <= error) {
            error = error2;
            cout << "Error: " << error << endl;
            guardarParametros();
        } else {
            cout << "No se mejora el error" << endl;
            exit(0);
        }
    }
}*/
    /*mejora = false;
    for (int i = 0; i < parametros.size(); i++) {
        cout << "Iteracion: " << i << endl;
        if (i < 5) {
            constantes::valorPieza[i + 1] += 1;
            constantes::valorPieza[i + 7] -= 1;
        } else if (i < 69 && i >= 5) {
            constantes::ocupacionPeonBlanco[i - 5] += 1;
        } else if (i < 133 && i >= 69) {
            constantes::ocupacionCaballoBlanco[i - 69] += 1;
        } else if (i < 197 && i >= 133) {
            constantes::ocupacionAlfilBlanco[i - 133] += 1;
        } else if (i < 261 && i >= 197) {
            constantes::ocupacionTorreBlanco[i - 197] += 1;
        } else if (i < 325 && i >= 261) {
            constantes::ocupacionReinaBlanco[i - 261] += 1;
        } else if (i < 389 && i >= 325) {
            constantes::ocupacionReyBlancoMedioJuego[i - 325] += 1;
        } else if (i < 453 && i >= 389) {
            constantes::ocupacionReyBlancoFinal[i - 389] += 1;
        } else if (i == 453) {
            constantes::premioPorEnrocar += 5;
        } else if (i == 454) {
            constantes::castigoPorSacarDama += 5;
        } else if (i == 455) {
            constantes::castigoMultiplesMovimientos += 5;
        } else if (i == 456) {
            constantes::castigoPeonBloqueado += 5;
        } else if (i == 457) {
            constantes::premioDesarrolloPiezaMenor += 5;
        } else if (i == 458) {
            constantes::premioEscudoDePeones += 5;
        } else if (i == 459) {
            constantes::castigoEnroqueSinEscudo += 5;
        }
        double error2 = meanSquareError(9);
        if (error2 < error) {
            mejora = true;
            error = error2;
            cout << "Error: " << error << endl;

            guardarParametros();
        } else {
            if (i < 5) {
                constantes::valorPieza[i + 1] -= 1 * 2;
                constantes::valorPieza[i + 7] += 1 * 2;
            } else if (i < 69 && i >= 5) {
                constantes::ocupacionPeonBlanco[i - 5] -= 1 * 2;
            } else if (i < 133 && i >= 69) {
                constantes::ocupacionCaballoBlanco[i - 69] -= 1 * 2;
            } else if (i < 197 && i >= 133) {
                constantes::ocupacionAlfilBlanco[i - 133] -= 1 * 2;
            } else if (i < 261 && i >= 197) {
                constantes::ocupacionTorreBlanco[i - 197] -= 1 * 2;
            } else if (i < 325 && i >= 261) {
                constantes::ocupacionReinaBlanco[i - 261] -= 1 * 2;
            } else if (i < 389 && i >= 325) {
                constantes::ocupacionReyBlancoMedioJuego[i - 325] -= 1 * 2;
            } else if (i < 453 && i >= 389) {
                constantes::ocupacionReyBlancoFinal[i - 389] -= 1 * 2;
            } else if (i == 453) {
                constantes::premioPorEnrocar -= 5 * 2;
            } else if (i == 454) {
                constantes::castigoPorSacarDama -= 5 * 2;
            } else if (i == 455) {
                constantes::castigoMultiplesMovimientos -= 5 * 2;
            } else if (i == 456) {
                constantes::castigoPeonBloqueado -= 5 * 2;
            } else if (i == 457) {
                constantes::premioDesarrolloPiezaMenor -= 5 * 2;
            } else if (i == 458) {
                constantes::premioEscudoDePeones -= 5 * 2;
            } else if (i == 459) {
                constantes::castigoEnroqueSinEscudo -= 5 * 2;
            }
            error2 = meanSquareError(9);
            if (error2 < error) {
                mejora = true;
                error = error2;
                cout << "Error: " << error << endl;
                guardarParametros();
            }
            else{
                if (i < 5) {
                    constantes::valorPieza[i + 1] += 1;
                    constantes::valorPieza[i + 7] -= 1;
                } else if (i < 69 && i >= 5) {
                    constantes::ocupacionPeonBlanco[i - 5] += 1;
                } else if (i < 133 && i >= 69) {
                    constantes::ocupacionCaballoBlanco[i - 69] += 1;
                } else if (i < 197 && i >= 133) {
                    constantes::ocupacionAlfilBlanco[i - 133] += 1;
                } else if (i < 261 && i >= 197) {
                    constantes::ocupacionTorreBlanco[i - 197] += 1;
                } else if (i < 325 && i >= 261) {
                    constantes::ocupacionReinaBlanco[i - 261] += 1;
                } else if (i < 389 && i >= 325) {
                    constantes::ocupacionReyBlancoMedioJuego[i - 325] += 1;
                } else if (i < 453 && i >= 389) {
                    constantes::ocupacionReyBlancoFinal[i - 389] += 1;
                } else if (i == 453) {
                    constantes::premioPorEnrocar += 5;
                } else if (i == 454) {
                    constantes::castigoPorSacarDama += 5;
                } else if (i == 455) {
                    constantes::castigoMultiplesMovimientos += 5;
                } else if (i == 456) {
                    constantes::castigoPeonBloqueado += 5;
                } else if (i == 457) {
                    constantes::premioDesarrolloPiezaMenor += 5;
                } else if (i == 458) {
                    constantes::premioEscudoDePeones += 5;
                } else if (i == 459) {
                    constantes::castigoEnroqueSinEscudo += 5;
                }
            }
        }*//*
    }
}*/

/*        std::vector<double> t = SGD(1, 0.01);
        actualizarParametros(t, 0.00002,0.002);*/
/*        double error2 = meanSquareError(100);
        cout << "Error: " << error2 << endl;*/




/*
guardarParametros();
double error2 = meanSquareError(100);
cout << "Error: " << error2 << endl;
    return 0;
*/







/*
    Tablero *t = new Tablero("position startpos");
    Motor *m = new Motor();
    u_short jugada;
    m->ply = -1;
    m->negamax(t, 4, -999999999, 999999999, jugada);
    t->moverPieza(7,22,QUIET);
    U64 zobrist = t->zobrist;
    auto e = m->TT->obtenerEntrada();
    u_short jug;
    m->ply = -1;


    double c = m->negamax(t, 5, -999999999, 999999999, jug);
    cout << m->nodos;


}


*/



/*
    Tablero *x = new Tablero("position startpos");
    Motor *m = new Motor();
    u_short jugada;
    m->ply = -1;
    m->negamax(x, 3, -999999999, 999999999, jugada);
    cout << m->nodos;
    delete x;
    m->TT->limpiarTabla();
    delete m->TT;
    delete m;
}
*/

