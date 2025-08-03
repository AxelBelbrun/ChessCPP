//
// Created by axel on 14/12/23.
//

#include "Motor.h"
#include <chrono>
#include <iostream>
#include "Peon.h"
#include <algorithm>

Motor::Motor() {
    TT = new Tabla_transposicion();
    ply = -1;
    tablaInicializada = false;
    //Inicializamos todos los killer moves como -1
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 128; j++) {
            killerMove[i][j] = -1;
        }
    }
};

bool Motor::endgame = false;


float Motor::valoracion(Tablero *tablero) {
    float evaluacionMedio = 0.0;
    float evaluacionFinal = 0.0;
    float fase = calcularFaseDeJuego(tablero);

    float materialDePeones = (tablero->peonesBlancos - tablero->peonesNegros)*constantes::valorPieza[5];
    float materialDeDamas = (tablero->damasBlancas - tablero->damasNegras)*constantes::valorPieza[1];
    float materialDeAlfiles = (tablero->alfilesBlancos - tablero->alfilesNegros)*constantes::valorPieza[3];
    float materialDeCaballos = (tablero->caballosBlancos - tablero->caballosNegros)*constantes::valorPieza[4];
    float materialDeTorres = (tablero->torresBlancas - tablero->torresNegras)*constantes::valorPieza[2];

    float peonesF = materialDePeones*pesoMaterialPeonF;
    float torresF = materialDeTorres*pesoMaterialTorreF;
    float damasF = materialDeDamas*pesoMaterialDamaF;

    evaluacionMedio += materialDePeones + materialDeDamas + materialDeAlfiles + materialDeCaballos + materialDeTorres;
    evaluacionFinal += peonesF + torresF + damasF + materialDeAlfiles + materialDeCaballos;


    // Peones
    float ocuPeonBlancoMedioJuego = tablero->ocupacionPiezas[5] * pesoOcupacionPeonM;
    float ocuPeonNegroMedioJuego = tablero->ocupacionPiezas[11] * pesoOcupacionPeonM;
    float ocuPeonBlancoFinalJuego = tablero->ocupacionPiezas[5] * pesoOcupacionPeonF;
    float ocuPeonNegroFinalJuego = tablero->ocupacionPiezas[11] * pesoOcupacionPeonF;

// Caballos
    float ocuCaballoBlancoMedioJuego = tablero->ocupacionPiezas[4] * pesoOcupacionCaballoM;
    float ocuCaballoNegroMedioJuego = tablero->ocupacionPiezas[10] * pesoOcupacionCaballoM;
    float ocuCaballoBlancoFinalJuego = tablero->ocupacionPiezas[4] * pesoOcupacionCaballoF;
    float ocuCaballoNegroFinalJuego = tablero->ocupacionPiezas[10] * pesoOcupacionCaballoF;

// Alfiles
    float ocuAlfilBlancoMedioJuego = tablero->ocupacionPiezas[3] * pesoOcupacionAlfilM;
    float ocuAlfilNegroMedioJuego = tablero->ocupacionPiezas[9] * pesoOcupacionAlfilM;
    float ocuAlfilBlancoFinalJuego = tablero->ocupacionPiezas[3] * pesoOcupacionAlfilF;
    float ocuAlfilNegroFinalJuego = tablero->ocupacionPiezas[9] * pesoOcupacionAlfilF;

// Torres
    float ocuTorreBlancoMedioJuego = tablero->ocupacionPiezas[2] * pesoOcupacionTorreM;
    float ocuTorreNegroMedioJuego = tablero->ocupacionPiezas[8] * pesoOcupacionTorreM;
    float ocuTorreBlancoFinalJuego = tablero->ocupacionPiezas[2] * pesoOcupacionTorreF;
    float ocuTorreNegroFinalJuego = tablero->ocupacionPiezas[8] * pesoOcupacionTorreF;

// Damas
    float ocuDamaBlancoMedioJuego = tablero->ocupacionPiezas[1] * pesoOcupacionDamaM;
    float ocuDamaNegroMedioJuego = tablero->ocupacionPiezas[7] * pesoOcupacionDamaM;
    float ocuDamaBlancoFinalJuego = tablero->ocupacionPiezas[1] * pesoOcupacionDamaF;
    float ocuDamaNegroFinalJuego = tablero->ocupacionPiezas[7] * pesoOcupacionDamaF;

// Reyes (los valores ya son específicos de fase, no se multiplican por pesoOcupacionReyM/F)
    float ocuReyBlancoMedioJuego = tablero->ocupacionPiezas[0];
    float ocuReyNegroMedioJuego = tablero->ocupacionPiezas[6];
    float ocuReyBlancoFinalJuego = tablero->ocupacionPiezas[12];
    float ocuReyNegroFinalJuego = tablero->ocupacionPiezas[13];

    evaluacionMedio += ((ocuPeonBlancoMedioJuego + ocuPeonNegroMedioJuego) +
                       (ocuCaballoBlancoMedioJuego + ocuCaballoNegroMedioJuego) +
                       (ocuAlfilBlancoMedioJuego + ocuAlfilNegroMedioJuego) +
                       (ocuTorreBlancoMedioJuego + ocuTorreNegroMedioJuego) +
                       (ocuDamaBlancoMedioJuego + ocuDamaNegroMedioJuego));

    evaluacionFinal += ((ocuPeonBlancoFinalJuego + ocuPeonNegroFinalJuego) +
                       (ocuCaballoBlancoFinalJuego + ocuCaballoNegroFinalJuego) +
                       (ocuAlfilBlancoFinalJuego + ocuAlfilNegroFinalJuego) +
                       (ocuTorreBlancoFinalJuego + ocuTorreNegroFinalJuego) +
                       (ocuDamaBlancoFinalJuego + ocuDamaNegroFinalJuego));

/*    //Penalizar el perder el derecho a enrocar para las blancas
    float perdidaEnroqueBlancas = 0;
    if (tablero->perdidaEnroqueCortoBlancas) {
        perdidaEnroqueBlancas -= 70;
    }
    if (tablero->perdidaEnroqueLargoBlancas) {
        perdidaEnroqueBlancas -= 70;
    }



    tablero->_turno == 0 ? evaluacion += perdidaEnroqueBlancas : evaluacion -= perdidaEnroqueBlancas;
    //Penalizar el perder el derecho a enrocar para las negras

    float perdidaEnroqueNegras = 0;
    if (tablero->perdidaEnroqueLargoNegras) {

        perdidaEnroqueNegras += 70;
    }
    if (tablero->perdidaEnroqueCortoNegras) {
        perdidaEnroqueNegras += 70;
    }

    tablero->_turno == 1 ? evaluacion -= perdidaEnroqueNegras : evaluacion += perdidaEnroqueNegras;*/


//Penalizar el movimiento temprano de la dama blanca
/*    float movimientoTempDamaBlanca = 0;

    if (tablero-> contadorJugadas <= 8 && tablero->movimientoTempranoDamaBlanca) {
        tablero->_turno == 0 ? movimientoTempDamaBlanca -= constantes::castigoPorSacarDama : movimientoTempDamaBlanca += constantes::castigoPorSacarDama;
    }

    evaluacion += movimientoTempDamaBlanca;
    float movimientoTempDamaNegra = 0;
    if(tablero->contadorJugadas <=8 && tablero->movimientoTempranoDamaNegra){
        tablero->_turno == 1 ? movimientoTempDamaNegra -= constantes::castigoPorSacarDama : movimientoTempDamaNegra += constantes::castigoPorSacarDama;
    }
    evaluacion += movimientoTempDamaNegra;*/
    /*if (tablero->contadorJugadas <= 12) {
        float total = 0;
        //Penalizar multiples movimientos de la misma pieza menor blanca en la apertura
        if (tablero->movimientosAlfilBlancoF.second > 1) {
            total -= constantes::castigoMultiplesMovimientos * (tablero->movimientosAlfilBlancoF.second - 1);
        }
        if (tablero->movimientosAlfilBlancoC.second > 1) {
            total -= constantes::castigoMultiplesMovimientos * (tablero->movimientosAlfilBlancoC.second - 1);
        }
        if (tablero->movimientosCaballoBlancoB.second > 1) {
            total -= constantes::castigoMultiplesMovimientos * (tablero->movimientosCaballoBlancoB.second - 1);
        }
        if (tablero->movimientosCaballoBlancoG.second > 1) {
            total -= constantes::castigoMultiplesMovimientos * (tablero->movimientosCaballoBlancoG.second - 1);
        }
        //Penalizar el movimiento temprano de la dama negra



        //Penalizar multiples movimientos de la misma pieza menor negra en la apertura
        if (tablero->movimientosAlfilNegroF.second > 1) {
            total += constantes::castigoMultiplesMovimientos * (tablero->movimientosAlfilNegroF.second - 1);
        }
        if (tablero->movimientosAlfilNegroC.second > 1) {
            total += constantes::castigoMultiplesMovimientos * (tablero->movimientosAlfilNegroC.second - 1);
        }
        if (tablero->movimientosCaballoNegroB.second > 1) {
            total += constantes::castigoMultiplesMovimientos * (tablero->movimientosCaballoNegroB.second - 1);
        }
        if (tablero->movimientosCaballoNegroG.second > 1) {
            total += constantes::castigoMultiplesMovimientos * (tablero->movimientosCaballoNegroG.second - 1);
        }

        tablero->_turno == 0 ? evaluacion += total : evaluacion -= total;
    }*/
    //Premiar si las blancas enrocan

    float enrocaronLasBlancas = 0;
    if (tablero->enrocoBlancas) {
        enrocaronLasBlancas += constantes::premioPorEnrocar;
    }
    evaluacionMedio += enrocaronLasBlancas*premioEnroqueM;
    evaluacionFinal += enrocaronLasBlancas*premioEnroqueF;

    //Premiar si las negras enrocan

    float enrocaronLasNegras = 0;
    if (tablero->enrocoNegras) {
        enrocaronLasNegras -= constantes::premioPorEnrocar;
    }

    evaluacionMedio += enrocaronLasNegras*premioEnroqueM;
    evaluacionFinal += enrocaronLasNegras*premioEnroqueF;






    //Penalizar si los peones de d y e blancos están bloqueados
    if (tablero->obtenerTipoDePieza(12) == PEON) {
        if (tablero->obtenerTipoDePieza(20) != VACIO) {
            evaluacionMedio -= constantes::castigoPeonBloqueado*castigoPeonBloqueadoM;
            evaluacionFinal -= constantes::castigoPeonBloqueado*castigoPeonBloqueadoF;
        }
    }

    if (tablero->obtenerTipoDePieza(13) == PEON) {
        if (tablero->obtenerTipoDePieza(21) != VACIO) {
            evaluacionMedio -= constantes::castigoPeonBloqueado*castigoPeonBloqueadoM;
            evaluacionFinal -= constantes::castigoPeonBloqueado*castigoPeonBloqueadoF;        }
    }

    //Penalizar si los peones de d y e negros están bloqueados
    if (tablero->obtenerTipoDePieza(53) == PEON) {
        if (tablero->obtenerTipoDePieza(45) != VACIO) {
            evaluacionMedio += constantes::castigoPeonBloqueado*castigoPeonBloqueadoM;
            evaluacionFinal += constantes::castigoPeonBloqueado*castigoPeonBloqueadoF;        }
    }
    if (tablero->obtenerTipoDePieza(52) == PEON) {
        if (tablero->obtenerTipoDePieza(44) != VACIO) {
            evaluacionMedio += constantes::castigoPeonBloqueado*castigoPeonBloqueadoM;
            evaluacionFinal += constantes::castigoPeonBloqueado*castigoPeonBloqueadoF;        }
    }

/*    //Evaluacion de movilidad
    tablero->generar_movimientos(tablero-> _turno, ply);
    int contador = tablero->cantMovesGenerados[ply];
    evaluacion += contador;
    tablero->cantMovesGenerados[ply] = -1;*/

    //Evaluamos la seguridad del rey

        float reysec = 0.5*seguridadDelRey(tablero);
        evaluacionMedio += reysec*seguridadDelReyM;
        evaluacionFinal += reysec*seguridadDelReyF;

    double cant = tablero->contador_movilidad(tablero->_turno);

    evaluacionMedio += cant*movilidadM;
    evaluacionFinal += cant*movilidadF;

    //Control del centro

/*
    evaluacion += controlDeCentro(tablero, tablero->_turno);
*/

/*    if(tablero->endgame) {
        premioPorPeonesPasados = tablero->detectarPeonesPasados(0) - tablero->detectarPeonesPasados(1);
        evaluacion += (tablero->_turno == 0) ? premioPorPeonesPasados : -premioPorPeonesPasados;
    }*/
/*    if (tablero->_turno == 1) {
        evaluacion = -evaluacion;
    }*/
    float pesoM = fase;
    float pesoF = 256 - fase;

    float evaluacion = (evaluacionMedio*pesoM + evaluacionFinal*pesoF)/256.0;

    return (tablero->_turno == 0) ? evaluacion : -evaluacion;
}

