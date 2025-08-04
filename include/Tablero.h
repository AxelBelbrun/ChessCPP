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
#include "Pieza.h"
#include <stack>

class Caballo;
class Rey;
class Peon;
class Torre;
class Alfil;
class Dama;

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
/*   bool ganoELBlanco();
   bool ganoElNegro();*/
    U64 zobrist;
    void actualizarZobristKey(u_short jugada);
    U64 historialZobrist[512];
    int contadorZobrist = -1;
    bool esUnJaque(u_short movimiento);
    U64 movimientosDeCaballo[64];
    U64 movimientosDeRey[64];
    U64 masksPeonesPasados[80];
    void calcularMasksPeonesPasados();
    int capturas = 0;
    int ep = 0;
    int check = 0;
    int comunes = 0;
    bool esCaptura(u_short jugada);
    std::string formatearJugada(u_short jugada);
    std:: string generarJugadaString(int salida, int llegada, int llegadaPromocion);
    struct derechosDeEnroque {
        bitmask derechosActuales;
        bitmask derechosAnteriores;
        int numeroDeMovimiento;
    };
    struct cambiosOcupacion {
        cambiosOcupacion() {
            cambio1 = std::make_pair(-1, -1);
            cambio2 = std::make_pair(-1, -1);
            cambio3 = std::make_pair(-1, -1);
        }
        std::pair<int, int> cambio1 = std::make_pair(-1, -1);
        std::pair<int, int> cambio2 = std::make_pair(-1, -1);
        std::pair<int, int> cambio3 = std::make_pair(-1, -1);
    };

    int detectarPeonesPasados(int turno);
    int numeroDeJugadas;
    std::stack<derechosDeEnroque> enroques;
    void setearEnroque(int tipoDeEnroque, bool hayDerecho);
    void guardarEnroque();
    void deshacerEnroque();
    bitmask derechosEnroqueAux;
    bool enroqueCortoBlancas();
    bool enroqueLargoBlancas();
    bool enroqueCortoNegras();
    bool enroqueLargoNegras();
    int _turno;
    void cambiarTurno();
/*
    std::vector<u_short> _jugadas;
*/
    u_short jugadas[512];
    int contadorJugadas = -1;
    std::vector<U64> bitboards;
/*
    std::vector<std::vector<U64>> historialPosiciones;
*/
    struct modificacionBitboard{
        modificacionBitboard(){
            tipoDeBitboard_1 = -1;
            bitboard_1 = 0;
            tipoDeBitboard_2 = -1;
            bitboard_2 = 0;
            tipoDeBitboard_3 = -1;
            bitboard_3 = 0;
        }
        modificacionBitboard(int tipo1, U64 bitboard1, int tipo2, U64 bitboard2,
                             int tipo3, U64 bitboard3){
            tipoDeBitboard_1 = tipo1;
            bitboard_1 = bitboard1;
            tipoDeBitboard_2 = tipo2;
            bitboard_2 = bitboard2;
            tipoDeBitboard_3 = tipo3;
            bitboard_3 = bitboard3;
        }
        int tipoDeBitboard_1;
        U64 bitboard_1;
        int tipoDeBitboard_2;
        U64 bitboard_2;
        int tipoDeBitboard_3;
        U64 bitboard_3;
    };
    modificacionBitboard historialBitboards[512];
    int contadorHistorialBitboards = -1;
    int caballosNegros = 0;
    int caballosBlancos = 0;
    int damasNegras = 0;
    int damasBlancas = 0;
    int peonesNegros = 0;
    int peonesBlancos = 0;
    int alfilesNegros = 0;
    int alfilesBlancos = 0;
    int torresNegras = 0;
    int torresBlancas = 0;

    //Los indices 12 y 13 son para la ocupacion del rey blanco y negro en final respectivamente
    int ocupacionPiezas[14];
    cambiosOcupacion historialCambiosOcupacion[512];


    void actualizarCantPiezas(int tipoDePieza, bool agregando);
    void setTurno(int turno);
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
    std::vector<int> historialEnPassant;
    /*std::vector<float> historialMaterialBlancas;
    std::vector<float> historialMaterialNegras;*/
    void inicializarContadoresDeMovimientos();
    bool agregueAlgoAlHistorialEP = false;
/*    std::pair<int,int> historial_material_negras[50];
    std::pair<int,int> historial_material_blancas[50];
    int modif_hist_material_negras = 0;
    int modif_hist_material_blancas = 0;
    int contadorMaterialNegras = -1;
    int contadorMaterialBlancas = -1;*/



    /*std::vector<float> historialOcupacionBlancas;
    std::vector<float> historialOcupacionNegras;*/
/*    float historial_ocupacion_negras[512];
    float historial_ocupacion_blancas[512];*/
