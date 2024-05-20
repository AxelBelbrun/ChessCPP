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

using namespace std;

Tablero::Tablero(std::string posicionASetear) {

    //Si la posicion a setear es la inicial del ajedrez
    if (posicionASetear.std::string::find("startpos") != std::string::npos) {

        _turno = 0;
        enroqueCortoBlanco.push_back(true);
        enroqueLargoBlanco.push_back(true);
        enroqueLargoNegro.push_back(true);
        enroqueCortoNegro.push_back(true);

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


        //El valor del material desde la posición inicial del ajedrez es siempre el mismo,
        //asi que en vez de "calcularlo" directamente lo asignamos
        historialMaterialBlancas.push_back(4050);
        historialMaterialNegras.push_back(-4050);

        //Feature a optimizar
        /*//El valor del control del centro inicial sí se calcula, aunque siempre sea el mismo:
        controlCentroBlancas = controlDeCentro(0);
        controlCentroNegras = controlDeCentro(1);*/


        //El valor de la ocupación inicial también se calcula, aunque siempre sea el mismo:
        historialOcupacionBlancas.push_back(valoracionMaterial(0));
        historialOcupacionNegras.push_back(valoracionMaterial(1));

        //Si estamos construyendo un tablero y el string contiene la palabra "moves"
        // significa que se nos pasaron jugadas para llegar a una determinada posición, entonces
        // las hacemos.
        if (posicionASetear.std::string::find("moves") != std::string::npos) {
            string jugadas = posicionASetear.substr(posicionASetear.std::string::find("moves") + 6,
                                                    posicionASetear.size() - 1);
            int i = 0;
            while (i < jugadas.size()) {
                string jugadaString = jugadas.substr(i, 2);
                int salida = constantes::casillaANumero[jugadaString];
                string jugadaString2 = jugadas.substr(i + 2, 2);
                int llegada = constantes::casillaANumero[jugadaString2];
                int tipoDeJugada = 0;
                u_short jugada = operaciones_bit::crearJugada(salida, llegada, tipoDeJugada);
                moverPiezaTrusted(salida, llegada, tipoDeJugada);
                i += 5;
            }

        }

    }

        //Si nos pasan un fen con una determinada posición
    else {

        //Inicializar bitboards en 0
        for (int k = 0; k < 12; k++) {
            bitboards.push_back(0ULL);
        }

        //Se inician los enroques en false, luego con la info del fen se sabe cuáles setear en true
        enroqueCortoBlanco.push_back(false);
        enroqueLargoBlanco.push_back(false);
        enroqueLargoNegro.push_back(false);
        enroqueCortoNegro.push_back(false);

        //Tomamos la parte que nos interesa del string, eliminando "position fen "

        string fen = posicionASetear.substr(13, posicionASetear.size() - 13);

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
                        break;

                    case 'R':
                        bitboards[2] = operaciones_bit::setBit(bitboards[2], casillaActual, 1);
                        casillaActual--;
                        break;

                    case 'B':
                        bitboards[3] = operaciones_bit::setBit(bitboards[3], casillaActual, 1);
                        casillaActual--;
                        break;

                    case 'N':
                        bitboards[4] = operaciones_bit::setBit(bitboards[4], casillaActual, 1);
                        casillaActual--;
                        break;

                    case 'P':
                        bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaActual, 1);
                        casillaActual--;
                        break;


                    case 'k':
                        bitboards[6] = operaciones_bit::setBit(bitboards[6], casillaActual, 1);
                        casillaActual--;
                        break;

                    case 'q':
                        bitboards[7] = operaciones_bit::setBit(bitboards[7], casillaActual, 1);
                        casillaActual--;
                        break;

                    case 'r':
                        bitboards[8] = operaciones_bit::setBit(bitboards[8], casillaActual, 1);
                        casillaActual--;
                        break;

                    case 'b':
                        bitboards[9] = operaciones_bit::setBit(bitboards[9], casillaActual, 1);
                        casillaActual--;
                        break;

                    case 'n':
                        bitboards[10] = operaciones_bit::setBit(bitboards[10], casillaActual, 1);
                        casillaActual--;
                        break;

                    case 'p':
                        bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaActual, 1);
                        casillaActual--;
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
                enroqueCortoBlanco.back() = true;
            } else if (fen[i] == 'Q') {
                enroqueLargoBlanco.back() = true;
            } else if (fen[i] == 'k') {
                enroqueCortoNegro.back() = true;
            } else if (fen[i] == 'q') {
                enroqueLargoNegro.back() = true;

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
                    _jugadas.push_back(jugada);

                }
            }
        }

        //Se calcula el valor del material y se agrega al historial
        historialMaterialBlancas.push_back(valoracionMaterial(0));
        historialMaterialNegras.push_back(valoracionMaterial(1));
/*        controlCentroBlancas = controlDeCentro(0);
        controlCentroNegras = controlDeCentro(1);*/


        historialOcupacionBlancas.push_back(calcularOcupacion(0));
        historialOcupacionNegras.push_back(calcularOcupacion(1));

    }
    zobrist = 0;

    //Se asigna la zobrist key inicial según la info del tablero.
    zobrist = zobristKey();

    historialPosiciones.push_back(bitboards);

    quienMovioPrimero = _turno;


    //Se generan las tablas de movimientos de las sliding pieces..
    for (int i = 0; i < 64; i++) {

        vector<U64> configuracionesPosiblesTorre = calculadoraMovesTorre::masksPiezasBloqueando(
                constantes::attackMasksTorre[i]);
        vector<U64> configuracionesPosiblesAlfil = calculadoraMovesAlfil::masksPiezasBloqueando(
                constantes::attackMasksAlfil[i]);
        U64 bitboardTorre = operaciones_bit::setBit(0L, i + 1, 1);
        U64 bitboardAlfil = operaciones_bit::setBit(0L, i + 1, 1);
        auto torre = new Torre();
        auto alfil = new Alfil();
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
        delete torre;
        delete alfil;

        auto caballo = new Caballo();
        auto rey = new Rey();
        auto peon = new Peon();
        U64 bitboardCaballo = operaciones_bit::setBit(0L, i + 1, 1);
        U64 movimientosCaballo = caballo->generar_movimientos_legales(bitboardCaballo, 0, 0, 0);
        movimientosDeCaballo[i] = movimientosCaballo;
        U64 bitboardRey = operaciones_bit::setBit(0L, i + 1, 1);
        U64 movimientosRey = rey->generar_movimientos_legales(bitboardRey, 0, 0, 0);
        movimientosDeRey[i] = movimientosRey;
        U64 bitboardPeon = operaciones_bit::setBit(0L, i + 1, 1);
        U64 movimientosPeonBlanco = peon->generar_movimientos_legales(bitboardPeon, 0, 0, 0);
        movimientosDePeon[i][0] = movimientosPeonBlanco;
        U64 movimientosPeonNegro = peon->generar_movimientos_legales(bitboardPeon, 0, 0, 1);
        movimientosDePeon[i][1] = movimientosPeonNegro;
        delete caballo;
        delete rey;
        delete peon;


    }
};


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

void Tablero::imprimirJugadas() {

    for (int i = 0; i < _jugadas.size(); i++) {
        cout << constantes::NumeroACasilla[operaciones_bit::getSalida(_jugadas[i])] +
                constantes::NumeroACasilla[operaciones_bit::getLlegada(_jugadas[i])] << endl;


    }
}

void Tablero::deshacerMovimiento() {
    actualizarZobristKey(_jugadas.back(), false);
    actualizarCantMovesPiezasMenores(_jugadas.back(), false);

    //En caso de que la jugada a deshacer sea un enroque, hay que volver a setear en "false"
    // la variable que representa si ese bando enrrocó.
    if (_turno == 0 && operaciones_bit::getTipoDeJugada(_jugadas.back()) == CASTLING) {
        enrocoNegras = false;
    } else if (_turno == 1 && operaciones_bit::getTipoDeJugada(_jugadas.back()) == CASTLING) {
        enrocoBlancas = false;
    }

    //Eliminamos el último elemento de todos los registros que tenemos.
    historialPosiciones.pop_back();
    _jugadas.pop_back();
    enroqueCortoBlanco.pop_back();
    enroqueCortoNegro.pop_back();
    enroqueLargoBlanco.pop_back();
    enroqueLargoNegro.pop_back();
    historialDePosiciones.pop_back();
    ganoNegro = false;
    ganoBlanco = false;

    historialMaterialBlancas.pop_back();
    historialMaterialNegras.pop_back();
    historialOcupacionBlancas.pop_back();
    historialOcupacionNegras.pop_back();

    //Podría suceder que la jugada que estamos deshaciendo haya sido una que
    // haga perder el derecho a enrocar, por lo que seteamos las variables que indican
    // qué enroques podemos hacer con el valor que tenían anteriormente a la jugada. (Como ya eliminamos
    // el último elemento de todos nuestros registros, el valor que nos interesa quedó ahora en el último lugar)
    if (_turno == 1) {
        if (enroqueCortoNegro.back()) {
            perdidaEnroqueCortoNegras = false;
        }
        if (enroqueLargoNegro.back()) {
            perdidaEnroqueLargoNegras = false;
        }
    } else {
        if (enroqueCortoBlanco.back()) {
            perdidaEnroqueCortoBlancas = false;
        }
        if (enroqueLargoBlanco.back()) {
            perdidaEnroqueLargoBlancas = false;
        }
    }


    //Actualizamos los bitboards
    for (int i = 0; i < 12; i++) {
        bitboards[i] = historialPosiciones.back()[i];
    }
    cambiarTurno();

};

