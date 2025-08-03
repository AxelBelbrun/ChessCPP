#include <iostream>
#include "Tablero.h"
#include "Rey.h"
#include "Torre.h"
#include "Alfil.h"
#include "Caballo.h"
#include "Dama.h"
#include "Peon.h"
#include <algorithm>
#include "calculadoraMovesAlfil.h"
#include "calculadoraMovesTorre.h"
#include <string>
#include <x86intrin.h>  // Para intrinsics específicas de la CPU

using namespace std;

int PEON_BLANCO = PEON;
int PEON_NEGRO = PEON + 6;
int REY_BLANCO = REY;
int REY_NEGRO = REY + 6;
int TORRE_BLANCA = TORRE;
int TORRE_NEGRA = TORRE + 6;
int DAMA_BLANCA = DAMA;
int DAMA_NEGRA = DAMA + 6;
int ALFIL_BLANCO = ALFIL;
int ALFIL_NEGRO = ALFIL + 6;
int CABALLO_BLANCO = CABALLO;
int CABALLO_NEGRO = CABALLO + 6;

Tablero::Tablero(std::string posicionASetear) {
    generarTablasDeMovimientos();
    zobrist = 0;
    inicializarContadoresDeMovimientos();
    calcularMasksPeonesPasados();
    contadorZobrist = -1;





    //Se asigna la zobrist key inicial según la info del tablero.
    //Si la posicion a setear es la inicial del ajedrez
    if (posicionASetear.std::string::find("startpos") != std::string::npos) {
        setearPosicionInicial(posicionASetear);
    }

        //Si nos pasan un fen con una determinada posición
    else {
        setearPosicionDeFen(posicionASetear);
    }

    numeroDeJugadas = 0;

    // Inicializamos la zobrist key en 0, lo que denota que aún no
    // está seteada.



}

void
Tablero::generarTablasDeMovimientos() {//Se generan las tablas de movimientos posibles para cada pieza en cada casilla
    for (int i = 0; i < 64; i++) {
        generarTablasSliding(i);
        generarTablasNoSliding(i);
    }
}

void Tablero::inicializarContadoresDeMovimientos() {
    for (int i = 0; i < 256; i++) {
        cantMovesGenerados[i] = -1;
    }
}

//Genera el bitboard de movimientos posibles para cada pieza no sliding en la casilla i
void Tablero::generarTablasNoSliding(
        int i) {
    //Creamos objetos de cada pieza faltante para poder generar los movimientos legales,
// y no los eliminamos porque serán de utilidad durante todo el programa.

    caballo = new Caballo();
    rey = new Rey();
    peon = new Peon();
    dama = new Dama();
    U64 bitboardCaballo = operaciones_bit::setBit(0L, i + 1, 1);
    U64 movimientosCaballo = caballo->generar_movimientos_legales(bitboardCaballo, 0, 0, 0);
    movimientosDeCaballo[i] = movimientosCaballo;
    U64 bitboardRey = operaciones_bit::setBit(0L, i + 1, 1);
    U64 movimientosRey = rey->generar_movimientos_legales(bitboardRey, 0, 0, 0);
    movimientosDeRey[i] = movimientosRey;
    U64 bitboardPeon = operaciones_bit::setBit(0L, i + 1, 1);
}

//Genera el bitboard de movimientos posibles para cada pieza sliding en la casilla i
void Tablero::generarTablasSliding(int i) {
    vector<U64> configuracionesPosiblesTorre = calculadoraMovesTorre::masksPiezasBloqueando(
            constantes::attackMasksTorre[i]);
    vector<U64> configuracionesPosiblesAlfil = calculadoraMovesAlfil::masksPiezasBloqueando(
            constantes::attackMasksAlfil[i]);
    U64 bitboardTorre = operaciones_bit::setBit(0L, i + 1, 1);
    U64 bitboardAlfil = operaciones_bit::setBit(0L, i + 1, 1);
    torre = new Torre();
    alfil = new Alfil();
    for (int j = 0; j < configuracionesPosiblesTorre.size(); j++) {
        U64 movimientosParaConfig = torre->generar_movimientos_legales(bitboardTorre, 0,
                                                                       configuracionesPosiblesTorre[j], 0);
        movimientosDeTorre[i][(configuracionesPosiblesTorre[j] * constantes::magicsParaTorre[i])
                >> (64 - 12)] = movimientosParaConfig;

    }
    for (int k = 0; k < configuracionesPosiblesAlfil.size(); k++) {
        U64 movimientosParaConfig = alfil->generar_movimientos_legales(bitboardAlfil, 0,
                                                                       configuracionesPosiblesAlfil[k], 0);
        movimientosDeAlfil[i][(configuracionesPosiblesAlfil[k] * constantes::magicsParaAlfil[i])
                >> (64 - 9)] = movimientosParaConfig;

    }

}

void Tablero::setearPosicionDeFen(const string &posicionASetear) {//Inicializar bitboards en 0
    inicializarBitboardsVacios();

    //Se inician los enroques en false, luego con la info del fen se sabe cuáles setear en true

    //Tomamos la parte que nos interesa del string, eliminando "position fen "

    string fen = posicionASetear.substr(13, posicionASetear.size() - 13);
    enroques = stack<derechosDeEnroque>();
    bitmask enroquesIniciales = 0;

    //Se van insertando las piezas y asignando valores de variables según el fen
    int casillaActual = 64; //Se arranca desde la casilla a8 porque así funciona la notacion FEN
    for (int i = 0; i < fen.size(); i++) {

        //Si "casillaActual" es > 0 entonces todavía estamos en la parte del FEN que contiene info
        //sobre dónde estás las piezas en el tablero. Según la letra, asignamos la pieza a su bitboard.
        if (casillaActual > 0) {
            switch (fen[i]) {
                case 'K':
                    bitboards[0] = operaciones_bit::setBit(bitboards[0], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'Q':
                    bitboards[1] = operaciones_bit::setBit(bitboards[1], casillaActual, 1);
                    damasBlancas++;
                    casillaActual--;
                    break;

                case 'R':
                    bitboards[2] = operaciones_bit::setBit(bitboards[2], casillaActual, 1);
                    casillaActual--;
                    torresBlancas++;
                    break;

                case 'B':
                    bitboards[3] = operaciones_bit::setBit(bitboards[3], casillaActual, 1);
                    casillaActual--;
                    alfilesBlancos++;
                    break;

                case 'N':
                    bitboards[4] = operaciones_bit::setBit(bitboards[4], casillaActual, 1);
                    casillaActual--;
                    caballosBlancos++;
                    break;

                case 'P':
                    bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaActual, 1);
                    casillaActual--;
                    peonesBlancos++;
                    break;


                case 'k':
                    bitboards[6] = operaciones_bit::setBit(bitboards[6], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'q':
                    bitboards[7] = operaciones_bit::setBit(bitboards[7], casillaActual, 1);
                    casillaActual--;
                    damasNegras++;
                    break;

                case 'r':
                    bitboards[8] = operaciones_bit::setBit(bitboards[8], casillaActual, 1);
                    casillaActual--;
                    torresNegras++;
                    break;

                case 'b':
                    bitboards[9] = operaciones_bit::setBit(bitboards[9], casillaActual, 1);
                    casillaActual--;
                    alfilesNegros++;
                    break;

                case 'n':
                    bitboards[10] = operaciones_bit::setBit(bitboards[10], casillaActual, 1);
                    casillaActual--;
                    caballosNegros++;
                    break;

                case 'p':
                    bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaActual, 1);
                    casillaActual--;
                    peonesNegros++;
                    break;

                case '/':
                    continue;

                default:
                    casillaActual -= fen[i] - '0';
            }
        } else if (fen[i] == ' ') {
            continue;

            //Si encontramos w es porque el turno es de las blancas.
        } else if (fen[i] == 'w') {
            _turno = 0;

            //K, Q, k y q indican los derechos a enroncar de blancas y negras.
        } else if (fen[i] == 'K') {
            enroquesIniciales = enroquesIniciales | 0b1;
        } else if (fen[i] == 'Q') {
            enroquesIniciales = enroquesIniciales | 0b10;
        } else if (fen[i] == 'k') {
            enroquesIniciales = enroquesIniciales | 0b100;
        } else if (fen[i] == 'q') {
            enroquesIniciales = enroquesIniciales | 0b1000;

            //Si llegamos a esta parte se está hablando de un posible casilla de enpassant o de la cantidad
            // de medio-movimientos
        } else if (fen[i] >= 97 && fen[i] < 105) {
            //Si encontramos una "b" puede ser una casilla de enpassant de la columna b (b3 x ej)
            // o bien indicar que le toca a las negras.
            if (fen[i] == 'b' && ((fen[i + 1] == ' ') || (i+1 == fen.size()))) {
                _turno = 1;
            } else {
                string jugadaString{fen[i], fen[i + 1]};
                int casilla = constantes::casillaANumero[jugadaString];
                int salida, llegada, tipoDeJugada;
                if (fen[i + 1] - '0' == 3) {
                    salida = casilla - 8;
                    llegada = casilla + 8;
                    tipoDeJugada = QUIET;
                } else {
                    salida = casilla + 8;
                    llegada = casilla - 8;
                    tipoDeJugada = QUIET;
                };

                //Agregamos el avance de peón al historial para que la detección de enpassant
                //funcione correctamente.
                u_short jugada = operaciones_bit::crearJugada(salida, llegada, tipoDeJugada);
/*
                _jugadas.push_back(jugada);
*/
                contadorJugadas++;
                jugadas[contadorJugadas] = jugada;
                int filaDeEnPassant = (llegada % 8) - 1;
                if (filaDeEnPassant == -1) {
                    filaDeEnPassant = 7;
                }
                historialEnPassant.push_back(filaDeEnPassant);

            }
        }
    }

    //Inicializamos las variables:
    enroques.push(derechosDeEnroque{enroquesIniciales, 0, -5});
    zobrist = zobristKey();
    contadorZobrist++;
    historialZobrist[contadorZobrist] = zobrist;

    //Se calcula el valor del material y se agrega al historial
/*    contadorMaterialBlancas = 0;
    historial_material_blancas[contadorMaterialBlancas] = std::make_pair(valoracionMaterial(0), 0);
    contadorMaterialNegras = 0;
    historial_material_negras[contadorMaterialNegras] = std::make_pair(valoracionMaterial(1), 0);*/

    calcularOcupacion();

    if (historialEnPassant.empty()) {
        historialEnPassant.push_back(-1);
    }

}



void Tablero::inicializarBitboardsVacios() {
    for (int k = 0; k < 12; k++) {
        bitboards.push_back(0ULL);
    }
}

void Tablero::setearPosicionInicial(const string &posicionASetear) {
    _turno = 0;
    enroques = stack<derechosDeEnroque>();
    enroques.push(derechosDeEnroque{0b1111, 0, -5});

    //bitboards es un vector que almacena los 12 bitboards que representan las historialPosiciones de
//cada tipo de pieza en el tablero. El orden es el siguiente: rey, dama, torre, alfil, caballo, peon.
// De 0 a 5 son los bitboards de blancas, de 6 a 11 los de negras.

    //Se van insertando los bitboards correspondientes en orden y con las piezas posicionadas
// según la posición inicial del ajedrez.
    bitboards.push_back(1L << 3);
    bitboards.push_back(1L << 4);
    bitboards.push_back(1L | 1L << 7);
    bitboards.push_back(1L << 2 | 1L << 5);
    bitboards.push_back(1L << 1 | 1L << 6);

    U64 peonesBlancosAlInicio = 0L;
    for (int i = 8; i < 16; i++) {
        peonesBlancosAlInicio = peonesBlancosAlInicio | 1L << i;
    }
    bitboards.push_back(peonesBlancosAlInicio);
    bitboards.push_back(1L << 59);
    bitboards.push_back(1L << 60);
    bitboards.push_back(1L << 63 | 1L << 56);
    bitboards.push_back(1L << 61 | 1L << 58);
    bitboards.push_back(1L << 62 | 1L << 57);
    U64 peonesNegrosAlInicio = 0L;
    for (int i = 48; i < 56; i++) {
        peonesNegrosAlInicio = peonesNegrosAlInicio | 1L << i;
    }
    bitboards.push_back(peonesNegrosAlInicio);

    torresNegras = 2;
    torresBlancas = 2;
    alfilesNegros = 2;
    alfilesBlancos = 2;
    caballosNegros = 2;
    caballosBlancos = 2;
    damasNegras = 1;
    damasBlancas = 1;
    peonesNegros = 8;
    peonesBlancos = 8;

    //El valor del material desde la posición inicial del ajedrez es siempre el mismo,
    //asi que en vez de "calcularlo" directamente lo asignamos
/*    contadorMaterialBlancas = 0;
    historial_material_blancas[contadorMaterialBlancas] = std::make_pair(3952, 0);
    contadorMaterialNegras = 0;
    historial_material_negras[contadorMaterialNegras] = std::make_pair(3952, 0);*/

    //El valor de la ocupación inicial se calcula, aunque siempre sea el mismo:
    /*historialOcupacionBlancas.push_back(valoracionMaterial(0));
    historialOcupacionNegras.push_back(valoracionMaterial(1));*/
    calcularOcupacion();
    historialEnPassant.push_back(-1);
    zobrist = zobristKey();
    contadorZobrist++;
    historialZobrist[contadorZobrist] = zobrist;

    //Si estamos construyendo un tablero y el string contiene la palabra "moves"
// significa que nos pasaron jugadas para llegar a una determinada posición, entonces
// las parseamos y luego las hacemos.
    if (posicionASetear.std::string::find("moves") != std::string::npos) {
        string jugadas = posicionASetear.substr(posicionASetear.std::string::find("moves") + 6,
                                                posicionASetear.size() - 1);
        int i = 0;
        while (i < jugadas.size()) {
            string jugadaString = jugadas.substr(i, 4);
            generar_movimientos(_turno, 0);
            for (int j = 0; j <= cantMovesGenerados[0]; j++) {
                u_short move = movimientos_generados[0][j];
                if (jugadaString == formatearJugada(move)) {
                    moverPieza(operaciones_bit::getSalida(move), operaciones_bit::getLlegada(move),
                               operaciones_bit::getTipoDeJugada(move));
                    break;
                }
            }
            cantMovesGenerados[0] = -1;
            contadorJugadas++;
            i += 5;
        }

    }
}


//Inicialiazamos las lookup tables.
U64 Tablero::movimientosDeTorre[64][4096];
U64 Tablero::movimientosDeAlfil[64][512];


U64 Tablero::piezas_blancas() {
    U64 piezas_blancas = 0L;
    for (int i = 0; i < 6; i++) {
        piezas_blancas = piezas_blancas | bitboards[i];
    }
    return piezas_blancas;
}

U64 Tablero::piezas_negras() {
    U64 piezas_negras = 0L;
    for (int i = 6; i < 12; i++) {
        piezas_negras = piezas_negras | bitboards[i];
    }
    return piezas_negras;
}

U64 Tablero::todas_las_piezas() {
    return piezas_blancas() | piezas_negras();
}


//Devuelve un bitboard que contiene a todas las piezas propias según el turno actual
U64 Tablero::piezasPropias() {
    if (_turno == 0) {
        return piezas_blancas();
    } else {
        return piezas_negras();
    }
}

//Devuelve un bitboard que contiene a todas las piezas rivales según el turno actual
U64 Tablero::piezasRivales() {
    if (_turno == 0) {
        return piezas_negras();
    } else {
        return piezas_blancas();
    }
};


void Tablero::imprimirTablero() {
    cout << "  +---+---+---+---+---+---+---+---+" << endl;
    for (int i = 8; i > 0; i--) {
        cout << 0 + i << " | ";
        for (int j = 0; j < 8; j++) {
            int casilla = 8 * i - j;
            bool hayPieza = false;
            for (int k = 0; k < 12; k++) {
                U64 bitboard = bitboards[k];
                U64 casillaBit = operaciones_bit::setBit(0L, casilla, 1);
                if ((bitboard & casillaBit) > 0) {
                    hayPieza = true;
                    if (k == 0) {
                        cout << "K";
                    } else if (k == 1) {
                        cout << "Q";
                    } else if (k == 2) {
                        cout << "R";
                    } else if (k == 3) {
                        cout << "B";
                    } else if (k == 4) {
                        cout << "N";
                    } else if (k == 5) {
                        cout << "P";
                    } else if (k == 6) {
                        cout << "k";
                    } else if (k == 7) {
                        cout << "q";
                    } else if (k == 8) {
                        cout << "r";
                    } else if (k == 9) {
                        cout << "b";
                    } else if (k == 10) {
                        cout << "n";
                    } else if (k == 11) {
                        cout << "p";
                    }
                }
            }
            if (!hayPieza) {
                cout << " ";
            }
            cout << " | ";
        }
        cout << endl;
        cout << "  +---+---+---+---+---+---+---+---+" << endl;
    }
    cout << "    a   b   c   d   e   f   g   h" << endl;
    cout << "\n";
    cout << "\n";
    cout << "\n";
    cout << "\n";
}

void Tablero::deshacerMovimiento() {

    //El orden de las siguientes operaciones es importante, ya que "actualizarZobristKey" asume que
    // los bitboards aún no fueron modificados con la posicioón que se quiere deshacer.

    u_short jugada = jugadas[contadorJugadas];
    cambiosOcupacion aDeshacer = historialCambiosOcupacion[contadorJugadas];
    int indiceCambio1 = aDeshacer.cambio1.first;
    int indiceCambio2 = aDeshacer.cambio2.first;
    int indiceCambio3 = aDeshacer.cambio3.first;

    if(indiceCambio1 != -1){
        ocupacionPiezas[indiceCambio1] -= aDeshacer.cambio1.second;
    }
    if(indiceCambio2 != -1){
        ocupacionPiezas[indiceCambio2] -= aDeshacer.cambio2.second;
    }
    if(indiceCambio3 != -1){
        ocupacionPiezas[indiceCambio3] -= aDeshacer.cambio3.second;
    }
    historialCambiosOcupacion[contadorJugadas] = cambiosOcupacion();
    derechosEnroqueAux = 0;
    contadorZobrist--;
    zobrist = historialZobrist[contadorZobrist];
    deshacerEnroque();
    deshacerUltimosRegistros();

    numeroDeJugadas--;


    /// Momentánemente la función de evaluación no utiliza la cantidad
    /// de movimientos realizados por piezas menores, por lo que no vale la pena
    /// realizar el cálculo.
    actualizarCantMovesPiezasMenores(jugada, false);
    //En caso de que la jugada a deshacer sea un enroque, hay que volver a setear en "false"
    // la variable que representa si ese bando enrrocó.
    if (_turno == 0 && operaciones_bit::getTipoDeJugada(jugada) == CASTLING) {
        enrocoNegras = false;
    } else if (_turno == 1 && operaciones_bit::getTipoDeJugada(jugada) == CASTLING) {
        enrocoBlancas = false;
    }


    //Actualizamos los bitboards
   modificacionBitboard cambios =
           historialBitboards[contadorHistorialBitboards];
   int indexPrimerCambio = cambios.tipoDeBitboard_1;
   U64 bitboardPrimerCambio = cambios.bitboard_1;
   bitboards[indexPrimerCambio] = bitboardPrimerCambio;
   if(cambios.tipoDeBitboard_2 != -1) {
       int indexSegundoCambio = cambios.tipoDeBitboard_2;
       if(operaciones_bit::getTipoDeJugada(jugada) != CASTLING){
           if((operaciones_bit::getTipoDeJugada(jugada) != PROMOTION) &&
                   (operaciones_bit::getTipoDeJugada(jugada) != PROMOTIONCHECK)) {
               actualizarCantPiezas(indexSegundoCambio, true);
           }
           else{
               actualizarCantPiezas(indexSegundoCambio, false);
               actualizarCantPiezas(indexPrimerCambio, true);

           }
       }
       U64 bitboardSegundoCambio = cambios.bitboard_2;
       bitboards[indexSegundoCambio] = bitboardSegundoCambio;


   }
    if(cambios.tipoDeBitboard_3 != -1) {
        int indexSegundoCambio = cambios.tipoDeBitboard_2;

        actualizarCantPiezas(indexSegundoCambio, false);
        actualizarCantPiezas(indexSegundoCambio, false);

        int indexTercerCambio = cambios.tipoDeBitboard_3;
        U64 bitboardTercerCambio = cambios.bitboard_3;
        bitboards[indexTercerCambio] = bitboardTercerCambio;
        actualizarCantPiezas(indexTercerCambio, true);
        actualizarCantPiezas(indexPrimerCambio, true);


    }
    contadorHistorialBitboards--;


    cambiarTurno();


}

void Tablero::deshacerUltimosRegistros() {//Eliminamos el último elemento de todos los registros que tenemos.

    contadorJugadas--;
    ganoNegro = false;
    ganoBlanco = false;
    historialEnPassant.pop_back();

/*    if(modif_hist_material_blancas == numeroDeJugadas){
        contadorMaterialBlancas--;
        modif_hist_material_blancas = historial_material_blancas[contadorMaterialBlancas].second;
    }
    if(modif_hist_material_negras == numeroDeJugadas){
        contadorMaterialNegras--;
        modif_hist_material_negras = historial_material_negras[contadorMaterialNegras].second;
    }*/
};

bool Tablero::moverPieza(int salida, int llegada, int tipo_jugada) {
    u_short aRealizar = operaciones_bit::crearJugada(salida, llegada, tipo_jugada);
    bool actualiceEnPassant = false;
    derechosEnroqueAux = enroques.top().derechosActuales;
    numeroDeJugadas++;
    if (tipo_jugada == CASTLING) {
        if (enrocar(aRealizar)) {

            return true;
        } else {
            return false;
        }
    } else if (tipo_jugada == ENPASSANT) {

/*
        actualizarMaterial(aRealizar);
*/
        contadorJugadas++;

        actualizarOcupacion(aRealizar);
        actualizarZobristKey(aRealizar);
        contadorHistorialBitboards++;


        if (_turno == 0) {
            peonesNegros--;
            modificacionBitboard cambios = {PEON_BLANCO, bitboards[PEON_BLANCO],
                                            PEON_NEGRO, bitboards[PEON_NEGRO], -1, 0};
            historialBitboards[contadorHistorialBitboards] = cambios;

            bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], llegada, 1);
            bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], salida, 0);
            bitboards[PEON_NEGRO] = operaciones_bit::setBit(bitboards[PEON_NEGRO], llegada - 8, 0);
        } else {
            peonesBlancos--;
            modificacionBitboard cambios = {PEON_NEGRO, bitboards[PEON_NEGRO],
                                            PEON_BLANCO, bitboards[PEON_BLANCO], -1, 0};
            historialBitboards[contadorHistorialBitboards] = cambios;
            bitboards[PEON_NEGRO] = operaciones_bit::setBit(bitboards[PEON_NEGRO], llegada, 1);
            bitboards[PEON_NEGRO] = operaciones_bit::setBit(bitboards[PEON_NEGRO], salida, 0);
            bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], llegada + 8, 0);
        }
        jugadas[contadorJugadas] = aRealizar;
        historialEnPassant.push_back(-1);
        if (reyPropioEnJaque(_turno)) {
            cambiarTurno();
            deshacerMovimiento();
            return false;
        }

        //Chequeamos si este movimiento da jaque al rival
        if (reyPropioEnJaque(1 - _turno)) {
            tipo_jugada = ENPASSANTCHECK;
        }


        cambiarTurno();
