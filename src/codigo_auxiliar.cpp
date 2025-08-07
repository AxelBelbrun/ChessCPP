/*
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
#include <math.h>

#include <memory>

//Este código fue utilizado para el proceso de optimización de parámetros de la función de evaluación

std::vector<double> velocidades(433, 0.0);
double velocity = 0.0;
int contador = 0;

double codigo_auxiliar:: meanSquareError(double k) {
    std::ifstream file("/home/axel/Documentos/posiciones-ML-Motor.txt");
    std::vector<std::string> positions;
    std::string line;
    while (getline(file, line)) {
        positions.push_back(line);
    }
    double EPS = 1e-9;

    if (positions.empty()) {
        std::cerr << "Error: No positions found in the file." << std::endl;
        return std::numeric_limits<double>::quiet_NaN();
    }

    std::unique_ptr<Motor> m = std::make_unique<Motor>();
    double error = 0.0;
    std::unique_ptr<Tablero> t = std::make_unique<Tablero>("position startpos");
    for (const auto& pos : positions) {
        std::string res = pos.substr(pos.size() - 4, 3);
        double resultado = std::stof(res);
        std::string fen = pos.substr(0, pos.size() - 5);
        t->configurarFen(fen);

        double score;
            m->ply = -1;
            score = m->quiescence(t.get(), -500000, 500000);


        if (score == std::numeric_limits<double>::infinity() || score == -std::numeric_limits<double>::infinity()) {
            std::cerr << "Error: Invalid score value." << std::endl;
            continue;
        }

        score = score/400; //Evitamos el overflow

        double sigmoid = 1.0 / (1 + std::exp(-k * score));
        error += - (score * std::log(sigmoid + EPS) + (1 - score) * std::log(1 - sigmoid + EPS));
    }

    return error/positions.size();
}

void codigo_auxiliar:: actualizarParametros(std::vector<double>& gradientes, double rate, double epsilon) {
    */
/*for (int i = 1; i < 6; i++) {
        constantes::valorPieza[i] += gradientes[i - 1] * rate + epsilon;
        constantes::valorPieza[i + 6] -= gradientes[i - 1] * rate + epsilon;
    }*//*

    for (int i = 0, j = 0; i < 48; i++, j++) {
        constantes::ocupacionPeon[j + 8] += gradientes[i] * rate + epsilon;
    }
    for (int i = 48, j = 0; i < 112; i++, j++) {
        constantes::ocupacionCaballo[j] += gradientes[i] * rate + epsilon;
    }
    for (int i = 112, j = 0; i < 176; i++, j++) {
        constantes::ocupacionAlfil[j] += gradientes[i] * rate + epsilon;
    }
    for (int i = 176, j = 0; i < 239; i++, j++) {
        constantes::ocupacionTorre[j] += gradientes[i] * rate + epsilon;
    }
    for (int i = 239, j = 0; i < 303; i++, j++) {
        constantes::ocupacionReina[j] += gradientes[i] * rate + epsilon;
    }
    for (int i = 303, j = 0; i < 368; i++, j++) {
        constantes::ocupacionReyMedioJuego[j] += gradientes[i] * rate + epsilon;
    }
    for (int i = 368, j = 0; i < 432; i++, j++) {
        constantes::ocupacionReyFinal[j] += gradientes[i] * rate + epsilon;
    }
*/
/*
    constantes::premioPorEnrocar += epsilon + rate * gradientes[432];
*//*

*/
/*
    constantes::castigoPorSacarDama += epsilon + rate * gradientes[454];
*//*

*/
/*
    constantes::castigoMultiplesMovimientos += epsilon + rate * gradientes[455];
*//*

*/
/*    constantes::castigoPeonBloqueado += epsilon + rate * gradientes[433];
    constantes::premioDesarrolloPiezaMenor += epsilon + rate * gradientes[434];
    constantes::premioEscudoDePeones += epsilon + rate * gradientes[435];
    constantes::castigoEnroqueSinEscudo += epsilon + rate * gradientes[436];
    constantes::pesoMovilidad += epsilon + rate * gradientes[437];*//*

}


void codigo_auxiliar:: guardarParametros(){
    std::ofstream file;
    contador++;
    file.open("/home/axel/Documentos/parametros/iteracion" + std::to_string(contador) + ".txt");
    file << "Pesos de las piezas: " << std::endl;
   */