bool Tablero::moverPieza(int salida, int llegada, int tipo_jugada) {
    u_short aRealizar = operaciones_bit::crearJugada(salida, llegada, tipo_jugada);


/*    //El siguiente fragmento de código evita cierto viejo bug que aún no se
    // pudo rastrear y eliminar.
    *//**************************************************************************//*

    if(_jugadas.size() > 0) {
        if (operaciones_bit::getSalida(_jugadas.back()) == salida &&
            operaciones_bit::getLlegada(_jugadas.back()) == llegada) {
            return false;
        }
    }
    */
    /**************************************************************************/
    if (tipo_jugada == CHECKMATE) {
        if (_turno == 0) {
            ganoBlanco = true;
        } else {
            ganoNegro = true;
        }
        moverPiezaTrusted(salida, llegada, tipo_jugada);
        return true;
    } else if (tipo_jugada == CASTLING) {
        u_short jugada = operaciones_bit::crearJugada(salida, llegada, CASTLING);
        if (enrocar(jugada)) {

            return true;
        } else {
            return false;
        }
    } else if (tipo_jugada == ENPASSANT) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        actualizarMaterial(aRealizar);
        actualizarOcupacion(aRealizar);
        actualizarZobristKey(aRealizar, true);
        historialDePosiciones.push_back(zobrist);
        if (_turno == 0) {
            bitboards[5] = operaciones_bit::setBit(bitboards[5], llegada, 1);
            bitboards[5] = operaciones_bit::setBit(bitboards[5], salida, 0);
            bitboards[11] = operaciones_bit::setBit(bitboards[11], llegada - 8, 0);
        } else {
            bitboards[11] = operaciones_bit::setBit(bitboards[11], llegada, 1);
            bitboards[11] = operaciones_bit::setBit(bitboards[11], salida, 0);
            bitboards[5] = operaciones_bit::setBit(bitboards[5], llegada + 8, 0);
        }


        _jugadas.push_back(aRealizar);
        historialPosiciones.push_back(bitboards);

        if (reyPropioEnJaque(_turno)) {
            cambiarTurno();
            deshacerMovimiento();
            return false;
        }


        cambiarTurno();
        ep++;

        return true;

    } else if (tipo_jugada == PROMOTION || tipo_jugada == PROMOTIONCHECK) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        actualizarMaterial(aRealizar);
        actualizarOcupacion(aRealizar);
        actualizarZobristKey(aRealizar, true);
        historialDePosiciones.push_back(zobrist);

        if (_turno == 0) {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) - 8;
            if (tipoDePieza(salida) == DAMA) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 1);
            }
            if (PROMOTIONCHECK) {
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 0);
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 0);
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 0);
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 0);
            }

        } else {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) + 8;
            if (tipoDePieza(salida) == DAMA) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 1);
            }
            if (PROMOTIONCHECK) {
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 0);
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 0);
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 0);
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 0);

            }
        }


        _jugadas.push_back(aRealizar);
        historialPosiciones.push_back(bitboards);

        if (reyPropioEnJaque(_turno)) {
            cambiarTurno();
            deshacerMovimiento();
            return false;
        }
        cambiarTurno();

        return true;
    } else if (tipo_jugada == PROMOTIONIZQ || tipo_jugada == CPIC) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        actualizarMaterial(aRealizar);
        actualizarOcupacion(aRealizar);
        actualizarZobristKey(aRealizar, true);
        historialDePosiciones.push_back(zobrist);

        if (_turno == 0) {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) - 9;
            if (tipoDePieza(salida) == DAMA) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 1);
            }
            if (CPIC) {
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 0);
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 0);
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 0);
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 0);
            }

        } else {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) + 9;
            if (tipoDePieza(salida) == DAMA) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 1);
            }
            if (CPIC) {
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 0);
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 0);
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 0);
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 0);
            }
        }


        _jugadas.push_back(aRealizar);
        historialPosiciones.push_back(bitboards);

        if (reyPropioEnJaque(_turno)) {
            cambiarTurno();
            deshacerMovimiento();
            return false;
        }
        cambiarTurno();

        return true;

    } else if (tipo_jugada == PROMOTIONDER || tipo_jugada == CPDC) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        actualizarMaterial(aRealizar);
        actualizarOcupacion(aRealizar);
        actualizarZobristKey(aRealizar, true);
        historialDePosiciones.push_back(zobrist);

        if (_turno == 0) {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) - 7;
            if (tipoDePieza(salida) == DAMA) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaPromocion, 0);
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 1);
            }
            if (CPDC) {
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 0);
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 0);
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 0);
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 0);

            }

        } else {
            int casillaPromocion = operaciones_bit::getLlegada(aRealizar) + 7;
            if (tipoDePieza(salida) == DAMA) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[7] = operaciones_bit::setBit(bitboards[7], llegada, 1);
            } else if (tipoDePieza(salida) == TORRE) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[8] = operaciones_bit::setBit(bitboards[8], llegada, 1);
            } else if (tipoDePieza(salida) == ALFIL) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[9] = operaciones_bit::setBit(bitboards[9], llegada, 1);
            } else if (tipoDePieza(salida) == CABALLO) {
                bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaPromocion, 0);
                bitboards[10] = operaciones_bit::setBit(bitboards[10], llegada, 1);
            }
            if (CPDC) {
                bitboards[1] = operaciones_bit::setBit(bitboards[1], llegada, 0);
                bitboards[2] = operaciones_bit::setBit(bitboards[2], llegada, 0);
                bitboards[3] = operaciones_bit::setBit(bitboards[3], llegada, 0);
                bitboards[4] = operaciones_bit::setBit(bitboards[4], llegada, 0);
            }
        }


        _jugadas.push_back(aRealizar);
        historialPosiciones.push_back(bitboards);

        if (reyPropioEnJaque(_turno)) {
            cambiarTurno();
            deshacerMovimiento();
            return false;
        }
        cambiarTurno();
        return true;

    }

    U64 salidaBitboard = operaciones_bit::setBit(0L, salida, 1);
    U64 llegadaBitboard = operaciones_bit::setBit(0L, llegada, 1);
    if (salida == 4) {
        enroqueCortoBlanco.push_back(false);
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(false);
        perdidaEnroqueLargoBlancas = true;
        perdidaEnroqueCortoBlancas = true;
    } else if (salida == 60) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(false);
        enroqueLargoNegro.push_back(false);
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        perdidaEnroqueLargoNegras = true;
        perdidaEnroqueCortoNegras = true;
    } else if (salida == 1) {
        enroqueCortoBlanco.push_back(false);
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        perdidaEnroqueCortoBlancas = true;
    } else if (salida == 57) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(false);
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        perdidaEnroqueCortoNegras = true;

    } else if (salida == 64) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(false);
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        perdidaEnroqueLargoNegras = true;
    } else if (salida == 8) {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(false);
        perdidaEnroqueLargoBlancas = true;
    } else {
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
    }
    if (obtenerTipoDePieza(salida) == DAMA && (_turno == 0) ? !desarrolloBlancasCompleto()
                                                            : !desarrolloNegrasCompleto()) {
        if (_turno == 0) {
            movimientoTempranoDamaBlanca = true;
        } else {
            movimientoTempranoDamaNegra = true;
        }
    }
    actualizarMaterial(aRealizar);
    actualizarOcupacion(aRealizar);
    actualizarZobristKey(aRealizar, true);
    actualizarCantMovesPiezasMenores(aRealizar, true);
    historialDePosiciones.push_back(zobrist);


    for (int k = 0; k < 12; k++) {

        if ((bitboards[k] & salidaBitboard) > 0) {
            bitboards[k] = operaciones_bit::setBit(bitboards[k], salida, 0);
            bitboards[k] = operaciones_bit::setBit(bitboards[k], llegada, 1);

        } else if ((bitboards[k] & llegadaBitboard) > 0) {
            bitboards[k] = operaciones_bit::setBit(bitboards[k], llegada, 0);

        }
    }


    _jugadas.push_back(aRealizar);
    historialPosiciones.push_back(bitboards);

    if (reyPropioEnJaque(_turno)) {
        cambiarTurno();
        deshacerMovimiento();
        return false;
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

    if (tipo_jugada == CHECK || tipo_jugada == CAPTURECHECK) {
        if (_turno == 0) {
            if (ganoELBlanco()) {
                ganoBlanco = true;
            }
        } else {
            if (ganoElNegro()) {
                ganoNegro = true;
            }
        }
    }


    return true;
    /*}*/


    return false;


};

bool Tablero::casillaAtacada(int casilla, int turno) {

    vector<u_short> movimientos = this->generar_movimientos(turno);
    for (u_short x: movimientos) {
        if (operaciones_bit::getLlegada(x) == casilla) {
            return true;
        }
    }
    return false;
}

std::vector<u_short> Tablero::generar_movimientos(int turno) {
    std::vector<u_short> movimientos;

    //turno = 0 es blancas, turno = 1 es negras
    if (turno == 0) {
        U64 bitboardTemp;

        bitboardTemp = bitboards[1]; //bitboard de la dama blanca
        obtener_movimientos_dama_blanca(movimientos, bitboardTemp);

        bitboardTemp = bitboards[2]; //bitboard de las torres blancas
        obtener_movimientos_torre_blanca(movimientos, bitboardTemp);

        bitboardTemp = bitboards[3]; //bitboard de los alfiles blancos
        obtener_movimientos_alfil_blanco(movimientos, bitboardTemp);

        obtener_movimientos_rey_blanco(movimientos);
        obtener_movimientos_caballo_blanco(movimientos);
        obtener_movimientos_peon_blanco(movimientos);

    } else {
        U64 bitboardTemp;

        bitboardTemp = bitboards[7];
        obtener_movimientos_dama_negra(movimientos, bitboardTemp);

        bitboardTemp = bitboards[8];
        obtener_movimientos_torre_negra(movimientos, bitboardTemp);

        bitboardTemp = bitboards[9];
        obtener_movimientos_alfil_negro(movimientos, bitboardTemp);

        obtener_movimientos_caballo_negro(movimientos);
        obtener_movimientos_rey_negro(movimientos);
        obtener_movimientos_peon_negro(movimientos);

    }
    return movimientos;
}