;

       /* U64 zobristActual = zobristKey();
        U64 nuevaZobristKey = zobrist;
 if(nuevaZobristKey != zobristActual){
            cout << "Zobrist erronea" << endl;
            imprimirFen();
            cout << zobristActual << "-----" << nuevaZobristKey << endl;
            for(auto x: jugadas) {
                cout << x << endl;
            } exit(0);
        }*/
        ep++;

        return true;

    } else if (tipo_jugada == PROMOTION) {
        ;


/*
        actualizarMaterial(aRealizar);
*/
        contadorJugadas++;

        actualizarOcupacion(aRealizar);
        actualizarZobristKey(aRealizar);

        contadorHistorialBitboards++;
        int cambio1;
        int cambio2;
        U64 bitboard1;
        U64 bitboard2;

        if (_turno == 0) {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) - 8;
            cambio1 = PEON_BLANCO;
            peonesBlancos--;
            bitboard1 = bitboards[PEON_BLANCO];
            if (tipoDePieza(salida) == DAMA) {
                damasBlancas++;
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 1;
                bitboard2 = bitboards[1];
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                torresBlancas++;
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 2;
                bitboard2 = bitboards[2];
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                alfilesBlancos++;
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 3;
                bitboard2 = bitboards[3];
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                caballosBlancos++;
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 4;
                bitboard2 = bitboards[4];
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 1);
            }

        } else {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) + 8;
            peonesNegros--;
            cambio1 = 11;
            bitboard1 = bitboards[11];
            if (tipoDePieza(salida) == DAMA) {
                damasNegras++;
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 7;
                bitboard2 = bitboards[7];
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                torresNegras++;
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 8;
                bitboard2 = bitboards[8];
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                alfilesNegros++;
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 9;
                bitboard2 = bitboards[9];
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                caballosNegros++;
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 10;
                bitboard2 = bitboards[10];
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 1);
            }

        }
        modificacionBitboard cambios = {cambio1, bitboard1, cambio2, bitboard2, -1, 0};
        historialBitboards[contadorHistorialBitboards] = cambios;



        jugadas[contadorJugadas] = aRealizar;

        historialEnPassant.push_back(-1);
        if (reyPropioEnJaque(_turno)) {
            cambiarTurno();
            deshacerMovimiento();
            return false;
        }

        //Chequeamos si este movimiento da jaque al rival
        if (reyPropioEnJaque(1 - _turno)) {
            tipo_jugada = PROMOTIONCHECK;
        }

        cambiarTurno();
        /*U64 zobristActual = zobristKey();
        U64 nuevaZobristKey = zobrist;

 if(nuevaZobristKey != zobristActual){
            cout << "Zobrist erronea" << endl;
            imprimirFen();
            cout << zobristActual << "-----" << nuevaZobristKey << endl;
            for(auto x: jugadas) {
                cout << x << endl;
            } exit(0);
        }*/
        return true;
    } else if (tipo_jugada == PROMOTIONIZQ) {
        ;


/*
        actualizarMaterial(aRealizar);
*/
        contadorJugadas++;

        actualizarOcupacion(aRealizar);
        actualizarZobristKey(aRealizar);
        contadorHistorialBitboards++;

        int cambio1;
        int cambio2;
        int cambio3;
        U64 bitboard1;
        U64 bitboard2;
        U64 bitboard3;
        if (_turno == 0) {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) - 9;
            peonesBlancos--;
            cambio1 = PEON_BLANCO;
            bitboard1 = bitboards[PEON_BLANCO];
            if (tipoDePieza(salida) == DAMA) {
                damasBlancas++;
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 1;
                bitboard2 = bitboards[1];
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                torresBlancas++;
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 2;
                bitboard2 = bitboards[2];
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                alfilesBlancos++;
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 3;
                bitboard2 = bitboards[3];
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                caballosBlancos++;
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 4;
                bitboard2 = bitboards[4];
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 1);
            }

            for (int i = 7; i < 11; i++){
                U64 bitboardAtaque = operaciones_bit::setBit(0L, llegada, 1);
                if(bitboards[i] & bitboardAtaque){
                    actualizarCantPiezas(i, false);
                    cambio3 = i;
                    bitboard3 = bitboards[i];
                    bitboards[i] = operaciones_bit::setBit(bitboards[i], llegada, 0);
                    break;
                }
            }
            modificacionBitboard cambios = {cambio1, bitboard1, cambio2, bitboard2, cambio3, bitboard3};
            historialBitboards[contadorHistorialBitboards] = cambios;


        } else {
            cambio1 = 11;
            bitboard1 = bitboards[11];
            peonesNegros--;
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) + 9;
            if (tipoDePieza(salida) == DAMA) {
                damasNegras++;
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 7;
                bitboard2 = bitboards[7];
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 8;
                torresNegras++;
                bitboard2 = bitboards[8];
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 9;
                alfilesNegros++;
                bitboard2 = bitboards[9];
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 10;
                caballosNegros++;
                bitboard2 = bitboards[10];
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 1);
            }
            U64 bitboardAtaque = operaciones_bit::setBit(0L, llegada, 1);

            for(int i = 1; i < 5; i++){
                if(bitboards[i] & bitboardAtaque){
                    cambio3 = i;
                    actualizarCantPiezas(i, false);
                    bitboard3 = bitboards[i];
                    bitboards[i] = operaciones_bit::setBit(bitboards[i], llegada, 0);
                    break;
                }
            }
            modificacionBitboard cambios = {cambio1, bitboard1, cambio2, bitboard2, cambio3, bitboard3};
            historialBitboards[contadorHistorialBitboards] = cambios;
        }



        jugadas[contadorJugadas] = aRealizar;

        historialEnPassant.push_back(-1);
        if (reyPropioEnJaque(_turno)) {
            cambiarTurno();
            deshacerMovimiento();
            return false;
        }

        //Chequeamos si este movimiento da jaque al rival
        if (reyPropioEnJaque(1 - _turno)) {
            tipo_jugada = CPIC;
        }

        cambiarTurno();
        /*U64 zobristActual = zobristKey();
        U64 nuevaZobristKey = zobrist;

 if(nuevaZobristKey != zobristActual){
            cout << "Zobrist erronea" << endl;
            imprimirFen();
            cout << zobristActual << "-----" << nuevaZobristKey << endl;
            for(auto x: jugadas) {
                cout << x << endl;
            } exit(0);
        }*/

        return true;

    } else if (tipo_jugada == PROMOTIONDER) {
        ;


/*
        actualizarMaterial(aRealizar);
*/
        contadorJugadas++;

        actualizarOcupacion(aRealizar);
        actualizarZobristKey(aRealizar);

        contadorHistorialBitboards++;
        int cambio1;
        int cambio2;
        int cambio3;
        U64 bitboard1;
        U64 bitboard2;
        U64 bitboard3;

        if (_turno == 0) {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) - 7;
            cambio1 = PEON_BLANCO;
            bitboard1 = bitboards[PEON_BLANCO];
            peonesBlancos--;
            if (tipoDePieza(salida) == DAMA) {
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 1;
                damasBlancas++;
                bitboard2 = bitboards[1];
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 2;
                torresBlancas++;
                bitboard2 = bitboards[2];
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 3;
                alfilesBlancos++;
                bitboard2 = bitboards[3];
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[PEON_BLANCO] = operaciones_bit::setBit(bitboards[PEON_BLANCO], casillaPromocion, 0);
                cambio2 = 4;
                caballosBlancos++;
                bitboard2 = bitboards[4];
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 1);
            }
            U64 bitboardAtaque = operaciones_bit::setBit(0L, llegada, 1);

            for (int i = 7; i < 11; i++){
                if(bitboards[i] & bitboardAtaque){
                    cambio3 = i;
                    actualizarCantPiezas(i, false);
                    bitboard3 = bitboards[i];
                    bitboards[i] = operaciones_bit::setBit(bitboards[i], llegada, 0);
                    break;
                }
            }
            modificacionBitboard cambios = {cambio1, bitboard1, cambio2, bitboard2, cambio3, bitboard3};
            historialBitboards[contadorHistorialBitboards] = cambios;

        } else {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) + 7;
            cambio1 = 11;
            bitboard1 = bitboards[11];
            peonesNegros--;
            if (tipoDePieza(salida) == DAMA) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 7;
                damasNegras++;
                bitboard2 = bitboards[7];
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 8;
                torresNegras++;
                bitboard2 = bitboards[8];
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 9;
                alfilesNegros++;
                bitboard2 = bitboards[9];
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                cambio2 = 10;
                caballosNegros++;
                bitboard2 = bitboards[10];
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 1);
            }

            for(int i = 1; i < 5; i++){
                U64 bitboardAtaque = operaciones_bit::setBit(0L, llegada, 1);
                if(bitboards[i] & bitboardAtaque){
                    cambio3 = i;
                    actualizarCantPiezas(i, false);

                    bitboard3 = bitboards[i];
                    bitboards[i] = operaciones_bit::setBit(bitboards[i], llegada, 0);
                    break;
                }
            }
            modificacionBitboard cambios = {cambio1, bitboard1, cambio2, bitboard2, cambio3, bitboard3};
            historialBitboards[contadorHistorialBitboards] = cambios;
        }



        jugadas[contadorJugadas] = aRealizar;

        historialEnPassant.push_back(-1);

        if (reyPropioEnJaque(_turno)) {
            cambiarTurno();
            deshacerMovimiento();
            return false;
        }

        //Chequeamos si este movimiento da jaque al rival
        if (reyPropioEnJaque(1 - _turno)) {
            tipo_jugada = CPDC;
        }
        cambiarTurno();


        ///Esta pieza de código sirve para debuggear el buen funcioniento de la zobristKey.
       /* U64 zobristActual = zobristKey();
        U64 nuevaZobristKey = zobrist;
 if(nuevaZobristKey != zobristActual){
            cout << "Zobrist erronea" << endl;
            imprimirFen();
            cout << zobristActual << "-----" << nuevaZobristKey << endl;
            for(auto x: jugadas) {
                cout << x << endl;
            } exit(0);
        }*/
        return true;

    }

    U64 salidaBitboard = operaciones_bit::setBit(0L, salida, 1);
    U64 llegadaBitboard = operaciones_bit::setBit(0L, llegada, 1);
/*
    bool ECB = false, ELB = false, ECN = false, ELN = false; //Para tener noción de qué historiales ya se modificaron
*/
    bool enroqueAGuardar = false;
    if (salida == 4) {
        if(enroqueCortoBlancas()) {
            setearEnroque(0, false);
            enroqueAGuardar = true;
        }
        if(enroqueLargoBlancas()) {
            setearEnroque(1, false);
            enroqueAGuardar = true;
        }

        /*enroqueCortoBlanco.push_back(false);
        enroqueLargoBlanco.push_back(false);*/
        perdidaEnroqueLargoBlancas = true;
        perdidaEnroqueCortoBlancas = true;
        /*ECB = true;
        ELB = true;*/
    } else if (salida == 60) {
        if(enroqueCortoNegras()) {
            setearEnroque(2, false);
            enroqueAGuardar = true;
        }
        if(enroqueLargoNegras()) {
            setearEnroque(3, false);
            enroqueAGuardar = true;
        }
        /*enroqueCortoNegro.push_back(false);
        enroqueLargoNegro.push_back(false);*/
        perdidaEnroqueLargoNegras = true;
        perdidaEnroqueCortoNegras = true;
        /*ECN = true;
        ELN = true;*/
    } else {
        if (salida == 1 || llegada == 1) {
            if(enroqueCortoBlancas()) {
                setearEnroque(0, false);
                enroqueAGuardar = true;
            }
            /*enroqueCortoBlanco.push_back(false);*/
            perdidaEnroqueCortoBlancas = true;
/*
            ECB = true;
*/
        }
        if (salida == 57 || llegada == 57) {
            if(enroqueCortoNegras()) {
                setearEnroque(2, false);
                enroqueAGuardar = true;
            }
            /*enroqueCortoNegro.push_back(false);*/
            perdidaEnroqueCortoNegras = true;
/*
            ECN = true;
*/

        }
        if (salida == 64 || llegada == 64) {
            if(enroqueLargoNegras()) {
                setearEnroque(3, false);
                enroqueAGuardar = true;
            }
            /*enroqueLargoNegro.push_back(false);*/
            perdidaEnroqueLargoNegras = true;
/*
            ELN = true;
*/
        }
        if (salida == 8 || llegada == 8) {
            if(enroqueLargoBlancas()) {
                setearEnroque(1, false);
                enroqueAGuardar = true;
            }
            /*enroqueLargoBlanco.push_back(false);*/
            perdidaEnroqueLargoBlancas = true;
/*
            ELB = true;
*/
        }

    }
    if(enroqueAGuardar) {
        guardarEnroque();
    }
    if (tipo_jugada == QUIET) {
        int tipo_de_pieza = obtenerTipoDePieza(salida);
        if (abs(salida - llegada) == 16 && (tipo_de_pieza == PEON)) {
            int columnaDeEnpassant = (llegada % 8) - 1;
            if (columnaDeEnpassant == -1) {
                columnaDeEnpassant = 7;
            }
            historialEnPassant.push_back(columnaDeEnpassant);
            agregueAlgoAlHistorialEP = true;
            actualiceEnPassant = true;
        }
    }
   /* if (!ECB) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
    }
    if (!ELB) {
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
    }
    if (!ECN) {
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
    }
    if (!ELN) {
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
    }*/

    if ((obtenerTipoDePieza(salida) == DAMA) && (((_turno == 0) && !desarrolloBlancasCompleto())
                                                || ((_turno == 1) && !desarrolloNegrasCompleto()))){
        if (_turno == 0) {
            movimientoTempranoDamaBlanca = true;
        } else {
            movimientoTempranoDamaNegra = true;
        }
    }
/*
    actualizarMaterial(aRealizar);
*/
    contadorJugadas++;

    actualizarOcupacion(aRealizar);
    actualizarZobristKey(aRealizar);

    /// Momentánemente la función de evaluación no utiliza la cantidad
    /// de movimientos realizados por piezas menores, por lo que no vale la pena
    /// realizar el cálculo.
    //actualizarCantMovesPiezasMenores(aRealizar, true);
/*
    historialDePosiciones.push_back(zobrist);
*/

    int cambio1;
    int cambio2 = -1;
    U64 bitboard1;
    U64 bitboard2 = 0;
    for (int k = 0; k < 12; k++) {

        if ((bitboards[k] & salidaBitboard) > 0) {
            cambio1 = k;
            bitboard1 = bitboards[k];
            bitboards[k] = operaciones_bit::setBit(bitboards[k], salida, 0);

            bitboards[k] = operaciones_bit::setBit(bitboards[k], llegada, 1);

        } else if ((bitboards[k] & llegadaBitboard) > 0) {
            cambio2 = k;
            bitboard2 = bitboards[k];
            actualizarCantPiezas(k, false);

            bitboards[k] = operaciones_bit::setBit(bitboards[k], llegada, 0);

        }
    }
    contadorHistorialBitboards++;
    modificacionBitboard cambios = {cambio1, bitboard1, cambio2, bitboard2, -1, 0};
    historialBitboards[contadorHistorialBitboards] = cambios;
    jugadas[contadorJugadas] = aRealizar;
/*
    _jugadas.push_back(aRealizar);
*/
/*
    historialPosiciones.push_back(bitboards);
*/
    if (!actualiceEnPassant) {
        historialEnPassant.push_back(-1);
    }

    if (reyPropioEnJaque(_turno)) {
        cambiarTurno();
        deshacerMovimiento();
        return false;
    }

    //Chequeamos si este movimiento da jaque al rival
    if (reyPropioEnJaque(1 - _turno)) {
        if (tipo_jugada == QUIET) {
            tipo_jugada = CHECK;
        } else if (tipo_jugada == CAPTURE) {
            tipo_jugada = CAPTURECHECK;
        }
    }
    if (tipo_jugada == CAPTURE) {
        capturas += 1;
    } else if (tipo_jugada == CAPTURECHECK) {
        capturas += 1;
        check += 1;
    } else if (tipo_jugada == CHECK) {
        check += 1;
    } else {
        comunes += 1;
    }
    cambiarTurno();
;
   /* U64 zobristActual = zobristKey();
    U64 nuevaZobristKey = zobrist;

    if(nuevaZobristKey != zobristActual){
        cout << "Zobrist erronea" << endl;
        imprimirFen();
        cout << zobristActual << "-----" << nuevaZobristKey << endl;
        for(auto x: jugadas) {
            cout << x << endl;
        }
    }*/

    /* if (tipo_jugada == CHECK || tipo_jugada == CAPTURECHECK) {
         if (_turno == 0) {
             if (ganoELBlanco()) {
                 ganoBlanco = true;
             }
         } else {
             if (ganoElNegro()) {
                 ganoNegro = true;
             }
         }
     }*/


    return true;


};

/*bool Tablero::casillaAtacada(int casilla, int turno) {

    vector<u_short> movimientos = this->generar_movimientos(turno);
    for (u_short x: movimientos) {
        if (operaciones_bit::getLlegada(x) == casilla) {
            return true;
        }
    }
    return false;
}*/

void Tablero::generar_movimientos(int turno, const int ply) {
    cantMovesGenerados[ply] = -1;
    //turno = 0 es blancas, turno = 1 es negras
    if (turno == 0) {
        U64 bitboardTemp;

        bitboardTemp = bitboards[1]; //bitboard de la dama blanca
        obtener_movimientos_dama_blanca(ply, bitboardTemp);

        bitboardTemp = bitboards[2]; //bitboard de las torres blancas
        obtener_movimientos_torre_blanca(ply, bitboardTemp);

        bitboardTemp = bitboards[3]; //bitboard de los alfiles blancos
        obtener_movimientos_alfil_blanco(ply, bitboardTemp);

        obtener_movimientos_rey_blanco(ply);
        obtener_movimientos_caballo_blanco(ply);
        obtener_movimientos_peon_blanco(ply);

    } else {
        U64 bitboardTemp;

        bitboardTemp = bitboards[7];
        obtener_movimientos_dama_negra(ply, bitboardTemp);

        bitboardTemp = bitboards[8];
        obtener_movimientos_torre_negra(ply, bitboardTemp);

        bitboardTemp = bitboards[9];
        obtener_movimientos_alfil_negro(ply, bitboardTemp);

        obtener_movimientos_caballo_negro(ply);
        obtener_movimientos_rey_negro(ply);
        obtener_movimientos_peon_negro(ply);

    }
}

void Tablero::obtener_movimientos_peon_negro(
        int ply) {//Movimientos de Perón... Mi general, cuánto valés!
    peon->movimientos_legales(this, ply, bitboards[11],
                              piezas_negras(),
                              piezas_blancas());

}

void Tablero::obtener_movimientos_rey_negro(int ply) {
    //Movimientos de rey
    rey->movimientos_legales(this, ply, bitboards[6], piezas_negras(), piezas_blancas());
    if (chequearEnroqueCorto()) {
        U64 bitboardTemp = operaciones_bit::setBit(0, 59, 1);
        /*if (esJaque(torre->generar_movimientos_legales(bitboardTemp, piezas_negras(), piezas_blancas(), 1), _turno)) {
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(60, 58, CASTLINGCHECK);
        } else {*/
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(60, 58, CASTLING);
        /*}*/
    }
    if (chequearEnroqueLargo()) {
        U64 bitboardTemp = operaciones_bit::setBit(0, 61, 1);
        /*if (esJaque(torre->generar_movimientos_legales(bitboardTemp, piezas_negras(), piezas_blancas(), 1), _turno)) {
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(60, 62, CASTLINGCHECK);
        } else {*/
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(60, 62, CASTLING);
        /*}*/
    }

}


void Tablero::obtener_movimientos_caballo_negro(int ply) {//Movimientos de caballo
    caballo->obtener_movimientos(this, ply, bitboards[10], piezas_negras(), piezas_blancas());
}


U64 Tablero::bitboard_movimientos_dama_negra(U64 bitboard) {

    return bitboard_movimientos_alfil_negro(bitboard) | bitboard_movimientos_torre_negra(bitboard);

}

void Tablero::obtener_movimientos_dama_negra(int ply, U64 bitboard) {//Movimientos de dama
    bitboard = bitboards[7];
    while (bitboard > 0) {
        //Movimientos de torre
        int LSB = operaciones_bit::LSB(bitboard);
        U64 dama = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeTorre[LSB - 1][dama * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];
        jugadasEnPos = jugadasEnPos & ~piezas_negras();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMoveTorre =
                    constantes::attackMasksTorre[casillaLlegada - 1] & (todas_las_piezas());
            U64 maskPiezasBloqueandoDelMoveAlfil =
                    constantes::attackMasksAlfil[casillaLlegada - 1] & (todas_las_piezas());
            /*if (esJaque(movimientosDeTorre[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMoveTorre * constantes::magicsParaTorre[casillaLlegada - 1] >> 52],
                        1)) {
                *//* if(ganoElNegro()){
                     cantMovesGenerados[ply][ply]++;
movimientos_generados[ply][cantMovesGenerados[ply][ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                 }
                 else*//* if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else if (esJaque((movimientosDeAlfil[casillaLlegada - 1][
                    maskPiezasBloqueandoDelMoveAlfil * constantes::magicsParaAlfil[casillaLlegada - 1] >> 55]), 1)) {
                *//*if(ganoElNegro()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else */if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   CAPTURE);
            } else {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   QUIET);
            }
        }


        //Movimientos de alfil
        U64 dama2 = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos2 = movimientosDeAlfil[LSB - 1][dama2 * constantes::magicsParaAlfil[LSB - 1]
                >> (64 - 9)];
        jugadasEnPos2 = jugadasEnPos2 & ~piezas_negras();
        while (jugadasEnPos2 > 0) {
            int casillaLlegada2 = operaciones_bit::LSB(jugadasEnPos2);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada2, 1);

            U64 maskPiezasBloqueandoDelMoveTorre2 =
                    constantes::attackMasksTorre[casillaLlegada2 - 1] & (todas_las_piezas());
            U64 maskPiezasBloqueandoDelMoveAlfil2 =
                    constantes::attackMasksAlfil[casillaLlegada2 - 1] & (todas_las_piezas());
            /*if (esJaque(movimientosDeAlfil[casillaLlegada2 - 1]
                        [maskPiezasBloqueandoDelMoveAlfil2 * constantes::magicsParaAlfil[casillaLlegada2 - 1] >> 55],
                        1)) {
                *//*if(ganoElNegro()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada2,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada2,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada2,
                                                                                                       CHECK);
                }
            } else if (esJaque((movimientosDeTorre[casillaLlegada2 - 1]
            [maskPiezasBloqueandoDelMoveTorre2 * constantes::magicsParaTorre[casillaLlegada2 - 1] >> 52]), 1)) {
                *//*if(ganoElNegro()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada2,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada2,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada2,
                                                                                                       CHECK);
                }
            } else*/ if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada2,
                                                                                                   CAPTURE);
            } else {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada2,
                                                                                                   QUIET);
            }
        }
    }
}

