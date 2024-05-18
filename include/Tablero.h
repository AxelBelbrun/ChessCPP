//
// Created by axel on 08/12/23.
//
#pragma once
#include <string>
#include "Jugada.h"
#include <list>
#include <vector>
#include "constantes.h"
#include <cstdio>


enum casillas {
    h1 = 1, g1, f1, e1, d1, c1, b1, a1,
    h2 = 9, g2, f2, e2, d2, c2, b2, a2,
    h3 = 17, g3, f3, e3, d3, c3, b3, a3,
    h4 = 25, g4, f4, e4, d4, c4, b4, a4,
    h5 = 33, g5, f5, e5, d5, c5, b5, a5,
    h6 = 41, g6, f6, e6, d6, c6, b6, a6,
    h7 = 49, g7, f7, e7, d7, c7, b7, a7,
    h8 = 57, g8, f8, e8, d8, c8, b8, a8
};

enum tipoDePieza {
    REY = 0, DAMA, TORRE, ALFIL, CABALLO, PEON, VACIO
};

class Tablero {

public:
    Tablero(std::string fen);
    U64 zobristKey();
    bool ganoBlanco = false;
    bool ganoNegro = false;
    void imprimirFen();
    static U64 movimientosDeTorre[64][4096];
   static U64 movimientosDeAlfil[64][512];
   bool ganoELBlanco();
   bool ganoElNegro();
    U64 zobrist;
    void actualizarZobristKey(u_short jugada, bool haciendoMovimiento);
   U64 movimientosDePeon[64][2];
    U64 movimientosDeCaballo[64];
    U64 movimientosDeRey[64];
    int capturas = 0;
    int ep = 0;
    int check = 0;
    int comunes = 0;
    bool esJugadaLegal(u_short);
    std::vector<bool> enroqueCortoNegro;
    std::vector<bool> enroqueLargoNegro;
    std::vector<bool> enroqueCortoBlanco;
    std::vector<bool> enroqueLargoBlanco;
    int _turno;
    void cambiarTurno();
    std::vector<u_short> _jugadas;
    std::vector<U64> bitboards;
    std::vector<std::vector<U64>> historialPosiciones;
    void setTurno(int turno);
    int getTurno();
    bool moverPieza(int salida, int llegada, int tipoDeJugada);
    void deshacerMovimiento();
    U64 piezas_blancas();
    U64 piezas_negras();
    U64 todas_las_piezas();

