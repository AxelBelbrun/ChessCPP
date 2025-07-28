//
// Created by axel on 14/12/23.
//

#ifndef CHESSCPP_MOTOR_H
#define CHESSCPP_MOTOR_H
#include "Tablero.h"
#include "Pieza.h"
#include "operaciones_bit.h"
#include "Tabla_transposicion.h"
#include <chrono>
class Motor {
public:
    Motor();
    bool tablaInicializada = false;
    int incrementos = 0;
    int porcentajeTabla();
     Tabla_transposicion* TT;
     int hashHits = 0;
    int totalMoves = 0;
    int totalPositions = 0;
    float branchingFactor = 0.0;
    int nodos = 0;
    int prioridades[256][256];
    int cantCapturas[256];
    int puntajeCaptura[256][256];
    int ply = -1;
    int nodosBusqueda;
    double tiempoDisponible = 0;
    std::chrono::steady_clock::time_point timeStart;
    bool stopSearch;
    int premioPorPeonesPasados;

    u_short bestMove;
    float quiescence(Tablero* tablero, float alfa, float beta);
     float valoracion(Tablero* tablero);
     float valoracionMaterial(Tablero* tablero);
    U64 perft(int profundidad, Tablero* tablero);
     float valoracionMovilidad(Tablero* tablero);
     float contarMaterialSinPeones(Tablero* tablero);
     float negamax(Tablero* tablero, int profundidad, float alfa, float beta, bool esRaiz);
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
     u_short killerMove[2][128];
     bool LMR = false;

     //Tabla para chequear la regla de triple repetición
     U64 tabla_de_repeticiones[512];
     int index_repeticion = -1;
     bool esRepeticion(U64 zobristActual);


};


#endif //CHESSCPP_MOTOR_H
