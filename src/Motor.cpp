//
// Created by axel on 14/12/23.
//

#include "Motor.h"
#include <chrono>
#include <iostream>
#include "Peon.h"
#include <algorithm>
Motor::Motor() {
    TT = new TablaTrans();
    tablaInicializada = false;
};

bool Motor:: endgame = false;




float Motor::valoracion(Tablero *tablero) {
    float evaluacion = 0.0;

    // Paso 1: Calcular la diferencia de material
    float diferenciaMaterial = valoracionMaterial(tablero);

    // Paso 2: Comprobar si la diferencia de material es mayor que un cierto umbral
    /*if (abs(diferenciaMaterial)*2 > 3000) {
        // Si la diferencia de material es demasiado grande, devolver inmediatamente una evaluación
        evaluacion = 2*diferenciaMaterial;
    } else {*/
        // Paso 3: Si la diferencia de material no es mayor que el umbral, continuar con el resto de los criterios de evaluación
        evaluacion += diferenciaMaterial;

        evaluacion += calcularOcupacion(tablero, tablero->_turno);


        evaluacion += calcularDesarrollo(tablero, tablero->_turno);

/*}*/
/*
        //Penalizar el perder el derecho a enrocar para las blancas
        float perdidaEnroqueBlancas = 0;
        if(tablero->perdidaEnroqueCortoBlancas) {
            perdidaEnroqueBlancas -= 100;
        }
        if(tablero->perdidaEnroqueLargoBlancas){
            perdidaEnroqueBlancas -= 100;
        }

        evaluacion += perdidaEnroqueBlancas;
        //Penalizar el perder el derecho a enrocar para las negras

        float perdidaEnroqueNegras = 0;
        if(tablero-> perdidaEnroqueLargoNegras){

            perdidaEnroqueNegras += 100;
        }
        if(tablero->perdidaEnroqueCortoNegras){
            perdidaEnroqueNegras += 100;
        }
        evaluacion += perdidaEnroqueNegras;*/

//Penalizar el movimiento temprano de la dama blanca
    float movimientoTempDamaBlanca = 0;

    if (tablero->movimientoTempranoDamaBlanca) {
        movimientoTempDamaBlanca -= constantes::castigoPorSacarDama;
    }

    evaluacion += movimientoTempDamaBlanca;
    float movimientoTempDamaNegra = 0;
    if (tablero->movimientoTempranoDamaNegra) {
        movimientoTempDamaNegra += constantes::castigoPorSacarDama;
    }
    evaluacion += movimientoTempDamaNegra;
        if(tablero->_jugadas.size() <= 20) {


            //Penalizar multiples movimientos de la misma pieza menor blanca en la apertura
            if(tablero->movimientosAlfilBlancoF.second > 1){
                evaluacion -= constantes::castigoMultiplesMovimientos*(tablero->movimientosAlfilBlancoF.second - 1);
            }
            if(tablero->movimientosAlfilBlancoC.second > 1){
                evaluacion -= constantes::castigoMultiplesMovimientos*(tablero->movimientosAlfilBlancoC.second - 1);
            }
            if(tablero->movimientosCaballoBlancoB.second > 1){
                evaluacion -= constantes::castigoMultiplesMovimientos*(tablero->movimientosCaballoBlancoB.second - 1);
            }
            if(tablero->movimientosCaballoBlancoG.second > 1){
                evaluacion -= constantes::castigoMultiplesMovimientos*(tablero->movimientosCaballoBlancoG.second - 1);
            }
            //Penalizar el movimiento temprano de la dama negra



            //Penalizar multiples movimientos de la misma pieza menor negra en la apertura
            if(tablero->movimientosAlfilNegroF.second > 1){
                evaluacion += constantes::castigoMultiplesMovimientos*(tablero->movimientosAlfilNegroF.second - 1);
            }
            if(tablero->movimientosAlfilNegroC.second > 1){
                evaluacion += constantes::castigoMultiplesMovimientos*(tablero->movimientosAlfilNegroC.second - 1);
            }
            if(tablero->movimientosCaballoNegroB.second > 1){
                evaluacion += constantes::castigoMultiplesMovimientos*(tablero->movimientosCaballoNegroB.second - 1);
            }
            if(tablero->movimientosCaballoNegroG.second > 1){
                evaluacion += constantes::castigoMultiplesMovimientos*(tablero->movimientosCaballoNegroG.second - 1);
            }
        }
        //Premiar si las blancas enrocan

        float enrocaronLasBlancas = 0;
        if(tablero->enrocoBlancas){
            enrocaronLasBlancas += constantes::premioPorEnrocar;
        }
        evaluacion += enrocaronLasBlancas;

        //Premiar si las negras enrocan

        float enrocaronLasNegras = 0;
        if(tablero->enrocoNegras){
            enrocaronLasNegras -= constantes::premioPorEnrocar;
        }

        evaluacion += enrocaronLasNegras;





        //Penalizar si los peones de d y e blancos están bloqueados
        if(tablero->obtenerTipoDePieza(12) == PEON){
            if(tablero->obtenerTipoDePieza(20) != VACIO){
                evaluacion -= constantes::castigoPeonBloqueado;
            }
        }

        if(tablero->obtenerTipoDePieza(13) == PEON){
            if(tablero->obtenerTipoDePieza(21) != VACIO){
                evaluacion -= constantes::castigoPeonBloqueado;
            }
        }

        //Penalizar si los peones de d y e negros están bloqueados
        if(tablero->obtenerTipoDePieza(53) == PEON){
            if(tablero->obtenerTipoDePieza(45) != VACIO){
                evaluacion += constantes::castigoPeonBloqueado;
            }
        }
        if(tablero->obtenerTipoDePieza(52) == PEON){
            if(tablero->obtenerTipoDePieza(44) != VACIO){
                evaluacion += constantes::castigoPeonBloqueado;
            }
        }

/*        //Evaluacion de movilidad
    vector<u_short> movesPseudo = tablero->generar_movimientos(tablero-> _turno);
        int contador = movesPseudo.size();
    evaluacion += contador*0.5;*/




    if(tablero->_turno == 1){
        evaluacion = -evaluacion;
    }

    return evaluacion;
}