vector<u_short> &Tablero::obtener_movimientos_peon_negro(
        vector<u_short> &movimientos) {//Movimientos de Perón... Mi general, cuánto valés!
    auto peon = new Peon();
    peon->movimientos_legales(this, &movimientos, bitboards[11],
                              piezas_negras(),
                              piezas_blancas());
    delete peon;
    return movimientos;
}

vector<u_short> &Tablero::obtener_movimientos_rey_negro(vector<u_short> &movimientos) {
    auto rey = new Rey();
    //Movimientos de rey
    rey->movimientos_legales(this, &movimientos, bitboards[6], piezas_negras(), piezas_blancas());
    if (chequearEnroqueCorto()) {
        movimientos.push_back(operaciones_bit::crearJugada(60, 58, CASTLING));
    }
    if (chequearEnroqueLargo()) {
        movimientos.push_back(operaciones_bit::crearJugada(60, 62, CASTLING));
    }
    delete rey;
    return movimientos;
}

vector<u_short> &
Tablero::obtener_movimientos_caballo_negro(vector<u_short> &movimientos) {//Movimientos de caballo
    auto caballo = new Caballo();
    caballo->obtener_movimientos(this, &movimientos, bitboards[10], piezas_negras(), piezas_blancas());
    delete caballo;
    return movimientos;
}


U64 Tablero::bitboard_movimientos_dama_negra(U64 bitboard) {

    return bitboard_movimientos_alfil_negro(bitboard) | bitboard_movimientos_torre_negra(bitboard);

}

void Tablero::obtener_movimientos_dama_negra(vector<u_short> &movimientos, U64 bitboard) {//Movimientos de dama
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

            U64 maskPiezasBloqueandoDelMove =
                    constantes::attackMasksTorre[casillaLlegada - 1] & (todas_las_piezas());
            if (esJaque(movimientosDeTorre[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaTorre[casillaLlegada - 1] >> 52], 1)) {
                /* if(ganoElNegro()){
                     movimientos.push_back(operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                 }
                 else*/ if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURECHECK));
                } else {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CHECK));
                }
            } else if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));
            } else {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, QUIET));
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

            U64 maskPiezasBloqueandoDelMove2 = constantes::attackMasksAlfil[casillaLlegada2 - 1] & (todas_las_piezas());
            if (esJaque(movimientosDeAlfil[casillaLlegada2 - 1]
                        [maskPiezasBloqueandoDelMove2 * constantes::magicsParaAlfil[casillaLlegada2 - 1] >> 55], 1)) {
                /*if(ganoElNegro()){
                    movimientos.push_back(operaciones_bit::crearJugada(LSB,casillaLlegada2,CHECKMATE));
                }
                else*/ if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada2, CAPTURECHECK));
                } else {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada2, CHECK));
                }
            } else if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada2, CAPTURE));
            } else {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada2, QUIET));
            }
        }
    }
}

void Tablero::obtener_movimientos_alfil_negro(vector<u_short> &movimientos, U64 bitboard) {
    while (bitboard > 0) {
        int LSB = operaciones_bit::LSB(bitboard);
        U64 alfil = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeAlfil[LSB - 1][alfil * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
        jugadasEnPos = jugadasEnPos & ~piezas_negras();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksAlfil[casillaLlegada - 1] & (todas_las_piezas());
            if (esJaque(movimientosDeAlfil[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaAlfil[casillaLlegada - 1] >> 55], 1)) {
                /*if(ganoElNegro()){
                    movimientos.push_back(operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else */if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURECHECK));
                } else {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CHECK));
                }
            } else if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));
            } else {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, QUIET));
            }
        }
    }
}

void Tablero::obtener_movimientos_torre_negra(vector<u_short> &movimientos, U64 bitboard) {//Movimientos de torre
    while (bitboard > 0) {
        int LSB = operaciones_bit::LSB(bitboard);
        U64 torre = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeTorre[LSB - 1][torre * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];
        jugadasEnPos = jugadasEnPos & ~piezas_negras();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksTorre[casillaLlegada - 1] & (todas_las_piezas());
            if (esJaque(movimientosDeTorre[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaTorre[casillaLlegada - 1] >> 52], 1)) {
                /*if(ganoElNegro()){
                    movimientos.push_back(operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*/ if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURECHECK));
                } else {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CHECK));
                }
            } else if ((casillaLlegadaBitboard & piezas_blancas()) > 0) {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));
            } else {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, QUIET));
            }
        }
    }
}

vector<u_short> &Tablero::obtener_movimientos_peon_blanco(vector<u_short> &movimientos) {
    auto peon = new Peon();

    //Generar movimientos de peón
    peon->movimientos_legales(this, &movimientos, bitboards[5],
                              piezas_blancas(),
                              piezas_negras());
    delete peon;
    return movimientos;
}

vector<u_short> &Tablero::obtener_movimientos_rey_blanco(vector<u_short> &movimientos) {
    auto rey = new Rey();
    //Generar movimientos de rey
    rey->movimientos_legales(this, &movimientos, bitboards[0], piezas_blancas(), piezas_negras());
    if (chequearEnroqueCorto()) {
        movimientos.push_back(operaciones_bit::crearJugada(4, 2, CASTLING));
    }
    if (chequearEnroqueLargo()) {
        movimientos.push_back(operaciones_bit::crearJugada(4, 6, CASTLING));
    }
    delete rey;
    return movimientos;
}

vector<u_short> &Tablero::obtener_movimientos_caballo_blanco(vector<u_short> &movimientos) {
    auto caballo = new Caballo();
    caballo->obtener_movimientos(this, &movimientos, bitboards[4], piezas_blancas(), piezas_negras());
    delete caballo;
    return movimientos;
}

void Tablero::obtener_movimientos_dama_blanca(vector<u_short> &movimientos, U64 bitboard) {
    while (bitboard > 0) {
        //Primero se generan los "movimientos de torre" para la dama
        int LSB = operaciones_bit::LSB(bitboard);
        U64 dama = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeTorre[LSB - 1][dama * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];
        jugadasEnPos = jugadasEnPos & ~piezas_blancas();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksTorre[casillaLlegada - 1] & (todas_las_piezas());
            if (esJaque(movimientosDeTorre[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaTorre[casillaLlegada - 1] >> 52], 0)) {
                /* if(ganoELBlanco()){
                     movimientos.push_back(operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                 }
                 else*/ if ((casillaLlegadaBitboard & piezas_negras()) > 0) {

                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURECHECK));
                } else {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CHECK));
                }
            } else if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));
            } else {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, QUIET));
            }
        }

        //Por ultimo se generan los "movimientos de alfil" para la dama
        U64 dama2 = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos2 = movimientosDeAlfil[LSB - 1][dama2 * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
        jugadasEnPos2 = jugadasEnPos2 & ~piezas_blancas();
        while (jugadasEnPos2 > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos2);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksAlfil[casillaLlegada - 1] & (todas_las_piezas());
            if (esJaque(movimientosDeAlfil[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaAlfil[casillaLlegada - 1] >> 55], 0)) {
                /*if(ganoELBlanco()){
                    movimientos.push_back(operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*/ if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURECHECK));
                } else {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CHECK));
                }
            } else if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));
            } else {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, QUIET));
            }
        }
    }
}

void Tablero::obtener_movimientos_alfil_blanco(vector<u_short> &movimientos, U64 bitboard) {
    while (bitboard > 0) {
        int LSB = operaciones_bit::LSB(bitboard);
        U64 alfil = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
        U64 jugadasEnPos = movimientosDeAlfil[LSB - 1][alfil * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
        jugadasEnPos = jugadasEnPos & ~piezas_blancas();
        while (jugadasEnPos > 0) {
            int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
            U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

            U64 maskPiezasBloqueandoDelMove = constantes::attackMasksAlfil[casillaLlegada - 1] & (todas_las_piezas());
            if (esJaque(movimientosDeAlfil[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaAlfil[casillaLlegada - 1] >> 55], 0)) {
                /*if(ganoELBlanco()){
                    movimientos.push_back(operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*/ if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURECHECK));
                } else {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CHECK));
                }
            } else if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));
            } else {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, QUIET));
            }
        }
    }
}

