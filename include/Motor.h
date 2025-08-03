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

    float maxEval2 = 0;
    // En algún lugar global o como constantes de tu clase Board/Evaluation
    const int peso_fase_caballo = 1;   // Caballo
    const int peso_fase_alfil = 1;   // Alfil
    const int peso_fase_torre   = 2;   // Torre (vale más porque es más fuerte)
    const int peso_fase_dama  = 4;   // Dama (la más valiosa en el medio juego)

// Define la Fase Máxima posible (todas las piezas al inicio)
// 2 Caballos, 2 Alfiles, 2 Torres, 1 Dama por bando. Suma de puntos por bando:
// (2*1) + (2*1) + (2*2) + (1*4) = 2 + 2 + 4 + 4 = 12
// Multiplicado por 2 bandos: 12 * 2 = 24
    const int fase_maxima = (2 * peso_fase_caballo) +
                          (2 * peso_fase_alfil) +
                          (2 * peso_fase_torre) +
                          (1 * peso_fase_dama); // Esto es por UN bando
    const int fase_maxima_total = fase_maxima * 2;
    float pesoMaterialPeonM = 1;
    float pesoMaterialPeonF = 1.3;
    float pesoMaterialDamaM = 1;
    float pesoMaterialDamaF = 0.9;
    float pesoMaterialTorreM = 1;
    float pesoMaterialTorreF = 1.1;
    int exitosNull = 0;
    float pesoOcupacionPeonM = 1.0;
    float pesoOcupacionPeonF = 2.0;


    float pesoOcupacionCaballoM = 1.0;
    float pesoOcupacionCaballoF = 0.5;


    float pesoOcupacionAlfilM = 1.0;
    float pesoOcupacionAlfilF = 0.7;


    float pesoOcupacionTorreM = 1.0;
    float pesoOcupacionTorreF = 1.2;


    float pesoOcupacionDamaM = 1.0;
    float pesoOcupacionDamaF = 0.8;

    float premioEnroqueM = 1;
    float premioEnroqueF = 0.2;

    float castigoPeonBloqueadoM = 1;
    float castigoPeonBloqueadoF = 2.5;

    float seguridadDelReyM = 1;
    float seguridadDelReyF = 0.1;

    float movilidadM = 1;
    float movilidadF = 0.8;

    bool primerMovimiento;



    Motor();
    int calcularFaseDeJuego(Tablero* tablero);
    bool tablaInicializada = false;
    int incrementos = 0;
    int porcentajeTabla();
    int misses = 0;

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
/*
     float valoracionMaterial(Tablero* tablero);
*/
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