void Tablero::obtener_movimientos_alfil_negro(int ply, U64 bitboard) {
    while (bitboard > 0) {
        int LSB = operaciones_bit::LSB(bitboard);
        U64 alfil = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeAlfil[LSB - 1][alfil * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
        jugadasEnPos = jugadasEnPos & ~piezas_negras();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksAlfil[casillaLlegada - 1] & (todas_las_piezas());
            /* if (esJaque(movimientosDeAlfil[casillaLlegada - 1]
                         [maskPiezasBloqueandoDelMove * constantes::magicsParaAlfil[casillaLlegada - 1] >> 55], 1)) {
                 *//*if(ganoElNegro()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else *//*if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else*/ if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   CAPTURE);
            } else {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   QUIET);
            }
        }
    }
}

void Tablero::obtener_movimientos_torre_negra(int ply, U64 bitboard) {//Movimientos de torre
    while (bitboard > 0) {
        int LSB = operaciones_bit::LSB(bitboard);
        U64 torre = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeTorre[LSB - 1][torre * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];
        jugadasEnPos = jugadasEnPos & ~piezas_negras();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksTorre[casillaLlegada - 1] & (todas_las_piezas());
            /*if (esJaque(movimientosDeTorre[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaTorre[casillaLlegada - 1] >> 52], 1)) {
                *//*if(ganoElNegro()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else*/ if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   CAPTURE);
            } else {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   QUIET);
            }
        }
    }
}

void Tablero::obtener_movimientos_peon_blanco(int ply) {

    //Generar movimientos de peón
    peon->movimientos_legales(this, ply, bitboards[5],
                              piezas_blancas(),
                              piezas_negras());

}

void Tablero::obtener_movimientos_rey_blanco(int ply) {
    //Generar movimientos de rey
    rey->movimientos_legales(this, ply, bitboards[0], piezas_blancas(), piezas_negras());
    if (chequearEnroqueCorto()) {
        U64 bitboardTemp = operaciones_bit::setBit(0, 3, 1);
        /*if (esJaque(torre->generar_movimientos_legales(bitboardTemp, piezas_blancas(), piezas_negras(), 0), _turno)) {
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4, 2, CASTLINGCHECK);
        } else {*/
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4, 2, CASTLING);
        /*}*/
    }
    if (chequearEnroqueLargo()) {
        U64 bitboardTemp = operaciones_bit::setBit(0, 5, 1);
        /*if (esJaque(torre->generar_movimientos_legales(bitboardTemp, piezas_blancas(), piezas_negras(), 0), _turno)) {
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4, 6, CASTLINGCHECK);
        } else {*/
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4, 6, CASTLING);
        /*}*/
    }

}

void Tablero::obtener_movimientos_caballo_blanco(int ply) {
    caballo->obtener_movimientos(this, ply, bitboards[4], piezas_blancas(), piezas_negras());

}

void Tablero::obtener_movimientos_dama_blanca(int ply, U64 bitboard) {
    while (bitboard > 0) {
        //Primero se generan los "movimientos de torre" para la dama
        int LSB = operaciones_bit::LSB(bitboard);
        U64 dama = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeTorre[LSB - 1][dama * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];
        jugadasEnPos = jugadasEnPos & ~piezas_blancas();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMoveTorre =
                    constantes::attackMasksTorre[casillaLlegada - 1] & (todas_las_piezas());
            U64 maskPiezasBloqueandoDelMoveAlfil =
                    constantes::attackMasksAlfil[casillaLlegada - 1] & (todas_las_piezas());
            /*if (esJaque(movimientosDeTorre[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMoveTorre * constantes::magicsParaTorre[casillaLlegada - 1] >> 52],
                        0)) {
                *//* if(ganoELBlanco()){
                     cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                 }
                 else*//* if ((casillaLlegadaBitboard & piezas_negras()) > 0) {

                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else if (esJaque((movimientosDeAlfil[casillaLlegada - 1]
            [maskPiezasBloqueandoDelMoveAlfil * constantes::magicsParaAlfil[casillaLlegada - 1] >> 55]), 0)) {
                *//*if(ganoELBlanco()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else*/ if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   CAPTURE);
            } else {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   QUIET);
            }
        }

        //Por ultimo se generan los "movimientos de alfil" para la dama
        U64 dama2 = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos2 = movimientosDeAlfil[LSB - 1][dama2 * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
        jugadasEnPos2 = jugadasEnPos2 & ~piezas_blancas();
        while (jugadasEnPos2 > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos2);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMoveTorre =
                    constantes::attackMasksAlfil[casillaLlegada - 1] & (todas_las_piezas());
            U64 maskPiezasBloqueandoDelMoveAlfil =
                    constantes::attackMasksTorre[casillaLlegada - 1] & (todas_las_piezas());
            /* if (esJaque(movimientosDeAlfil[casillaLlegada - 1]
                         [maskPiezasBloqueandoDelMoveAlfil * constantes::magicsParaAlfil[casillaLlegada - 1] >> 55],
                         0)) {
                 *//*if(ganoELBlanco()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else if (esJaque((movimientosDeTorre[casillaLlegada - 1]
            [maskPiezasBloqueandoDelMoveTorre * constantes::magicsParaTorre[casillaLlegada - 1] >> 52]), 0)) {
                *//*if(ganoELBlanco()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else*/ if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   CAPTURE);
            } else {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   QUIET);
            }
        }
    }
}

void Tablero::obtener_movimientos_alfil_blanco(int ply, U64 bitboard) {
    while (bitboard > 0) {
        int LSB = operaciones_bit::LSB(bitboard);
        U64 alfil = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeAlfil[LSB - 1][alfil * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
        jugadasEnPos = jugadasEnPos & ~piezas_blancas();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksAlfil[casillaLlegada - 1] & (todas_las_piezas());
            /*if (esJaque(movimientosDeAlfil[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaAlfil[casillaLlegada - 1] >> 55], 0)) {
                *//*if(ganoELBlanco()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);
                }
            } else*/ if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   CAPTURE);
            } else {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   QUIET);
            }
        }
    }
}

void Tablero::obtener_movimientos_torre_blanca(int ply, U64 bitboard) {
    while (bitboard > 0) {
        //Se obtiene la casilla de la torre más próxima empezando desde la derecha del bitboard
        int LSB = operaciones_bit::LSB(bitboard);

        //Se obtiene la máscara de la torre en la casilla correspondiente
        U64 torre = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());

        //Se obtienen los movimientos posibles de la torre mediante la tabla de hash
        U64 jugadasEnPos = movimientosDeTorre[LSB - 1][torre * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];

        //Se filtran las posibles capturas a piezas propias:
        jugadasEnPos = jugadasEnPos & (~piezas_blancas());

        //Se itera sobre el bitboard de las jugadas posibles
        while (jugadasEnPos > 0) {
            //Se obtiene la casilla más próxima comenzando desde la derecha
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);
            //Se obtiene la máscara de las piezas que bloquean el movimiento en esa posicion, cosa que va
            // a ser útil para chequear posibles jaques
            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksTorre[casillaLlegada - 1] & (todas_las_piezas());


            //Si desde la casilla donde arriba la torre se puede atacar al rey enemigo, se agrega como jaque
            // Si además se toma una pieza rival, se agrega como CAPTURECHECK
            /*if (esJaque(movimientosDeTorre[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaTorre[casillaLlegada - 1] >> 52], 0)) {
                *//*if(ganoELBlanco()){
                    cantMovesGenerados[ply]++;
movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*//* if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CAPTURECHECK);
                } else {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB,
                                                                                                       casillaLlegada,
                                                                                                       CHECK);

                }

            }*/

            //Si se toma una pieza rival, se agrega como CAPTURE
            /* else*/ if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   CAPTURE);
            }

                //Si no se cumple ninguna de las condiciones anteriores, es un movimiento "QUIET"
            else {
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, casillaLlegada,
                                                                                                   QUIET);
            }

        }

    }
}

void Tablero::setTurno(int turno) {
    _turno = turno;
}


int Tablero::obtenerTipoDePieza(int casillaDePieza) {
    U64 casillaSalidaBitboard = operaciones_bit::setBit(0L, casillaDePieza, 1);
    //Se itera sobre los bitboards para ver cuál contiene la casilla de salida y así determinar
    // el tipo de pieza
    int tipoDePieza = -1;
    for (int i = 0; i < 12; i++) {
        if ((bitboards[i] & casillaSalidaBitboard) > 0) {
            tipoDePieza = i;
            break;
        }
    }
    if (tipoDePieza == -1) {
        return VACIO;
    } else if (tipoDePieza == 0 | tipoDePieza == 6) {
        return REY;
    } else if (tipoDePieza == 1 | tipoDePieza == 7) {
        return DAMA;
    } else if (tipoDePieza == 2 | tipoDePieza == 8) {
        return TORRE;
    } else if (tipoDePieza == 3 | tipoDePieza == 9) {
        return ALFIL;
    } else if (tipoDePieza == 4 | tipoDePieza == 10) {
        return CABALLO;
    } else if (tipoDePieza == 5 | tipoDePieza == 11) {
        return PEON;
    }
}

bool Tablero::esUnaPromocion(u_short jugada) {
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    if (tipoDeJugada == PROMOTIONDER | tipoDeJugada == PROMOTIONIZQ
        | tipoDeJugada == PROMOTION | tipoDeJugada == PROMOTIONCHECK
        | tipoDeJugada == CPDC | tipoDeJugada == CPIC) {
        return true;
    } else {
        return false;
    }
}

//Toma una jugada de tipo promoción, y la reescribe como una jugada normal con casillas
// de salida, de entrada y tipo de promoción.
u_short Tablero::reescribirPromocion(u_short jugada) {
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    u_short jugadaReescrita = 0;
    int casillaSalida = 0;
    if (tipoDeJugada == PROMOTIONDER) {
        if (_turno == 0) {
            casillaSalida = operaciones_bit::getLlegada(jugada) - 7;
        } else {
            casillaSalida = operaciones_bit::getLlegada(jugada) + 7;
        }
        jugadaReescrita = operaciones_bit::crearJugada(casillaSalida, operaciones_bit::getLlegada(jugada),
                                                       PROMOTIONDER);
    } else if (tipoDeJugada == PROMOTIONIZQ) {
        if (_turno == 0) {
            casillaSalida = operaciones_bit::getLlegada(jugada) - 9;
        } else {
            casillaSalida = operaciones_bit::getLlegada(jugada) + 9;
        }
        jugadaReescrita = operaciones_bit::crearJugada(casillaSalida, operaciones_bit::getLlegada(jugada),
                                                       PROMOTIONIZQ);
    } else if (tipoDeJugada == PROMOTION) {
        if (_turno == 0) {
            casillaSalida = operaciones_bit::getLlegada(jugada) - 8;
        } else {
            casillaSalida = operaciones_bit::getLlegada(jugada) + 8;
        }
        jugadaReescrita = operaciones_bit::crearJugada(casillaSalida, operaciones_bit::getLlegada(jugada),
                                                       PROMOTION);
    } else if (tipoDeJugada == PROMOTIONCHECK) {
        if (_turno == 0) {
            casillaSalida = operaciones_bit::getLlegada(jugada) - 8;
        } else {
            casillaSalida = operaciones_bit::getLlegada(jugada) + 8;
        }
        jugadaReescrita = operaciones_bit::crearJugada(casillaSalida, operaciones_bit::getLlegada(jugada),
                                                       PROMOTIONCHECK);
    } else if (tipoDeJugada == CPDC) {
        if (_turno == 0) {
            casillaSalida = operaciones_bit::getLlegada(jugada) - 7;
        } else {
            casillaSalida = operaciones_bit::getLlegada(jugada) + 7;
        }
        jugadaReescrita = operaciones_bit::crearJugada(casillaSalida, operaciones_bit::getLlegada(jugada),
                                                       CPDC);
    } else if (tipoDeJugada == CPIC) {
        if (_turno == 0) {
            casillaSalida = operaciones_bit::getLlegada(jugada) - 9;
        } else {
            casillaSalida = operaciones_bit::getLlegada(jugada) + 9;
        }
        jugadaReescrita = operaciones_bit::crearJugada(casillaSalida, operaciones_bit::getLlegada(jugada),
                                                       CPIC);
    }
    return jugadaReescrita;
}


/*bool Tablero::esJugadaLegal(u_short jugada) {
    //Se obtiene el tipo de pieza

    // Si la jugada que se pasa por parámetro es una promoción, hay que
    // reescribirla como una jugada cualquiera

    if(esUnaPromocion(jugada)){
        jugada = reescribirPromocion(jugada);
    }

    int casillaSalida = operaciones_bit::getSalida(jugada);
    U64 casillaSalidaBitboard = operaciones_bit::setBit(0L, casillaSalida, 1);
    int casillaLlegada = operaciones_bit::getLlegada(jugada);
    U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);
    U64 movimientosDeLaPieza = 0ULL;
    int tipoDePieza = obtenerTipoDePieza(casillaSalida);

//Primero se chequea si la pieza a mover está clavada
    //Bitboard de la posible pieza que está clavando
    U64 esta_clavada = estaClavada(casillaSalida);
    if(esta_clavada > 0){
        int casillaPiezaClavadora = operaciones_bit::LSB(esta_clavada);
        movimientosDeLaPieza = generarMovimientosPiezaClavada(casillaSalida, casillaPiezaClavadora);

    }



//Si no estaba clavada, se obtienen los movimientos posibles para la casilla de salida
// y el tipo de pieza obtenido.
    else {
        //Si la casilla de salida no contiene ninguna pieza, se devuelve false
        if (tipoDePieza == VACIO) {
            return false;
        }

            //Caso rey
        else if (tipoDePieza == REY) {

            U64 reyBitboard = operaciones_bit::setBit(0L, casillaSalida, 1);
            movimientosDeLaPieza = Rey::generar_movimientos_legales(reyBitboard, piezasPropias(), piezasRivales());
        }
            //Caso dama
        else if (tipoDePieza == DAMA) {
            //Se calculan los movimientos de la pieza consultando la tabla de ataques
            U64 attackMaskTorre = constantes::attackMasksTorre[casillaSalida-1];
            attackMaskTorre &= todas_las_piezas();
            U64 magic = constantes::magicsParaTorre[casillaSalida-1];
            movimientosDeLaPieza |= movimientosDeTorre[casillaSalida-1][(attackMaskTorre * magic) >> 52];
            U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaSalida-1];
            attackMaskAlfil &= todas_las_piezas();
            magic = constantes::magicsParaAlfil[casillaSalida-1];
            movimientosDeLaPieza |= movimientosDeAlfil[casillaSalida-1][(attackMaskAlfil * magic) >> 55];
            movimientosDeLaPieza &= ~piezasPropias();

        }
            //Caso torre
        else if (tipoDePieza == TORRE) {
            //Se calculan los movimientos de la pieza consultando la tabla de ataques
            U64 attackMaskTorre = constantes::attackMasksTorre[casillaSalida-1];
            attackMaskTorre &= todas_las_piezas();
            U64 magic = constantes::magicsParaTorre[casillaSalida-1];
            movimientosDeLaPieza |= movimientosDeTorre[casillaSalida-1][(attackMaskTorre * magic) >> 52];
            movimientosDeLaPieza &= ~piezasPropias();

        }
            //Caso alfil
        else if (tipoDePieza == ALFIL) {
            U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaSalida-1];
            attackMaskAlfil &= todas_las_piezas();
            U64 magic = constantes::magicsParaAlfil[casillaSalida-1];
            movimientosDeLaPieza |= movimientosDeAlfil[casillaSalida-1][(attackMaskAlfil * magic) >> 55];
            movimientosDeLaPieza &= ~piezasPropias();
        }
            //Caso caballo
        else if (tipoDePieza == CABALLO) {
            U64 caballoBitboard = operaciones_bit::setBit(0L, casillaSalida, 1);
            movimientosDeLaPieza = Caballo::generar_movimientos_legales(caballoBitboard, piezasPropias(),
                                                                        piezasRivales());
        }
        else{
            //Caso peón
            U64 peonBitboard = operaciones_bit::setBit(0L, casillaSalida, 1);
            movimientosDeLaPieza = Peon::generar_movimientos_legales(peonBitboard, piezasPropias(),
                                                                     piezasRivales(), _turno);
        }
    }



    //Si el movimiento que queremos hacer está dentro de los posibles, retornamos true
    if((movimientosDeLaPieza & casillaLlegadaBitboard) > 0){
        return true;
    }
    else{
        return false;
    }
}*/
void Tablero::promocionar(u_short jugada) {
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    int casillaDeLlegada = operaciones_bit::getLlegada(jugada);
    int tipoDePieza = operaciones_bit::getSalida(jugada);
    if (tipoDeJugada == PROMOTION) {
        if (_turno == 0) {
            bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaDeLlegada - 8, 0);
            if (tipoDePieza == 1) {
                bitboards[1] = operaciones_bit::setBit(bitboards[1], casillaDeLlegada, 1);
            } else if (tipoDePieza == 2) {
                bitboards[2] = operaciones_bit::setBit(bitboards[2], casillaDeLlegada, 1);
            } else if (tipoDePieza == 3) {
                bitboards[3] = operaciones_bit::setBit(bitboards[3], casillaDeLlegada, 1);
            } else if (tipoDePieza == 4) {
                bitboards[4] = operaciones_bit::setBit(bitboards[4], casillaDeLlegada, 1);
            }
        } else {
            bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaDeLlegada + 8, 0);
            if (tipoDePieza == 1) {
                bitboards[7] = operaciones_bit::setBit(bitboards[7], casillaDeLlegada, 1);
            } else if (tipoDePieza == 2) {
                bitboards[8] = operaciones_bit::setBit(bitboards[8], casillaDeLlegada, 1);
            } else if (tipoDePieza == 3) {
                bitboards[9] = operaciones_bit::setBit(bitboards[9], casillaDeLlegada, 1);
            } else if (tipoDePieza == 4) {
                bitboards[10] = operaciones_bit::setBit(bitboards[10], casillaDeLlegada, 1);
            }
        }
    } else if (tipoDeJugada == PROMOTIONDER) {
        if (_turno == 0) {
            bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaDeLlegada - 7, 0);
            if (tipoDePieza == 1) {
                bitboards[1] = operaciones_bit::setBit(bitboards[1], casillaDeLlegada, 1);
            } else if (tipoDePieza == 2) {
                bitboards[2] = operaciones_bit::setBit(bitboards[2], casillaDeLlegada, 1);
            } else if (tipoDePieza == 3) {
                bitboards[3] = operaciones_bit::setBit(bitboards[3], casillaDeLlegada, 1);
            } else if (tipoDePieza == 4) {
                bitboards[4] = operaciones_bit::setBit(bitboards[4], casillaDeLlegada, 1);
            }
        } else {
            bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaDeLlegada + 7, 0);

            if (tipoDePieza == 1) {
                bitboards[7] = operaciones_bit::setBit(bitboards[7], casillaDeLlegada, 1);
            } else if (tipoDePieza == 2) {
                bitboards[8] = operaciones_bit::setBit(bitboards[8], casillaDeLlegada, 1);
            } else if (tipoDePieza == 3) {
                bitboards[9] = operaciones_bit::setBit(bitboards[9], casillaDeLlegada, 1);
            } else if (tipoDePieza == 4) {
                bitboards[10] = operaciones_bit::setBit(bitboards[10], casillaDeLlegada, 1);
            }
        }
    } else if (tipoDeJugada == PROMOTIONIZQ) {
        if (_turno == 0) {
            bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaDeLlegada - 9, 0);
            if (tipoDePieza == 1) {
                bitboards[1] = operaciones_bit::setBit(bitboards[1], casillaDeLlegada, 1);
            } else if (tipoDePieza == 2) {
                bitboards[2] = operaciones_bit::setBit(bitboards[2], casillaDeLlegada, 1);
            } else if (tipoDePieza == 3) {
                bitboards[3] = operaciones_bit::setBit(bitboards[3], casillaDeLlegada, 1);
            } else if (tipoDePieza == 4) {
                bitboards[4] = operaciones_bit::setBit(bitboards[4], casillaDeLlegada, 1);
            }
        } else {
            bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaDeLlegada + 9, 0);

            if (tipoDePieza == 1) {
                bitboards[7] = operaciones_bit::setBit(bitboards[7], casillaDeLlegada, 1);
            } else if (tipoDePieza == 2) {
                bitboards[8] = operaciones_bit::setBit(bitboards[8], casillaDeLlegada, 1);
            } else if (tipoDePieza == 3) {
                bitboards[9] = operaciones_bit::setBit(bitboards[9], casillaDeLlegada, 1);
            } else if (tipoDePieza == 4) {
                bitboards[10] = operaciones_bit::setBit(bitboards[10], casillaDeLlegada, 1);
            }
        }
    }

}


// Chequea si una pieza está dando jaque, recibiendo como parámetro los movimientos posibles para
// dicha pieza
bool Tablero::esJaque(U64 jugadasPosibles, int turno) {
    if (turno == 0) {

        if ((jugadasPosibles & bitboards[6]) > 0) {
            return true;
        } else {
            return false;
        }
    } else {
        if ((jugadasPosibles & bitboards[0]) > 0) {
            return true;
        } else {
            return false;
        }
    }
}