void Tablero::obtener_movimientos_torre_blanca(vector<u_short> &movimientos, U64 bitboard) {
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
            if (esJaque(movimientosDeTorre[casillaLlegada - 1]
                        [maskPiezasBloqueandoDelMove * constantes::magicsParaTorre[casillaLlegada - 1] >> 52], 0)) {
                /*if(ganoELBlanco()){
                    movimientos.push_back(operaciones_bit::crearJugada(LSB,casillaLlegada,CHECKMATE));
                }
                else*/ if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURECHECK));
                } else {
                    movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CHECK));

                }

            }

                //Si se toma una pieza rival, se agrega como CAPTURE
            else if ((casillaLlegadaBitboard & piezas_negras()) > 0) {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));
            }

                //Si no se cumple ninguna de las condiciones anteriores, es un movimiento "QUIET"
            else {
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, QUIET));
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
        U64 movimientosDamaNegra = bitboard_movimientos_dama_negra(bitboards[7]);
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
        }


        U64 movimientosCaballoNegro = bitboard_movimientos_caballo_negro(bitboards[10]);

        if ((movimientosCaballoNegro & reyPropio) > 0) {
            return true;
        }

        U64 movimientosPeonNegro = bitboard_movimientos_peon_negro(bitboards[11]);

        if ((movimientosPeonNegro & reyPropio) > 0) {
            return true;
        }

    } else {
        U64 movimientosDamaBlanca = bitboard_movimientos_dama_blanca(bitboards[1]);

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


    }
    return false;
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
        U64 bitboardCaballo = operaciones_bit::setBit(0L, casillaCaballoBlanco, 1);
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
    U64 bitboardDeTorres = bitboards[2];
    U64 bitboardDeAlfiles = bitboards[3];
    U64 bitboardDeDamas = bitboards[1];
    U64 bitboardDePeones = bitboards[5];
    U64 bitboardDeCaballo = bitboards[4];
    U64 movimientosPosibles = 0ULL;
    while (bitboardDeTorres > 0) {
        int casillaDeTorre = operaciones_bit::LSB(bitboardDeTorres);
        U64 attackMaskTorre = constantes::attackMasksTorre[casillaDeTorre - 1];
        U64 occupancyMask = attackMaskTorre & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaTorre[casillaDeTorre - 1] >> (64 - 12);
        U64 movimientosDePieza = movimientosDeTorre[casillaDeTorre - 1][key];
        movimientosPosibles |= movimientosDePieza;
    }
    while (bitboardDeAlfiles > 0) {
        int casillaDeAlfil = operaciones_bit::LSB(bitboardDeAlfiles);
        U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaDeAlfil - 1];
        U64 occupancyMask = attackMaskAlfil & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaAlfil[casillaDeAlfil - 1] >> (64 - 9);
        U64 movimientosDePieza = movimientosDeAlfil[casillaDeAlfil - 1][key];
        movimientosPosibles |= movimientosDePieza;
    }
    while (bitboardDeDamas > 0) {
        int casillaDeDama = operaciones_bit::LSB(bitboardDeDamas);
        U64 attackMaskTorre = constantes::attackMasksTorre[casillaDeDama - 1];
        U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaDeDama - 1];
        U64 occupancyMask = attackMaskTorre & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaTorre[casillaDeDama - 1] >> (64 - 12);
        U64 movimientosDePieza = movimientosDeTorre[casillaDeDama - 1][key];
        occupancyMask = attackMaskAlfil & todas_las_piezas();
        key = occupancyMask * constantes::magicsParaAlfil[casillaDeDama - 1] >> (64 - 9);
        movimientosDePieza |= movimientosDeAlfil[casillaDeDama - 1][key];
        movimientosPosibles |= movimientosDePieza;
    }
    auto peon = new Peon();
    while (bitboardDePeones > 0) {
        int casillaDePeon = operaciones_bit::LSB(bitboardDePeones);

        U64 movimientosDePieza = peon->generar_movimientos_legales(casillaDePeon, piezas_blancas(), piezas_negras(), 0);
        movimientosPosibles |= movimientosDePieza;

    }
    delete peon;
    auto caballo = new Caballo();

    while (bitboardDeCaballo > 0) {
        int casillaDeCaballo = operaciones_bit::LSB(bitboardDeCaballo);
        U64 bitboardCaballo = operaciones_bit::setBit(0L, casillaDeCaballo, 1);
        U64 movimientosDePieza = caballo->generar_movimientos_legales(bitboardCaballo, piezas_blancas(),
                                                                      piezas_negras(), 0);
        movimientosPosibles |= movimientosDePieza;
    }
    delete caballo;

    return movimientosPosibles;
}

U64 Tablero::obtenerAttackMapNegras() {

    U64 bitboardDeTorres = bitboards[8];
    U64 bitboardDeAlfiles = bitboards[9];
    U64 bitboardDeDamas = bitboards[7];
    U64 bitboardDePeones = bitboards[11];
    U64 bitboardDeCaballo = bitboards[10];
    U64 movimientosPosibles = 0ULL;
    while (bitboardDeTorres > 0) {
        int casillaDeTorre = operaciones_bit::LSB(bitboardDeTorres);
        U64 attackMaskTorre = constantes::attackMasksTorre[casillaDeTorre - 1];
        U64 occupancyMask = attackMaskTorre & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaTorre[casillaDeTorre - 1] >> (64 - 12);
        U64 movimientosDePieza = movimientosDeTorre[casillaDeTorre - 1][key];
        movimientosPosibles |= movimientosDePieza;
    }
    while (bitboardDeAlfiles > 0) {
        int casillaDeAlfil = operaciones_bit::LSB(bitboardDeAlfiles);
        U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaDeAlfil - 1];
        U64 occupancyMask = attackMaskAlfil & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaAlfil[casillaDeAlfil - 1] >> (64 - 9);
        U64 movimientosDePieza = movimientosDeAlfil[casillaDeAlfil - 1][key];
        movimientosPosibles |= movimientosDePieza;
    }
    while (bitboardDeDamas > 0) {
        int casillaDeDama = operaciones_bit::LSB(bitboardDeDamas);
        U64 attackMaskTorre = constantes::attackMasksTorre[casillaDeDama - 1];
        U64 attackMaskAlfil = constantes::attackMasksAlfil[casillaDeDama - 1];
        U64 occupancyMask = attackMaskTorre & todas_las_piezas();
        U64 key = occupancyMask * constantes::magicsParaTorre[casillaDeDama - 1] >> (64 - 12);
        U64 movimientosDePieza = movimientosDeTorre[casillaDeDama - 1][key];
        occupancyMask = attackMaskAlfil & todas_las_piezas();
        key = occupancyMask * constantes::magicsParaAlfil[casillaDeDama - 1] >> (64 - 9);
        movimientosDePieza |= movimientosDeAlfil[casillaDeDama - 1][key];
        movimientosPosibles |= movimientosDePieza;

    }
    while (bitboardDePeones > 0) {
        int casillaDePeon = operaciones_bit::LSB(bitboardDePeones);
        auto peon = new Peon();
        U64 movimientosDePieza = peon->generar_movimientos_legales(casillaDePeon, piezas_negras(), piezas_blancas(), 1);
        movimientosPosibles |= movimientosDePieza;
        delete peon;

    }
    while (bitboardDeCaballo > 0) {
        int casillaDeCaballo = operaciones_bit::LSB(bitboardDeCaballo);
        auto caballo = new Caballo();
        U64 bitboardCaballo = operaciones_bit::setBit(0L, casillaDeCaballo, 1);
        U64 movimientosDePieza = caballo->generar_movimientos_legales(bitboardCaballo, piezas_negras(),
                                                                      piezas_blancas(), 0);
        movimientosPosibles |= movimientosDePieza;
        delete caballo;

    }
    return movimientosPosibles;
}