/*float Motor::valoracionMaterial(Tablero *tablero) {
*//*    int valoracionBlancas = tablero->historial_material_blancas[tablero->contadorMaterialBlancas].first;
    int valoracionNegras = tablero->historial_material_negras[tablero->contadorMaterialNegras].first;
    if (!tablero->endgame && ( valoracionBlancas <= 900 &&
    valoracionNegras >= -900)) {
        tablero->endgame = true;
    }
    if(!tablero->endgame && (tablero->bitboards[1] == 0) && (tablero->bitboards[7] == 0)){
        tablero->endgame = true;
    }*//*


    return (tablero->_turno == 0) ? valor : -valor;
}*/

float Motor::valoracionMovilidad(Tablero *tablero) {
    float valor = 0;
    tablero->generar_movimientos(tablero->_turno, ply);
    if (tablero->_turno == 0) {

        valor += (float) (tablero->cantMovesGenerados[ply]);

    } else {

        valor -= (float) (tablero->cantMovesGenerados[ply]);

    }
    tablero->cantMovesGenerados[ply] = -1;


    return valor;
}


U64 Motor::perft(int depth, Tablero *tablero) {
    ply++;
    if (depth == 0) {
        return 1;
    }

    U64 nodes = 0;
    tablero->generar_movimientos(tablero->_turno, ply);

/*
    U64 clave1 = tablero-> zobrist;
*/
    for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
        u_short move = tablero->movimientos_generados[ply][i];
        if (tablero->moverPieza(operaciones_bit::getSalida(move), operaciones_bit::getLlegada(move),
                                operaciones_bit::getTipoDeJugada(move))) {

            nodes += perft(depth - 1, tablero);
            ply--;
           /* if((tablero-> _jugadas.size()) != (tablero->historialEnPassant.size())){
                cout << "cantidad distinta de jugadas y enpassant" << endl;
                for(auto x: tablero->_jugadas){
                    std::cout << x << std::endl;
                }
                exit(0);
            }*/
            tablero->deshacerMovimiento();
            /*U64 clave2 = tablero-> zobrist;
            if(clave1 != clave2){
                cout << "Zobrist no se actualizó correctamente" << endl;
                for (auto x: tablero->jugadas){
                    std::cout << x << std::endl;
                }
                cout << move << endl;
                exit(0);
            }*/
        }
    }

    tablero->cantMovesGenerados[ply] = -1;


    return nodes;
}