U64 Tablero::torresRivales() {
    if (_turno == 0) return bitboards[8];
    else return bitboards[2];
}

U64 Tablero::alfilesRivales() {
    if (_turno == 0) return bitboards[9];
    else return bitboards[3];

}

U64 Tablero::damasRivales() {
    if (_turno == 0) return bitboards[7];
    else return bitboards[1];
}

U64 Tablero::reyPropio(int color) {
    if (color == 0) return bitboards[0];
    else return bitboards[6];
}

//Chequea si una pieza está clavada de la siguiente manera:
//  1- Se obtienen las attack masks de las sliding pieces rivales
//  2- Se intersecan las attack masks con el bitboard de piezas menos la pieza que se quiere chequear
//  3- Se chequea en la tabla de hash los movimientos para la posición generada
//  4- Si una de las casillas atacadas es la del rey, entonces la pieza  estaba clavada

// Devuelve un bitboard con la pieza que clava  si es que la hay, si no
// devuelve 0.
U64 Tablero::estaClavada(int casillaDePieza) {
    U64 torresRivales = this->torresRivales();
    U64 alfilesRivales = this->alfilesRivales();
    U64 damasRivales = this->damasRivales();
    U64 bitboardPiezaAChequear = operaciones_bit::setBit(0L, casillaDePieza, 1);
    U64 res = 0ULL;
    //Chequear si alguna torre clava a la pieza
    while (torresRivales > 0) {
        int casillaDeTorre = operaciones_bit::LSB(torresRivales);
        U64 attackMaskTorre = constantes::attackMasksTorre[casillaDeTorre - 1];

        //Se chequea si la torre ataca en la dirección de la pieza a chequear. Si no lo hace, se pasa a la
        // siguiente torre

        if ((attackMaskTorre & bitboardPiezaAChequear) == 0) {
            continue;
        }
        // Se calcula el bitboard de todas las piezas menos la pieza a chequear
        U64 bitboardSinPieza = todas_las_piezas() & ~(bitboardPiezaAChequear);

        // Se interseca la attack mask con el bitboard de todas las piezas menos la pieza a chequear
        U64 occupancyMask = attackMaskTorre & bitboardSinPieza;

        // Se calcula la key de la posición generada y luego se obtienen los movimientos posibles
        U64 key = occupancyMask * constantes::magicsParaTorre[casillaDeTorre] >> (64 - 12);
        U64 movimientosPosibles = movimientosDeTorre[casillaDeTorre][key];

        // Se chequea si la casilla del rey está atacada. Si lo está, ya puede concluirse que
        // la pieza en cuestión estaba clavada
        if ((movimientosPosibles & reyPropio(_turno)) > 0) {
            res = operaciones_bit::setBit(res, casillaDeTorre, 1);
            return res;
        }
    }

    //Chequear si algún alfil clava a la pieza
    while (alfilesRivales > 0) {
        int casillaDeAlfil = operaciones_bit::LSB(alfilesRivales);
        U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaDeAlfil - 1];
        //Se chequea si la torre ataca en la dirección de la pieza a chequear. Si no lo hace, se pasa a la
        // siguiente torre

        if ((attackMaskAlfil & bitboardPiezaAChequear) == 0) {
            continue;
        }


        // Se calcula el bitboard de todas las piezas menos la pieza a chequear
        U64 bitboardSinPieza = todas_las_piezas() & ~(bitboardPiezaAChequear);

        // Se interseca la attack mask con el bitboard de todas las piezas menos la pieza a chequear
        U64 occupancyMask = attackMaskAlfil & bitboardSinPieza;

        // Se calcula la key de la posición generada y luego se obtienen los movimientos posibles
        U64 key = occupancyMask * constantes::magicsParaAlfil[casillaDeAlfil] >> (64 - 9);
        U64 movimientosPosibles = movimientosDeAlfil[casillaDeAlfil][key];

        // Se chequea si la casilla del rey está atacada. Si lo está, ya puede concluirse que
        // la pieza en cuestión estaba clavada
        if ((movimientosPosibles & reyPropio(_turno)) > 0) {
            operaciones_bit::setBit(res, casillaDeAlfil, 1);
            return res;
        }
    }

    //Chequear si alguna dama clava a la pieza
    while (damasRivales > 0) {
        int casillaDeDama = operaciones_bit::LSB(damasRivales);
        U64 attackMaskTorre = constantes::attackMasksTorre[casillaDeDama - 1];
        U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaDeDama - 1];
        U64 bitboardSinPieza = todas_las_piezas() & ~(bitboardPiezaAChequear);

        bool chequearAlfil = true;
        bool chequearTorre = true;
        //Caso torre


        //Se chequea si la torre ataca en la dirección de la pieza a chequear. Si no lo hace, se pasa al alfil

        if ((attackMaskTorre & bitboardPiezaAChequear) == 0) {
            chequearTorre = false;
        }
        if ((attackMaskAlfil & bitboardPiezaAChequear) == 0) {
            chequearAlfil = false;
        }
        if (chequearTorre) {

            // Se interseca la attack mask con el bitboard de todas las piezas menos la pieza a chequear
            U64 occupancyMask = attackMaskTorre & bitboardSinPieza;

            // Se calcula la key de la posición generada y luego se obtienen los movimientos posibles
            U64 key = occupancyMask * constantes::magicsParaTorre[casillaDeDama - 1] >> (64 - 12);
            U64 movimientosPosibles = movimientosDeTorre[casillaDeDama - 1][key];

            // Se chequea si la casilla del rey está atacada. Si lo está, ya puede concluirse que
            // la pieza en cuestión estaba clavada
            if ((movimientosPosibles & reyPropio(_turno)) > 0) {
                res = operaciones_bit::setBit(res, casillaDeDama, 1);
                return res;
            }
        } else if (chequearAlfil) {
            // Se interseca la attack mask con el bitboard de todas las piezas menos la pieza a chequear
            U64 occupancyMask = attackMaskAlfil & bitboardSinPieza;

            // Se calcula la key de la posición generada y luego se obtienen los movimientos posibles
            U64 key = occupancyMask * constantes::magicsParaAlfil[casillaDeDama - 1] >> (64 - 9);
            U64 movimientosPosibles = movimientosDeAlfil[casillaDeDama - 1][key];

            // Se chequea si la casilla del rey está atacada. Si lo está, ya puede concluirse que
            // la pieza en cuestión estaba clavada
            if ((movimientosPosibles & reyPropio(_turno)) > 0) {
                res = operaciones_bit::setBit(res, casillaDeDama, 1);
                return res;
            }
        }
    }
    //Si se llega hasta acá es porque la pieza no está clavada
    return res;
}

bool Tablero::esSlidingPiece(int tipoDePieza) {
    if (tipoDePieza == 1 | tipoDePieza == 2 | tipoDePieza == 3) {
        return true;
    } else {
        return false;
    }
}

U64 Tablero::obtenerAttackMask(int casillaDePieza, int tipoDePieza) {
    if (tipoDePieza == ALFIL) {
        return constantes::attackMasksAlfil[casillaDePieza - 1];
    } else if (tipoDePieza == TORRE) {
        return constantes::attackMasksTorre[casillaDePieza - 1];
    } else if (tipoDePieza == DAMA) {
        U64 attackMask = 0ULL;
        attackMask |= constantes::attackMasksTorre[casillaDePieza - 1] |
                      constantes::attackMasksAlfil[casillaDePieza - 1];
        return attackMask;
    } else {
        return 0;
    }
}

U64 Tablero::generarMovimientosPiezaClavada(int casillaPiezaClavada, int casillaPiezaClavadora) {
    int tipoDePiezaClavadora = obtenerTipoDePieza(casillaPiezaClavadora);
    int tipoDePiezaClavada = obtenerTipoDePieza(casillaPiezaClavada);
    U64 bitboardMovimientosPosibles = 0LL;

    //Si la pieza clavada es un caballo, no hay movimiento posible
    if (tipoDePiezaClavada == CABALLO) {
        return bitboardMovimientosPosibles;
    }

        //Si la pieza clavada es sliding piece, podría tener movimientos posibles
    else if (esSlidingPiece(tipoDePiezaClavada)) {

        //Si nos está clavando una torre, las únicas piezas que podrían tener movimientos
        // disponibles son otra torre o una dama
        if (tipoDePiezaClavadora == TORRE && (tipoDePiezaClavada == TORRE | tipoDePiezaClavada == DAMA)) {
            U64 attackMaskTorreClavadora = obtenerAttackMask(casillaPiezaClavadora, TORRE);
            U64 attackMaskPiezaClavada = obtenerAttackMask(casillaPiezaClavada, TORRE);
            U64 movimientosPiezaClavada;
            U64 piezaClavadoraBitboard = operaciones_bit::setBit(0L, casillaPiezaClavadora, 1);
            U64 occupancyMask = attackMaskPiezaClavada & todas_las_piezas();
            U64 key = occupancyMask * constantes::magicsParaTorre[casillaPiezaClavada - 1] >> (64 - 12);
            movimientosPiezaClavada = movimientosDeTorre[casillaPiezaClavada - 1][key];
            bitboardMovimientosPosibles |= (movimientosPiezaClavada & attackMaskTorreClavadora)
                                           | piezaClavadoraBitboard;
        }

            //Misma idea para el alfil
        else if (tipoDePiezaClavadora == ALFIL && (tipoDePiezaClavada == ALFIL | tipoDePiezaClavada == DAMA)) {
            U64 attackMaskAlfilClavador = obtenerAttackMask(casillaPiezaClavadora, ALFIL);
            U64 attackMaskPiezaClavada = obtenerAttackMask(casillaPiezaClavada, ALFIL);
            U64 movimientosPiezaClavada;
            U64 piezaClavadoraBitboard = operaciones_bit::setBit(0L, casillaPiezaClavadora, 1);
            U64 occupancyMask = attackMaskPiezaClavada & todas_las_piezas();
            U64 key = occupancyMask * constantes::magicsParaAlfil[casillaPiezaClavada - 1] >> (64 - 9);
            movimientosPiezaClavada = movimientosDeAlfil[casillaPiezaClavada - 1][key];
            bitboardMovimientosPosibles |= (movimientosPiezaClavada & attackMaskAlfilClavador)
                                           | piezaClavadoraBitboard;
        }

            //Si lo que nos clava es una dama hay que chequear si nos clava "como torre" o "como alfil", y
            // después generar los movimientos posibles en consecuencia
        else if (tipoDePiezaClavadora == DAMA) {
            if (tipoDePiezaClavada == TORRE) {
                U64 attackMaskTorreClavadora = obtenerAttackMask(casillaPiezaClavadora, TORRE);
                U64 attackMaskPiezaClavada = obtenerAttackMask(casillaPiezaClavada, TORRE);
                U64 movimientosPiezaClavada;
                U64 piezaClavadoraBitboard = operaciones_bit::setBit(0L, casillaPiezaClavadora, 1);
                U64 occupancyMask = attackMaskPiezaClavada & todas_las_piezas();
                U64 key = occupancyMask * constantes::magicsParaTorre[casillaPiezaClavada - 1] >> (64 - 12);
                movimientosPiezaClavada = movimientosDeTorre[casillaPiezaClavada - 1][key];
                bitboardMovimientosPosibles |= (movimientosPiezaClavada & attackMaskTorreClavadora)
                                               | piezaClavadoraBitboard;

            } else if (tipoDePiezaClavada == ALFIL) {
                U64 attackMaskAlfilClavador = obtenerAttackMask(casillaPiezaClavadora, ALFIL);
                U64 attackMaskPiezaClavada = obtenerAttackMask(casillaPiezaClavada, ALFIL);
                U64 movimientosPiezaClavada;
                U64 piezaClavadoraBitboard = operaciones_bit::setBit(0L, casillaPiezaClavadora, 1);
                U64 occupancyMask = attackMaskPiezaClavada & todas_las_piezas();
                U64 key = occupancyMask * constantes::magicsParaAlfil[casillaPiezaClavada - 1] >> (64 - 9);
                movimientosPiezaClavada = movimientosDeAlfil[casillaPiezaClavada - 1][key];
                bitboardMovimientosPosibles |= (movimientosPiezaClavada & attackMaskAlfilClavador)
                                               | piezaClavadoraBitboard;

                bitboardMovimientosPosibles |= attackMaskPiezaClavada | attackMaskAlfilClavador;
            } else {
                U64 attackMaskTorreClavadora = obtenerAttackMask(casillaPiezaClavadora, TORRE);
                U64 attackMaskAlfilClavador = obtenerAttackMask(casillaPiezaClavadora, ALFIL);
                U64 attackMaskPiezaClavadaTorre = obtenerAttackMask(casillaPiezaClavada, TORRE);
                U64 attackMaskPiezaClavadaAlfil = obtenerAttackMask(casillaPiezaClavada, ALFIL);
                U64 piezaClavadaBitboard = operaciones_bit::setBit(0L, casillaPiezaClavada, 1);
                if ((attackMaskTorreClavadora & piezaClavadaBitboard) > 0) {
                    U64 movimientosPiezaClavada;
                    U64 piezaClavadoraBitboard = operaciones_bit::setBit(0L, casillaPiezaClavadora, 1);
                    U64 occupancyMask = attackMaskPiezaClavadaTorre & todas_las_piezas();
                    U64 key = occupancyMask * constantes::magicsParaTorre[casillaPiezaClavada - 1] >> (64 - 12);
                    movimientosPiezaClavada = movimientosDeTorre[casillaPiezaClavada - 1][key];
                    bitboardMovimientosPosibles |= (movimientosPiezaClavada & attackMaskTorreClavadora)
                                                   | piezaClavadoraBitboard;
                } else if ((attackMaskAlfilClavador & piezaClavadaBitboard) > 0) {
                    U64 movimientosPiezaClavada;
                    U64 piezaClavadoraBitboard = operaciones_bit::setBit(0L, casillaPiezaClavadora, 1);
                    U64 occupancyMask = attackMaskPiezaClavadaAlfil & todas_las_piezas();
                    U64 key = occupancyMask * constantes::magicsParaAlfil[casillaPiezaClavada - 1] >> (64 - 9);
                    movimientosPiezaClavada = movimientosDeAlfil[casillaPiezaClavada - 1][key];
                    bitboardMovimientosPosibles |= (movimientosPiezaClavada & attackMaskAlfilClavador)
                                                   | piezaClavadoraBitboard;
                }
            }
        }
    }

        //Si la pieza clavada es un peón podría tener movimientos posibles
    else if (tipoDePiezaClavada == PEON) {

        //Si nos clava una torre, solo podríamos avanzar el peón
        if (tipoDePiezaClavadora == TORRE) {
            bitboardMovimientosPosibles = Peon::avances(casillaPiezaClavada, piezasPropias(), piezasRivales(), _turno);
        }

            //Si nos clava un alfil, solo podríamos capturarlo
        else if (tipoDePiezaClavadora == ALFIL) {
            bitboardMovimientosPosibles = Peon::capturas(casillaPiezaClavada, piezasPropias(), piezasRivales(), _turno);
        }

            //Si es una dama, hay que chequear si nos clava como torre o como alfil..
        else if (tipoDePiezaClavadora == DAMA) {
            U64 attackMaskAlfil = obtenerAttackMask(casillaPiezaClavadora, ALFIL);
            U64 attackMaskTorre = obtenerAttackMask(casillaPiezaClavadora, TORRE);
            U64 piezaClavadaBitboard = operaciones_bit::setBit(0L, casillaPiezaClavada, 1);
            if ((attackMaskTorre & piezaClavadaBitboard) > 0) {
                bitboardMovimientosPosibles = Peon::avances(casillaPiezaClavada, piezasPropias(), piezasRivales(),
                                                            _turno);

            } else {
                bitboardMovimientosPosibles = Peon::capturas(casillaPiezaClavada, piezasPropias(), piezasRivales(),
                                                             _turno);

            }
        }
    }

    return bitboardMovimientosPosibles;

}

bool Tablero::reyPropioEnJaque(int color) {

    U64 reyPropio = this->reyPropio(color);

    if (color == 0) {
        U64 bitboardDelRey = bitboards[0];

        U64 movimientosPosibles = bitboard_movimientos_torre_blanca(bitboardDelRey);
        if ((movimientosPosibles & bitboards[7]) || (movimientosPosibles & bitboards[8])) {
            return true;
        }
        movimientosPosibles = bitboard_movimientos_alfil_blanco(bitboardDelRey);
        if ((movimientosPosibles & bitboards[7]) || (movimientosPosibles & bitboards[9])) {
            return true;
        }

        movimientosPosibles = bitboard_movimientos_caballo_blanco(bitboardDelRey);
        if (movimientosPosibles & bitboards[10]) {
            return true;
        }

        /* U64 movimientosDamaNegra = bitboard_movimientos_dama_negra(bitboards[7]);
         if ((movimientosDamaNegra & reyPropio) > 0) {
             return true;
         }
         U64 movimientosTorreNegra = bitboard_movimientos_torre_negra(bitboards[8]);
         if ((movimientosTorreNegra & reyPropio) > 0) {
             return true;
         }
         U64 movimientosAlfilNegro = bitboard_movimientos_alfil_negro(bitboards[9]);
         if ((movimientosAlfilNegro & reyPropio) > 0) {
             return true;
         }*/

        if ((((bitboardDelRey & ~constantes::AFile) << 9) | ((bitboardDelRey & ~constantes::HFile) << 7)) &
            (bitboards[11])) {
            return true;
        }

        movimientosPosibles = bitboard_movimientos_rey_blanco(bitboardDelRey);

        if (movimientosPosibles & bitboards[6]) {
            return true;
        }

        /*U64 movimientosCaballoNegro = bitboard_movimientos_caballo_negro(bitboards[10]);

        if ((movimientosCaballoNegro & reyPropio) > 0) {
            return true;
        }

        U64 movimientosPeonNegro = bitboard_movimientos_peon_negro(bitboards[11]);

        if ((movimientosPeonNegro & reyPropio) > 0) {
            return true;
        }

        U64 movimientosReyNegro = bitboard_movimientos_rey_negro(bitboards[6]);
        if((movimientosReyNegro & reyPropio) > 0){
            return true;
        }*/

    } else {

        U64 bitboardDelRey = bitboards[6];

        U64 movimientosPosibles = bitboard_movimientos_torre_negra(bitboardDelRey);
        if ((movimientosPosibles & bitboards[1]) || (movimientosPosibles & bitboards[2])) {
            return true;
        }
        movimientosPosibles = bitboard_movimientos_alfil_negro(bitboardDelRey);
        if ((movimientosPosibles & bitboards[1]) || (movimientosPosibles & bitboards[3])) {
            return true;
        }

        movimientosPosibles = bitboard_movimientos_caballo_negro(bitboardDelRey);
        if (movimientosPosibles & bitboards[4]) {
            return true;
        }

        /*U64 movimientosDamaNegra = bitboard_movimientos_dama_negra(bitboards[7]);
        if ((movimientosDamaNegra & reyPropio) > 0) {
            return true;
        }
        U64 movimientosTorreNegra = bitboard_movimientos_torre_negra(bitboards[8]);
        if ((movimientosTorreNegra & reyPropio) > 0) {
            return true;
        }
        U64 movimientosAlfilNegro = bitboard_movimientos_alfil_negro(bitboards[9]);
        if ((movimientosAlfilNegro & reyPropio) > 0) {
            return true;
        }*/

        if ((((bitboardDelRey & ~constantes::HFile) >> 9) | ((bitboardDelRey & ~constantes::AFile) >> 7)) &
            (bitboards[5])) {
            return true;
        }

        movimientosPosibles = bitboard_movimientos_rey_negro(bitboardDelRey);

        if (movimientosPosibles & bitboards[0]) {
            return true;
        }

        /* U64 movimientosDamaBlanca = bitboard_movimientos_dama_blanca(bitboards[1]);

         if ((movimientosDamaBlanca & reyPropio) > 0) {
             return true;
         }

         U64 movimientosTorreBlanca = bitboard_movimientos_torre_blanca(bitboards[2]);

         if ((movimientosTorreBlanca & reyPropio) > 0) {
             return true;
         }

         U64 movimientosAlfilBlanco = bitboard_movimientos_alfil_blanco(bitboards[3]);

         if ((movimientosAlfilBlanco & reyPropio) > 0) {
             return true;
         }


         U64 movimientosCaballoBlanco = bitboard_movimientos_caballo_blanco(bitboards[4]);

         if ((movimientosCaballoBlanco & reyPropio) > 0) {
             return true;
         }

         U64 movimientosPeonBlanco = bitboard_movimientos_peon_blanco(bitboards[5]);

         if ((movimientosPeonBlanco & reyPropio) > 0) {
             return true;
         }

         U64 movimientosReyBlanco = bitboard_movimientos_rey_blanco(bitboards[0]);

         if((movimientosReyBlanco & reyPropio) > 0){
             return true;
         }*/


    }
    return false;
}

U64 Tablero::bitboard_movimientos_rey_negro(U64 bitboard) {
    U64 movimientosReyNegro = 0;
    int casillaDelRey = operaciones_bit::LSB(bitboard);
    movimientosReyNegro |= movimientosDeRey[casillaDelRey - 1];
    return movimientosReyNegro;

}

U64 Tablero::bitboard_movimientos_rey_blanco(U64 bitboard) {
    U64 movimientosReyBlanco = 0;
    int casillaDelRey = operaciones_bit::LSB(bitboard);
    movimientosReyBlanco |= movimientosDeRey[casillaDelRey - 1];
    return movimientosReyBlanco;

}


U64 Tablero::bitboard_movimientos_peon_blanco(U64 bitboard) {
    U64 movimientosPeonBlanco = 0;
    while (bitboard > 0) {
        int casillaPeonBlanco = operaciones_bit::LSB(bitboard);
        movimientosPeonBlanco |= Peon::capturas(casillaPeonBlanco, piezasPropias(), piezasRivales(), 0);
    }
    return movimientosPeonBlanco;
}

U64 Tablero::bitboard_movimientos_caballo_blanco(U64 bitboard) const {
    U64 movimientosCaballoBlanco = 0;
    while (bitboard > 0) {
        int casillaCaballoBlanco = operaciones_bit::LSB(bitboard);
        movimientosCaballoBlanco |= movimientosDeCaballo[casillaCaballoBlanco - 1];
    }
    return movimientosCaballoBlanco;
}