bool Tablero::chequearEnroqueCorto() {
    // enroque corto blancas
    if (_turno == 0) {

        if (enroqueCortoBlanco.back()) {
            U64 attackMapNegras = (obtenerAttackMapNegras());
            bool estaLaTorre = (bitboards[2] & 0b1ULL) > 0;
            bool reyEnJaque = reyPropioEnJaque(_turno);
            bool sinAtaqueRival = (attackMapNegras & 0b110ULL) == 0;
            bool zonaLiberada = (0b110ULL & todas_las_piezas()) == 0; // 110 es f1 y g1
            return sinAtaqueRival && zonaLiberada && !reyEnJaque && estaLaTorre;
        }

    } else {

        if (enroqueCortoNegro.back()) {
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
        if (enroqueLargoBlanco.back()) {
            U64 attackMapNegras = (obtenerAttackMapNegras());
            bool estaLaTorre = (bitboards[2] & 0b10000000ULL) > 0;
            bool reyEnJaque = reyPropioEnJaque(_turno);
            bool sinAtaqueRival = ((attackMapNegras & 0b1110000ULL) == 0);
            bool zonaLiberada = ((0b1110000ULL & todas_las_piezas()) == 0); // 1110000 es b1, c1 y d1
            return sinAtaqueRival && zonaLiberada && !reyEnJaque && estaLaTorre;
        }
    } else {

        if (enroqueLargoNegro.back()) {
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
        actualizarMaterial(jugada);
        enroqueCortoBlanco.push_back(false);
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(false);
        actualizarOcupacion(jugada);
        actualizarZobristKey(jugada, true);
        historialDePosiciones.push_back(zobrist);

        bitboards[0] = operaciones_bit::setBit(bitboards[0], 4, 0);
        bitboards[0] = operaciones_bit::setBit(bitboards[0], 2, 1);
        bitboards[2] = operaciones_bit::setBit(bitboards[2], 1, 0);
        bitboards[2] = operaciones_bit::setBit(bitboards[2], 3, 1);
        _jugadas.push_back(jugada);
        historialPosiciones.push_back(bitboards);
        _turno = 1;


        enrocoBlancas = true;
        return true;

    } else if (operaciones_bit::getLlegada(jugada) == 6 && chequearEnroqueLargo()) {
        actualizarMaterial(jugada);
        enroqueCortoBlanco.push_back(false);
        enroqueCortoNegro.push_back(enroqueCortoNegro.back());
        enroqueLargoNegro.push_back(enroqueLargoNegro.back());
        enroqueLargoBlanco.push_back(false);
        actualizarOcupacion(jugada);
        actualizarZobristKey(jugada, true);
        historialDePosiciones.push_back(zobrist);

        bitboards[0] = operaciones_bit::setBit(bitboards[0], 4, 0);
        bitboards[0] = operaciones_bit::setBit(bitboards[0], 6, 1);
        bitboards[2] = operaciones_bit::setBit(bitboards[2], 8, 0);
        bitboards[2] = operaciones_bit::setBit(bitboards[2], 5, 1);
        _jugadas.push_back(jugada);
        historialPosiciones.push_back(bitboards);
        _turno = 1;
        enrocoBlancas = true;


        return true;
    } else if (operaciones_bit::getLlegada(jugada) == 58 && chequearEnroqueCorto()) {
        actualizarMaterial(jugada);
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(false);
        enroqueLargoNegro.push_back(false);
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        actualizarOcupacion(jugada);
        actualizarZobristKey(jugada, true);
        historialDePosiciones.push_back(zobrist);

        bitboards[6] = operaciones_bit::setBit(bitboards[6], 60, 0);
        bitboards[6] = operaciones_bit::setBit(bitboards[6], 58, 1);
        bitboards[8] = operaciones_bit::setBit(bitboards[8], 57, 0);
        bitboards[8] = operaciones_bit::setBit(bitboards[8], 59, 1);
        _jugadas.push_back(jugada);
        historialPosiciones.push_back(bitboards);
        _turno = 0;

        enrocoNegras = true;
        return true;
    } else if (operaciones_bit::getLlegada(jugada) == 62 && chequearEnroqueLargo()) {
        actualizarMaterial(jugada);
        enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
        enroqueCortoNegro.push_back(false);
        enroqueLargoNegro.push_back(false);
        enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());
        actualizarOcupacion(jugada);
        actualizarZobristKey(jugada, true);
        historialDePosiciones.push_back(zobrist);

        bitboards[6] = operaciones_bit::setBit(bitboards[6], 60, 0);
        bitboards[6] = operaciones_bit::setBit(bitboards[6], 62, 1);
        bitboards[8] = operaciones_bit::setBit(bitboards[8], 64, 0);
        bitboards[8] = operaciones_bit::setBit(bitboards[8], 61, 1);
        _jugadas.push_back(jugada);
        historialPosiciones.push_back(bitboards);
        _turno = 0;

        enrocoNegras = true;

        return true;
    }
    return false;
}

void Tablero::moverPiezaTrusted(int salida, int llegada, int tipoDeJugada) {
    U64 salidaBitboard = operaciones_bit::setBit(0L, salida, 1);
    U64 llegadaBitboard = operaciones_bit::setBit(0L, llegada, 1);
    u_short aRealizar = operaciones_bit::crearJugada(salida, llegada, tipoDeJugada);
    if (_jugadas.size() > 0) {
        if (operaciones_bit::getSalida(_jugadas.back()) == salida &&
            operaciones_bit::getLlegada(_jugadas.back()) == llegada) {
            return;
        }
    }

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
    actualizarZobristKey(aRealizar, true);
    actualizarCantMovesPiezasMenores(aRealizar, true);
    historialDePosiciones.push_back(zobrist);

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
    enroqueCortoBlanco.push_back(enroqueCortoBlanco.back());
    enroqueCortoNegro.push_back(enroqueCortoNegro.back());
    enroqueLargoNegro.push_back(enroqueLargoNegro.back());
    enroqueLargoBlanco.push_back(enroqueLargoBlanco.back());

    if (tipoDeJugada == CHECKMATE) {
        if (_turno == 0) {
            ganoBlanco = true;
        } else {
            ganoNegro = true;
        }

    }
    cambiarTurno();

}

void Tablero::cambiarTurno() {
    if (_turno == 0) {
        _turno = 1;
    } else {
        _turno = 0;
    }
}

bool Tablero::ganoELBlanco() {
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
}


U64 Tablero::zobristKey() {
    if (zobrist != 0) {
        return zobrist;

    }
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
    if (enroqueLargoBlanco.back()) {
        key ^= constantes::enroqueLargoBlanco;
    }
    if (enroqueCortoBlanco.back()) {
        key ^= constantes::enroqueCortoBlanco;
    }
    if (enroqueLargoNegro.back()) {
        key ^= constantes::enroqueLargoNegro;
    }

    if (enroqueCortoNegro.back()) {
        key ^= constantes::enroqueCortoNegro;
    }
    if (!(_jugadas.empty()) && tipoDePieza(_jugadas.back()) == PEON) {
        int salida = operaciones_bit::getSalida(_jugadas.back());
        int llegada = operaciones_bit::getLlegada(_jugadas.back());
        int tipoDeMovimiento = operaciones_bit::getTipoDeJugada(_jugadas.back());
        if (tipoDeMovimiento == QUIET || tipoDeMovimiento == CHECK) {
            if (abs(salida - llegada) == 16) {
                int columnaDeEnpassant = (llegada % 8) - 1;
                if (columnaDeEnpassant == 1) {
                    key ^= constantes::enPassant[7];
                } else {
                    key ^= constantes::enPassant[columnaDeEnpassant - 1];
                }
            }
        }
    }
    return key;

}

vector<u_short> Tablero::generar_capturas(int turno) {
    vector<u_short> movimientos;
    //turno = 0 es blancas, turno = 1 es negras
    if (turno == 0) {

        //Generar movimientos de torres
        U64 bitboardTemp = bitboards[2]; //bitboard de torres

        //Mientras haya alguna torre en el tablero:
        while (bitboardTemp > 0) {
            //Se obtiene la casilla de la torre más próxima al bit menos significativo
            int LSB = operaciones_bit::LSB(bitboardTemp);

            //Se obtiene la máscara de la torre en la casilla correspondiente
            U64 torre = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());

            //Se obtienen los movimientos posibles de la torre mediante la tabla de hash
            U64 jugadasEnPos = movimientosDeTorre[LSB - 1][torre * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];

            //Se toman solo las capturas:
            jugadasEnPos = jugadasEnPos & piezas_negras();

            //Se itera sobre cada movimiento posible
            while (jugadasEnPos > 0) {
                //Se obtiene la casilla de llegada del movimiento como int
                int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));


            }

        }

        //Generar movimientos de alfiles. Aplica la misma lógica que en el caso de la torre.
        bitboardTemp = bitboards[3];
        while (bitboardTemp > 0) {
            int LSB = operaciones_bit::LSB(bitboardTemp);
            U64 alfil = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
            U64 jugadasEnPos = movimientosDeAlfil[LSB - 1][alfil * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
            //Se toman solo las capturas:
            jugadasEnPos = jugadasEnPos & piezas_negras();

            //Se itera sobre cada movimiento posible
            while (jugadasEnPos > 0) {
                //Se obtiene la casilla de llegada del movimiento como int
                int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);

                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));


            }
        }

        //Generar movimientos de dama. Aplica la misma lógica que en el caso de la torre y alfil.
        bitboardTemp = bitboards[1];
        while (bitboardTemp > 0) {
            //Primero se generan los "movimientos de torre" para la dama
            int LSB = operaciones_bit::LSB(bitboardTemp);
            U64 dama = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());
            U64 jugadasEnPos = movimientosDeTorre[LSB - 1][dama * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];
            //Se toman solo las capturas:
            jugadasEnPos = jugadasEnPos & piezas_negras();

            //Se itera sobre cada movimiento posible
            while (jugadasEnPos > 0) {
                //Se obtiene la casilla de llegada del movimiento como int
                int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
                U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

                //Si se toma una pieza rival, se agrega como CAPTURE
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));


            }

            //Por ultimo se generan los "movimientos de alfil" para la dama
            U64 dama2 = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
            U64 jugadasEnPos2 = movimientosDeAlfil[LSB - 1][dama2 * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
            //Se toman solo las capturas:
            jugadasEnPos = jugadasEnPos & piezas_negras();

            //Se itera sobre cada movimiento posible
            while (jugadasEnPos > 0) {
                //Se obtiene la casilla de llegada del movimiento como int
                int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
                U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

                //Si se toma una pieza rival, se agrega como CAPTURE
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));


            }
        }

        //Generar movimientos de caballo.
        Caballo::capturas(this, &movimientos, bitboards[4], this->piezas_blancas(), this->piezas_negras());

        //Generar movimientos de rey
        Rey::capturas(this, &movimientos, bitboards[0], this->piezas_blancas(), this->piezas_negras());


        //Generar movimientos de peón
        bitboardTemp = bitboards[5];
        while (bitboardTemp > 0) {
            int casilla = operaciones_bit::LSB(bitboardTemp);
            U64 posiblesCapturasBitboard = Peon::capturas(casilla, this->piezas_blancas(),
                                                          this->piezas_negras(), 0);

            posiblesCapturasBitboard = posiblesCapturasBitboard & this->piezas_negras();
            while (posiblesCapturasBitboard > 0) {
                int casillaLlegada = operaciones_bit::LSB(posiblesCapturasBitboard);
                movimientos.push_back(operaciones_bit::crearJugada(casilla, casillaLlegada, CAPTURE));
            }
        }

    }

        //Generar movimientos para las negras. Sigue la misma lógica que para las blancas.
    else {
        //Generar movimientos de torres
        U64 bitboardTemp = bitboards[8]; //bitboard de torres

        //Mientras haya alguna torre en el tablero:
        while (bitboardTemp > 0) {
            //Se obtiene la casilla de la torre más próxima al bit menos significativo
            int LSB = operaciones_bit::LSB(bitboardTemp);

            //Se obtiene la máscara de la torre en la casilla correspondiente
            U64 torre = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());

            //Se obtienen los movimientos posibles de la torre mediante la tabla de hash
            U64 jugadasEnPos = movimientosDeTorre[LSB - 1][torre * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];

            //Se toman solo las capturas:
            jugadasEnPos = jugadasEnPos & piezas_blancas();

            //Se itera sobre cada movimiento posible
            while (jugadasEnPos > 0) {
                //Se obtiene la casilla de llegada del movimiento como int
                int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
                U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);


                //Si se toma una pieza rival, se agrega como CAPTURE
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));


            }

        }

        //Generar movimientos de alfiles. Aplica la misma lógica que en el caso de la torre.
        bitboardTemp = bitboards[9];
        while (bitboardTemp > 0) {
            int LSB = operaciones_bit::LSB(bitboardTemp);
            U64 alfil = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
            U64 jugadasEnPos = movimientosDeAlfil[LSB - 1][alfil * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
            //Se toman solo las capturas:
            jugadasEnPos = jugadasEnPos & piezas_blancas();

            //Se itera sobre cada movimiento posible
            while (jugadasEnPos > 0) {
                //Se obtiene la casilla de llegada del movimiento como int
                int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
                U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

                //Si se toma una pieza rival, se agrega como CAPTURE
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));


            }
        }

        //Generar movimientos de dama. Aplica la misma lógica que en el caso de la torre y alfil.
        bitboardTemp = bitboards[7];
        while (bitboardTemp > 0) {
            //Primero se generan los "movimientos de torre" para la dama
            int LSB = operaciones_bit::LSB(bitboardTemp);
            U64 dama = constantes::attackMasksTorre[LSB - 1] & (todas_las_piezas());
            U64 jugadasEnPos = movimientosDeTorre[LSB - 1][dama * constantes::magicsParaTorre[LSB - 1] >> (64 - 12)];
            //Se toman solo las capturas:
            jugadasEnPos = jugadasEnPos & piezas_blancas();

            //Se itera sobre cada movimiento posible
            while (jugadasEnPos > 0) {
                //Se obtiene la casilla de llegada del movimiento como int
                int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
                U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

                //Si se toma una pieza rival, se agrega como CAPTURE
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));


            }

            //Por ultimo se generan los "movimientos de alfil" para la dama
            U64 dama2 = constantes::attackMasksAlfil[LSB - 1] & (todas_las_piezas());
            U64 jugadasEnPos2 = movimientosDeAlfil[LSB - 1][dama2 * constantes::magicsParaAlfil[LSB - 1] >> (64 - 9)];
            //Se toman solo las capturas:
            jugadasEnPos = jugadasEnPos & piezas_blancas();

            //Se itera sobre cada movimiento posible
            while (jugadasEnPos > 0) {
                //Se obtiene la casilla de llegada del movimiento como int
                int casillaLlegada = operaciones_bit::LSB(jugadasEnPos);
                U64 casillaLlegadaBitboard = operaciones_bit::setBit(0L, casillaLlegada, 1);

                //Si se toma una pieza rival, se agrega como CAPTURE
                movimientos.push_back(operaciones_bit::crearJugada(LSB, casillaLlegada, CAPTURE));


            }
        }

        //Generar movimientos de caballo.
        Caballo::capturas(this, &movimientos, bitboards[10], this->piezas_negras(), this->piezas_blancas());

        //Generar movimientos de rey
        Rey::capturas(this, &movimientos, bitboards[6], this->piezas_negras(), this->piezas_blancas());


        //Generar movimientos de peón
        bitboardTemp = bitboards[11];
        while (bitboardTemp > 0) {
            int casilla = operaciones_bit::LSB(bitboardTemp);
            U64 posiblesCapturasBitboard = Peon::capturas(casilla,
                                                          this->piezas_negras(), this->piezas_blancas(), 1);
            posiblesCapturasBitboard = posiblesCapturasBitboard & this->piezas_blancas();

            while (posiblesCapturasBitboard > 0) {
                int casillaLlegada = operaciones_bit::LSB(posiblesCapturasBitboard);
                movimientos.push_back(operaciones_bit::crearJugada(casilla, casillaLlegada, CAPTURE));
            }
        }

    }





    /*//Se ordenan los movimientos según el orden del enum "tipoDeJugada"
    std::sort(movimientos.begin(),movimientos.end());*/
    return movimientos;

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