/*
    std::vector<U64> historialDePosiciones;
*/
    bool esTripleRepeticion();
    void movimientoNulo();
    void deshacerMovimientoNulo();
    std::pair<int,int> movimientosCaballoBlancoB;
    std::pair<int,int> movimientosCaballoBlancoG;
    std::pair<int,int> movimientosCaballoNegroB;
    std::pair<int,int> movimientosCaballoNegroG;
    std::pair<int,int> movimientosAlfilBlancoC;
    std::pair<int,int> movimientosAlfilBlancoF;
    std::pair<int,int> movimientosAlfilNegroC;
    std::pair<int,int> movimientosAlfilNegroF;
    void calcularOcupacion();
    void calcularOcupacionPeon(int color);
    void calcularOcupacionCaballo(int color);
    void calcularOcupacionAlfil( int color);
    void calcularOcupacionTorre( int color);
    void calcularOcupacionReina( int color);
    void calcularOcupacionRey( int color);
    void actualizarOcupacion(u_short jugada);
    int cantPiezas(int color);
/*
    void actualizarMaterial(u_short jugada);
*/
    void actualizarCantMovesPiezasMenores(u_short jugada, bool haciendoMovimiento);
    void imprimirTablero();
    bool esUnaPromocion(u_short jugada);
    u_short reescribirPromocion(u_short jugada);
/*
    bool casillaAtacada(int casilla, int turno);
*/
    bool reyPropioEnJaque(int color);
    void generar_movimientos(int turno, int ply);
    void generar_capturas(int turno, int ply);
    void promocionar(u_short jugada);
    bool esJaque(U64 jugada, int turno);
    U64 estaClavada(int casillaDePieza);
    int obtenerTipoDePieza(int casillaDePieza);
    U64 generarMovimientosPiezaClavada(int casillaPiezaClavada, int casillaPiezaClavadora);
    bool esSlidingPiece(int tipoDePieza);
    U64 obtenerAttackMask(int casillaDePieza, int tipoDePieza);
    U64 obtenerAttackMap(int color);
    U64 attackMapBlancas;
    U64 attackMapNegras;
    void actualizarAttackMap();
    U64 obtenerAttackMapBlancas();
    U64 obtenerAttackMapNegras();
    bool chequearEnroqueCorto();
    bool chequearEnroqueLargo();
    bool enrocar(u_short jugada);
    void moverPiezaTrusted(int salida, int llegada, int tipoDeJugada);
    void enrocarTrusted(u_short jugada);
    Caballo* caballo;
    Rey* rey;
    Peon* peon;
    Torre* torre;
    Alfil* alfil;
    Dama* dama;






    void obtener_movimientos_torre_blanca(int ply, U64 bitboard);

    void obtener_movimientos_alfil_blanco(int ply, U64 bitboard);

    void obtener_movimientos_dama_blanca(int ply, U64 bitboard);

    void obtener_movimientos_caballo_blanco(int ply);

    void obtener_movimientos_rey_blanco(int ply);

    void obtener_movimientos_peon_blanco(int ply);

    void obtener_movimientos_torre_negra(int ply, U64 bitboard);
    U64 bitboard_movimientos_dama_negra( U64 bitboard);
    U64 bitboard_movimientos_torre_negra( U64 bitboard);
    U64 bitboard_movimientos_rey_negro( U64 bitboard);
    U64 bitboard_movimientos_rey_blanco( U64 bitboard);
    void obtener_movimientos_alfil_negro(int ply, U64 bitboard);

    void obtener_movimientos_dama_negra(int ply, U64 bitboard);

    void obtener_movimientos_caballo_negro(int ply);

    void obtener_movimientos_rey_negro(int ply);

    void obtener_movimientos_peon_negro(int ply);

    U64 bitboard_movimientos_alfil_negro(U64 bitboard);

    U64 bitboard_movimientos_caballo_negro(U64 bitboard) const;

    U64 bitboard_movimientos_peon_negro(U64 bitboard);

    U64 bitboard_movimientos_dama_blanca(U64 bitboard);

    U64 bitboard_movimientos_torre_blanca(U64 bitboard);

    U64 bitboard_movimientos_alfil_blanco(U64 bitboard);

    U64 bitboard_movimientos_caballo_blanco(U64 bitboard) const;

    U64 bitboard_movimientos_peon_blanco(U64 bitboard);

    void setearPosicionInicial(const string &posicionASetear);

    void setearPosicionDeFen(const string &posicionASetear);

    void generarTablasSliding(int i) ;

    void generarTablasNoSliding(int i);

    void generarTablasDeMovimientos();

    void inicializarBitboardsVacios();


    void deshacerUltimosRegistros();

    u_short movimientos_generados[256][256];

    int cantMovesGenerados[256];

    int contador_movilidad(int turno);

};