U64 Tablero::bitboard_movimientos_alfil_blanco(U64 bitboard) {
    U64 movimientosAlfilBlanco = 0;
    while (bitboard > 0) {
        int casillaAlfilBlanco = operaciones_bit::LSB(bitboard);
        U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaAlfilBlanco - 1];
        U64 occupancyMask = attackMaskAlfil & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaAlfil[casillaAlfilBlanco - 1] >> (64 - 9);
        movimientosAlfilBlanco |= movimientosDeAlfil[casillaAlfilBlanco - 1][key];
    }
    return movimientosAlfilBlanco;
}

U64 Tablero::bitboard_movimientos_torre_blanca(U64 bitboard) {
    U64 movimientosTorreBlanca = 0;
    while (bitboard > 0) {
        int casillaTorreBlanca = operaciones_bit::LSB(bitboard);
        U64 attackMaskTorre = constantes::attackMasksTorre[casillaTorreBlanca - 1];
        U64 occupancyMask = attackMaskTorre & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaTorre[casillaTorreBlanca - 1] >> (64 - 12);
        movimientosTorreBlanca |= movimientosDeTorre[casillaTorreBlanca - 1][key];
    }
    return movimientosTorreBlanca;
}

U64 Tablero::bitboard_movimientos_dama_blanca(U64 bitboard) {

    return bitboard_movimientos_alfil_blanco(bitboard) | bitboard_movimientos_torre_blanca(bitboard);
}

U64 Tablero::bitboard_movimientos_peon_negro(U64 bitboard) {
    U64 movimientosPeonNegro = 0;
    while (bitboard > 0) {
        int casillaPeonNegro = operaciones_bit::LSB(bitboard);
        movimientosPeonNegro |= Peon::capturas(casillaPeonNegro, piezasPropias(), piezasRivales(), 1);
    }
    return movimientosPeonNegro;
}

U64 Tablero::bitboard_movimientos_caballo_negro(U64 bitboard) const {
    U64 movimientosCaballoNegro = 0;
    while (bitboard > 0) {
        int casillaCaballoNegro = operaciones_bit::LSB(bitboard);
        U64 bitboardCaballo = operaciones_bit::setBit(0L, casillaCaballoNegro, 1);
        movimientosCaballoNegro |= movimientosDeCaballo[casillaCaballoNegro - 1];
    }
    return movimientosCaballoNegro;
}

U64 Tablero::bitboard_movimientos_alfil_negro(U64 bitboard) {
    U64 movimientosAlfilNegro = 0;
    while (bitboard > 0) {
        int casillaAlfilNegro = operaciones_bit::LSB(bitboard);
        U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaAlfilNegro - 1];
        U64 occupancyMask = attackMaskAlfil & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaAlfil[casillaAlfilNegro - 1] >> (64 - 9);
        movimientosAlfilNegro |= movimientosDeAlfil[casillaAlfilNegro - 1][key];
    }
    return movimientosAlfilNegro;
}

U64 Tablero::bitboard_movimientos_torre_negra(U64 bitboard) {
    U64 movimientosTorreNegra = 0;
    while (bitboard > 0) {
        int casillaTorreNegra = operaciones_bit::LSB(bitboard);
        U64 attackMaskTorre = constantes::attackMasksTorre[casillaTorreNegra - 1];
        U64 occupancyMask = attackMaskTorre & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaTorre[casillaTorreNegra - 1] >> (64 - 12);
        movimientosTorreNegra |= movimientosDeTorre[casillaTorreNegra - 1][key];
    }
    return movimientosTorreNegra;
}

U64 Tablero::obtenerAttackMap(int color) {
    if (color == 0) {
        return obtenerAttackMapBlancas();
    } else {
        return obtenerAttackMapNegras();
    }
}

U64 Tablero::obtenerAttackMapBlancas() {
    return bitboard_movimientos_rey_blanco(bitboards[0]) | bitboard_movimientos_dama_blanca(bitboards[1]) |
           bitboard_movimientos_torre_blanca(bitboards[2]) | bitboard_movimientos_alfil_blanco(bitboards[3]) |
           bitboard_movimientos_caballo_blanco(bitboards[4]) | bitboard_movimientos_peon_blanco(bitboards[5]);
}

U64 Tablero::obtenerAttackMapNegras() {
    return bitboard_movimientos_rey_negro(bitboards[6]) | bitboard_movimientos_dama_negra(bitboards[7]) |
           bitboard_movimientos_torre_negra(bitboards[8]) | bitboard_movimientos_alfil_negro(bitboards[9]) |
           bitboard_movimientos_caballo_negro(bitboards[10]) | bitboard_movimientos_peon_negro(bitboards[11]);
}

bool Tablero::chequearEnroqueCorto() {
    // enroque corto blancas
    if (_turno == 0) {

        if (enroqueCortoBlancas()) {
            U64 attackMapNegras = (obtenerAttackMapNegras());
            bool estaLaTorre = (bitboards[2] & 0b1ULL) > 0;
            bool reyEnJaque = reyPropioEnJaque(_turno);
            bool sinAtaqueRival = (attackMapNegras & 0b110ULL) == 0;
            bool zonaLiberada = (0b110ULL & todas_las_piezas()) == 0; // 110 es f1 y g1
            return sinAtaqueRival && zonaLiberada && !reyEnJaque && estaLaTorre;
        }

    } else {

        if (enroqueCortoNegras()) {
            U64 attackMapBlancas = (obtenerAttackMapBlancas());
            bool estaLaTorre = (bitboards[8] & 0x100000000000000) > 0;
            bool reyEnJaque = reyPropioEnJaque(_turno);
            bool sinAtaqueRival = (attackMapBlancas & 0x600000000000000ULL) == 0;
            bool zonaLiberada = (0x600000000000000ULL & todas_las_piezas()) == 0; // 110 es f8 y g8
            return sinAtaqueRival && zonaLiberada && !reyEnJaque && estaLaTorre;
        }
        // enroque largo blancas tas chispoton

    }
    return false;
}

bool Tablero::chequearEnroqueLargo() {
    // enroque largo blancas tas chispoton
    if (_turno == 0) {
        if (enroqueLargoBlancas()) {
            U64 attackMapNegras = (obtenerAttackMapNegras());
            bool estaLaTorre = (bitboards[2] & 0b10000000ULL) > 0;
            bool reyEnJaque = reyPropioEnJaque(_turno);
            bool sinAtaqueRival = ((attackMapNegras & 0b110000ULL) == 0);
            bool zonaLiberada = ((0b1110000ULL & todas_las_piezas()) == 0); // 1110000 es b1, c1 y d1
            return sinAtaqueRival && zonaLiberada && !reyEnJaque && estaLaTorre;
        }
    } else {

        if (enroqueLargoNegras()) {
            U64 attackMapBlancas = (obtenerAttackMapBlancas());
            bool estaLaTorre = (bitboards[8] & 0x8000000000000000ULL) > 0;
            bool reyEnJaque = reyPropioEnJaque(_turno);
            bool sinAtaqueRival = ((attackMapBlancas & 0x3000000000000000ULL) == 0);
            bool zonaLiberada = ((0x7000000000000000ULL & todas_las_piezas()) == 0); // 1110000 es b8, c8 y d8
            return sinAtaqueRival && zonaLiberada && !reyEnJaque && estaLaTorre;
        }
    }
    return false;
}

bool Tablero::enrocar(u_short jugada) {

    if (operaciones_bit::getLlegada(jugada) == 2 && chequearEnroqueCorto()) {
/*
        actualizarMaterial(jugada);
*/
        setearEnroque(0, false);

        /*enroqueCortoBlanco.push_back(false);*/
        /*enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());*/
        setearEnroque(1, false);
        guardarEnroque();
        /*enroqueLargoBlanco.push_back(false);*/
        contadorJugadas++;

        actualizarOcupacion(jugada);
        actualizarZobristKey(jugada);

        jugadas[contadorJugadas] = jugada;
        contadorHistorialBitboards++;
        modificacionBitboard cambios = {0,bitboards[0], 2, bitboards[2], -1, 0};
        historialBitboards[contadorHistorialBitboards] = cambios;
        bitboards[0] = operaciones_bit::setBit(bitboards[0], 4, 0);
        bitboards[0] = operaciones_bit::setBit(bitboards[0], 2, 1);
        bitboards[2] = operaciones_bit::setBit(bitboards[2], 1, 0);
        bitboards[2] = operaciones_bit::setBit(bitboards[2], 3, 1);
/*
        _jugadas.push_back(jugada);
*/


        historialEnPassant.push_back(-1);
        _turno = 1;


        enrocoBlancas = true;
        /*U64 zobristActual = zobristKey();
        U64 nuevaZobristKey = zobrist;


        if(nuevaZobristKey != zobristActual){
            cout << "Zobrist erronea" << endl;
            imprimirFen();
            cout << zobristActual << "-----" << nuevaZobristKey << endl;
            cout << constantes::NumeroACasilla[operaciones_bit::getSalida(jugada)] << endl;
            cout << constantes::NumeroACasilla[operaciones_bit::getLlegada(jugada)] << endl;
        exit(0);}*/

        return true;

    } else if (operaciones_bit::getLlegada(jugada) == 6 && chequearEnroqueLargo()) {
/*
        actualizarMaterial(jugada);
*/
        setearEnroque(0, false);
/*
        enroqueCortoBlanco.push_back(false);
*/
        /*enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());*/
        setearEnroque(1, false);
        guardarEnroque();
/*
        enroqueLargoBlanco.push_back(false);
*/        contadorJugadas++;

        actualizarOcupacion(jugada);
        actualizarZobristKey(jugada);

        contadorHistorialBitboards++;
        modificacionBitboard cambios = {0,bitboards[0], 2, bitboards[2], -1, 0};
        historialBitboards[contadorHistorialBitboards] = cambios;
        bitboards[0] = operaciones_bit::setBit(bitboards[0], 4, 0);
        bitboards[0] = operaciones_bit::setBit(bitboards[0], 6, 1);
        bitboards[2] = operaciones_bit::setBit(bitboards[2], 8, 0);
        bitboards[2] = operaciones_bit::setBit(bitboards[2], 5, 1);
/*
        _jugadas.push_back(jugada);
*/
        jugadas[contadorJugadas] = jugada;

        historialEnPassant.push_back(-1);
        _turno = 1;
        enrocoBlancas = true;
       /* U64 zobristActual = zobristKey();
        U64 nuevaZobristKey = zobrist;

       if(nuevaZobristKey != zobristActual){
            cout << "Zobrist erronea" << endl;
            imprimirFen();
            cout << zobristActual << "-----" << nuevaZobristKey << endl;
            cout << constantes::NumeroACasilla[operaciones_bit::getSalida(jugada)] << endl;
            cout << constantes::NumeroACasilla[operaciones_bit::getLlegada(jugada)] << endl;
        exit(0);}*/

        return true;
    } else if (operaciones_bit::getLlegada(jugada) == 58 && chequearEnroqueCorto()) {
/*
        actualizarMaterial(jugada);
*/
/*
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
*/

        setearEnroque(2, false);
        setearEnroque(3, false);
        guardarEnroque();
        /*enroqueCortoNegro.push_back(false);
        enroqueLargoNegro.push_back(false);*/
/*
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
*/
        contadorJugadas++;

        actualizarOcupacion(jugada);
        actualizarZobristKey(jugada);

        historialEnPassant.push_back(-1);

        contadorHistorialBitboards++;
        modificacionBitboard cambios = {6,bitboards[6], 8, bitboards[8], -1, 0};
        historialBitboards[contadorHistorialBitboards] = cambios;
        bitboards[6] = operaciones_bit::setBit(bitboards[6], 60, 0);
        bitboards[6] = operaciones_bit::setBit(bitboards[6], 58, 1);
        bitboards[8] = operaciones_bit::setBit(bitboards[8], 57, 0);
        bitboards[8] = operaciones_bit::setBit(bitboards[8], 59, 1);
/*
        _jugadas.push_back(jugada);
*/
        jugadas[contadorJugadas] = jugada;

        _turno = 0;

        enrocoNegras = true;
       /* U64 zobristActual = zobristKey();
        U64 nuevaZobristKey = zobrist;

        if(nuevaZobristKey != zobristActual){
            cout << "Zobrist erronea" << endl;
            imprimirFen();
            cout << zobristActual << "-----" << nuevaZobristKey << endl;
            cout << constantes::NumeroACasilla[operaciones_bit::getSalida(jugada)] << endl;
            cout << constantes::NumeroACasilla[operaciones_bit::getLlegada(jugada)] << endl;
        exit(0);}*/
        return true;
    } else if (operaciones_bit::getLlegada(jugada) == 62 && chequearEnroqueLargo()) {
/*
        actualizarMaterial(jugada);
*/
/*
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
*/
        /*enroqueCortoNegro.push_back(false);
        enroqueLargoNegro.push_back(false);*/
        setearEnroque(2, false);
        setearEnroque(3, false);
        guardarEnroque();
/*
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
*/
        contadorJugadas++;

        actualizarOcupacion(jugada);
        actualizarZobristKey(jugada);

        historialEnPassant.push_back(-1);

        contadorHistorialBitboards++;
        modificacionBitboard cambios = {6,bitboards[6], 8, bitboards[8], -1, 0};
        historialBitboards[contadorHistorialBitboards] = cambios;
        bitboards[6] = operaciones_bit::setBit(bitboards[6], 60, 0);
        bitboards[6] = operaciones_bit::setBit(bitboards[6], 62, 1);
        bitboards[8] = operaciones_bit::setBit(bitboards[8], 64, 0);
        bitboards[8] = operaciones_bit::setBit(bitboards[8], 61, 1);
/*
        _jugadas.push_back(jugada);
*/
        jugadas[contadorJugadas] = jugada;

        _turno = 0;

        enrocoNegras = true;

       /* U64 zobristActual = zobristKey();
        U64 nuevaZobristKey = zobrist;
        if(nuevaZobristKey != zobristActual){
            cout << "Zobrist erronea" << endl;
            imprimirFen();
            cout << zobristActual << "-----" << nuevaZobristKey << endl;
            cout << constantes::NumeroACasilla[operaciones_bit::getSalida(jugada)] << endl;
            cout << constantes::NumeroACasilla[operaciones_bit::getLlegada(jugada)] << endl;
        exit(0);}*/


        return true;
    }
    return false;
}

/*
void Tablero::moverPiezaTrusted(int salida, int llegada, int tipoDeJugada) {
    U64 salidaBitboard = operaciones_bit::setBit(0L, salida, 1);
    U64 llegadaBitboard = operaciones_bit::setBit(0L, llegada, 1);
    u_short aRealizar = operaciones_bit::crearJugada(salida, llegada, tipoDeJugada);


    int tipoDePieza = obtenerTipoDePieza(salida);
    if (tipoDePieza == REY) {
        if (salida == 4 && llegada == 2) {
            enrocar(45123);
            return;//Enroque corto codificado como u_short
        } else if (salida == 4 && llegada == 6) {
            enrocar(45379);

            return;
        } else if (salida == 60 && llegada == 58) {
            enrocar(48763);

            return;
        } else if (salida == 60 && llegada == 62) {
            enrocar(49019);

            return;
        }
    }


    actualizarMaterial(aRealizar);
    actualizarOcupacion(aRealizar);
    actualizarZobristKey(aRealizar);
    /// Momentánemente la función de evaluación no utiliza la cantidad
    /// de movimientos realizados por piezas menores, por lo que no vale la pena
    /// realizar el cálculo.
    //actualizarCantMovesPiezasMenores(aRealizar, true);
*/
/*
    historialDePosiciones.push_back(zobrist);
*//*


    for (int k = 0; k < 12; k++) {

        if ((bitboards[k] & salidaBitboard) > 0) {
            bitboards[k] = operaciones_bit::setBit(bitboards[k], salida, 0);
            bitboards[k] = operaciones_bit::setBit(bitboards[k], llegada, 1);

        } else if ((bitboards[k] & llegadaBitboard) > 0) {
            bitboards[k] = operaciones_bit::setBit(bitboards[k], llegada, 0);

        }
    }
    u_short jugada = operaciones_bit::crearJugada(salida, llegada, 0);
    _jugadas.push_back(jugada);
    historialPosiciones.push_back(bitboards);
   */
/* enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
    enroqueCortoNegro.push_back(enroqueCortoNegro.back());
    enroqueLargoNegro.push_back(enroqueLargoNegro.back());
    enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());*//*


    if (tipoDeJugada == CHECKMATE) {
        if (_turno == 0) {
            ganoBlanco = true;
        } else {
            ganoNegro = true;
        }

    }
    cambiarTurno();

}
*/

void Tablero::cambiarTurno() {
    if (_turno == 0) {
        _turno = 1;
    } else {
        _turno = 0;
    }
}

/*bool Tablero::ganoELBlanco() {
    if (reyPropioEnJaque(1)) {
        vector<u_short> movimientos = generar_movimientos(1);
        int contador = 0;
        for (u_short movimiento: movimientos) {
            if (contador > 0) {
                return false;
            }
            int salida = operaciones_bit::getSalida(movimiento);
            int llegada = operaciones_bit::getLlegada(movimiento);
            int tipoDeMovimiento = operaciones_bit::getTipoDeJugada(movimiento);
            if (moverPieza(salida, llegada, tipoDeMovimiento)) {
                contador++;
                deshacerMovimiento();
            }

        }
        if (contador == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool Tablero::ganoElNegro() {
    if (reyPropioEnJaque(0)) {
        vector<u_short> movimientos = generar_movimientos(0);
        int contador = 0;
        for (u_short movimiento: movimientos) {
            if (contador > 0) {
                return false;
            }
            int salida = operaciones_bit::getSalida(movimiento);
            int llegada = operaciones_bit::getLlegada(movimiento);
            int tipoDeMovimiento = operaciones_bit::getTipoDeJugada(movimiento);
            if (moverPieza(salida, llegada, tipoDeMovimiento)) {
                contador++;
                deshacerMovimiento();
            }

        }
        if (contador == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}*/


U64 Tablero::zobristKey() {
/*    if (zobrist != 0) {
        return zobrist;

    }*/
    U64 key = 0ULL;
    for (int i = 0; i < 12; i++) {
        U64 bitboard = bitboards[i];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            key ^= constantes::zobristKeys[i][casilla - 1];
        }
    }
    if (_turno == 1) {
        key ^= constantes::mueveElNegro;
    }
    if (enroqueLargoBlancas()) {
        key ^= constantes::enroqueLargoBlanco;
    }
    if (enroqueCortoBlancas()) {
        key ^= constantes::enroqueCortoBlanco;
    }
    if (enroqueLargoNegras()) {
        key ^= constantes::enroqueLargoNegro;
    }

    if (enroqueCortoNegras()) {
        key ^= constantes::enroqueCortoNegro;
    }
    /*if (!(_jugadas.empty()) && obtenerTipoDePieza(operaciones_bit::getLlegada(_jugadas.back())) == PEON) {
        int salida = operaciones_bit::getSalida(_jugadas.back());
        int llegada = operaciones_bit::getLlegada(_jugadas.back());
        int tipoDeMovimiento = operaciones_bit::getTipoDeJugada(_jugadas.back());
        if (tipoDeMovimiento == QUIET || tipoDeMovimiento == CHECK) {
            if (abs(salida - llegada) == 16) {
                int columnaDeEnpassant = (llegada % 8) - 1;
                if(columnaDeEnpassant == -1) {
                    key ^= constantes::enPassant[7];
                }
                else {
                    key ^= constantes::enPassant[columnaDeEnpassant];
                }


            }
        }
    }*/
    if (!historialEnPassant.empty() && historialEnPassant.back() != -1) {
        key ^= constantes::enPassant[historialEnPassant.back()];
    }



    return key;

}

