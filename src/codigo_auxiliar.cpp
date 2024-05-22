//
// Created by axel on 22/05/24.
//

#include <iostream>
#include <fstream>
#include "Caballo.h"
#include "Tablero.h"
#include "Motor.h"

#include <string>
#include "constantes.h"
#include "codigo_auxiliar.h"
#include <random>

#include <memory>

//Este código fue utilizado para el proceso de optimización de parámetros de la función de evaluación

Tablero* t = new Tablero("position startpos");
Motor* m = new Motor();
std::vector<double> velocidades(459, 0.0);
double velocity = 0.0;

double codigo_auxiliar:: meanSquareError(double k){
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

        double sigmoid = 1.0/(double) (1+pow(10, -1.13*score/400));
        error = error + pow(resultado - sigmoid,2);


    }

    return error/(double)5000000;
}

void codigo_auxiliar:: actualizarParametros(std::vector<double>& x, double rate, double epsilon){
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

void codigo_auxiliar:: guardarParametros(){
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

std::vector<double> codigo_auxiliar:: obtenerParametros(){
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
std::vector<double> codigo_auxiliar:: calcularGradiente(std::string fen, double epsilon) {
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

std::vector<double> codigo_auxiliar:: SGD(int epochs, double learningRate){
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