float Motor::seguridadDelRey(Tablero *tablero) {

    float valor = 0;


        if ((tablero->bitboards[0] & (0b1ULL | 0b10ULL)) > 0) {
            U64 escudoDePeonesAUnaCasilla = tablero->bitboards[5] & (0x100 | 0x200 | 0x400);
            U64 escudoDePeonesADosCasillas = tablero ->bitboards[5] & (0x10000 | 0x20000 | 0x40000);
            int cantPeonesEscudoUna = __builtin_popcountll(escudoDePeonesAUnaCasilla);
            int cantPeonesEscudoDos = __builtin_popcountll(escudoDePeonesADosCasillas);
            valor += constantes::premioEscudoDePeones * cantPeonesEscudoUna;
            valor += (constantes::premioEscudoDePeones*0.6) * cantPeonesEscudoDos;
        } else if ((tablero->bitboards[0] & 0xe0) > 0) {

            U64 escudoDePeonesAUnaCasilla = tablero->bitboards[5] & (0xe000);
            U64 escudoDePeonesADosCasillas = tablero ->bitboards[5] & (0xe00000);
            int cantPeonesEscudoUna = __builtin_popcountll(escudoDePeonesAUnaCasilla);
            int cantPeonesEscudoDos = __builtin_popcountll(escudoDePeonesADosCasillas);
            valor += constantes::premioEscudoDePeones * cantPeonesEscudoUna;
            valor += (constantes::premioEscudoDePeones*0.6) * cantPeonesEscudoDos;

            /*
             if ((tablero->bitboards[5] & 0x2000) > 0) {
                 valor += constantes::premioEscudoDePeones;
             }
             if ((tablero->bitboards[5] & 0x4000) > 0) {
                 valor += constantes::premioEscudoDePeones;
             }
             if ((tablero->bitboards[5] & 0x8000) > 0) {
                 valor += constantes::premioEscudoDePeones;
             }*/
            /*           if ((tablero->bitboards[4] & 0x200000) > 0){
                           valor += 20;
                       }*/
        }



        //Chequear lo mismo para las negras
        if ((tablero->bitboards[6] & 0x300000000000000ULL) > 0) {

            U64 escudoDePeonesAUnaCasilla = tablero->bitboards[11] & (0x7000000000000);
            U64 escudoDePeonesADosCasillas = tablero ->bitboards[11] & (0x70000000000);
            int cantPeonesEscudoUna = __builtin_popcountll(escudoDePeonesAUnaCasilla);
            int cantPeonesEscudoDos = __builtin_popcountll(escudoDePeonesADosCasillas);
            valor -= constantes::premioEscudoDePeones * cantPeonesEscudoUna;
            valor -= (constantes::premioEscudoDePeones*0.6) * cantPeonesEscudoDos;
            /*
            if ((tablero->bitboards[11] & 0x1000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[11] & 0x2000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[11] & 0x4000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }*/
            /*          if ((tablero->bitboards[10] & 0x40000000000ULL) > 0) {
                          valor -= 20;
                      }*/
        } else if ((tablero->bitboards[6] & 0xe000000000000000ULL) > 0) {

            U64 escudoDePeonesAUnaCasilla = tablero->bitboards[11] & (0xe0000000000000);
            U64 escudoDePeonesADosCasillas = tablero ->bitboards[11] & (0xe00000000000);
            int cantPeonesEscudoUna = __builtin_popcountll(escudoDePeonesAUnaCasilla);
            int cantPeonesEscudoDos = __builtin_popcountll(escudoDePeonesADosCasillas);
            valor -= constantes::premioEscudoDePeones * cantPeonesEscudoUna;
            valor -= (constantes::premioEscudoDePeones*0.6) * cantPeonesEscudoDos;


            /*if ((tablero->bitboards[11] & 0x20000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[11] & 0x40000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[11] & 0x80000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }*/
            /*           if ((tablero->bitboards[10] & 0x200000000000ULL) > 0) {
                           valor -= 20;
                       }*/
        }


    return valor;

}


