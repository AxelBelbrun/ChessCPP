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
    float evaluacion = 0.0;

    // Paso 1: Calcular la diferencia de material
    float diferenciaMaterial = valoracionMaterial(tablero);

    // Paso 2: Comprobar si la diferencia de material es mayor que un cierto umbral
    if (abs(diferenciaMaterial)*2 > 3000) {
        // Si la diferencia de material es demasiado grande, devolver inmediatamente una evaluación
        evaluacion = diferenciaMaterial;
    } else {
        // Paso 3: Si la diferencia de material no es mayor que el umbral, continuar con el resto de los criterios de evaluación
        evaluacion += diferenciaMaterial;
        evaluacion += calcularOcupacion(tablero, tablero->_turno);


        evaluacion += calcularDesarrollo(tablero, tablero->_turno);
    }

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
    if (tablero->contadorJugadas <= 12) {
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
    }
    //Premiar si las blancas enrocan

    float enrocaronLasBlancas = 0;
    if (tablero->enrocoBlancas) {
        enrocaronLasBlancas += constantes::premioPorEnrocar;
    }

    tablero->_turno == 0 ? evaluacion += enrocaronLasBlancas : evaluacion -= enrocaronLasBlancas;
    //Premiar si las negras enrocan

    float enrocaronLasNegras = 0;
    if (tablero->enrocoNegras) {
        enrocaronLasNegras -= constantes::premioPorEnrocar;
    }

    tablero->_turno == 0 ? evaluacion += enrocaronLasNegras : evaluacion -= enrocaronLasNegras;






    //Penalizar si los peones de d y e blancos están bloqueados
    if (tablero->obtenerTipoDePieza(12) == PEON) {
        if (tablero->obtenerTipoDePieza(20) != VACIO) {
            tablero-> _turno == 0 ? evaluacion -= constantes::castigoPeonBloqueado : evaluacion += constantes::castigoPeonBloqueado;
        }
    }

    if (tablero->obtenerTipoDePieza(13) == PEON) {
        if (tablero->obtenerTipoDePieza(21) != VACIO) {
            tablero-> _turno == 0 ? evaluacion -= constantes::castigoPeonBloqueado : evaluacion += constantes::castigoPeonBloqueado;
        }
    }

    //Penalizar si los peones de d y e negros están bloqueados
    if (tablero->obtenerTipoDePieza(53) == PEON) {
        if (tablero->obtenerTipoDePieza(45) != VACIO) {
            tablero-> _turno == 1 ? evaluacion -= constantes::castigoPeonBloqueado : evaluacion += constantes::castigoPeonBloqueado;
        }
    }
    if (tablero->obtenerTipoDePieza(52) == PEON) {
        if (tablero->obtenerTipoDePieza(44) != VACIO) {
            tablero-> _turno == 1 ? evaluacion -= constantes::castigoPeonBloqueado : evaluacion += constantes::castigoPeonBloqueado;
        }
    }

/*    //Evaluacion de movilidad
    tablero->generar_movimientos(tablero-> _turno, ply);
    int contador = tablero->cantMovesGenerados[ply];
    evaluacion += contador;
    tablero->cantMovesGenerados[ply] = -1;*/

    //Evaluamos la seguridad del rey

/*    if(!tablero->endgame) {
        float reysec = 0.7*seguridadDelRey(tablero);
        evaluacion += reysec;
    }*/

    double cant = tablero->contador_movilidad(tablero->_turno);

    tablero->_turno == 0 ? evaluacion += constantes::pesoMovilidad*cant : evaluacion -= constantes::pesoMovilidad*cant;
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

    return evaluacion;
}

float Motor::valoracionMaterial(Tablero *tablero) {
    int valoracionBlancas = tablero->historial_material_blancas[tablero->contadorMaterialBlancas].first;
    int valoracionNegras = tablero->historial_material_negras[tablero->contadorMaterialNegras].first;
    if (!tablero->endgame && ( valoracionBlancas <= 900 &&
    valoracionNegras >= -900)) {
        tablero->endgame = true;
    }
    if(!tablero->endgame && (tablero->bitboards[1] == 0) && (tablero->bitboards[7] == 0)){
        tablero->endgame = true;
    }
    float valor = valoracionBlancas + valoracionNegras;

    return (tablero->_turno == 0) ? valor : -valor;
}

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
            U64 escudoDePeones = tablero->bitboards[5] & (0x100 | 0x200 | 0x400);
            int cantPeonesEscudo = __builtin_popcountll(escudoDePeones);
            valor += constantes::premioEscudoDePeones * cantPeonesEscudo;
           /* if ((tablero->bitboards[5] & 0x100) > 0) {
                valor += constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[5] & 0x200) > 0) {
                valor += constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[5] & 0x400) > 0) {
                valor += constantes::premioEscudoDePeones;
            }*/
            /*           if ((tablero->bitboards[4] & 0x40000) > 0){
                           valor += 20;
                       }*/
        } else if ((tablero->bitboards[0] & 0xe0) > 0) {

            U64 escudoDePeones = tablero->bitboards[5] & (0x2000 | 0x4000 | 0x8000);
            int cantPeonesEscudo = __builtin_popcountll(escudoDePeones);
            valor += constantes::premioEscudoDePeones * cantPeonesEscudo;
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
        } else {
            valor -= constantes::castigoEnroqueSinEscudo;
        }



        //Chequear lo mismo para las negras
        if ((tablero->bitboards[6] & 0x300000000000000ULL) > 0) {

            U64 escudoDePeones = tablero->bitboards[11] & (0x1000000000000ULL | 0x2000000000000ULL | 0x4000000000000ULL);
            int cantPeonesEscudo = __builtin_popcountll(escudoDePeones);
            valor -= constantes::premioEscudoDePeones * cantPeonesEscudo;
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

            U64 escudoDePeones = tablero->bitboards[11] & (0x20000000000000ULL | 0x40000000000000ULL | 0x80000000000000ULL);
            int cantPeonesEscudo = __builtin_popcountll(escudoDePeones);
            valor -= constantes::premioEscudoDePeones * cantPeonesEscudo;


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
        } else {
            valor += constantes::castigoEnroqueSinEscudo;
        }



    return tablero->_turno == 0 ? valor : -valor;

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

    valor += tablero->historial_ocupacion_blancas[tablero->contadorOcupacion];
    valor += tablero->historial_ocupacion_negras[tablero->contadorOcupacion];


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
    if ((profundidad == 0) || (ply == 255)) {
        ply--;
        return quiescence(tablero, alfa, beta);
    }


