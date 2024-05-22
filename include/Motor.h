//
// Created by axel on 14/12/23.
//

#ifndef CHESSCPP_MOTOR_H
#define CHESSCPP_MOTOR_H
#include "Tablero.h"
#include "Pieza.h"
#include "operaciones_bit.h"
#include "TablaTrans.h"
class Motor {
public:
    Motor();
    bool tablaInicializada = false;
    int incrementos = 0;
     TablaTrans* TT;
     int hashHits = 0;
    int totalMoves = 0;
    int totalPositions = 0;
    float branchingFactor = 0.0;
    int ply;
    int nodos = 0;

    float quiescence(Tablero* tablero, float alfa, float beta);
     float valoracion(Tablero* tablero);
    static float valoracionMaterial(Tablero* tablero);
    U64 perft(int profundidad, Tablero* tablero);
    static float valoracionMovilidad(Tablero* tablero);
    static float contarMaterialSinPeones(Tablero* tablero);
     float negamax(Tablero* tablero, int profundidad, float alfa, float beta, u_short &zbestMove);
     double tiempoGeneracion = 0;
     double tiempoHacer = 0;
     double TiempoDeshacer = 0;
     double TiempoDevolver = 0;
      float calcularDesarrollo (Tablero* tablero, int color);
     static float calcularOcupacion(Tablero* tablero, int color);
    static float calcularOcupacionPeon(Tablero* tablero, int color);
    static float calcularOcupacionCaballo(Tablero* tablero, int color);
    static float calcularOcupacionAlfil(Tablero* tablero, int color);
    static float calcularOcupacionTorre(Tablero* tablero, int color);
    static float calcularOcupacionReina(Tablero* tablero, int color);
    static float calcularOcupacionRey(Tablero* tablero, int color);
     //bool que indica si estamos en la etapa final del juego
     static bool endgame;
     float seguridadDelRey(Tablero* tablero);
     static float controlDeCentroCaballo(Tablero* tablero, int color);
     static float controlDeCentroPeon(Tablero* tablero, int color);
     static float controlDeCentroAlfil(Tablero* tablero, int color);
     static float controlDeCentro(Tablero* tablero, int color);


};


#endif //CHESSCPP_MOTOR_H