/* for(int i = 0; i < 5; i++){
        file << constantes::valorPieza[i+1] << ", " << std::endl;
    }*//*

    file << "Ocupacion peon blanco: " << std::endl;

    for(int i = 5; i < 69; i++){

        file << constantes::ocupacionPeon[i - 5] << ", " << std::endl;
    }
    file << "Ocupacion caballo blanco: " << std::endl;
    for(int i = 69; i < 133; i++){
        file << constantes::ocupacionCaballo[i - 69] << ", " << std::endl;
    }
    file << "Ocupacion alfil blanco: " << std::endl;
    for(int i = 133; i < 197; i++) {
        file << constantes::ocupacionAlfil[i - 133] << ", " << std::endl;
    }
    file << "Ocupacion torre blanco: " << std::endl;
    for(int i = 197; i < 261; i++){
        file << constantes::ocupacionTorre[i - 197] << ", " << std::endl;
    }
    file << "Ocupacion reina blanco: " << std::endl;
    for(int i = 261; i < 325; i++){
        file << constantes::ocupacionReina[i - 261] << ", " << std::endl;
    }
    file << "Ocupacion rey blanco medio juego: " << std::endl;
    for(int i = 325; i < 389; i++){
        file << constantes::ocupacionReyMedioJuego[i - 325] << ", " << std::endl;
    }
    file << "Ocupacion rey blanco final: " << std::endl;
    for(int i = 389; i < 453; i++){
        file << constantes::ocupacionReyFinal[i - 389] << ", " << std::endl;
    }
*/
/*
    file << "Premio por enrocar: " << constantes::premioPorEnrocar << std::endl;
*//*

*/
/*
    file << "Castigo por sacar dama: " << constantes::castigoPorSacarDama << std::endl;
*//*

*/
/*
    file << "Castigo por multiples movimientos: " << constantes::castigoMultiplesMovimientos << std::endl;
*//*

*/
/*    file << "Castigo por peon bloqueado: " << constantes::castigoPeonBloqueado << std::endl;
    file << "Premio desarrollo pieza menor: " << constantes::premioDesarrolloPiezaMenor << std::endl;
    file << "Premio escudo de peones: " << constantes::premioEscudoDePeones << std::endl;
    file << "Castigo enroque sin escudo: " << constantes::castigoEnroqueSinEscudo << std::endl;
    file << "Peso movilidad: " << constantes::pesoMovilidad << std::endl;*//*



    file.close();
}

std::vector<double> codigo_auxiliar:: obtenerParametros(){
    std:vector<double> parametros;
    */
/*for(int i = 1; i < 6; i++){
        parametros.push_back(constantes::valorPieza[i]);
    }*//*

    for(int i = 5; i < 69; i++){
        if(5 <= i && i <= 12){
            continue;
        }
        if(61 <= i && i <= 68){
            continue;
        }

        parametros.push_back(constantes::ocupacionPeon[i - 5]);
    }
    for(int i = 69; i < 133; i++){
        parametros.push_back(constantes::ocupacionCaballo[i - 69]);
    }
    for(int i = 133; i < 197; i++){
        parametros.push_back(constantes::ocupacionAlfil[i - 133]);
    }
    for(int i = 197; i < 261; i++){
        parametros.push_back(constantes::ocupacionTorre[i - 197]);
    }
    for(int i = 261; i < 325; i++){
        parametros.push_back(constantes::ocupacionReina[i - 261]);
    }
    for(int i = 325; i < 389; i++){
        parametros.push_back(constantes::ocupacionReyMedioJuego[i - 325]);
    }
    for(int i = 389; i < 453; i++){
        parametros.push_back(constantes::ocupacionReyFinal[i - 389]);
    }
*/
/*
    parametros.push_back(constantes::premioPorEnrocar);
*//*

*/
/*
    parametros.push_back(constantes::castigoPorSacarDama);
*//*

*/
/*
    parametros.push_back(constantes::castigoMultiplesMovimientos);
*//*

    */