void Tablero::generar_capturas(int turno, int ply) {
    vector<u_short> movimientos;
    if (turno == 0) {
        U64 torres = bitboards[2];
        while (torres > 0) {
            int casilla = operaciones_bit::LSB(torres);
            U64 movimientosDeLaTorre = bitboard_movimientos_torre_blanca(operaciones_bit::setBit(0L, casilla, 1));
            movimientosDeLaTorre = movimientosDeLaTorre & piezasRivales();
            while (movimientosDeLaTorre > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDeLaTorre);
                U64 bitboardMovimiento = operaciones_bit::setBit(0L, casillaAtacada, 1);
                /*if (esJaque(bitboard_movimientos_torre_blanca(bitboardMovimiento), turno)) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                       casillaAtacada,
                                                                                                       CAPTURECHECK);
                } else {*/
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                   casillaAtacada,
                                                                                                   CAPTURE);
                /*}*/
            }
        }

        U64 alfiles = bitboards[3];
        while (alfiles > 0) {
            int casilla = operaciones_bit::LSB(alfiles);
            U64 movimientosDelAlfil = bitboard_movimientos_alfil_blanco(operaciones_bit::setBit(0L, casilla, 1));
            movimientosDelAlfil = movimientosDelAlfil & piezasRivales();
            while (movimientosDelAlfil > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDelAlfil);
                U64 bitboardMovimiento = operaciones_bit::setBit(0L, casillaAtacada, 1);
                /*if (esJaque(bitboard_movimientos_alfil_blanco(bitboardMovimiento), turno)) {
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                       casillaAtacada,
                                                                                                       CAPTURECHECK);
                } else {*/
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                   casillaAtacada,
                                                                                                   CAPTURE);
                /*}*/
            }
        }

        U64 caballos = bitboards[4];
        while (caballos > 0) {
            int casilla = operaciones_bit::LSB(caballos);
            U64 movimientosDelCaballo = bitboard_movimientos_caballo_blanco(operaciones_bit::setBit(0L, casilla, 1));
            movimientosDelCaballo = movimientosDelCaballo & piezasRivales();
            while (movimientosDelCaballo > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDelCaballo);
                U64 bitboardMovimiento = operaciones_bit::setBit(0L, casillaAtacada, 1);
                /* if (esJaque(bitboard_movimientos_caballo_blanco(bitboardMovimiento), turno)) {
                     cantMovesGenerados[ply]++;
                     movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                        casillaAtacada,
                                                                                                        CAPTURECHECK);
                 } else {*/
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                   casillaAtacada,
                                                                                                   CAPTURE);
                /*}*/
            }
        }

        U64 damas = bitboards[1];

        while (damas > 0) {
            int casilla = operaciones_bit::LSB(damas);
            U64 movimientosDeLaDama = bitboard_movimientos_dama_blanca(operaciones_bit::setBit(0L, casilla, 1));
            movimientosDeLaDama = movimientosDeLaDama & piezasRivales();
            while (movimientosDeLaDama > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDeLaDama);
                U64 bitboardMovimiento = operaciones_bit::setBit(0L, casillaAtacada, 1);
                /* if (esJaque(bitboard_movimientos_dama_blanca(bitboardMovimiento), turno)) {
                     cantMovesGenerados[ply]++;
                     movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                        casillaAtacada,
                                                                                                        CAPTURECHECK);
                 } else {*/
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                   casillaAtacada,
                                                                                                   CAPTURE);
                /*}*/
            }
        }

        U64 peones = bitboards[5];

        while (peones > 0) {
            int casilla = operaciones_bit::LSB(peones);
            U64 movimientosDelPeon = bitboard_movimientos_peon_blanco(operaciones_bit::setBit(0L, casilla, 1));
            movimientosDelPeon = movimientosDelPeon & piezasRivales();
            while (movimientosDelPeon > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDelPeon);
                U64 bitboardMovimiento = operaciones_bit::setBit(0L, casillaAtacada, 1);
                U64 bitboardPeon = operaciones_bit::setBit(0L, casilla, 1);

                //Chequeamos si es una promoción

                if (casillaAtacada > 55) {
                    if (casillaAtacada == (casilla + 7)) {
                        /*if (esJaque(bitboard_movimientos_dama_blanca(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(1,
                                                                                                               casillaAtacada,
                                                                                                               CPDC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(1,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONDER);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_torre_blanca(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(2,
                                                                                                               casillaAtacada,
                                                                                                               CPDC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(2,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONDER);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_alfil_blanco(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(3,
                                                                                                               casillaAtacada,
                                                                                                               CPDC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(3,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONDER);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_caballo_blanco(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4,
                                                                                                               casillaAtacada,
                                                                                                               CPDC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONDER);
                        /*}*/
                    } else {
                        /*if (esJaque(bitboard_movimientos_dama_blanca(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(1,
                                                                                                               casillaAtacada,
                                                                                                               CPIC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(1,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONIZQ);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_torre_blanca(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(2,
                                                                                                               casillaAtacada,
                                                                                                               CPIC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(2,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONIZQ);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_alfil_blanco(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(3,
                                                                                                               casillaAtacada,
                                                                                                               CPIC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(3,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONIZQ);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_caballo_blanco(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4,
                                                                                                               casillaAtacada,
                                                                                                               CPIC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONIZQ);
                        /*}*/
                    }
                } else {
                    /* if (esJaque(Peon::capturas(casillaAtacada, piezasPropias(), piezasRivales(), 0), turno)) {
                         cantMovesGenerados[ply]++;
                         movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                            casillaAtacada,
                                                                                                            CAPTURECHECK);
                     } else {*/
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                       casillaAtacada,
                                                                                                       CAPTURE);
                    /*}*/
                }


            }

        }

        U64 rey = bitboards[0];
        int casilla = operaciones_bit::LSB(rey);
        U64 movimientosDelRey = bitboard_movimientos_rey_blanco(operaciones_bit::setBit(0L, casilla, 1));
        movimientosDelRey = movimientosDelRey & piezasRivales();
        while (movimientosDelRey > 0) {
            int casillaAtacada = operaciones_bit::LSB(movimientosDelRey);
            U64 bitboardMovimiento = operaciones_bit::setBit(0L, casillaAtacada, 1);
            cantMovesGenerados[ply]++;
            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla, casillaAtacada,
                                                                                               CAPTURE);

        }

    } else {
        U64 torres = bitboards[8];
        while (torres > 0) {
            int casilla = operaciones_bit::LSB(torres);
            U64 movimientosDeLaTorre = bitboard_movimientos_torre_negra(operaciones_bit::setBit(0L, casilla, 1));
            movimientosDeLaTorre = movimientosDeLaTorre & piezasRivales();
            while (movimientosDeLaTorre > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDeLaTorre);
                U64 bitboardMovimiento = operaciones_bit::setBit(0L, casillaAtacada, 1);
                /* if (esJaque(bitboard_movimientos_torre_negra(bitboardMovimiento), turno)) {
                     cantMovesGenerados[ply]++;
                     movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                        casillaAtacada,
                                                                                                        CAPTURECHECK);
                 } else {*/
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                   casillaAtacada,
                                                                                                   CAPTURE);
                /*}*/
            }
        }

        U64 alfiles = bitboards[9];

        while (alfiles > 0) {
            int casilla = operaciones_bit::LSB(alfiles);
            U64 movimientosDelAlfil = bitboard_movimientos_alfil_negro(operaciones_bit::setBit(0, casilla, 1));
            movimientosDelAlfil = movimientosDelAlfil & piezasRivales();
            while (movimientosDelAlfil > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDelAlfil);
                U64 bitboardMovimiento = operaciones_bit::setBit(0, casillaAtacada, 1);
                /* if (esJaque(bitboard_movimientos_alfil_negro(bitboardMovimiento), turno)) {
                     cantMovesGenerados[ply]++;
                     movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                        casillaAtacada,
                                                                                                        CAPTURECHECK);
                 } else {*/
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                   casillaAtacada,
                                                                                                   CAPTURE);
                /*}*/
            }
        }

        //Quiero que me sugieras como seguir con el codigo

        U64 caballos = bitboards[10];

        while (caballos > 0) {
            int casilla = operaciones_bit::LSB(caballos);
            U64 movimientosDelCaballo = bitboard_movimientos_caballo_negro(operaciones_bit::setBit(0, casilla, 1));
            movimientosDelCaballo = movimientosDelCaballo & piezasRivales();
            while (movimientosDelCaballo > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDelCaballo);
                U64 bitboardMovimiento = operaciones_bit::setBit(0, casilla, 1);
                /* if (esJaque(bitboard_movimientos_caballo_negro(bitboardMovimiento), turno)) {
                     cantMovesGenerados[ply]++;
                     movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                        casillaAtacada,
                                                                                                        CAPTURECHECK);
                 } else {*/
                cantMovesGenerados[ply]++;
                movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                   casillaAtacada,
                                                                                                   CAPTURE);
                /*}*/
            }


        }

        U64 peones = bitboards[11];

        while (peones > 0) {
            int casilla = operaciones_bit::LSB(peones);
            U64 movimientosDelPeon = bitboard_movimientos_peon_negro(operaciones_bit::setBit(0L, casilla, 1));
            movimientosDelPeon = movimientosDelPeon & piezasRivales();
            while (movimientosDelPeon > 0) {
                int casillaAtacada = operaciones_bit::LSB(movimientosDelPeon);
                U64 bitboardMovimiento = operaciones_bit::setBit(0L, casillaAtacada, 1);
                U64 bitboardPeon = operaciones_bit::setBit(0L, casilla, 1);

                //Chequeamos si es una promoción

                if (casillaAtacada < 9) {
                    if (casillaAtacada == (casilla - 7)) {
                        /*if (esJaque(bitboard_movimientos_dama_negra(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(1,
                                                                                                               casillaAtacada,
                                                                                                               CPDC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(1,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONDER);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_torre_negra(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(2,
                                                                                                               casillaAtacada,
                                                                                                               CPDC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(2,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONDER);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_alfil_negro(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(3,
                                                                                                               casillaAtacada,
                                                                                                               CPDC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(3,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONDER);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_caballo_negro(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4,
                                                                                                               casillaAtacada,
                                                                                                               CPDC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONDER);
                        /*}*/
                    } else {
                        /* if (esJaque(bitboard_movimientos_dama_negra(bitboardMovimiento), turno)) {
                             cantMovesGenerados[ply]++;
                             movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(1,
                                                                                                                casillaAtacada,
                                                                                                                CPIC);
                         } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(1,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONIZQ);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_torre_negra(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(2,
                                                                                                               casillaAtacada,
                                                                                                               CPIC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(2,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONIZQ);
                        /*}*/

                        /* if (esJaque(bitboard_movimientos_alfil_negro(bitboardMovimiento), turno)) {
                             cantMovesGenerados[ply]++;
                             movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(3,
                                                                                                                casillaAtacada,
                                                                                                                CPIC);
                         } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(3,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONIZQ);
                        /*}*/

                        /*if (esJaque(bitboard_movimientos_caballo_negro(bitboardMovimiento), turno)) {
                            cantMovesGenerados[ply]++;
                            movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4,
                                                                                                               casillaAtacada,
                                                                                                               CPIC);
                        } else {*/
                        cantMovesGenerados[ply]++;
                        movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(4,
                                                                                                           casillaAtacada,
                                                                                                           PROMOTIONIZQ);
                        /*}*/
                    }
                } else {
                    /* if (esJaque(Peon::capturas(casillaAtacada, piezasPropias(), piezasRivales(), 1), turno)) {
                         cantMovesGenerados[ply]++;
                         movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                            casillaAtacada,
                                                                                                            CAPTURECHECK);
                     } else {*/
                    cantMovesGenerados[ply]++;
                    movimientos_generados[ply][cantMovesGenerados[ply]] = operaciones_bit::crearJugada(casilla,
                                                                                                       casillaAtacada,
                                                                                                       CAPTURE);
                    /*}*/
                }


            }

        }


    }


}

/*float Tablero:: controlDeCentroCaballo(int color) {
    float valor = 0;
    if(color == 0){
        U64 bitboardCaballo = bitboards[4];
        while(bitboardCaballo > 0){
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            U64 attackMapCaballo = movimientosDeCaballo[casilla-1];
            while(attackMapCaballo > 0){
                int casillaAtacada = operaciones_bit::LSB(attackMapCaballo);
                valor += constantes::casillasCentralesCaballoBlanco[casillaAtacada-1];
            }
        }
    }
    else{
        U64 bitboardCaballo = bitboards[10];
        while(bitboardCaballo > 0){
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            U64 attackMapCaballo = movimientosDeCaballo[casilla-1];
            while(attackMapCaballo > 0){
                int casillaAtacada = operaciones_bit::LSB(attackMapCaballo);
                valor -= constantes::casillasCentralesCaballoNegro[casillaAtacada-1];
            }
        }
    }
    return 1.7*valor;
}

float Tablero:: controlDeCentroAlfil(int color){
    float valor = 0;
    if(color == 0){
        U64 bitboardAlfil = bitboards[3];
        while(bitboardAlfil > 0){
            int casilla = operaciones_bit::LSB(bitboardAlfil);

            U64 attackMapAlfil = constantes::attackMasksAlfil[casilla-1];
            U64 occupancy =  todas_las_piezas() & attackMapAlfil;
            U64 magic = constantes::magicsParaAlfil[casilla-1];
            U64 key = (occupancy * magic) >> (64 - 9);
            attackMapAlfil = Tablero::movimientosDeAlfil[casilla-1][key];
            while(attackMapAlfil > 0){
                int casillaAtacada = operaciones_bit::LSB(attackMapAlfil);
                valor += constantes::casillasCentralesAlfilBlanco[casillaAtacada-1];
            }
        }
    }
    else{
        U64 bitboardAlfil = bitboards[9];
        while(bitboardAlfil > 0){
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            U64 attackMapAlfil = constantes::attackMasksAlfil[casilla-1];
            U64 occupancy =  todas_las_piezas() & attackMapAlfil;
            U64 magic = constantes::magicsParaAlfil[casilla-1];
            U64 key = (occupancy * magic) >> (64 - 9);
            attackMapAlfil = Tablero::movimientosDeAlfil[casilla-1][key];
            while(attackMapAlfil > 0){
                int casillaAtacada = operaciones_bit::LSB(attackMapAlfil);
                valor -= constantes::casillasCentralesAlfilNegro[casillaAtacada-1];
            }
        }
    }
    return 2*valor;
}

float Tablero:: controlDeCentroPeon(int color){
    float valor = 0;
    if(color == 0){
        U64 bitboardPeon = bitboards[5];
        while(bitboardPeon > 0){
            int casilla = operaciones_bit::LSB(bitboardPeon);
            U64 attackMapPeon = Peon::capturas(casilla, 0, 0, 0);
            while(attackMapPeon > 0){
                int casillaAtacada = operaciones_bit::LSB(attackMapPeon);
                valor += constantes::casillasCentralesPeonBlanco[casillaAtacada-1];
            }
        }
    }
    else{
        U64 bitboardPeon = bitboards[11];
        while(bitboardPeon > 0){
            int casilla = operaciones_bit::LSB(bitboardPeon);
            U64 attackMapPeon = Peon::capturas(casilla, 0, 0, 1);
            while(attackMapPeon > 0){
                int casillaAtacada = operaciones_bit::LSB(attackMapPeon);
                valor -= constantes::casillasCentralesPeonNegro[casillaAtacada-1];
            }
        }
    }
    return 1.5*valor;
}

float Tablero:: controlDeCentro(int color){
    return (controlDeCentroAlfil( color) + controlDeCentroCaballo( color) + controlDeCentroPeon( color));
}*/

void Tablero::calcularOcupacionAlfil(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardAlfil = bitboards[3];
        while (bitboardAlfil > 0) {
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            valor += constantes::ocupacionAlfil[casilla - 1];
        }
        ocupacionPiezas[3] = valor;
    } else {
        U64 bitboardAlfil = bitboards[9];
        while (bitboardAlfil > 0) {
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            valor -= constantes::ocupacionAlfil[operaciones_bit::espejarCasilla(casilla)];
        }
        ocupacionPiezas[9] = valor;

    }

}

void Tablero::calcularOcupacionCaballo(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardCaballo = bitboards[4];
        while (bitboardCaballo > 0) {
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            valor += constantes::ocupacionCaballo[casilla - 1];
        }
        ocupacionPiezas[4] = valor;

    } else {
        U64 bitboardCaballo = bitboards[10];
        while (bitboardCaballo > 0) {
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            valor -= constantes::ocupacionCaballo[operaciones_bit::espejarCasilla(casilla)];
        }
        ocupacionPiezas[10] = valor;

    }
}

void Tablero::calcularOcupacionPeon(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardPeon = bitboards[5];
        while (bitboardPeon > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeon);
            valor += constantes::ocupacionPeon[casilla - 1];
        }
        ocupacionPiezas[5] = valor;
    } else {
        U64 bitboardPeon = bitboards[11];
        while (bitboardPeon > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeon);
            valor -= constantes::ocupacionPeon[operaciones_bit::espejarCasilla(casilla)];
        }
        ocupacionPiezas[11] = valor;

    }
}

void  Tablero::calcularOcupacionTorre(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardTorre = bitboards[2];
        while (bitboardTorre > 0) {
            int casilla = operaciones_bit::LSB(bitboardTorre);
            valor += constantes::ocupacionTorre[casilla - 1];
        }
        ocupacionPiezas[2] = valor;
    } else {
        U64 bitboardTorre = bitboards[8];
        while (bitboardTorre > 0) {
            int casilla = operaciones_bit::LSB(bitboardTorre);
            valor -= constantes::ocupacionTorre[operaciones_bit::espejarCasilla(casilla)];
        }
        ocupacionPiezas[8] = valor;
    }
}

void  Tablero::calcularOcupacionReina(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardReina = bitboards[1];
        while (bitboardReina > 0) {
            int casilla = operaciones_bit::LSB(bitboardReina);
            valor += constantes::ocupacionReina[casilla - 1];
        }
        ocupacionPiezas[1] = valor;
    } else {
        U64 bitboardReina = bitboards[7];
        while (bitboardReina > 0) {
            int casilla = operaciones_bit::LSB(bitboardReina);
            valor -= constantes::ocupacionReina[operaciones_bit::espejarCasilla(casilla)];
        }
        ocupacionPiezas[7] = valor;
    }
}

void  Tablero::calcularOcupacionRey(int color) {
    float valorMedio = 0;
    float valorFinal = 0;
    if (color == 0) {
        U64 bitboardRey = bitboards[0];
        while (bitboardRey > 0) {
            int casilla = operaciones_bit::LSB(bitboardRey);
                valorFinal += constantes::ocupacionReyFinal[casilla - 1];
                valorMedio += constantes::ocupacionReyMedioJuego[casilla - 1];
            ocupacionPiezas[0] = valorMedio;
            ocupacionPiezas[12] = valorFinal;
        }
    } else {
        U64 bitboardRey = bitboards[6];
        while (bitboardRey > 0) {
            int casilla = operaciones_bit::LSB(bitboardRey);

                valorFinal-= constantes::ocupacionReyFinal[operaciones_bit::espejarCasilla(casilla)];
                valorMedio -= constantes::ocupacionReyMedioJuego[operaciones_bit::espejarCasilla(casilla)];
            ocupacionPiezas[6] = valorMedio;
            ocupacionPiezas[13] = valorFinal;
        }
    }
}

void Tablero::calcularOcupacion() {
            calcularOcupacionAlfil(0);
            calcularOcupacionCaballo(0);
            calcularOcupacionPeon(0);
            calcularOcupacionReina(0);
            calcularOcupacionTorre(0);
            calcularOcupacionRey(0);
            calcularOcupacionAlfil(1);
            calcularOcupacionCaballo(1);
            calcularOcupacionPeon(1);
            calcularOcupacionReina(1);
            calcularOcupacionTorre(1);
            calcularOcupacionRey(1);

}

float Tablero::valoracionMaterial(int color) {
    float valor = 0;
    valor = contarMaterialSinPeones(color);
    if (contarMaterialSinPeones(0) <= 900 && contarMaterialSinPeones(1) >= -900) {
        endgame = true;
    }
    if((bitboards[1] == 0) && (bitboards[7] == 0)){
        endgame = true;
    }

    if (color == 0) {
        U64 bitboardPeonesBlancos = bitboards[5];
        while (bitboardPeonesBlancos > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeonesBlancos);
            valor += constantes::valorPieza[5];
        }
    } else {
        U64 bitboardPeonesNegros = bitboards[11];
        while (bitboardPeonesNegros > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeonesNegros);
            valor += constantes::valorPieza[11];
        }
    }

    return valor;
}

float Tablero::contarMaterialSinPeones(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboard = bitboards[0];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[0];
        }
        bitboard = bitboards[1];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[1];
        }
        bitboard = bitboards[2];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[2];
        }
        bitboard = bitboards[3];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[3];
        }
        bitboard = bitboards[4];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[4];
        }
    } else {
        U64 bitboard = bitboards[6];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[6];
        }
        bitboard = bitboards[7];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[7];
        }
        bitboard = bitboards[8];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[8];
        }
        bitboard = bitboards[9];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[9];
        }
        bitboard = bitboards[10];
        while (bitboard > 0) {
            int casilla = operaciones_bit::LSB(bitboard);
            valor += constantes::valorPieza[10];
        }
    }

    return valor;
}