    U64 piezasPropias();
    U64 piezasRivales();
    U64 torresRivales();
    U64 alfilesRivales();
    U64 damasRivales();
    U64 reyPropio(int color);
    bool perdidaEnroqueCortoBlancas = false;
    bool perdidaEnroqueCortoNegras = false;
    bool perdidaEnroqueLargoBlancas = false;
    bool perdidaEnroqueLargoNegras = false;
    bool movimientoTempranoDamaNegra = false;
    bool movimientoTempranoDamaBlanca = false;
    bool enrocoBlancas = false;
    bool enrocoNegras = false;
    void imprimirJugadas();
    bool seRecapturo();
    bool desarrolloCaballoBlancoB();
    bool desarrolloCaballoBlancoG();
    bool desarrolloCaballoNegroB();
    bool desarrolloCaballoNegroG();
    bool desarrolloAlfilBlancoC();
    bool desarrolloAlfilBlancoF();
    bool desarrolloAlfilNegroC();
    bool desarrolloAlfilNegroF();
    bool desarrolloBlancasCompleto();
    bool desarrolloNegrasCompleto();
    bool endgame = false;
    void configurarFen(std::string fen);
    float valoracionMaterial(int color);
    float contarMaterialSinPeones(int color);
    std::vector<float> historialMaterialBlancas;
    std::vector<float> historialMaterialNegras;
    std::vector<float> historialOcupacionBlancas;
    std::vector<float> historialOcupacionNegras;
    std::vector<U64> historialDePosiciones;
    bool esTripleRepeticion();
    std::pair<int,int> movimientosCaballoBlancoB;
    std::pair<int,int> movimientosCaballoBlancoG;
    std::pair<int,int> movimientosCaballoNegroB;
    std::pair<int,int> movimientosCaballoNegroG;
    std::pair<int,int> movimientosAlfilBlancoC;
    std::pair<int,int> movimientosAlfilBlancoF;
    std::pair<int,int> movimientosAlfilNegroC;
    std::pair<int,int> movimientosAlfilNegroF;
    int quienMovioPrimero;
    
/*    float controlCentroBlancas = 0;
    float controlCentroNegras = 0;*/
  /*  float controlDeCentroCaballo(int color);
    float controlDeCentroPeon(int color);
    float controlDeCentroAlfil(int color);
    float controlDeCentro(int color);*/
    float calcularOcupacion( int color);
    float calcularOcupacionPeon(int color);
    float calcularOcupacionCaballo(int color);
    float calcularOcupacionAlfil( int color);
    float calcularOcupacionTorre( int color);
    float calcularOcupacionReina( int color);
    float calcularOcupacionRey( int color);
    void actualizarOcupacion(u_short jugada);
    int cantPiezas(int color);
/*
    void actualizarControlCentro();
*/
    void actualizarMaterial(u_short jugada);
    void actualizarCantMovesPiezasMenores(u_short jugada, bool haciendoMovimiento);
    void imprimirTablero();
    bool esUnaPromocion(u_short jugada);
    u_short reescribirPromocion(u_short jugada);
    bool casillaAtacada(int casilla, int turno);
    bool reyPropioEnJaque(int color);
    std::vector<u_short> generar_movimientos(int turno);
    std::vector<u_short> generar_capturas(int turno);
    void promocionar(u_short jugada);
    bool esJaque(U64 jugada, int turno);
    U64 estaClavada(int casillaDePieza);
    int obtenerTipoDePieza(int casillaDePieza);
    U64 generarMovimientosPiezaClavada(int casillaPiezaClavada, int casillaPiezaClavadora);
    bool esSlidingPiece(int tipoDePieza);
    U64 obtenerAttackMask(int casillaDePieza, int tipoDePieza);
    U64 obtenerAttackMap(int color);
    U64 obtenerAttackMapBlancas();
    U64 obtenerAttackMapNegras();
    bool chequearEnroqueCorto();
    bool chequearEnroqueLargo();
    bool enrocar(u_short jugada);
    void moverPiezaTrusted(int salida, int llegada, int tipoDeJugada);




private:


    void obtener_movimientos_torre_blanca(std::vector <u_short> &movimientos, U64 bitboard);

    void obtener_movimientos_alfil_blanco(std::vector <u_short> &movimientos, U64 bitboard);

    void obtener_movimientos_dama_blanca(std::vector<u_short> &movimientos, U64 bitboard);

    std::vector <u_short> &obtener_movimientos_caballo_blanco(std::vector<u_short> &movimientos);

    std::vector <u_short> &obtener_movimientos_rey_blanco(std::vector<u_short> &movimientos);

    std::vector <u_short> &obtener_movimientos_peon_blanco(std::vector<u_short> &movimientos);

    void obtener_movimientos_torre_negra(std::vector<u_short> &movimientos, U64 bitboard);

    void obtener_movimientos_alfil_negro(std::vector<u_short> &movimientos, U64 bitboard);

    void obtener_movimientos_dama_negra(std::vector<u_short> &movimientos, U64 bitboard);

    std::vector <u_short> &obtener_movimientos_caballo_negro(std::vector<u_short> &movimientos);

    std::vector<u_short> &obtener_movimientos_rey_negro(std::vector<u_short> &movimientos);

    std::vector<u_short> &obtener_movimientos_peon_negro(std::vector<u_short> &movimientos);
};