float Tablero::calcularOcupacionAlfil(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardAlfil = bitboards[3];
        while (bitboardAlfil > 0) {
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            valor += constantes::ocupacionAlfilBlanco[casilla - 1];
        }
    } else {
        U64 bitboardAlfil = bitboards[9];
        while (bitboardAlfil > 0) {
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            valor -= constantes::ocupacionAlfilBlanco[operaciones_bit::espejarCasilla(casilla - 1)];
        }
    }
    return valor;

}

float Tablero::calcularOcupacionCaballo(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardCaballo = bitboards[4];
        while (bitboardCaballo > 0) {
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            valor += constantes::ocupacionCaballoBlanco[casilla - 1];
        }
    } else {
        U64 bitboardCaballo = bitboards[10];
        while (bitboardCaballo > 0) {
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            valor -= constantes::ocupacionCaballoBlanco[operaciones_bit::espejarCasilla(casilla - 1)];
        }
    }
    return valor;
}

float Tablero::calcularOcupacionPeon(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardPeon = bitboards[5];
        while (bitboardPeon > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeon);
            valor += constantes::ocupacionPeonBlanco[casilla - 1];
        }
    } else {
        U64 bitboardPeon = bitboards[11];
        while (bitboardPeon > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeon);
            valor -= constantes::ocupacionPeonBlanco[operaciones_bit::espejarCasilla(casilla - 1)];
        }
    }
    return valor;
}

float Tablero::calcularOcupacionTorre(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardTorre = bitboards[2];
        while (bitboardTorre > 0) {
            int casilla = operaciones_bit::LSB(bitboardTorre);
            valor += constantes::ocupacionTorreBlanco[casilla - 1];
        }
    } else {
        U64 bitboardTorre = bitboards[8];
        while (bitboardTorre > 0) {
            int casilla = operaciones_bit::LSB(bitboardTorre);
            valor -= constantes::ocupacionTorreBlanco[operaciones_bit::espejarCasilla(casilla - 1)];
        }
    }
    return valor;
}

float Tablero::calcularOcupacionReina(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardReina = bitboards[1];
        while (bitboardReina > 0) {
            int casilla = operaciones_bit::LSB(bitboardReina);
            valor += constantes::ocupacionReinaBlanco[casilla - 1];
        }
    } else {
        U64 bitboardReina = bitboards[7];
        while (bitboardReina > 0) {
            int casilla = operaciones_bit::LSB(bitboardReina);
            valor -= constantes::ocupacionReinaBlanco[operaciones_bit::espejarCasilla(casilla - 1)];
        }
    }
    return valor;
}

float Tablero::calcularOcupacionRey(int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardRey = bitboards[0];
        while (bitboardRey > 0) {
            int casilla = operaciones_bit::LSB(bitboardRey);
            if (endgame) {
                valor += constantes::ocupacionReyBlancoFinal[casilla - 1];
            } else {
                valor += constantes::ocupacionReyBlancoMedioJuego[casilla - 1];
            }
        }
    } else {
        U64 bitboardRey = bitboards[6];
        while (bitboardRey > 0) {
            int casilla = operaciones_bit::LSB(bitboardRey);
            if (endgame) {
                valor -= constantes::ocupacionReyBlancoFinal[operaciones_bit::espejarCasilla(casilla - 1)];
            } else {
                valor -= constantes::ocupacionReyBlancoMedioJuego[operaciones_bit::espejarCasilla(casilla - 1)];
            }
        }
    }
    return valor;
}

float Tablero::calcularOcupacion(int color) {
    return (calcularOcupacionAlfil(color) +
            calcularOcupacionCaballo(color) +
            calcularOcupacionPeon(color) +
            calcularOcupacionReina(color) +
            calcularOcupacionTorre(color) + calcularOcupacionRey(color));

}