float Motor::contarMaterialSinPeones(Tablero *tablero) {
    float valor = 0;

    U64 bitboard = tablero->bitboards[0];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[0];
    }
    bitboard = tablero->bitboards[1];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[1];
    }
    bitboard = tablero->bitboards[2];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[2];
    }
    bitboard = tablero->bitboards[3];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[3];
    }
    bitboard = tablero->bitboards[4];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[4];
    }
    bitboard = tablero->bitboards[7];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[7];
    }
    bitboard = tablero->bitboards[8];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[8];
    }
    bitboard = tablero->bitboards[9];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[9];
    }
    bitboard = tablero->bitboards[10];
    while (bitboard > 0) {
        int casilla = operaciones_bit::LSB(bitboard);
        valor += constantes::valorPieza[10];
    }

    return valor;
}

float Motor::calcularOcupacionAlfil(Tablero *tablero, int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardAlfil = tablero->bitboards[3];
        while (bitboardAlfil > 0) {
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            valor += constantes::ocupacionAlfil[casilla - 1];
        }
    } else {
        U64 bitboardAlfil = tablero->bitboards[9];
        while (bitboardAlfil > 0) {
            int casilla = operaciones_bit::LSB(bitboardAlfil);
        }
    }
    return valor;

}

float Motor::calcularOcupacionCaballo(Tablero *tablero, int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardCaballo = tablero->bitboards[4];
        while (bitboardCaballo > 0) {
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            valor += constantes::ocupacionCaballo[casilla - 1];
        }
    } else {
        U64 bitboardCaballo = tablero->bitboards[10];
        while (bitboardCaballo > 0) {
            int casilla = operaciones_bit::LSB(bitboardCaballo);
        }
    }
    return valor;
}