void Tablero::actualizarOcupacion(u_short jugada) {

    int salida = operaciones_bit::getSalida(jugada);
    int llegada = operaciones_bit::getLlegada(jugada);
    int tipoDePieza = obtenerTipoDePieza(salida);
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);

    //Aca estamos usando una estructura que guarda 3 pairs que denotan los 3 posibles cambios.
    //Para evitar codigo engorroso, a la hora de registrar cambios en la pieza que se movio, directamente
    //lo asignamos al cambio numero 2, ya que hay posibilidad de que el cambio 1 ya este asignado

    cambiosOcupacion cambios = cambiosOcupacion();
    if (_turno == 0) {

        if (tipoDeJugada == ENPASSANT) {
            cambios.cambio1.first = 5;
            //Si la jugada fue ENPASSANT, se elimina el valor de ocupacion del peon rival y se
            //actualiza el valor de ocupacion del peon propio
            cambios.cambio1.second = (constantes::ocupacionPeon[llegada - 1]
                                 - constantes::ocupacionPeon[salida - 1]);
            ocupacionPiezas[5] += cambios.cambio1.second;

            //"Eliminar" en este contexto es simplemente sumar el valor, ya que para las negras los
            //valores de ocupación favorables son negativos
            cambios.cambio2.first = 11;
            cambios.cambio2.second = constantes::ocupacionPeon[operaciones_bit::espejarCasilla(llegada - 8)];
            ocupacionPiezas[11] += cambios.cambio2.second;
            historialCambiosOcupacion[contadorJugadas] = cambios;
            return;
        } else if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
            //Si la última jugada fue una promoción sin captura, se elimina el
            // valor de ocupación del peón coronado
            cambios.cambio1.first = 5;
            cambios.cambio1.second = -constantes::ocupacionPeon[llegada - 9];
            ocupacionPiezas[5] += cambios.cambio1.second;
        } else if (tipoDeJugada == CASTLING) {
            //Si la última jugada fue un enroque, se actualiza la ocupación de las torres
            cambios.cambio1.first = 2;
            if (llegada == 2) {
                cambios.cambio1.second = (constantes::ocupacionTorre[2]
                                     - constantes::ocupacionTorre[0]);
                ocupacionPiezas[2] += cambios.cambio1.second;
            } else {
                cambios.cambio1.second = (constantes::ocupacionTorre[4]
                                     - constantes::ocupacionTorre[7]);
                ocupacionPiezas[2] += cambios.cambio1.second;

            }

        } else if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
                   tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
                   tipoDeJugada == CPDC || tipoDeJugada == CPIC) {
            //Si la última jugada fue una captura, se elimina el valor de ocupación de la pieza capturada
            int piezaCapturada = obtenerTipoDePieza(llegada);
            if (piezaCapturada == DAMA) {
                cambios.cambio1.first = 7;
                 cambios.cambio1.second = constantes::ocupacionReina[operaciones_bit::espejarCasilla(llegada)];
                ocupacionPiezas[7] += cambios.cambio1.second;

            } else if (piezaCapturada == TORRE) {
                cambios.cambio1.first = 8;
                cambios.cambio1.second = constantes::ocupacionTorre[operaciones_bit::espejarCasilla(llegada)];
                ocupacionPiezas[8] += cambios.cambio1.second;

            } else if (piezaCapturada == ALFIL) {
                cambios.cambio1.first = 9;
                cambios.cambio1.second= constantes::ocupacionAlfil[operaciones_bit::espejarCasilla(llegada)];
                ocupacionPiezas[9] += cambios.cambio1.second;
            } else if (piezaCapturada == CABALLO) {
                cambios.cambio1.first = 10;
                cambios.cambio1.second= constantes::ocupacionCaballo[operaciones_bit::espejarCasilla(llegada)];
                ocupacionPiezas[10] += cambios.cambio1.second;
            } else if (piezaCapturada == PEON) {
                cambios.cambio1.first = 11;
                cambios.cambio1.second= constantes::ocupacionPeon[operaciones_bit::espejarCasilla(llegada)];
                ocupacionPiezas[11] += cambios.cambio1.second;
            }

        }
        bool esPromocion = false;
        if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK || tipoDeJugada == CPDC
            || tipoDeJugada == CPIC || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER) {

            tipoDePieza = obtenerTipoDePieza(salida);
            if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
                salida = llegada - 8;
            } else if (tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == CPIC) {
                salida = llegada - 9;
            } else if (tipoDeJugada == PROMOTIONDER || tipoDeJugada == CPDC) {
                salida = llegada - 7;
            }
            esPromocion = true;
        }
        //Se agrega ahora el valor de ocupación de la pieza movida en la casilla nueva
        if (tipoDePieza == REY) {

            cambios.cambio2.first = 0;
            cambios.cambio2.second = (constantes::ocupacionReyMedioJuego[llegada - 1]
                                     - constantes::ocupacionReyMedioJuego[salida - 1]);
            ocupacionPiezas[0] += cambios.cambio2.second;


            cambios.cambio3.first = 12;
            cambios.cambio3.second = (constantes::ocupacionReyFinal[llegada - 1]
                                     - constantes::ocupacionReyFinal[salida - 1]);
            ocupacionPiezas[12] += cambios.cambio3.second;


        } else if (tipoDePieza == DAMA) {
            cambios.cambio2.first = 1;
            cambios.cambio2.second = constantes::ocupacionReina[llegada - 1];
            ocupacionPiezas[1] += cambios.cambio2.second;
            if (!esPromocion) {
                cambios.cambio3.first = 1;
                cambios.cambio3.second  = -constantes::ocupacionReina[salida - 1];
                ocupacionPiezas[1] += cambios.cambio3.second;
            }
            else if ((tipoDeJugada != PROMOTION) && (tipoDeJugada != PROMOTIONCHECK)){
                cambios.cambio3.first = 5;
                cambios.cambio3.second = -constantes::ocupacionPeon[salida - 1];
                ocupacionPiezas[5] += cambios.cambio3.second;
            }
        } else if (tipoDePieza == TORRE) {
            cambios.cambio2.first = 2;
            cambios.cambio2.second = constantes::ocupacionTorre[llegada - 1];
            ocupacionPiezas[2] += cambios.cambio2.second;
            if (!esPromocion) {
                cambios.cambio3.first = 2;
                cambios.cambio3.second  =  -constantes::ocupacionTorre[salida - 1];
                ocupacionPiezas[2] += cambios.cambio3.second;


            }
            else if ((tipoDeJugada != PROMOTION) && (tipoDeJugada != PROMOTIONCHECK)){
                cambios.cambio3.first = 5;
                cambios.cambio3.second = -constantes::ocupacionPeon[salida - 1];
                ocupacionPiezas[5] += cambios.cambio3.second;

            }
        } else if (tipoDePieza == ALFIL) {
            cambios.cambio2.first = 3;
            cambios.cambio2.second = constantes::ocupacionAlfil[llegada - 1];
            ocupacionPiezas[3] += cambios.cambio2.second;
            if (!esPromocion) {
                cambios.cambio3.first = 3;
                cambios.cambio3.second = -constantes::ocupacionAlfil[salida - 1];
                ocupacionPiezas[3] += cambios.cambio3.second;
            }
            else if ((tipoDeJugada != PROMOTION) && (tipoDeJugada != PROMOTIONCHECK)){
                cambios.cambio3.first = 5;
                cambios.cambio3.second = -constantes::ocupacionPeon[salida - 1];
                ocupacionPiezas[5] += cambios.cambio3.second;

            }
        } else if (tipoDePieza == CABALLO) {
            cambios.cambio2.first = 4;
            cambios.cambio2.second = constantes::ocupacionCaballo[llegada - 1];
            ocupacionPiezas[4] += cambios.cambio2.second;

            if (!esPromocion) {
                cambios.cambio3.first = 4;
                cambios.cambio3.second = -constantes::ocupacionCaballo[salida - 1];
                ocupacionPiezas[4] += cambios.cambio3.second;
            }
            else if ((tipoDeJugada != PROMOTION) && (tipoDeJugada != PROMOTIONCHECK)){
                cambios.cambio3.first = 5;
                cambios.cambio3.second = -constantes::ocupacionPeon[salida - 1];
                ocupacionPiezas[5] += cambios.cambio3.second;

            }
        } else if (tipoDePieza == PEON) {
            cambios.cambio2.first = 5;
            cambios.cambio2.second  = (constantes::ocupacionPeon[llegada - 1]
                                 - constantes::ocupacionPeon[salida - 1]);
            ocupacionPiezas[5] += cambios.cambio2.second;
        }

    } else {
        if (tipoDeJugada == ENPASSANT) {
            //Si la jugada fue ENPASSANT, se elimina el valor de ocupacion del peon rival y se
            //actualiza el valor de ocupacion del peon propio
            cambios.cambio1.first = 11;
            cambios.cambio1.second = -((constantes::ocupacionPeon[operaciones_bit::espejarCasilla(llegada)]
                                - constantes::ocupacionPeon[operaciones_bit::espejarCasilla(salida)]));
            ocupacionPiezas[11] += cambios.cambio1.second;

            //"Eliminar" en este contexto es simplemente restar el valor, ya que para las blancas los
            //valores de ocupación favorables son positivos
            cambios.cambio2.first = 5;
            cambios.cambio2.second = -constantes::ocupacionPeon[llegada + 7];
            ocupacionPiezas[5] += cambios.cambio2.second;

            historialCambiosOcupacion[contadorJugadas] = cambios;


            return;
        } else if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
            //Si la última jugada fue una promoción sin captura, se elimina el
            // valor de ocupación del peón coronado
            cambios.cambio1.first = 11;
            cambios.cambio1.second = constantes::ocupacionPeon[operaciones_bit::espejarCasilla(llegada + 8)];
            ocupacionPiezas[11] += cambios.cambio1.second;
        } else if (tipoDeJugada == CASTLING) {
            //Si la última jugada fue un enroque, se actualiza la ocupación de las torres
            cambios.cambio1.first = 8;
            if (llegada == 58) {
                cambios.cambio1.second = -((constantes::ocupacionTorre[2]
                                    - constantes::ocupacionTorre[0]));
                ocupacionPiezas[8] += cambios.cambio1.second;
            } else {
                cambios.cambio1.second = -((constantes::ocupacionTorre[4]
                                    - constantes::ocupacionTorre[7]));
                ocupacionPiezas[8] += cambios.cambio1.second;
            }

        } else if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
                   tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
                   tipoDeJugada == CPDC || tipoDeJugada == CPIC) {
            //Si la última jugada fue una captura, se elimina el valor de ocupación de la pieza capturada
            int piezaCapturada = obtenerTipoDePieza(llegada);
            if (piezaCapturada == DAMA) {
                cambios.cambio1.first = 1;
                cambios.cambio1.second = -constantes::ocupacionReina[llegada - 1];
                ocupacionPiezas[1] += cambios.cambio1.second;
            } else if (piezaCapturada == TORRE) {
                cambios.cambio1.first = 2;
                cambios.cambio1.second = -constantes::ocupacionTorre[llegada - 1];
                ocupacionPiezas[2] += cambios.cambio1.second;
            } else if (piezaCapturada == ALFIL) {
                cambios.cambio1.first = 3;
                cambios.cambio1.second = -constantes::ocupacionAlfil[llegada - 1];
                ocupacionPiezas[3] += cambios.cambio1.second;
            } else if (piezaCapturada == CABALLO) {
                cambios.cambio1.first = 4;
                cambios.cambio1.second = - constantes::ocupacionCaballo[llegada - 1];
                ocupacionPiezas[4] += cambios.cambio1.second;
            } else if (piezaCapturada == PEON) {
                cambios.cambio1.first = 5;
                cambios.cambio1.second = - constantes::ocupacionPeon[llegada - 1];
                ocupacionPiezas[5] += cambios.cambio1.second;
            }
        }

        bool esPromocion = false;
        if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK || tipoDeJugada == CPDC
            || tipoDeJugada == CPIC || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER) {

            tipoDePieza = obtenerTipoDePieza(salida);
            if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
                salida = llegada + 8;
            } else if (tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == CPIC) {
                salida = llegada + 9;
            } else if (tipoDeJugada == PROMOTIONDER || tipoDeJugada == CPDC) {
                salida = llegada + 7;
            }
            esPromocion = true;
        }

        //Se agrega ahora el valor de ocupación de la pieza movida en la casilla nueva
        if (tipoDePieza == REY) {
                cambios.cambio2.first = 6;
                cambios.cambio2.second = -((
                        constantes::ocupacionReyMedioJuego[operaciones_bit::espejarCasilla(llegada)]
                        - constantes::ocupacionReyMedioJuego[operaciones_bit::espejarCasilla(salida)]));
            ocupacionPiezas[6] += cambios.cambio2.second;
            cambios.cambio3.first = 13;
            cambios.cambio3.second = -((constantes::ocupacionReyFinal[operaciones_bit::espejarCasilla(llegada)]
                                    - constantes::ocupacionReyFinal[operaciones_bit::espejarCasilla(salida)]));
            ocupacionPiezas[13] += cambios.cambio3.second;

        } else if (tipoDePieza == DAMA) {
            cambios.cambio2.first = 7;
            cambios.cambio2.second = -constantes::ocupacionReina[operaciones_bit::espejarCasilla(llegada)];
            ocupacionPiezas[7] += cambios.cambio2.second;
            if (!esPromocion) {
                cambios.cambio3.first = 7;
                cambios.cambio3.second = constantes::ocupacionReina[operaciones_bit::espejarCasilla(salida)];
                ocupacionPiezas[7] += cambios.cambio3.second;
            }
            else if ((tipoDeJugada != PROMOTION) && (tipoDeJugada != PROMOTIONCHECK)){
                cambios.cambio3.first = 11;
                cambios.cambio3.second = constantes::ocupacionPeon[operaciones_bit::espejarCasilla(salida)];
                ocupacionPiezas[11] += cambios.cambio3.second;

            }
        } else if (tipoDePieza == TORRE) {
            cambios.cambio2.first = 8;
            cambios.cambio2.second = -constantes::ocupacionTorre[operaciones_bit::espejarCasilla(llegada)];
            ocupacionPiezas[8] += cambios.cambio2.second;
            if (!esPromocion) {
                cambios.cambio3.first = 8;
                cambios.cambio3.second = constantes::ocupacionTorre[operaciones_bit::espejarCasilla(salida)];
                ocupacionPiezas[8] += cambios.cambio3.second;
            }
            else if ((tipoDeJugada != PROMOTION) && (tipoDeJugada != PROMOTIONCHECK)){
                cambios.cambio3.first = 11;
                cambios.cambio3.second = constantes::ocupacionPeon[operaciones_bit::espejarCasilla(salida)];
                ocupacionPiezas[11] += cambios.cambio3.second;

            }
        } else if (tipoDePieza == ALFIL) {
            cambios.cambio2.first = 9;
            cambios.cambio2.second = -constantes::ocupacionAlfil[operaciones_bit::espejarCasilla(llegada)];
            ocupacionPiezas[9] += cambios.cambio2.second;
            if (!esPromocion) {
                cambios.cambio3.first = 9;
                cambios.cambio3.second = constantes::ocupacionAlfil[operaciones_bit::espejarCasilla(salida)];
                ocupacionPiezas[9] += cambios.cambio3.second;
            }
            else if ((tipoDeJugada != PROMOTION) && (tipoDeJugada != PROMOTIONCHECK)){
                cambios.cambio3.first = 11;
                cambios.cambio3.second = constantes::ocupacionPeon[operaciones_bit::espejarCasilla(salida)];
                ocupacionPiezas[11] += cambios.cambio3.second;

            }
        } else if (tipoDePieza == CABALLO) {
            cambios.cambio2.first = 10;
            cambios.cambio2.second = -constantes::ocupacionCaballo[operaciones_bit::espejarCasilla(llegada)];
            ocupacionPiezas[10] += cambios.cambio2.second;
            if (!esPromocion) {
                cambios.cambio3.first = 10;
                cambios.cambio3.second = constantes::ocupacionCaballo[operaciones_bit::espejarCasilla(salida)];
                ocupacionPiezas[10] += cambios.cambio3.second;
            }
            else if ((tipoDeJugada != PROMOTION) && (tipoDeJugada != PROMOTIONCHECK)){
                cambios.cambio3.first = 11;
                cambios.cambio3.second = constantes::ocupacionPeon[operaciones_bit::espejarCasilla(salida)];
                ocupacionPiezas[11] += cambios.cambio3.second;

            }

        } else if (tipoDePieza == PEON) {
            cambios.cambio2.first = 11;
            cambios.cambio2.second = -((constantes::ocupacionPeon[operaciones_bit::espejarCasilla(llegada)]
                                - constantes::ocupacionPeon[operaciones_bit::espejarCasilla(salida)]));
            ocupacionPiezas[11] += cambios.cambio2.second;
        }

    }

    historialCambiosOcupacion[contadorJugadas] = cambios;


}

/*void Tablero::actualizarMaterial(u_short jugada) {
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    if(tipoDeJugada == CASTLING || tipoDeJugada == QUIET || tipoDeJugada == CHECK){
        return;
    }
*//*    int material_blancas = historial_material_blancas[contadorMaterialBlancas].first;
    int material_negras = historial_material_negras[contadorMaterialNegras].first;*//*
    int salida = operaciones_bit::getSalida(jugada);
    int llegada = operaciones_bit::getLlegada(jugada);
    int tipoDePieza = obtenerTipoDePieza(salida);
    if (_turno == 0) {
        if (tipoDeJugada == ENPASSANT) {
            //Si la jugada fue ENPASSANT, se elimina el valor del peon rival
*//*
            materialNegras -= constantes::valorPieza[11];
*//*
            material_negras -= constantes::valorPieza[11];
            std::pair<int, int> par = std::make_pair(material_negras, numeroDeJugadas);
            contadorMaterialNegras++;
            modif_hist_material_negras = numeroDeJugadas;
            historial_material_negras[contadorMaterialNegras] = par;
            *//*historialMaterialBlancas.push_back(materialBlancas);
            historialMaterialNegras.push_back(materialNegras);*//*
            return;
        }  else if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
                   tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
                   tipoDeJugada == CPDC || tipoDeJugada == CPIC) {

            int piezaCapturada = obtenerTipoDePieza(llegada);
            material_negras += constantes::valorPieza[piezaCapturada];
            std::pair<int, int> par = std::make_pair(material_negras, numeroDeJugadas);
            contadorMaterialNegras++;
            modif_hist_material_negras = numeroDeJugadas;
            historial_material_negras[contadorMaterialNegras] = par;

        }

        if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK || tipoDeJugada == CPDC
            || tipoDeJugada == CPIC || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER) {

            material_blancas += constantes::valorPieza[salida];
            material_blancas -= constantes::valorPieza[5];
            std::pair<int, int> par = std::make_pair(material_blancas, numeroDeJugadas);
            contadorMaterialBlancas++;
            modif_hist_material_blancas = numeroDeJugadas;

            historial_material_blancas[contadorMaterialBlancas] = par;

        }


    } else {
        if (tipoDeJugada == ENPASSANT) {
            //Si la jugada fue ENPASSANT, se elimina el valor del peon rival
            material_blancas -= constantes::valorPieza[5];
            std::pair<int, int> par = std::make_pair(material_blancas, numeroDeJugadas);
            contadorMaterialBlancas++;
            modif_hist_material_blancas = numeroDeJugadas;

            historial_material_blancas[contadorMaterialBlancas] = par;

            return;
        } else if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
                   tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
                   tipoDeJugada == CPDC || tipoDeJugada == CPIC) {

            int piezaCapturada = obtenerTipoDePieza(llegada);
            material_blancas -= constantes::valorPieza[piezaCapturada];
            std::pair<int, int> par = std::make_pair(material_blancas, numeroDeJugadas);
            contadorMaterialBlancas++;
            modif_hist_material_blancas = numeroDeJugadas;

            historial_material_blancas[contadorMaterialBlancas] = par;

        }

        if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK || tipoDeJugada == CPDC
            || tipoDeJugada == CPIC || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER) {

            material_negras -= constantes::valorPieza[salida];
            material_negras -= constantes::valorPieza[11];
            std::pair<int, int> par = std::make_pair(material_negras, numeroDeJugadas);
            contadorMaterialNegras++;
            modif_hist_material_negras = numeroDeJugadas;
            historial_material_negras[contadorMaterialNegras] = par;


        }

    }

}*/

void Tablero::actualizarZobristKey(u_short jugada) {
    U64 nuevaZobristKey = zobrist;

        if ((historialEnPassant.back() != -1) && agregueAlgoAlHistorialEP) {
            nuevaZobristKey ^= constantes::enPassant[historialEnPassant.back()];
            if (historialEnPassant[historialEnPassant.size() - 2] != -1) {
                nuevaZobristKey ^= constantes::enPassant[historialEnPassant[historialEnPassant.size() - 2]];
            }
        }

        if (!agregueAlgoAlHistorialEP && (historialEnPassant.back() != -1)) {
            nuevaZobristKey ^= constantes::enPassant[historialEnPassant.back()];
        }



    int salida = operaciones_bit::getSalida(jugada);
    int llegada = operaciones_bit::getLlegada(jugada);
    int tipo_de_pieza;
        tipo_de_pieza = obtenerTipoDePieza(salida);

    //Necesitamos diferenciar entre tipo de pieza blanca y negra (cosa que el enum que tenemos
    // no hace) para poder obtener el índice correcto. Para lograr eso simplemente se suma 6
    // al tipo de pieza que se obtenga solo en el caso de que sea negra. Si es blanca queda
    // igual.
    if (_turno == 1) {
        tipo_de_pieza += 6;
    }
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);


    //Si el movimiento es una captura, se actualiza  el hash de la pieza capturada
    if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
        tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
        tipoDeJugada == CPDC || tipoDeJugada == CPIC) {
            int piezaCapturada = obtenerTipoDePieza(llegada);
            if (_turno == 0) {
                piezaCapturada += 6;
            }
            nuevaZobristKey ^= constantes::zobristKeys[piezaCapturada][llegada - 1];

    }
        //Se actualiza el hash para el peón que se captura en caso de que sea enpassant
    else if (tipoDeJugada == ENPASSANT || tipoDeJugada == ENPASSANTCHECK) {
        int casillaPeonCapturado;
        int tipoDePeon;
        if (_turno == 0 ) {
            casillaPeonCapturado = llegada - 8;

            tipoDePeon = PEON + 6;
        } else {
            casillaPeonCapturado = llegada + 8;
            tipoDePeon = PEON;
        }
        nuevaZobristKey ^= constantes::zobristKeys[tipoDePeon][casillaPeonCapturado - 1];
    }

        //Caso enroque
    else if (tipoDeJugada == CASTLING || tipoDeJugada == CASTLINGCHECK) {
        if (llegada == 2) {
            nuevaZobristKey ^= constantes::zobristKeys[TORRE][0];
            nuevaZobristKey ^= constantes::zobristKeys[TORRE][2];
        } else if (llegada == 6) {
            nuevaZobristKey ^= constantes::zobristKeys[TORRE][7];
            nuevaZobristKey ^= constantes::zobristKeys[TORRE][4];
        } else if (llegada == 58) {
            nuevaZobristKey ^= constantes::zobristKeys[TORRE + 6][56];
            nuevaZobristKey ^= constantes::zobristKeys[TORRE + 6][58];
        } else {
            nuevaZobristKey ^= constantes::zobristKeys[TORRE + 6][63];
            nuevaZobristKey ^= constantes::zobristKeys[TORRE + 6][60];
        }
    }
    //Se actualiza el hash del turno
    nuevaZobristKey ^= constantes::mueveElNegro;

    bool haciendoCambioDeEnroques = (numeroDeJugadas == enroques.top().numeroDeMovimiento);

    if(haciendoCambioDeEnroques) {
        bitmask enroquesACambiar = enroques.top().derechosActuales ^ enroques.top().derechosAnteriores;
        if(enroquesACambiar & 1){
            nuevaZobristKey ^= constantes::enroqueCortoBlanco;
        }
        if(enroquesACambiar & 0b10){
            nuevaZobristKey ^= constantes::enroqueLargoBlanco;
        }
        if(enroquesACambiar & 0b100){
            nuevaZobristKey ^= constantes::enroqueCortoNegro;
        }
        if(enroquesACambiar & 0b1000){
            nuevaZobristKey ^= constantes::enroqueLargoNegro;
        }
    }



    if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK || tipoDeJugada == CPDC
        || tipoDeJugada == CPIC || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER) {
        int tipoDePeon = PEON;
        if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
            tipo_de_pieza = tipoDePieza(salida);
            salida = llegada - 8;
            if (_turno == 1 ) {
                tipo_de_pieza += 6;
                salida = llegada + 8;
                tipoDePeon += 6;
            }

        } else if (tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == CPIC) {
            tipo_de_pieza = tipoDePieza(salida);
            salida = llegada - 9;
            if (_turno == 1) {
                tipo_de_pieza += 6;
                salida = llegada + 9;
                tipoDePeon += 6;
            }
        } else if (tipoDeJugada == PROMOTIONDER || tipoDeJugada == CPDC) {
            tipo_de_pieza = tipoDePieza(salida);
            salida = llegada - 7;
            if (_turno == 1 ) {
                tipo_de_pieza += 6;
                salida = llegada + 7;
                tipoDePeon += 6;
            }
        }
        nuevaZobristKey ^= constantes::zobristKeys[tipoDePeon][salida - 1];
        nuevaZobristKey ^= constantes::zobristKeys[tipo_de_pieza][llegada - 1];
        zobrist = nuevaZobristKey;
        contadorZobrist++;
        historialZobrist[contadorZobrist] = zobrist;

        return;
    }

    //Se actualiza el hash para la pieza propia que se mueve
    nuevaZobristKey ^= constantes::zobristKeys[tipo_de_pieza][salida - 1];
    nuevaZobristKey ^= constantes::zobristKeys[tipo_de_pieza][llegada - 1];


    zobrist = nuevaZobristKey;
    agregueAlgoAlHistorialEP = false;
    contadorZobrist++;
    historialZobrist[contadorZobrist] = zobrist;
}

bool Tablero::esTripleRepeticion() {
    if (zobrist == 3) {
        return true;
    } else {
        return false;
    }
}