float Motor::valoracionMaterial(Tablero* tablero) {
    if((tablero->historialMaterialBlancas.back() <= 900 && tablero->historialMaterialNegras.back() >= -900)
    || tablero->cantPiezas(tablero->_turno) <= 2){
        endgame = true;
    }
    float valor = tablero->historialMaterialBlancas.back() + tablero->historialMaterialNegras.back();

    return valor;
}

float Motor:: valoracionMovilidad(Tablero* tablero) {
    float valor = 0;
    vector<u_short> movimientos = tablero->generar_movimientos(tablero->_turno);
    if(tablero->_turno == 0){

        valor += (float) (movimientos.size() );

    }
    else{

            valor -= (float) (movimientos.size() );

    }


    return valor;
}



U64 Motor::perft(int depth, Tablero* tablero) {
    if (depth == 0) {
        return 1;
    }

    U64 nodes = 0;
    vector<u_short> moves = tablero->generar_movimientos(tablero->_turno);
    U64 zobrist1 = tablero->zobristKey();
    for (auto &move : moves) {
        if (tablero->moverPieza(operaciones_bit::getSalida(move), operaciones_bit::getLlegada(move), operaciones_bit::getTipoDeJugada(move))) {
            nodes += perft(depth - 1, tablero);
            tablero->deshacerMovimiento();
            U64 zobrist2 = tablero->zobristKey();
            if (zobrist1 != zobrist2) {
                tablero->imprimirFen();
                cout << constantes::NumeroACasilla[operaciones_bit::getSalida(move)]
                     << constantes::NumeroACasilla[operaciones_bit::getLlegada(move)] << endl;
            }
        }
    }

    return nodes;
}