float Motor::calcularOcupacionPeon(Tablero *tablero, int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardPeon = tablero->bitboards[5];
        while (bitboardPeon > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeon);
            valor += constantes::ocupacionPeon[casilla - 1];
        }
    } else {
        U64 bitboardPeon = tablero->bitboards[11];
        while (bitboardPeon > 0) {
            int casilla = operaciones_bit::LSB(bitboardPeon);
        }
    }
    return valor;
}

float Motor::calcularOcupacionTorre(Tablero *tablero, int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardTorre = tablero->bitboards[2];
        while (bitboardTorre > 0) {
            int casilla = operaciones_bit::LSB(bitboardTorre);
            valor += constantes::ocupacionTorre[casilla - 1];
        }
    } else {
        U64 bitboardTorre = tablero->bitboards[8];
        while (bitboardTorre > 0) {
            int casilla = operaciones_bit::LSB(bitboardTorre);
        }
    }
    return valor;
}

float Motor::calcularOcupacionReina(Tablero *tablero, int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardReina = tablero->bitboards[1];
        while (bitboardReina > 0) {
            int casilla = operaciones_bit::LSB(bitboardReina);
            valor += constantes::ocupacionReina[casilla - 1];
        }
    } else {
        U64 bitboardReina = tablero->bitboards[7];
        while (bitboardReina > 0) {
            int casilla = operaciones_bit::LSB(bitboardReina);
        }
    }
    return valor;
}