/*parametros.push_back(constantes::castigoPeonBloqueado);
    parametros.push_back(constantes::premioDesarrolloPiezaMenor);
    parametros.push_back(constantes::premioEscudoDePeones);
    parametros.push_back(constantes::castigoEnroqueSinEscudo);
    parametros.push_back(constantes::pesoMovilidad);*//*



    return parametros;
}
std::vector<double> codigo_auxiliar:: calcularGradiente(std::string fen, double epsilon) {
*/
/*
    t->configurarFen(fen);
*//*

    std::vector<double> gradiente;
    std::vector<double> parametros = obtenerParametros();
    int k;
   */
/* if(t->_turno == 0){
        k = 1;
    }*//*

*/
/*    else{
        k = -1;
    }*//*

    for (int i = 0; i < parametros.size(); i++) {

        if (i < 5) {
            constantes::valorPieza[i + 1] += epsilon;
            constantes::valorPieza[i + 7] -= epsilon;
        } else if (i < 69 && i >= 5) {
            constantes::ocupacionPeon[i - 5] += epsilon;
        } else if (i < 133 && i >= 69) {
            constantes::ocupacionCaballo[i - 69] += epsilon;
        } else if (i < 197 && i >= 133) {
            constantes::ocupacionAlfil[i - 133] += epsilon;
        } else if (i < 261 && i >= 197) {
            constantes::ocupacionTorre[i - 197] += epsilon;
        } else if (i < 325 && i >= 261) {
            constantes::ocupacionReina[i - 261] += epsilon;
        } else if (i < 389 && i >= 325) {
            constantes::ocupacionReyMedioJuego[i - 325] += epsilon;
        } else if (i < 453 && i >= 389) {
            constantes::ocupacionReyFinal[i - 389] += epsilon;
        } */
/*else if (i == 453) {
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
            }*//*


*/
/*
        double val1 = k * m->quiescence(t, -500000, 500000);
*//*



        if (i < 5) {
            constantes::valorPieza[i + 1] -= 2 * epsilon;
            constantes::valorPieza[i + 7] += 2 * epsilon;
        } else if (i < 69 && i >= 5) {
            constantes::ocupacionPeon[i - 5] -= 2 * epsilon;
        } else if (i < 133 && i >= 69) {
            constantes::ocupacionCaballo[i - 69] -= 2 * epsilon;
        } else if (i < 197 && i >= 133) {
            constantes::ocupacionAlfil[i - 133] -= 2 * epsilon;
        } else if (i < 261 && i >= 197) {
            constantes::ocupacionTorre[i - 197] -= 2 * epsilon;
        } else if (i < 325 && i >= 261) {
            constantes::ocupacionReina[i - 261] -= 2 * epsilon;
        } else if (i < 389 && i >= 325) {
            constantes::ocupacionReyMedioJuego[i - 325] -= 2 * epsilon;
        } else if (i < 453 && i >= 389) {
            constantes::ocupacionReyFinal[i - 389] -= 2 * epsilon;
        } */
/*else if (i == 453) {
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
            }*//*


  */
/*      double val2 = k * m->quiescence(t, -500000, 500000);
        double v = (val2 - val1) / (epsilon * 2);*//*

*/
/*
        gradiente.push_back(v);
*//*

        if (i < 5) {
            constantes::valorPieza[i + 1] += epsilon;
            constantes::valorPieza[i + 7] -= epsilon;
        } else if (i < 69 && i >= 5) {
            constantes::ocupacionPeon[i - 5] += epsilon;
        } else if (i < 133 && i >= 69) {
            constantes::ocupacionCaballo[i - 69] += epsilon;
        } else if (i < 197 && i >= 133) {
            constantes::ocupacionAlfil[i - 133] += epsilon;
        } else if (i < 261 && i >= 197) {
            constantes::ocupacionTorre[i - 197] += epsilon;
        } else if (i < 325 && i >= 261) {
            constantes::ocupacionReina[i - 261] += epsilon;
        } else if (i < 389 && i >= 325) {
            constantes::ocupacionReyMedioJuego[i - 325] += epsilon;
        } else if (i < 453 && i >= 389) {
            constantes::ocupacionReyFinal[i - 389] += epsilon;
        } else if (i == 453) {
            constantes::premioPorEnrocar += epsilon;
        }*/