float Tablero::valoracionMaterial(int color) {
    float valor = 0;
    valor = contarMaterialSinPeones(color);
    if (contarMaterialSinPeones(0) <= 800 && contarMaterialSinPeones(1) >= -800) {
        endgame = true;
    }

    if (color == 0) {
        U64 bitboardPeonesBlancos = bitboards[5];
        while (bitboardPeonesBlancos > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeonesBlancos);
            valor += 100;
        }
    } else {
        U64 bitboardPeonesNegros = bitboards[11];
        while (bitboardPeonesNegros > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeonesNegros);
            valor -= 100;
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
    float ocupacionBlancas = historialOcupacionBlancas.back();
    float ocupacionNegras = historialOcupacionNegras.back();
    if (_turno == 0) {

        if (tipoDeJugada == ENPASSANT) {
            //Si la jugada fue ENPASSANT, se elimina el valor de ocupacion del peon rival y se
            //actualiza el valor de ocupacion del peon propio
            ocupacionBlancas += (constantes::ocupacionPeonBlanco[llegada - 1]
                                 - constantes::ocupacionPeonBlanco[salida - 1]);

            //"Eliminar" en este contexto es simplemente sumar el valor, ya que para las negras los
            //valores de ocupación favorables son negativos
            ocupacionNegras += constantes::ocupacionPeonBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            historialOcupacionBlancas.push_back(ocupacionBlancas);
            historialOcupacionNegras.push_back(ocupacionNegras);
            return;
        } else if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
            //Si la última jugada fue una promoción sin captura, se elimina el
            // valor de ocupación del peón coronado
            ocupacionBlancas -= constantes::ocupacionPeonBlanco[llegada - 8];
        } else if (tipoDeJugada == CASTLING) {
            //Si la última jugada fue un enroque, se actualiza la ocupación de las torres
            if (llegada == 2) {
                ocupacionBlancas += (constantes::ocupacionTorreBlanco[2]
                                     - constantes::ocupacionTorreBlanco[0]);
            } else {
                ocupacionBlancas += (constantes::ocupacionTorreBlanco[4]
                                     - constantes::ocupacionTorreBlanco[7]);
            }

        } else if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
                   tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
                   tipoDeJugada == CPDC || tipoDeJugada == CPIC) {
            //Si la última jugada fue una captura, se elimina el valor de ocupación de la pieza capturada
            int piezaCapturada = obtenerTipoDePieza(llegada);
            if (piezaCapturada == DAMA) {
                ocupacionNegras += constantes::ocupacionReinaBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            } else if (piezaCapturada == TORRE) {
                ocupacionNegras += constantes::ocupacionTorreBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            } else if (piezaCapturada == ALFIL) {
                ocupacionNegras += constantes::ocupacionAlfilBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            } else if (piezaCapturada == CABALLO) {
                ocupacionNegras += constantes::ocupacionCaballoBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            } else if (piezaCapturada == PEON) {
                ocupacionNegras += constantes::ocupacionPeonBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
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
            if (!endgame) {
                ocupacionBlancas += (constantes::ocupacionReyBlancoMedioJuego[llegada - 1]
                                     - constantes::ocupacionReyBlancoMedioJuego[salida - 1]);
            } else {
                ocupacionBlancas += (constantes::ocupacionReyBlancoFinal[llegada - 1]
                                     - constantes::ocupacionReyBlancoFinal[salida - 1]);
            }
        } else if (tipoDePieza == DAMA) {
            ocupacionBlancas += constantes::ocupacionReinaBlanco[llegada - 1];
            if (!esPromocion) {
                ocupacionBlancas -= constantes::ocupacionReinaBlanco[salida - 1];
            }
        } else if (tipoDePieza == TORRE) {
            ocupacionBlancas += constantes::ocupacionTorreBlanco[llegada - 1];
            if (!esPromocion) {
                ocupacionBlancas -= constantes::ocupacionTorreBlanco[salida - 1];
            }
        } else if (tipoDePieza == ALFIL) {
            ocupacionBlancas += constantes::ocupacionAlfilBlanco[llegada - 1];
            if (!esPromocion) {
                ocupacionBlancas -= constantes::ocupacionAlfilBlanco[salida - 1];
            }
        } else if (tipoDePieza == CABALLO) {
            ocupacionBlancas += constantes::ocupacionCaballoBlanco[llegada - 1];
            if (!esPromocion) {
                ocupacionBlancas -= constantes::ocupacionCaballoBlanco[salida - 1];
            }
        } else if (tipoDePieza == PEON) {
            ocupacionBlancas += (constantes::ocupacionPeonBlanco[llegada - 1]
                                 - constantes::ocupacionPeonBlanco[salida - 1]);
        }

        historialOcupacionBlancas.push_back(ocupacionBlancas);
        historialOcupacionNegras.push_back(ocupacionNegras);
    } else {
        if (tipoDeJugada == ENPASSANT) {
            //Si la jugada fue ENPASSANT, se elimina el valor de ocupacion del peon rival y se
            //actualiza el valor de ocupacion del peon propio
            ocupacionNegras -= (constantes::ocupacionPeonBlanco[operaciones_bit::espejarCasilla(llegada - 1)]
                                - constantes::ocupacionPeonBlanco[operaciones_bit::espejarCasilla(salida - 1)]);

            //"Eliminar" en este contexto es simplemente restar el valor, ya que para las blancas los
            //valores de ocupación favorables son positivos
            ocupacionBlancas -= constantes::ocupacionPeonBlanco[llegada - 1];

            historialOcupacionBlancas.push_back(ocupacionBlancas);
            historialOcupacionNegras.push_back(ocupacionNegras);

            return;
        } else if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
            //Si la última jugada fue una promoción sin captura, se elimina el
            // valor de ocupación del peón coronado
            ocupacionNegras += constantes::ocupacionPeonBlanco[operaciones_bit::espejarCasilla(llegada + 8)];
        } else if (tipoDeJugada == CASTLING) {
            //Si la última jugada fue un enroque, se actualiza la ocupación de las torres
            if (llegada == 58) {
                ocupacionNegras -= (constantes::ocupacionTorreBlanco[2]
                                    - constantes::ocupacionTorreBlanco[0]);
            } else {
                ocupacionNegras -= (constantes::ocupacionTorreBlanco[4]
                                    - constantes::ocupacionTorreBlanco[7]);
            }

        } else if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
                   tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
                   tipoDeJugada == CPDC || tipoDeJugada == CPIC) {
            //Si la última jugada fue una captura, se elimina el valor de ocupación de la pieza capturada
            int piezaCapturada = obtenerTipoDePieza(llegada);
            if (piezaCapturada == DAMA) {
                ocupacionBlancas -= constantes::ocupacionReinaBlanco[llegada - 1];
            } else if (piezaCapturada == TORRE) {
                ocupacionBlancas -= constantes::ocupacionTorreBlanco[llegada - 1];
            } else if (piezaCapturada == ALFIL) {
                ocupacionBlancas -= constantes::ocupacionAlfilBlanco[llegada - 1];
            } else if (piezaCapturada == CABALLO) {
                ocupacionBlancas -= constantes::ocupacionCaballoBlanco[llegada - 1];
            } else if (piezaCapturada == PEON) {
                ocupacionBlancas -= constantes::ocupacionPeonBlanco[llegada - 1];
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
            if (!endgame) {
                ocupacionNegras -= (
                        constantes::ocupacionReyBlancoMedioJuego[operaciones_bit::espejarCasilla(llegada - 1)]
                        - constantes::ocupacionReyBlancoMedioJuego[operaciones_bit::espejarCasilla(salida - 1)]);
            } else {
                ocupacionNegras -= (constantes::ocupacionReyBlancoFinal[operaciones_bit::espejarCasilla(llegada - 1)]
                                    - constantes::ocupacionReyBlancoFinal[operaciones_bit::espejarCasilla(salida - 1)]);
            }
        } else if (tipoDePieza == DAMA) {
            ocupacionNegras -= constantes::ocupacionReinaBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            if (!esPromocion) {
                ocupacionNegras += constantes::ocupacionReinaBlanco[operaciones_bit::espejarCasilla(salida - 1)];
            }
        } else if (tipoDePieza == TORRE) {
            ocupacionNegras -= constantes::ocupacionTorreBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            if (!esPromocion) {
                ocupacionNegras += constantes::ocupacionTorreBlanco[operaciones_bit::espejarCasilla(salida - 1)];
            }
        } else if (tipoDePieza == ALFIL) {
            ocupacionNegras -= constantes::ocupacionAlfilBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            if (!esPromocion) {
                ocupacionNegras += constantes::ocupacionAlfilBlanco[operaciones_bit::espejarCasilla(salida - 1)];
            }
        } else if (tipoDePieza == CABALLO) {
            ocupacionNegras -= constantes::ocupacionCaballoBlanco[operaciones_bit::espejarCasilla(llegada - 1)];
            if (!esPromocion) {
                ocupacionNegras += constantes::ocupacionCaballoBlanco[operaciones_bit::espejarCasilla(salida - 1)];
            }

        } else if (tipoDePieza == PEON) {
            ocupacionNegras -= (constantes::ocupacionPeonBlanco[operaciones_bit::espejarCasilla(llegada - 1)]
                                - constantes::ocupacionPeonBlanco[operaciones_bit::espejarCasilla(salida - 1)]);
        }
        historialOcupacionBlancas.push_back(ocupacionBlancas);
        historialOcupacionNegras.push_back(ocupacionNegras);
    }

}

void Tablero::actualizarMaterial(u_short jugada) {
    float materialBlancas = historialMaterialBlancas.back();
    float materialNegras = historialMaterialNegras.back();
    int salida = operaciones_bit::getSalida(jugada);
    int llegada = operaciones_bit::getLlegada(jugada);
    int tipoDePieza = obtenerTipoDePieza(salida);
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);
    if (_turno == 0) {
        if (tipoDeJugada == ENPASSANT) {
            //Si la jugada fue ENPASSANT, se elimina el valor del peon rival
            materialNegras -= constantes::valorPieza[11];
            historialMaterialBlancas.push_back(materialBlancas);
            historialMaterialNegras.push_back(materialNegras);
            return;
        } else if (tipoDeJugada == CASTLING) {
            historialMaterialBlancas.push_back(materialBlancas);
            historialMaterialNegras.push_back(materialNegras);
            return;
        } else if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
                   tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
                   tipoDeJugada == CPDC || tipoDeJugada == CPIC) {

            int piezaCapturada = obtenerTipoDePieza(llegada);
            materialNegras += constantes::valorPieza[piezaCapturada];

        }

        if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK || tipoDeJugada == CPDC
            || tipoDeJugada == CPIC || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER) {

            materialBlancas += constantes::valorPieza[salida];
            materialBlancas -= 100;

        }


    } else {
        if (tipoDeJugada == ENPASSANT) {
            //Si la jugada fue ENPASSANT, se elimina el valor del peon rival
            materialBlancas -= constantes::valorPieza[5];
            historialMaterialBlancas.push_back(materialBlancas);
            historialMaterialNegras.push_back(materialNegras);
            return;
        } else if (tipoDeJugada == CASTLING) {
            historialMaterialBlancas.push_back(materialBlancas);
            historialMaterialNegras.push_back(materialNegras);
            return;
        } else if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
                   tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
                   tipoDeJugada == CPDC || tipoDeJugada == CPIC) {

            int piezaCapturada = obtenerTipoDePieza(llegada);
            materialBlancas -= constantes::valorPieza[piezaCapturada];

        }

        if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK || tipoDeJugada == CPDC
            || tipoDeJugada == CPIC || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER) {

            materialNegras -= constantes::valorPieza[salida];
            materialNegras += 100;

        }

    }
    historialMaterialBlancas.push_back(materialBlancas);
    historialMaterialNegras.push_back(materialNegras);
}