float Motor::calcularOcupacionRey(Tablero *tablero, int color) {
    float valor = 0;
    if (color == 0) {
        U64 bitboardRey = tablero->bitboards[0];
        while (bitboardRey > 0) {
            int casilla = operaciones_bit::LSB(bitboardRey);
            if (endgame) {
                valor += constantes::ocupacionReyFinal[casilla - 1];
            } else {
                valor += constantes::ocupacionReyMedioJuego[casilla - 1];
            }
        }
    } else {
        U64 bitboardRey = tablero->bitboards[6];
        while (bitboardRey > 0) {
            int casilla = operaciones_bit::LSB(bitboardRey);


        }
    }
    return valor;
}

float Motor::calcularOcupacion(Tablero *tablero, int color) {
    float valor = 0;




    return (color == 0) ? valor : -valor;

}


float Motor::negamax(Tablero *tablero, int profundidad, float alfa, float beta, bool esRaiz) {
    U64 clave = tablero->zobrist;
    ply++;
    nodosBusqueda++;
    index_repeticion++;
    tabla_de_repeticiones[index_repeticion] = clave;
    if(nodosBusqueda == 2048){
        nodosBusqueda = 0;
        auto timeEnd = std::chrono::steady_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();
        if(time > tiempoDisponible){
            stopSearch = true;
            return 0;
        }
    }
    if (ply > 255) {
        return valoracion(tablero);
    }



/*    if (tablero->reyPropioEnJaque(tablero->_turno)) {
        profundidad += 1;
    }*/

    if (!esRaiz) {
        if (esRepeticion(clave)) {
            return 0;
        }
    }

    if (!tablaInicializada) {
        TT->asignarTablero(tablero);
        tablaInicializada = true;
    }
    misses++;

    Tabla_transposicion::entrada e = TT->obtenerEntrada(clave);
    if (e.jugada != 0 && e.profundidad >= profundidad && e.clave == clave && !esRaiz) {
        hashHits++;
        if (e.tipo == 0) {
            return e.valor;
        }
        if (e.tipo == 1 && e.valor <= alfa) {
            return alfa;
        }
        if (e.tipo == 2 && e.valor >= beta) {
            return beta;
        }
    }
    nodos++;
    if ((profundidad <= 0) || (ply == 255)) {
        ply--;
        return quiescence(tablero, alfa, beta);
    }


/*    if (profundidad == 0) {
        return valoracion(tablero);
    }*/


    //Null move pruning
    if (profundidad > 3 && (contarMaterialSinPeones(tablero) > 0) &&
         (valoracion(tablero) > beta) && !(tablero->reyPropioEnJaque(tablero->_turno))) {
        exitosNull++;
        tablero->movimientoNulo();
        float eval = -negamax(tablero, profundidad - 3, -beta, -beta + 1, false);
        ply--;
        index_repeticion--;
        tablero->deshacerMovimientoNulo();
        tablero->cantMovesGenerados[ply] = -1;

        if (eval >= beta) {
            return beta;
        }
    }
    // C++


    u_short mejorJugada = 0;
    float maxEval = -50000;
    float maxEvalSegunda = -98989898;
    tablero->generar_movimientos(tablero->_turno, ply);

    Tabla_transposicion::entrada entrada = TT->obtenerEntrada(clave);


    std::vector<std::pair<u_short, float>> movimientosConPrioridad;
    for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
        u_short movimiento = tablero->movimientos_generados[ply][i];
        float prioridad = 0;
        if (esRaiz && (movimiento == bestMove)) prioridad = 2000000; // Prioridad máxima para bestMove
        else if (entrada.jugada == movimiento) prioridad = 1000000;
        else if (killerMove[0][ply] == movimiento || killerMove[1][ply] == movimiento) prioridad = 900000;
        else if (tablero->esCaptura(movimiento)) {
            int victima = tablero->obtenerTipoDePieza(operaciones_bit::getLlegada(movimiento));
            int atacante = tablero->obtenerTipoDePieza(operaciones_bit::getSalida(movimiento));
            prioridad = 800000 + 100 * constantes::valorPieza[victima] - constantes::valorPieza[atacante];
        }
        else if (tablero->esUnaPromocion(movimiento)) prioridad = 700000;
        else prioridad = operaciones_bit::getTipoDeJugada(movimiento);
        movimientosConPrioridad.emplace_back(movimiento, prioridad);
    }
    std::sort(movimientosConPrioridad.begin(), movimientosConPrioridad.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    int cantidadDeMovesBuscados = 0;
    for (const auto& [movimiento, prioridad] : movimientosConPrioridad) {
        // Futility pruning: solo en profundidad 1, no jaque/captura/promoción
        if (profundidad == 1 &&
            !tablero->esCaptura(movimiento) &&
            !tablero->esUnaPromocion(movimiento) &&
            !tablero->esUnJaque(movimiento)) {
            float evalEstatico = valoracion(tablero);
            // Si el valor estático + margen no supera alfa, podar
            if (evalEstatico + 150 <= alfa) continue;
        }
        if (tablero->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento),
                                operaciones_bit::getTipoDeJugada(movimiento))) {
            float eval;
            // LMR: reducir profundidad en jugadas poco prometedoras
            bool esCaptura = tablero->esCaptura(movimiento);
            bool esPromocion = tablero->esUnaPromocion(movimiento);
            bool esKiller = (killerMove[0][ply] == movimiento || killerMove[1][ply] == movimiento);
            bool esJaque = tablero->esUnJaque(movimiento);

            if (!esCaptura && !esPromocion && !esKiller && !esJaque && cantidadDeMovesBuscados > 2 && profundidad > 2) {
                eval = -negamax(tablero, profundidad - 2, -alfa - 1, -alfa, false);
                ply--;
                index_repeticion--;
                if (eval < alfa) {
                    tablero->deshacerMovimiento();
                    cantidadDeMovesBuscados++;
                    continue;
                }
            }

            eval = -negamax(tablero, profundidad - 1, -beta, -alfa, false);
            ply--;
            index_repeticion--;
            tablero->deshacerMovimiento();
            if (stopSearch) {
                tablero->cantMovesGenerados[ply] = -1;
                return 0;
                }
            cantidadDeMovesBuscados++;

            if (eval > maxEval) {
                if (esRaiz && !stopSearch) {
                    bestMove = movimiento;
                    maxEvalSegunda = maxEval;


                }
                maxEval = eval;

                mejorJugada = movimiento;
            }
            else if ((eval > maxEvalSegunda) && (eval < maxEval) && esRaiz) {
                if (esRaiz && !stopSearch) {
                    maxEvalSegunda = eval; // Actualiza si es la segunda mejor
                }
            }
            alfa = std::max(alfa, eval);
            if (alfa >= beta) {
                // Killer move
                if (operaciones_bit::getTipoDeJugada(movimiento) == QUIET) {
                    killerMove[1][ply] = killerMove[0][ply];
                    killerMove[0][ply] = movimiento;
                }
                TT->insertar(clave, maxEval, profundidad, 2, movimiento);
                break;
            }
        }
    }

    tablero->cantMovesGenerados[ply] = -1;

    if (cantidadDeMovesBuscados == 0) {
        if (tablero->reyPropioEnJaque(tablero->_turno)) {
            return -10000000;
        } else {
            return 0;
        }
    }