/* else if (i == 454) {
            constantes::castigoPorSacarDama += epsilon;
        } else if (i == 455) {
            constantes::castigoMultiplesMovimientos += epsilon;
        } *//*
else if (i == 456) {
            constantes::castigoPeonBloqueado += epsilon;
        } else if (i == 457) {
            constantes::premioDesarrolloPiezaMenor += epsilon;
        } else if (i == 458) {
            constantes::premioEscudoDePeones += epsilon;
        } else if (i == 459) {
            constantes::castigoEnroqueSinEscudo += epsilon;
        }
    }


    return gradiente;
}

std::vector<double> codigo_auxiliar::SGD(int epochs, double learningRate) {
    std::ifstream file("/home/axel/Documentos/posiciones-ML-Motor.txt");
    std::vector<std::string> positions;
    std::string line;
    while (getline(file, line)) {
        positions.push_back(line);
    }

*/
/*    if (positions.empty()) {
        std::cerr << "Error: No positions found in the file." << std::endl;
        return std::vector<double>(432, std::numeric_limits<double>::quiet_NaN());
    }*//*

    Motor* m = new Motor();

    std::vector<double> gradientes(432, 0.0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, positions.size() - 1);
    std::uniform_real_distribution<> dis2(-1,1);


    std::vector<double> variacionesRandom(432);
    float variacion = 0.1;
    for (int i = 0; i < 432; i++) {
        variacionesRandom[i] = variacion;
    }
    double epsilon = 0; // Adjust epsilon value as needed
    double acumulado = 0.0;
    for (int d = 0; d < epochs; d++) {
        int x = dis(gen);

        std::string fen = positions[x].substr(0, positions[x].size() - 5);
        std::string res = positions[x].substr(positions[x].size() - 4, 3);
        double score = std::stof(res);
        Tablero t = Tablero();
        int k = (t._turno == 0) ? 1 : -1;
        m->ply = -1;

        double scorePropio = m->quiescence(&t, -5000000, 5000000);
        scorePropio = 1.0 / (1 + std::exp(-k * scorePropio)); //Pasamos el score por la sigmoide
        for (int i = 0; i < 432; i++) {
                int casillaActual;
                if ( i < 48){
                    casillaActual = i + 8;
                }
                else {
                    casillaActual = (((i + 16) % 64));
                }
                int tipoDePieza = t.obtenerTipoDePieza(casillaActual + 1);

                int hayPieza;
                if(tipoDePieza != VACIO){
                    hayPieza = 1;
                }
                else{
                    hayPieza = 0;
                }

                //Chequear si tenemos una pieza negra, en ese caso hay que espejar la casilla
                if(hayPieza && (t.piezas_negras() & (1ULL << (casillaActual)))){
                    casillaActual = operaciones_bit::espejarCasilla(casillaActual+1);
                    //Hay que encontrar la casilla espejada pero en función de i:
                    int casillaEnFuncionDei;
                    if(i < 48) {
                        casillaEnFuncionDei = (casillaActual);
                    }
                    else {casillaEnFuncionDei = i - ((i + 16) % 64) + (casillaActual);}
                    if (casillaEnFuncionDei >= 0 && casillaEnFuncionDei < gradientes.size()) {
                        gradientes[casillaEnFuncionDei] += (scorePropio - score)*hayPieza;
                    } else {
                        std::cerr << "Error: casillaEnFuncionDei out of bounds: " << casillaEnFuncionDei << std::endl;
                    }

                    gradientes[casillaEnFuncionDei] += (scorePropio - score) * hayPieza;
                }

                else {gradientes[i] += (scorePropio - score) * hayPieza;}

        }



    }


    */
/*//*
/ Clipping de gradientes
    double clipValue = 1.0;
    for (int i = 0; i < 432; i++) {
        if (gradientes[i] > clipValue) {
            gradientes[i] = clipValue;
        } else if (gradientes[i] < -clipValue) {
            gradientes[i] = -clipValue;
        }
    }*//*


    // Promediar los gradientes
    for (int i = 0; i < 432; i++) {
        gradientes[i] /= epochs;
    }
    delete m;

    return gradientes;
}*/