void Tablero::actualizarCantMovesPiezasMenores(u_short jugada, bool haciendoMovimiento) {
    int salida = operaciones_bit::getSalida(jugada);
    int llegada = operaciones_bit::getLlegada(jugada);
    int tipoDePieza = obtenerTipoDePieza(salida);
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    int actualizacion;
    if (haciendoMovimiento) {
        actualizacion = 1;
    } else {
        actualizacion = -1;

        //Se swapean los valores de salida y llegada en caso de estar deshaciendo el movimiento
        int temp = salida;
        salida = llegada;
        llegada = temp;
    }
    if (tipoDeJugada == QUIET) {
        if (_turno == 0) {
            if (tipoDePieza == ALFIL) {
                if (salida == movimientosAlfilBlancoC.first) {
                    movimientosAlfilBlancoC.second += actualizacion;
                    movimientosAlfilBlancoC.first = llegada;
                } else if (salida == movimientosAlfilBlancoF.first) {
                    movimientosAlfilBlancoF.second += actualizacion;
                    movimientosAlfilBlancoF.first = llegada;
                }
            } else if (tipoDePieza == CABALLO) {
                if (salida == movimientosCaballoBlancoB.first) {
                    movimientosCaballoBlancoB.second += actualizacion;
                    movimientosCaballoBlancoB.first = llegada;
                } else if (salida == movimientosCaballoBlancoG.first) {
                    movimientosCaballoBlancoG.second += actualizacion;
                    movimientosCaballoBlancoG.first = llegada;
                }
            }
        } else {
            if (tipoDePieza == ALFIL) {
                if (salida == movimientosAlfilNegroC.first) {
                    movimientosAlfilNegroC.second += actualizacion;
                    movimientosAlfilNegroC.first = llegada;
                } else if (salida == movimientosAlfilNegroF.first) {
                    movimientosAlfilNegroF.second += actualizacion;
                    movimientosAlfilNegroF.first = llegada;
                }
            } else if (tipoDePieza == CABALLO) {
                if (salida == movimientosCaballoNegroB.first) {
                    movimientosCaballoNegroB.second += actualizacion;
                    movimientosCaballoNegroB.first = llegada;
                } else if (salida == movimientosCaballoNegroG.first) {
                    movimientosCaballoNegroG.second += actualizacion;
                    movimientosCaballoNegroG.first = llegada;
                }
            }
        }
    }
}

void Tablero::imprimirFen() {
    std::string fen = "";
    int emptySpaces = 0;
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0 && i != 0) {
            if (emptySpaces > 0) {
                fen += std::to_string(emptySpaces);
                emptySpaces = 0;
            }
            fen += "/";
        }
        bool isPiece = false;
        for (int j = 0; j < 12; j++) {
            if ((bitboards[j] >> i) & 1) {
                if (emptySpaces > 0) {
                    fen += std::to_string(emptySpaces);
                    emptySpaces = 0;
                }
                switch (j) {
                    case 0:
                        fen += "K";
                        break;
                    case 1:
                        fen += "Q";
                        break;
                    case 2:
                        fen += "R";
                        break;
                    case 3:
                        fen += "B";
                        break;
                    case 4:
                        fen += "N";
                        break;
                    case 5:
                        fen += "P";
                        break;
                    case 6:
                        fen += "k";
                        break;
                    case 7:
                        fen += "q";
                        break;
                    case 8:
                        fen += "r";
                        break;
                    case 9:
                        fen += "b";
                        break;
                    case 10:
                        fen += "n";
                        break;
                    case 11:
                        fen += "p";
                        break;
                }
                isPiece = true;
                break;
            }
        }
        if (!isPiece) {
            emptySpaces++;
        }
    }
    if (emptySpaces > 0) {
        fen += std::to_string(emptySpaces);
    }


    std::reverse(fen.begin(), fen.end());
    fen += " ";
    fen += _turno == 0 ? "w" : "b";
    fen += " ";

    // Add castling rights
    if (enroqueCortoBlancas()) fen += "K";
    if (enroqueLargoBlancas()) fen += "Q";
    if (enroqueCortoNegras()) fen += "k";
    if (enroqueLargoNegras()) fen += "q";
    if (fen.back() == ' ') fen += "-";  // No castling rights


    fen += " ";


    if (contadorJugadas < 0) {
        int salidaUltimaJugada = operaciones_bit::getSalida(jugadas[contadorJugadas]);
        int llegadaUltimaJugada = operaciones_bit::getLlegada(jugadas[contadorJugadas]);
        if ((contadorJugadas>=0) && abs(salidaUltimaJugada - llegadaUltimaJugada) == 16 &&
            (obtenerTipoDePieza(llegadaUltimaJugada) == PEON)) {
            // Add en-passant target square

            if (salidaUltimaJugada > llegadaUltimaJugada) {
                fen += constantes::NumeroACasilla[llegadaUltimaJugada + 8];
            } else {
                fen += constantes::NumeroACasilla[llegadaUltimaJugada - 8];
            }
        } else {
            fen += "-";
        }
    } else {
        fen += "-";
    }

    std::cout << fen << std::endl;
}

bool Tablero::desarrolloCaballoBlancoB() {
    return !(bitboards[4] >> 6 & 1);
}

bool Tablero::desarrolloCaballoBlancoG() {
    return !(bitboards[4] >> 1 & 1);
}

bool Tablero::desarrolloAlfilBlancoC() {
    return !(bitboards[3] >> 5 & 1);
}

bool Tablero::desarrolloAlfilBlancoF() {
    return !(bitboards[3] >> 2 & 1);
}

bool Tablero::desarrolloCaballoNegroB() {
    return !(bitboards[10] >> 62 & 1);
}

bool Tablero::desarrolloCaballoNegroG() {
    return !(bitboards[10] >> 57 & 1);
}

bool Tablero::desarrolloAlfilNegroC() {
    return !(bitboards[9] >> 61 & 1);
}

bool Tablero::desarrolloAlfilNegroF() {
    return !(bitboards[9] >> 58 & 1);
}

bool Tablero::desarrolloBlancasCompleto() {
    return desarrolloCaballoBlancoG() && desarrolloCaballoBlancoB() && desarrolloAlfilBlancoC() &&
           desarrolloAlfilBlancoF();
}

bool Tablero::desarrolloNegrasCompleto() {
    return desarrolloCaballoNegroG() && desarrolloCaballoNegroB() && desarrolloAlfilNegroC() && desarrolloAlfilNegroF();
}

int Tablero::cantPiezas(int color) {
    int cant = 0;
    int negras;
    if (color == 1) {
        negras = 6;
    } else {
        negras = 0;
    }
    for (int i = 1; i < 5; i++) {
        U64 bitboardAux = bitboards[i + negras];
        while (bitboardAux > 0) {
            cant++;
            operaciones_bit::LSB(bitboardAux);
        }
    }


    return cant;
}

void Tablero::configurarFen(std::string fen) {
    //Inicializar bitboards en 0
    bitboards.clear();
    contadorZobrist = -1;
    contadorJugadas = -1;
    enroques = std::stack<derechosDeEnroque>();

/*    contadorMaterialBlancas = -1;
    contadorMaterialNegras = -1;*/


    for (int k = 0; k < 12; k++) {
        bitboards.push_back(0ULL);
    }
    bitmask enroquesIniciales = 0;

    //Se van insertando las piezas y asignando valores de variables según el fen
    int casillaActual = 64; //Se arranca desde la casilla a8 porque así funciona la notacion FEN
    for (int i = 0; i < fen.size(); i++) {

        //Si "casillaActual" es > 0 entonces todavía estamos en la parte del FEN que contiene info
        //sobre dónde estás las piezas en el tablero. Según la letra, asignamos la pieza a su bitboard.
        if (casillaActual > 0) {
            switch (fen[i]) {
                case 'K':
                    bitboards[0] = operaciones_bit::setBit(bitboards[0], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'Q':
                    bitboards[1] = operaciones_bit::setBit(bitboards[1], casillaActual, 1);
                    casillaActual--;
                    damasBlancas++;
                    break;

                case 'R':
                    bitboards[2] = operaciones_bit::setBit(bitboards[2], casillaActual, 1);
                    casillaActual--;
                    torresBlancas++;
                    break;

                case 'B':
                    bitboards[3] = operaciones_bit::setBit(bitboards[3], casillaActual, 1);
                    casillaActual--;
                    alfilesBlancos++;
                    break;

                case 'N':
                    bitboards[4] = operaciones_bit::setBit(bitboards[4], casillaActual, 1);
                    casillaActual--;
                    caballosBlancos++;
                    break;

                case 'P':
                    bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaActual, 1);
                    casillaActual--;
                    peonesBlancos++;
                    break;


                case 'k':
                    bitboards[6] = operaciones_bit::setBit(bitboards[6], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'q':
                    bitboards[7] = operaciones_bit::setBit(bitboards[7], casillaActual, 1);
                    casillaActual--;
                    damasNegras++;
                    break;

                case 'r':
                    bitboards[8] = operaciones_bit::setBit(bitboards[8], casillaActual, 1);
                    casillaActual--;
                    torresNegras++;
                    break;

                case 'b':
                    bitboards[9] = operaciones_bit::setBit(bitboards[9], casillaActual, 1);
                    casillaActual--;
                    alfilesNegros++;
                    break;

                case 'n':
                    bitboards[10] = operaciones_bit::setBit(bitboards[10], casillaActual, 1);
                    casillaActual--;
                    caballosNegros++;
                    break;

                case 'p':
                    bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaActual, 1);
                    casillaActual--;
                    peonesNegros++;
                    break;

                case '/':
                    continue;

                default:
                    casillaActual -= fen[i] - '0';
            }
        } else if (fen[i] == ' ') {
            continue;

            //Si encontramos w es porque el turno es de las blancas.
        } else if (fen[i] == 'w') {
            _turno = 0;

            //K, Q, k y q indican los derechos a enroncar de blancas y negras.
        } else if (fen[i] == 'K') {
            enroquesIniciales = enroquesIniciales | 0b1;
        } else if (fen[i] == 'Q') {
            enroquesIniciales = enroquesIniciales | 0b10;
        } else if (fen[i] == 'k') {
            enroquesIniciales = enroquesIniciales | 0b100;
        } else if (fen[i] == 'q') {
            enroquesIniciales = enroquesIniciales | 0b1000;

            //Si llegamos a esta parte se está hablando de un posible casilla de enpassant o de la cantidad
            // de medio-movimientos
        } else if (fen[i] >= 97 && fen[i] < 105) {
            //Si encontramos una "b" puede ser una casilla de enpassant de la columna b (b3 x ej)
            // o bien indicar que le toca a las negras.
            if (fen[i] == 'b' && fen[i + 1] == ' ') {
                _turno = 1;
            } else {
                string jugadaString{fen[i], fen[i + 1]};
                int casilla = constantes::casillaANumero[jugadaString];
                int salida, llegada, tipoDeJugada;
                if (fen[i + 1] - '0' == 3) {
                    salida = casilla - 8;
                    llegada = casilla + 8;
                    tipoDeJugada = QUIET;
                } else {
                    salida = casilla + 8;
                    llegada = casilla - 8;
                    tipoDeJugada = QUIET;
                };

                //Agregamos el avance de peón al historial para que la detección de enpassant
                //funcione correctamente.
                u_short jugada = operaciones_bit::crearJugada(salida, llegada, tipoDeJugada);
                /*_jugadas.push_back(jugada);*/
                contadorJugadas++;
                jugadas[contadorJugadas] = jugada;

            }
        }
    }
    enroques.push({enroquesIniciales, 0, -5});
    //Se actualiza el hash de Zobrist
    zobrist = zobristKey();
    contadorZobrist++;
    historialZobrist[contadorZobrist] = zobrist;

    //Se calcula el valor del material y se agrega al historial
/*    contadorMaterialBlancas = 0;
    contadorMaterialNegras = 0;
    historial_material_blancas[contadorMaterialBlancas] = std::make_pair(valoracionMaterial(0), 0);
    historial_material_negras[contadorMaterialNegras] = std::make_pair(valoracionMaterial(1), 0);*/
    calcularOcupacion();


}

std::string Tablero::formatearJugada(u_short jugada) {
    string jugadaString;
    int salida, llegada;
    //Chequear el tipo de jugada para imprimir la jugada en el formato correcto
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
        llegada = operaciones_bit::getLlegada(jugada);
        int piezaAPromover = operaciones_bit::getSalida(jugada);

        if (llegada > 32) {
            salida = llegada - 8;
        } else {
            salida = llegada + 8;
        }
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);

    } else if (tipoDeJugada == PROMOTIONDER || tipoDeJugada == CPDC) {
        llegada = operaciones_bit::getLlegada(jugada);
        int piezaAPromover = operaciones_bit::getSalida(jugada);
        if (llegada > 32) {
            salida = llegada - 7;
        } else {
            salida = llegada + 7;
        }
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);

    } else if (tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == CPIC) {
        llegada = operaciones_bit::getLlegada(jugada);
        int piezaAPromover = operaciones_bit::getSalida(jugada);
        if (llegada > 32) {
            salida = llegada - 9;
        } else {
            salida = llegada + 9;
        }
        jugadaString = generarJugadaString(piezaAPromover, salida, llegada);

    } else {
        salida = operaciones_bit::getSalida(jugada);
        llegada = operaciones_bit::getLlegada(jugada);

        jugadaString += constantes::NumeroACasilla[salida];
        jugadaString += constantes::NumeroACasilla[llegada];
    }
    return jugadaString;
}

std::string Tablero::generarJugadaString(int piezaAPromover, int salida, int llegada) {
    std::string jugadaString = "";
    jugadaString += constantes::NumeroACasilla[salida];
    jugadaString += constantes::NumeroACasilla[llegada];
    jugadaString += constantes::NumeroACasilla[llegada];

    switch (piezaAPromover) {
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

bool Tablero::esCaptura(u_short jugada) {

    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    return tipoDeJugada == CAPTURE || tipoDeJugada == ENPASSANT
           || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER;
}

void Tablero::movimientoNulo() {
    cambiarTurno();
    numeroDeJugadas++;
    zobrist ^= constantes::mueveElNegro;
    contadorJugadas++;
    jugadas[contadorJugadas] = 0;

    if (historialEnPassant.back() != -1) {
        zobrist ^= constantes::enPassant[historialEnPassant.back()];
    }

    historialEnPassant.push_back(-1);
    contadorZobrist++;
    historialZobrist[contadorZobrist] = zobrist;


}

void Tablero::deshacerMovimientoNulo() {
    cambiarTurno();
    numeroDeJugadas--;
    contadorZobrist--;
    zobrist = historialZobrist[contadorZobrist];
    contadorJugadas--;
    historialEnPassant.pop_back();


}

bool Tablero::esUnJaque(u_short movimiento) {
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(movimiento);
    if (tipoDeJugada == CAPTURECHECK || tipoDeJugada == ENPASSANTCHECK || tipoDeJugada == CPDC
        || tipoDeJugada == CPIC) {
        return true;
    }
    return false;
}

int Tablero::contador_movilidad(int turno) {
    int cantMoves = 0;
        U64 torres = bitboards[2];
        while (torres > 0) {
            int casilla = operaciones_bit::LSB(torres);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves += __builtin_popcountll(bitboard_movimientos_torre_blanca(bitboard) & ~piezas_blancas());
        }
        U64 alfiles = bitboards[3];
        while (alfiles > 0) {
            int casilla = operaciones_bit::LSB(alfiles);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves += __builtin_popcountll(bitboard_movimientos_alfil_blanco(bitboard) & ~piezas_blancas());
        }
        U64 caballos = bitboards[4];
        while (caballos > 0) {
            int casilla = operaciones_bit::LSB(caballos);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves += __builtin_popcountll(bitboard_movimientos_caballo_blanco(bitboard) & ~piezas_blancas());
        }
        U64 damas = bitboards[1];
        while (damas > 0) {
            int casilla = operaciones_bit::LSB(damas);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves += __builtin_popcountll(bitboard_movimientos_dama_blanca(bitboard) & ~piezas_blancas());
        }
        U64 rey = bitboards[0];
        while (rey > 0) {
            int casilla = operaciones_bit::LSB(rey);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves += __builtin_popcountll(bitboard_movimientos_rey_blanco(bitboard) & ~piezas_blancas());
        }
        U64 peones = bitboards[5];
        cantMoves += __builtin_popcountll(peon->todos_los_movimientos(peones, piezas_blancas(),
                                                                      piezas_negras(), 0));

        U64 torresn = bitboards[8];
        while (torresn > 0) {
            int casilla = operaciones_bit::LSB(torresn);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves -= __builtin_popcountll(bitboard_movimientos_torre_negra(bitboard) & ~piezas_negras());
        }
        U64 alfilesn = bitboards[9];
        while (alfilesn > 0) {
            int casilla = operaciones_bit::LSB(alfilesn);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves -= __builtin_popcountll(bitboard_movimientos_alfil_negro(bitboard) & ~piezas_negras());
        }

        U64 caballosn = bitboards[10];
        while (caballosn > 0) {
            int casilla = operaciones_bit::LSB(caballosn);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves -= __builtin_popcountll(bitboard_movimientos_caballo_negro(bitboard) & ~piezas_negras());
        }

        U64 damasn = bitboards[7];
        while (damasn > 0) {
            int casilla = operaciones_bit::LSB(damasn);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves -= __builtin_popcountll(bitboard_movimientos_dama_negra(bitboard) & ~piezas_negras());
        }

        U64 reyn = bitboards[6];
        while (reyn > 0) {
            int casilla = operaciones_bit::LSB(reyn);
            U64 bitboard = operaciones_bit::setBit(0, casilla, 1);
            cantMoves -= __builtin_popcountll(bitboard_movimientos_rey_negro(bitboard) & ~piezas_negras());
        }
        U64 peonesn = bitboards[11];
        cantMoves -= __builtin_popcountll(peon->todos_los_movimientos(peonesn, piezas_negras(),
                                                                      piezas_blancas(), 1));


    return cantMoves;
}

void Tablero::setearEnroque(int tipoDeEnroque, bool hayDerecho) {
    // 0 es enroque corto de blancas
    // 1 es enroque largo de blancas
    // 2 es enroque corto de negras
    // 3 es enroque largo de negras
    derechosEnroqueAux = (derechosEnroqueAux &
                   ~(1 << tipoDeEnroque)) | (hayDerecho << tipoDeEnroque);


}

void Tablero:: guardarEnroque() {
    derechosDeEnroque nuevos;
    nuevos.derechosAnteriores = enroques.top().derechosActuales;
    nuevos.derechosActuales = derechosEnroqueAux;
    nuevos.numeroDeMovimiento = numeroDeJugadas;
    enroques.push(nuevos);
}

void Tablero::deshacerEnroque() {
    if(numeroDeJugadas == (enroques.top().numeroDeMovimiento) ){
        enroques.pop();
    }
}

bool Tablero::enroqueCortoBlancas() {return enroques.top().derechosActuales & 1;}
bool Tablero::enroqueLargoBlancas() {return enroques.top().derechosActuales & 0b10;}
bool Tablero::enroqueCortoNegras() {return enroques.top().derechosActuales & 0b100;}
bool Tablero::enroqueLargoNegras() {return enroques.top().derechosActuales & 0b1000;}

int Tablero::detectarPeonesPasados(int turno){
    int peonesPasados = 0;
    U64 peones = bitboards[5 + turno*6];
    if(turno == 0) {
        while (peones > 0) {
            int casilla = operaciones_bit::LSB(peones);
            if(0xff000000000000 & (1ULL <<(casilla - 1))){
                peonesPasados += constantes::premioPeonPasado[5];
                continue;
            }
            if((0xffffffffff00 & (1ULL << (casilla - 1))) &&
            (masksPeonesPasados[casilla - 9] & bitboards[11])){
                peonesPasados += constantes::premioPeonPasado[((casilla - 1) / 8) - 1];
            }
        }
    }
    else{
        while (peones > 0) {
            int casilla = operaciones_bit::LSB(peones);
            if(0xff00 & (1ULL << (casilla - 1))){
                peonesPasados += constantes::premioPeonPasado[5];
                continue;
            }
            if((0xffffffffff0000 & (1ULL << (casilla - 1))) &&
            (masksPeonesPasados[96 - casilla] & bitboards[5])){
                peonesPasados += constantes::premioPeonPasado[(7 - (casilla-1) / 8) - 1];
            }
        }
    }

    return peonesPasados;
}

void Tablero::calcularMasksPeonesPasados() {
    for(int i = 9; i < 49; i++) {
        int filaDelPeon = (i / 8);
        int columnaDelPeon = ((i - 1) % 8);
        U64 columnaPeonBitboard = constantes::HFile << columnaDelPeon;
        U64 columnaPeonIzqBitboard = columnaDelPeon == 7 ? 0 : columnaPeonBitboard << 1;
        U64 columnaPeonDerBitboard = columnaDelPeon == 0 ? 0 : columnaPeonBitboard >> 1;
        U64 maskDelPeon;

        maskDelPeon = (columnaDelPeon | columnaPeonIzqBitboard | columnaPeonDerBitboard) << filaDelPeon;
        masksPeonesPasados[i - 9] = maskDelPeon;
    }
    for(int i = 56; i > 16; i--) {
        int filaDelPeon = (i / 8);
        int columnaDelPeon = ((i - 1) % 8);
        U64 columnaPeonBitboard = constantes::HFile << columnaDelPeon;
        U64 columnaPeonIzqBitboard = columnaDelPeon == 0 ? 0 : columnaPeonBitboard << 1;
        U64 columnaPeonDerBitboard = columnaDelPeon == 7 ? 0 : columnaPeonBitboard >> 1;
        U64 maskDelPeon;

        maskDelPeon = (columnaDelPeon | columnaPeonIzqBitboard | columnaPeonDerBitboard) >> filaDelPeon;
        masksPeonesPasados[96 - i] = maskDelPeon;
    }
}

void Tablero::actualizarCantPiezas(int tipoDePieza, bool agregando){

    int agregandoInt;
    agregando ? agregandoInt = 1 : agregandoInt = 0;
    if (tipoDePieza == 1){
        damasBlancas += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 2){
        torresBlancas += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 3){
        alfilesBlancos += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 4){
        caballosBlancos += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 5){
        peonesBlancos += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 7){
        damasNegras += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 8){
        torresNegras += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 9){
        alfilesNegros += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 10){
        caballosNegros += -1 + (2*agregandoInt);
    }
    else if (tipoDePieza == 11){
        peonesNegros += -1 + (2*agregandoInt);
    }

}