void Tablero::actualizarZobristKey(u_short jugada, bool haciendoMovimiento) {
    U64 nuevaZobristKey = zobrist;
    int salida = operaciones_bit::getSalida(jugada);
    int llegada = operaciones_bit::getLlegada(jugada);
    int tipo_de_pieza;
    if (haciendoMovimiento) {
        tipo_de_pieza = obtenerTipoDePieza(salida);
    } else {
        tipo_de_pieza = obtenerTipoDePieza(llegada);
    }
    //Necesitamos diferenciar entre tipo de pieza blanca y negra (cosa que el enum que tenemos
    // no hace) para poder obtener el índice correcto. Para lograr eso simplemente se suma 6
    // al tipo de pieza que se obtenga solo en el caso de que sea negra. Si es blanca queda
    // igual.
    if ((_turno == 1 && haciendoMovimiento) || (_turno == 0 && !haciendoMovimiento)) {
        tipo_de_pieza += 6;
    }
    int tipoDeJugada = operaciones_bit::getTipoDeJugada(jugada);


    //Si el movimiento es una captura, se actualiza  el hash de la pieza capturada
    if (tipoDeJugada == CAPTURE || tipoDeJugada == CAPTURECHECK ||
        tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER ||
        tipoDeJugada == CPDC || tipoDeJugada == CPIC) {
        if (haciendoMovimiento) {
            int piezaCapturada = obtenerTipoDePieza(llegada);
            if (_turno == 0) {
                piezaCapturada += 6;
            }
            nuevaZobristKey ^= constantes::zobristKeys[piezaCapturada][llegada - 1];
        } else {
            for (int i = 0; i < 12; i++) {
                if ((historialPosiciones[historialPosiciones.size() - 2][i] >> (llegada - 1)) & 1) {
                    nuevaZobristKey ^= constantes::zobristKeys[i][llegada - 1];
                    break;
                }
            }
        }
    }
        //Se actualiza el hash para el peón que se captura en caso de que sea enpassant
    else if (tipoDeJugada == ENPASSANT) {
        int casillaPeonCapturado;
        int tipoDePeon;
        if ((_turno == 0 && haciendoMovimiento) || (_turno == 1 && !haciendoMovimiento)) {
            casillaPeonCapturado = llegada - 8;

            tipoDePeon = PEON + 6;
        } else {
            casillaPeonCapturado = llegada + 8;
            tipoDePeon = PEON;
        }
        nuevaZobristKey ^= constantes::zobristKeys[tipoDePeon][casillaPeonCapturado - 1];
    }

        //Caso enroque
    else if (tipoDeJugada == CASTLING) {
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

    //SE actualizan los hash para el enroque si es necesario
    if (enroqueLargoBlanco.back() != enroqueLargoBlanco[enroqueLargoBlanco.size() - 2]) {
        nuevaZobristKey ^= constantes::enroqueLargoBlanco;
    }
    if (enroqueCortoBlanco.back() != enroqueCortoBlanco[enroqueCortoBlanco.size() - 2]) {
        nuevaZobristKey ^= constantes::enroqueCortoBlanco;
    }
    if (enroqueLargoNegro.back() != enroqueLargoNegro[enroqueLargoNegro.size() - 2]) {
        nuevaZobristKey ^= constantes::enroqueLargoNegro;

    }
    if (enroqueCortoNegro.back() != enroqueCortoNegro[enroqueCortoNegro.size() - 2]) {
        nuevaZobristKey ^= constantes::enroqueCortoNegro;
    }

    if (tipoDeJugada == QUIET || tipoDeJugada == CHECK) {
        if (abs(salida - llegada) == 16 && (tipo_de_pieza == PEON || tipo_de_pieza == PEON + 6)) {
            int columnaDeEnpassant = (llegada % 8) - 1;
            if (columnaDeEnpassant == 1) {
                nuevaZobristKey ^= constantes::enPassant[7];
            } else {
                nuevaZobristKey ^= constantes::enPassant[columnaDeEnpassant - 1];
            }
        }
    }

    if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK || tipoDeJugada == CPDC
        || tipoDeJugada == CPIC || tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == PROMOTIONDER) {
        int tipoDePeon = PEON;
        if (tipoDeJugada == PROMOTION || tipoDeJugada == PROMOTIONCHECK) {
            tipo_de_pieza = tipoDePieza(salida);
            salida = llegada - 8;
            if ((_turno == 1 && haciendoMovimiento) || (_turno == 0 && !haciendoMovimiento)) {
                tipo_de_pieza += 6;
                salida = llegada + 8;
                tipoDePeon += 6;
            }

        } else if (tipoDeJugada == PROMOTIONIZQ || tipoDeJugada == CPIC) {
            tipo_de_pieza = tipoDePieza(salida);
            salida = llegada - 9;
            if ((_turno == 1 && haciendoMovimiento) || (_turno == 0 && !haciendoMovimiento)) {
                tipo_de_pieza += 6;
                salida = llegada + 9;
                tipoDePeon += 6;
            }
        } else if (tipoDeJugada == PROMOTIONDER || tipoDeJugada == CPDC) {
            tipo_de_pieza = tipoDePieza(salida);
            salida = llegada - 7;
            if ((_turno == 1 && haciendoMovimiento) || (_turno == 0 && !haciendoMovimiento)) {
                tipo_de_pieza += 6;
                salida = llegada + 7;
                tipoDePeon += 6;
            }
        }
        nuevaZobristKey ^= constantes::zobristKeys[tipoDePeon][salida - 1];
        nuevaZobristKey ^= constantes::zobristKeys[tipo_de_pieza][llegada - 1];
        zobrist = nuevaZobristKey;
        return;
    }

    //Se actualiza el hash para la pieza propia que se mueve
    nuevaZobristKey ^= constantes::zobristKeys[tipo_de_pieza][salida - 1];
    nuevaZobristKey ^= constantes::zobristKeys[tipo_de_pieza][llegada - 1];


    zobrist = nuevaZobristKey;
}

bool Tablero::esTripleRepeticion() {
    if (std::count(historialDePosiciones.begin(), historialDePosiciones.end(), zobrist) == 3) {
        return true;
    } else {
        return false;
    }
}

bool Tablero::seRecapturo() {
    int tipoDeJugadaUltima = operaciones_bit::getTipoDeJugada(_jugadas.back());
    int tipoDeJugadaAnteUltima = operaciones_bit::getTipoDeJugada(_jugadas[_jugadas.size() - 2]);
    int casillaLlegadaUltima = operaciones_bit::getLlegada(_jugadas.back());
    int casillaLlegadaAnteUltima = operaciones_bit::getLlegada(_jugadas[_jugadas.size() - 2]);
    if ((tipoDeJugadaUltima == CAPTURE || tipoDeJugadaUltima == CAPTURECHECK) &&
        (tipoDeJugadaAnteUltima == CAPTURE || tipoDeJugadaAnteUltima == CAPTURECHECK) &&
        casillaLlegadaUltima == casillaLlegadaAnteUltima) {
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
    if (enroqueCortoBlanco.back()) fen += "K";
    if (enroqueLargoBlanco.back()) fen += "Q";
    if (enroqueCortoNegro.back()) fen += "k";
    if (enroqueLargoNegro.back()) fen += "q";
    if (fen.back() == ' ') fen += "-";  // No castling rights


    fen += " ";


    if (!_jugadas.empty()) {
        int salidaUltimaJugada = operaciones_bit::getSalida(_jugadas.back());
        int llegadaUltimaJugada = operaciones_bit::getLlegada(_jugadas.back());
        if (!_jugadas.empty() && abs(salidaUltimaJugada - llegadaUltimaJugada) == 16 &&
            (obtenerTipoDePieza(llegadaUltimaJugada) == PEON)) {
            // Add en-passant target square

            if (salidaUltimaJugada > llegadaUltimaJugada) {
                fen += (char) ('a' + (llegadaUltimaJugada % 8) - 1);
                fen += std::to_string((llegadaUltimaJugada / 8) + 1);
            } else {
                fen += (char) ('a' + (llegadaUltimaJugada % 8) + 1);
                fen += std::to_string((llegadaUltimaJugada / 8) + 1);
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
    enroqueLargoBlanco.clear();
    enroqueCortoBlanco.clear();
    enroqueLargoNegro.clear();
    enroqueCortoNegro.clear();
    _jugadas.clear();
    historialDePosiciones.clear();
    historialMaterialBlancas.clear();
    historialMaterialNegras.clear();
    historialOcupacionBlancas.clear();
    historialOcupacionNegras.clear();

    for (int k = 0; k < 12; k++) {
        bitboards.push_back(0ULL);
    }

    //Se inician los enroques en false, luego con la info del fen se sabe cuáles setear en true
    enroqueCortoBlanco.push_back(false);
    enroqueLargoBlanco.push_back(false);
    enroqueLargoNegro.push_back(false);
    enroqueCortoNegro.push_back(false);



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
                    break;

                case 'R':
                    bitboards[2] = operaciones_bit::setBit(bitboards[2], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'B':
                    bitboards[3] = operaciones_bit::setBit(bitboards[3], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'N':
                    bitboards[4] = operaciones_bit::setBit(bitboards[4], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'P':
                    bitboards[5] = operaciones_bit::setBit(bitboards[5], casillaActual, 1);
                    casillaActual--;
                    break;


                case 'k':
                    bitboards[6] = operaciones_bit::setBit(bitboards[6], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'q':
                    bitboards[7] = operaciones_bit::setBit(bitboards[7], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'r':
                    bitboards[8] = operaciones_bit::setBit(bitboards[8], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'b':
                    bitboards[9] = operaciones_bit::setBit(bitboards[9], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'n':
                    bitboards[10] = operaciones_bit::setBit(bitboards[10], casillaActual, 1);
                    casillaActual--;
                    break;

                case 'p':
                    bitboards[11] = operaciones_bit::setBit(bitboards[11], casillaActual, 1);
                    casillaActual--;
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
            enroqueCortoBlanco.back() = true;
        } else if (fen[i] == 'Q') {
            enroqueLargoBlanco.back() = true;
        } else if (fen[i] == 'k') {
            enroqueCortoNegro.back() = true;
        } else if (fen[i] == 'q') {
            enroqueLargoNegro.back() = true;

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
                _jugadas.push_back(jugada);

            }
        }
    }

    //Se calcula el valor del material y se agrega al historial
    historialMaterialBlancas.push_back(valoracionMaterial(0));
    historialMaterialNegras.push_back(valoracionMaterial(1));
/*        controlCentroBlancas = controlDeCentro(0);
        controlCentroNegras = controlDeCentro(1);*/


    historialOcupacionBlancas.push_back(calcularOcupacion(0));
    historialOcupacionNegras.push_back(calcularOcupacion(1));

}