/*    U64 clave2 = tablero->zobrist;
    if(clave2 != clave) {
        cout << "Clave 1: " << clave << endl;
        cout << "Clave 2: " << clave2 << endl;
    }*/
    if (esRaiz && (mejorJugada != 0)) {
        bestMove = mejorJugada;
        maxEval2 = maxEvalSegunda;
    }
    int tipoEntrada = 0;
    if (maxEval <= alfa) {
        tipoEntrada = 1;
    } else if (maxEval >= beta) {
        tipoEntrada = 2;
    }
    TT->insertar(clave, maxEval, profundidad, tipoEntrada, mejorJugada);


    return maxEval;
}

bool Motor::esRepeticion(U64 zobristActual) {
    int repeticiones = 0;
    for (int i = 0; i < index_repeticion; i++) {
        if (tabla_de_repeticiones[i] == zobristActual) {
            repeticiones++;
        }
    }
    return repeticiones >= 3;
}

float Motor::quiescence(Tablero *tablero, float alfa, float beta) {
    ply++;
    nodos++;
    float stand_pat = valoracion(tablero);
    float max_value = stand_pat;

    if (((stand_pat) >= beta) || (ply == 256)) {
        return stand_pat;
    }
    alfa = max(alfa, stand_pat);


    tablero->generar_capturas(tablero->_turno, ply);

    //Me quedo con los movimientos que son capturas
    for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
        int from = operaciones_bit::getSalida(tablero->movimientos_generados[ply][i]);
        int to = operaciones_bit::getLlegada(tablero->movimientos_generados[ply][i]);

        puntajeCaptura[ply][i] = abs(constantes::valorPieza[tablero->obtenerTipoDePieza(to)]) -
                                 abs(constantes::valorPieza[tablero->obtenerTipoDePieza(from)]);
    }

    // Filtrar movimientos con score negativo (remover malas capturas)
    int validCount = -1;
    for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
        if (puntajeCaptura[ply][i] >= 0) {
            validCount++;
            tablero->movimientos_generados[ply][validCount] = tablero->movimientos_generados[ply][i];
            puntajeCaptura[ply][validCount] = puntajeCaptura[ply][i];

        }
    }
    tablero->cantMovesGenerados[ply] = validCount;

    // Ordenar de mayor a menor usando bubble sort in-place
    for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
        for (int j = i + 1; j <= tablero->cantMovesGenerados[ply]; j++) {
            if (puntajeCaptura[ply][j] > puntajeCaptura[ply][i]) {
                std::swap(tablero->movimientos_generados[ply][i], tablero->movimientos_generados[ply][j]);
                std::swap(puntajeCaptura[ply][i], puntajeCaptura[ply][j]);
            }
        }
    }


    for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
        u_short movimiento = tablero->movimientos_generados[ply][i];
        if (tablero->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento),
                                operaciones_bit::getTipoDeJugada(movimiento))) {
            float score = -quiescence(tablero, -beta, -alfa);
            ply--;
            tablero->deshacerMovimiento();

            if (score >= beta) {
                tablero->cantMovesGenerados[ply] = -1;
                return score;
            }

            if (score > max_value) {
                max_value = score;
            }

            if (score > alfa) {
                alfa = score;
            }
        }

    }
    tablero->cantMovesGenerados[ply] = -1;

    return max_value;
}