/*    if (profundidad == 0) {
        return valoracion(tablero);
    }*/


    //Null move pruning
    if (profundidad > 3 && (contarMaterialSinPeones(tablero) > 0) &&
         (valoracion(tablero) > beta) && !(tablero->reyPropioEnJaque(tablero->_turno))) {
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


    u_short mejorJugada = 0;
    float maxEval = -50000;
    tablero->generar_movimientos(tablero->_turno, ply);


    U64 zobristPruebaPrioridad1 = tablero->zobrist;
    Tabla_transposicion::entrada entrada = TT->obtenerEntrada(clave);


    for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
        float prioridad = 0;
        u_short movimiento = tablero->movimientos_generados[ply][i];
        //string jugadaEscrita = tablero->formatearJugada(movimiento);;
        if (esRaiz && (movimiento == bestMove)) {
            prioridad = 999999999;
        } else if ((entrada.jugada == movimiento) && entrada.profundidad >= profundidad && entrada.clave == clave) {
            prioridad = 9999999;
            hashHits++;

        } else if (killerMove[0][ply] == movimiento) {
            if (tablero->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento),
                                    operaciones_bit::getTipoDeJugada(movimiento))) {
                prioridad = 200;
                tablero->deshacerMovimiento();
            }

        } else if (killerMove[1][ply] == movimiento) {
            if (tablero->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento),
                                    operaciones_bit::getTipoDeJugada(movimiento))) {
                prioridad = 200;
                tablero->deshacerMovimiento();
            }
        } else {
            //Ordenamos capturas según el valor de la pieza capturada - pieza que captura

            if (tablero->esCaptura(movimiento)) {
                prioridad = (abs(constantes::valorPieza[tablero->obtenerTipoDePieza(
                        operaciones_bit::getLlegada(movimiento))]) -
                             abs(constantes::valorPieza[tablero->obtenerTipoDePieza(
                                     operaciones_bit::getSalida(movimiento))]));
            }

            if (tablero->esUnaPromocion(movimiento)) {
                prioridad += 180;
            }

            if (tablero->esUnJaque(movimiento)) {
                prioridad += 180;
            }

            prioridad += operaciones_bit::getTipoDeJugada(movimiento);
        }
        prioridades[ply][i] = prioridad;


    }




/*
    sort(movimientosConPrioridad.begin(), movimientosConPrioridad.end(),
         [](const pair<u_short, float> &a, const pair<u_short, float> &b) {
             return a.second > b.second;
         });
*/
    int cantidadDeMovesBuscados = 0;


    for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
        int maxIndex = 0;
        for (int j = 0; j <= tablero->cantMovesGenerados[ply]; j++) {
            if (prioridades[ply][j] >= prioridades[ply][maxIndex]) {
                maxIndex = j;
            }
        }
        u_short movimiento = tablero->movimientos_generados[ply][maxIndex];
        prioridades[ply][maxIndex] = -1000000000;

        if (tablero->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento),
                                operaciones_bit::getTipoDeJugada(movimiento))) {
            float eval;

            if (!LMR && (cantidadDeMovesBuscados >= 2) && (profundidad > 2) &&
            !tablero->esUnJaque(movimiento)
            && !tablero->esCaptura(movimiento) &&
                    (movimiento != killerMove[0][ply]) && (movimiento != killerMove[1][ply]) &&
                !tablero->reyPropioEnJaque(tablero->_turno)) {
                LMR = true;
                eval = -negamax(tablero, profundidad - 2, -alfa - 1, -alfa, false);

                ply--;
                index_repeticion--;

                LMR = false;
                if ((eval < alfa)) {
                    tablero->deshacerMovimiento();
                    continue;
                }
            }


            eval = -negamax(tablero, profundidad - 1, -beta, -alfa, false);
            cantidadDeMovesBuscados++;
            ply--;
            index_repeticion--;

            tablero->deshacerMovimiento();
            if (stopSearch) {
                tablero->cantMovesGenerados[ply] = -1;
                return 0;
            }

            if (eval > maxEval) {
                if(esRaiz && !stopSearch){
                    bestMove = movimiento;
                }
                maxEval = eval;
                mejorJugada = movimiento;
               /* if (esRaiz) {
                    cout << "Hasta ahora la mejor jugada es:" << tablero->formatearJugada(mejorJugada) << endl;
                }*/
            }
            alfa = std::max(alfa, eval);
            if (alfa >= beta) {
                //Killer move
                u_short killer = movimiento;
                if (operaciones_bit::getTipoDeJugada(killer) == QUIET) {
                    killerMove[1][ply] = killerMove[0][ply];
                    killerMove[0][ply] = killer;
                }
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
    for (int i = 0; i < index_repeticion; i++) {
        if (tabla_de_repeticiones[i] == zobristActual) {
            return true;
        }
    }
    return false;
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