float Motor::seguridadDelRey(Tablero* tablero) {
    float valor = 0;

    //Chequear si hay peones en las casillas h2,g2 y f2 para el blanco
    if(tablero->_turno == 0){
        if((tablero->bitboards[0] & (0b1ULL | 0b10ULL)) > 0) {

            if ((tablero->bitboards[5] & 0b1001ULL) > 0) {
                valor += constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[5] & 0b1010ULL) > 0) {
                valor += constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[5] & 0b1011ULL) > 0) {
                valor += constantes::premioEscudoDePeones;
            }
 /*           if ((tablero->bitboards[4] & 0x40000) > 0){
                valor += 20;
            }*/
        }
        else if((tablero->bitboards[0] & 0xe0) > 0) {
            if ((tablero->bitboards[5] & 0x2000) > 0) {
                valor += constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[5] & 0x4000) > 0) {
                valor += constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[5] & 0x8000) > 0) {
                valor += constantes::premioEscudoDePeones;
            }
 /*           if ((tablero->bitboards[4] & 0x200000) > 0){
                valor += 20;
            }*/
        }
        else {
            valor -= constantes::castigoEnroqueSinEscudo;
        }

    }

    //Chequear lo mismo para las negras
    else{
        if((tablero->bitboards[6] & 0x300000000000000) > 0) {
            if ((tablero->bitboards[11] & 0x1000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[11] & 0x2000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[11] & 0x3000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
  /*          if ((tablero->bitboards[10] & 0x40000000000ULL) > 0) {
                valor -= 20;
            }*/
        }
        else if((tablero->bitboards[6] & 0xe000000000000000ULL) > 0){
            if ((tablero->bitboards[11] & 0x20000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[11] & 0x40000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
            if ((tablero->bitboards[11] & 0x80000000000000ULL) > 0) {
                valor -= constantes::premioEscudoDePeones;
            }
 /*           if ((tablero->bitboards[10] & 0x200000000000ULL) > 0) {
                valor -= 20;
            }*/
        }
        else{
            valor += constantes::castigoEnroqueSinEscudo;
        }



    }
    return valor;

}

float Motor:: controlDeCentroCaballo(Tablero *tablero, int color) {
    float valor = 0;
    if(color == 0){
        U64 bitboardCaballo = tablero->bitboards[4];
        while(bitboardCaballo > 0){
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            U64 attackMapCaballo = tablero->movimientosDeCaballo[casilla-1];
            while(attackMapCaballo > 0){
                int casillaAtacada = operaciones_bit::LSB(attackMapCaballo);
                valor += constantes::casillasCentralesCaballoBlanco[casillaAtacada-1];
            }
        }
    }
    else{
        U64 bitboardCaballo = tablero->bitboards[10];
        while(bitboardCaballo > 0){
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            U64 attackMapCaballo = tablero->movimientosDeCaballo[casilla-1];
            while(attackMapCaballo > 0){
                int casillaAtacada = operaciones_bit::LSB(attackMapCaballo);
                valor -= constantes::casillasCentralesCaballoNegro[casillaAtacada-1];
            }
        }
    }
    return 1.7*valor;
}

float Motor:: controlDeCentroAlfil(Tablero* tablero, int color){
    float valor = 0;
    if(color == 0){
        U64 bitboardAlfil = tablero->bitboards[3];
        while(bitboardAlfil > 0){
            int casilla = operaciones_bit::LSB(bitboardAlfil);

            U64 attackMapAlfil = constantes::attackMasksAlfil[casilla-1];
            U64 occupancy = tablero-> todas_las_piezas() & attackMapAlfil;
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
        U64 bitboardAlfil = tablero->bitboards[9];
        while(bitboardAlfil > 0){
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            U64 attackMapAlfil = constantes::attackMasksAlfil[casilla-1];
            U64 occupancy = tablero-> todas_las_piezas() & attackMapAlfil;
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

float Motor:: controlDeCentroPeon(Tablero* tablero, int color){
    float valor = 0;
    if(color == 0){
        U64 bitboardPeon = tablero->bitboards[5];
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
        U64 bitboardPeon = tablero->bitboards[11];
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

float Motor:: controlDeCentro(Tablero* tablero, int color){
    return (controlDeCentroAlfil(tablero, color) + controlDeCentroCaballo(tablero, color) + controlDeCentroPeon(tablero, color));
}

float Motor:: contarMaterialSinPeones(Tablero* tablero) {
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
    if(color == 0){
        U64 bitboardAlfil = tablero->bitboards[3];
        while(bitboardAlfil > 0){
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            valor += constantes::ocupacionAlfilBlanco[casilla-1];
        }
    }
    else{
        U64 bitboardAlfil = tablero->bitboards[9];
        while(bitboardAlfil > 0){
            int casilla = operaciones_bit::LSB(bitboardAlfil);
            valor -= constantes::ocupacionAlfilNegro[casilla-1];
        }
    }
    return valor;

}

float Motor::calcularOcupacionCaballo(Tablero *tablero, int color) {
    float valor = 0;
    if(color == 0){
        U64 bitboardCaballo = tablero->bitboards[4];
        while(bitboardCaballo > 0){
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            valor += constantes::ocupacionCaballoBlanco[casilla-1];
        }
    }
    else{
        U64 bitboardCaballo = tablero->bitboards[10];
        while(bitboardCaballo > 0){
            int casilla = operaciones_bit::LSB(bitboardCaballo);
            valor -= constantes::ocupacionCaballoNegro[casilla-1];
        }
    }
    return valor;
}

float Motor::calcularOcupacionPeon(Tablero *tablero, int color) {
    float valor = 0;
    if(color == 0){
        U64 bitboardPeon = tablero->bitboards[5];
        while(bitboardPeon > 0){
            int casilla = operaciones_bit::LSB(bitboardPeon);
            valor += constantes::ocupacionPeonBlanco[casilla-1];
        }
    }
    else{
        U64 bitboardPeon = tablero->bitboards[11];
        while(bitboardPeon > 0){
            int casilla = operaciones_bit::LSB(bitboardPeon);
            valor -= constantes::ocupacionPeonNegro[casilla-1];
        }
    }
    return valor;
}

float Motor::calcularOcupacionTorre(Tablero *tablero, int color) {
    float valor = 0;
    if(color == 0){
        U64 bitboardTorre = tablero->bitboards[2];
        while(bitboardTorre > 0){
            int casilla = operaciones_bit::LSB(bitboardTorre);
            valor += constantes::ocupacionTorreBlanco[casilla-1];
        }
    }
    else{
        U64 bitboardTorre = tablero->bitboards[8];
        while(bitboardTorre > 0){
            int casilla = operaciones_bit::LSB(bitboardTorre);
            valor -= constantes::ocupacionTorreNegro[casilla-1];
        }
    }
    return valor;
}

float Motor::calcularOcupacionReina(Tablero *tablero, int color) {
    float valor = 0;
    if(color == 0){
        U64 bitboardReina = tablero->bitboards[1];
        while(bitboardReina > 0){
            int casilla = operaciones_bit::LSB(bitboardReina);
            valor += constantes::ocupacionReinaBlanco[casilla-1];
        }
    }
    else{
        U64 bitboardReina = tablero->bitboards[7];
        while(bitboardReina > 0){
            int casilla = operaciones_bit::LSB(bitboardReina);
            valor -= constantes::ocupacionReinaNegro[casilla-1];
        }
    }
    return valor;
}

float Motor::calcularOcupacionRey(Tablero *tablero, int color) {
    float valor = 0;
    if(color == 0){
        U64 bitboardRey = tablero->bitboards[0];
        while(bitboardRey > 0){
            int casilla = operaciones_bit::LSB(bitboardRey);
            if(endgame){
                valor += constantes::ocupacionReyBlancoFinal[casilla-1];
            }
            else{
                valor += constantes::ocupacionReyBlancoMedioJuego[casilla-1];
            }
        }
    }
    else{
        U64 bitboardRey = tablero->bitboards[6];
        while(bitboardRey > 0){
            int casilla = operaciones_bit::LSB(bitboardRey);
            if(endgame){
                valor -= constantes::ocupacionReyNegroFinal[casilla-1];
            }
            else{
                valor -= constantes::ocupacionReyNegroMedioJuego[casilla-1];
            }
        }
    }
    return valor;
}

float Motor::calcularOcupacion(Tablero *tablero, int color) {
    float valor = 0;

            valor += tablero->historialOcupacionBlancas.back();
            valor += tablero->historialOcupacionNegras.back();



    return valor;

}


float Motor::negamax(Tablero *tablero, int profundidad, float alfa, float beta, u_short &bestMove) {
    ply++;
    bool reyEnJaque = tablero->reyPropioEnJaque(tablero->_turno);
    /*if(reyEnJaque){
        profundidad += 1;
        incrementos += 1;
    }*/
    if (!tablaInicializada) {
        TT->asignarTablero(tablero);
        tablaInicializada = true;
    }

    TablaTrans::entrada e = TT->obtenerEntrada();
    if (e.jugada != 0 && e.profundidad >= profundidad && e.clave == tablero->zobristKey()) {
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

    if(tablero->esTripleRepeticion()){
        return 0;
    }
    if (profundidad == 0) {
        float puntaje = quiescence(tablero, alfa, beta);
        TT->insertar(tablero->zobristKey(), puntaje, profundidad, 0, 1);
        return puntaje;

    }


    // Null move pruning
    // Null move pruning
    u_short x;

    if (!reyEnJaque && profundidad >= 3 && ply >= 1 && tablero->_jugadas.back() != 1
        && valoracion(tablero) >= beta) {
        U64 zobrist = tablero->zobristKey();
        tablero->cambiarTurno();
        tablero->_jugadas.push_back(1);
        float nullEval = -negamax(tablero, profundidad - 1 - 2, -beta, -beta + 1, x);
        if (nullEval >= beta) {
            tablero->cambiarTurno();
            tablero->_jugadas.pop_back();
            tablero->zobrist = zobrist;
            
            return beta;
        }
        if(nullEval > alfa){
            alfa = nullEval;
        }

        tablero->cambiarTurno();
        tablero->_jugadas.pop_back();
        tablero->zobrist = zobrist;
    }



    float maxEval = -99999999;

   /* if (bestMove != 0) {
        // Si bestMove es válido, evalúa esa jugada primero
        if (tablero->moverPieza(operaciones_bit::getSalida(bestMove), operaciones_bit::getLlegada(bestMove), operaciones_bit::getTipoDeJugada(bestMove))) {
            u_short posBestMove;
            maxEval = -negamax(tablero, profundidad - 1, -beta, -alfa, posBestMove);
            tablero->deshacerMovimiento();

            if (maxEval >= beta) {
                TT->insertar(tablero->zobristKey(), beta, profundidad, 2, bestMove);
                return beta;
            }

            alfa = std::max(alfa, maxEval);
        }
    }*/
    vector<u_short> movimientos = tablero->generar_movimientos(tablero->_turno);



    vector<pair<u_short, float>> movimientosConPrioridad;

    for (auto &movimiento : movimientos) {
        if (tablero->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento), operaciones_bit::getTipoDeJugada(movimiento))) {
            TablaTrans::entrada e = TT->obtenerEntrada();
            float prioridad = 0.0;

            if(ply == 0 && movimiento == bestMove){
                prioridad = 10000000;

            }
            else if (e.jugada != 0 && e.profundidad >= profundidad && e.clave == tablero->zobristKey()) {

                prioridad = (tablero->_turno == 1) ? -e.valor : e.valor;

/*
            prioridad = e.valor;D
*/


                prioridad = prioridad + operaciones_bit::getTipoDeJugada(movimiento);

            }

            else {
                // Si el movimiento no está en la tabla de transposición, asigna la prioridad basándote en el tipo de movimiento
                prioridad = operaciones_bit::getTipoDeJugada(movimiento);
            }
            movimientosConPrioridad.push_back(make_pair(movimiento, prioridad));
            tablero->deshacerMovimiento();
        }
    }

    sort(movimientosConPrioridad.begin(), movimientosConPrioridad.end(), [](const pair<u_short, float>& a, const pair<u_short, float>& b) {
        return a.second > b.second;
    });

    //Si no hay movimientos disponibles es porque la partida terminó en mate o tablas
    if(movimientosConPrioridad.empty()){

        //Si el rey está en jaque entonces es jaque mate, se devuelve un valor alto.
        if(reyEnJaque){
/*            u_short jaque_mate = operaciones_bit::crearJugada(operaciones_bit::getSalida(bestMove),
                                                              operaciones_bit::getLlegada(bestMove),
                                                              CHECKMATE);
            bestMove = jaque_mate;*/
            return valoracion(tablero) - 999999;
        }

        //Si no está en jaque entonces son tablas, se devuelve 0
        else{
            return 0;
        }
    }
    int aumentoProfundidad = 0;
    if(movimientosConPrioridad.size() == 1){
        aumentoProfundidad += 1;
    }


    u_short maxMove = 0;
    int movimientosEnMaxDepth = 4;
    int minimoParaReduccion = 3;
    int movimientosBuscados = 0;

    for (auto &par : movimientosConPrioridad) {
        u_short movimiento = par.first;
        if (tablero->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento), operaciones_bit::getTipoDeJugada(movimiento))) {
            u_short posBestMove;
            float eval;
            int tipoDeJugada = operaciones_bit::getTipoDeJugada(movimiento);
            nodos++;


   /*         // Futility pruning
            if (profundidad == 1 && !reyEnJaque && operaciones_bit::getTipoDeJugada(movimiento) != CAPTURE
            && operaciones_bit::getTipoDeJugada(movimiento) != CAPTURECHECK && operaciones_bit::getTipoDeJugada(movimiento) != CHECK){
                float futility_margin = 200;  // Define a suitable futility margin.
                float static_eval = valoracion(tablero);
                if (static_eval + futility_margin < alfa) {
                    tablero->deshacerMovimiento();
                    continue;
                }
            }*/

            /*// Recapture extension
            if(tablero->seRecapturo()){
                profundidad += 1;
            }*/

            if(movimientosBuscados == 0){
                eval = -negamax(tablero, profundidad + aumentoProfundidad -1, -beta, -alfa, posBestMove);
            }
            else{
                if (movimientosBuscados > movimientosEnMaxDepth && profundidad >= minimoParaReduccion
                    && tipoDeJugada != CHECK && tipoDeJugada != CAPTURE && tipoDeJugada != CAPTURECHECK
                    && !reyEnJaque) {
                    u_short posBestMove2;
                    eval = -negamax(tablero, profundidad + aumentoProfundidad - 2, -(alfa + 1), -alfa, posBestMove2);
                } else {
                    eval = alfa + 1;
                }
                    if (eval > alfa) {
                        u_short posBestMove3;
                        eval = -negamax(tablero, profundidad + aumentoProfundidad - 1, -(alfa + 1), -alfa,  posBestMove3);
                        if (eval > alfa && eval < beta) {
                            u_short posBestMove4;
                            eval = -negamax(tablero, profundidad + aumentoProfundidad - 1, -beta, -alfa, posBestMove4);
                        }
                    }
            }
            tablero->deshacerMovimiento();

            if (eval > maxEval) {

                maxEval = eval;
                maxMove = movimiento;
            }



            if (eval >= beta) {
                TT->insertar(tablero->zobristKey(), beta, profundidad, 2, movimiento);
                return beta;
            }

            alfa = std::max(alfa, eval);
            movimientosBuscados++;
        }

    }
    if(maxMove != 0){
        bestMove = maxMove;
        TT->insertar(tablero->zobristKey(), maxEval, profundidad, (maxEval <= alfa) ? 1 : 0, maxMove);

    }



    return maxEval;
}

float Motor::quiescence(Tablero *tablero, float alfa, float beta) {
    float stand_pat = valoracion(tablero);
    alfa = max(alfa, stand_pat);
    if(stand_pat >= beta){
        return stand_pat;
    }

    vector<u_short> movimientos = tablero->generar_capturas(tablero->_turno);
    // Ordenar los movimientos
    movimientos.erase(std::remove_if(movimientos.begin(), movimientos.end(), [&](const u_short& a) {
        float valorA = abs(constantes::valorPieza[tablero->obtenerTipoDePieza(operaciones_bit::getLlegada(a))]) -
                       abs(constantes::valorPieza[tablero->obtenerTipoDePieza(operaciones_bit::getSalida(a))]);
        return valorA < 0;
    }), movimientos.end());
    std::sort(movimientos.begin(), movimientos.end(), [&](const u_short& a, const u_short& b) {
        float valorA = abs(constantes::valorPieza[tablero->obtenerTipoDePieza(operaciones_bit::getLlegada(a))]) -
                abs(constantes::valorPieza[tablero->obtenerTipoDePieza(operaciones_bit::getSalida(a))]);
        float valorB = abs(constantes::valorPieza[tablero->obtenerTipoDePieza(operaciones_bit::getLlegada(b))]) -
                abs(constantes::valorPieza[tablero->obtenerTipoDePieza(operaciones_bit::getSalida(b))]);

            return valorA > valorB;

    });
    for (auto &movimiento : movimientos) {
        if (tablero->moverPieza(operaciones_bit::getSalida(movimiento), operaciones_bit::getLlegada(movimiento), CAPTURE)) {
            float score = -quiescence(tablero, -beta, -alfa);
            tablero->deshacerMovimiento();

            if (score >= beta) {
                return beta;
            }
            if (score > alfa) {
                alfa = score;
            }
        }

    }

    return alfa;
}

float Motor::calcularDesarrollo(Tablero *tablero, int color) {
    float valoracion = 0;

        if(!tablero->desarrolloBlancasCompleto()){
            if(!tablero->desarrolloCaballoBlancoG()){
                valoracion -= constantes::premioDesarrolloPiezaMenor;
            }
            if(!tablero->desarrolloCaballoBlancoB()){
                valoracion -= constantes::premioDesarrolloPiezaMenor;
            }
            if(!tablero->desarrolloAlfilBlancoF()){
                valoracion -= constantes::premioDesarrolloPiezaMenor;
            }
            if(!tablero->desarrolloAlfilBlancoC()){
                valoracion -= constantes::premioDesarrolloPiezaMenor;
            }
        }


        if(!tablero->desarrolloNegrasCompleto()){
            if(!tablero->desarrolloCaballoNegroG()){
                valoracion += constantes::premioDesarrolloPiezaMenor;
            }
            if(!tablero->desarrolloCaballoNegroB()){
                valoracion += constantes::premioDesarrolloPiezaMenor;
            }
            if(!tablero->desarrolloAlfilNegroF()){
                valoracion += constantes::premioDesarrolloPiezaMenor;
            }
            if(!tablero->desarrolloAlfilNegroC()){
                valoracion += constantes::premioDesarrolloPiezaMenor;
            }
        }


return valoracion;

}