float Motor::calcularDesarrollo(Tablero *tablero, int color) {
    float valoracion = 0;

    if (!tablero->desarrolloBlancasCompleto()) {
        if (!tablero->desarrolloCaballoBlancoG()) {
            valoracion -= constantes::premioDesarrolloPiezaMenor;
        }
        if (!tablero->desarrolloCaballoBlancoB()) {
            valoracion -= constantes::premioDesarrolloPiezaMenor;
        }
        if (!tablero->desarrolloAlfilBlancoF()) {
            valoracion -= constantes::premioDesarrolloPiezaMenor;
        }
        if (!tablero->desarrolloAlfilBlancoC()) {
            valoracion -= constantes::premioDesarrolloPiezaMenor;
        }
    }


    if (!tablero->desarrolloNegrasCompleto()) {
        if (!tablero->desarrolloCaballoNegroG()) {
            valoracion += constantes::premioDesarrolloPiezaMenor;
        }
        if (!tablero->desarrolloCaballoNegroB()) {
            valoracion += constantes::premioDesarrolloPiezaMenor;
        }
        if (!tablero->desarrolloAlfilNegroF()) {
            valoracion += constantes::premioDesarrolloPiezaMenor;
        }
        if (!tablero->desarrolloAlfilNegroC()) {
            valoracion += constantes::premioDesarrolloPiezaMenor;
        }
    }


    return (color == 0) ? valoracion : -valoracion;

}

//Indica qué porcentaje de la tabla tiene alguna entrada
int Motor::porcentajeTabla() {

    return TT->count;
}

int Motor::calcularFaseDeJuego(Tablero* tablero) {
    int faseActual = 0;
    faseActual += (tablero->caballosBlancos + tablero->caballosNegros) * peso_fase_caballo;
    faseActual += (tablero->alfilesBlancos + tablero->alfilesNegros) * peso_fase_alfil;
    faseActual += (tablero->damasBlancas + tablero->damasNegras) * peso_fase_dama;
    faseActual += (tablero->torresBlancas + tablero->torresNegras) * peso_fase_torre;

    faseActual = std::min(faseActual, fase_maxima_total);
    faseActual = std::max(faseActual, 0);

    return (faseActual * 256 + (fase_maxima_total / 2)) / fase_maxima_total;

}