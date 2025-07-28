//
// Created by axel on 14/12/23.
//

#include "Rey.h"
#include "operaciones_bit.h"
using namespace constantes;

Rey::Rey(){};

void Rey::capturas(Tablero* tablero, std::vector<u_short> *movimientos, U64 rey, U64 piezasPropias, U64 piezasRivales) {

    int LSB = operaciones_bit::LSB(rey);
    rey = operaciones_bit::setBit(0L,LSB,1);

    // Movimiento hacia arriba
    int pos = LSB + 8;
    if (((rey & fila8) == 0) && (((rey << 8) & piezasRivales) > 0)) {
        movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 8, CAPTURE));
    }
    // Movimiento hacia abajo
    if (((rey & fila1) == 0) && (((rey >> 8) & piezasRivales) > 0)) {
        movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 8, CAPTURE));
    }
    // Movimiento hacia la izquierda
    if (((rey & AFile) == 0) && (((rey << 1) & piezasRivales) > 0)) {
        movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 1, CAPTURE));
    }
    // Movimiento hacia la derecha
    if (((rey & HFile) == 0) && (((rey >> 1) & piezasRivales) > 0)) {
        movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 1, CAPTURE));
    }
    // Movimiento hacia arriba derecha
    if (((rey & fila8) == 0) && ((rey & HFile) == 0) && (((rey << 7) & piezasRivales) > 0)) {
        movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 7, CAPTURE));
    }
    // Movimiento hacia arriba izquierda
    if (((rey & fila8) == 0) && ((rey & AFile) == 0) && (((rey << 9) & piezasRivales) > 0)) {
        movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB + 9, CAPTURE));
    }
    // Movimiento hacia abajo derecha
    if (((rey & fila1) == 0) && ((rey & HFile) == 0) && (((rey >> 9) & piezasRivales) > 0)) {
        movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 9, CAPTURE));
    }
    // Movimiento hacia abajo izquierda
    if (((rey & fila1) == 0) && ((rey & AFile) == 0) && (((rey >> 7) & piezasRivales) > 0)) {
        movimientos->push_back(operaciones_bit::crearJugada(LSB, LSB - 7, CAPTURE));
    }


}
void Rey:: movimientos_legales(Tablero* tablero, int ply,U64 rey, U64 piezasPropias, U64 piezasRivales) {


    int LSB = operaciones_bit::LSB(rey);
    rey = operaciones_bit::setBit(0L,LSB,1);

    // Movimiento hacia arriba
    int pos = LSB + 8;
    if (((rey & fila8) == 0) && (((rey << 8) & (piezasPropias | piezasRivales)) == 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB + 8, QUIET);
    }
    // Capturando hacia arriba
    else if (((rey & fila8) == 0) && (((rey << 8) & piezasRivales) > 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB + 8, CAPTURE);
    }
    // Movimiento hacia abajo
    if (((rey & fila1) == 0) && (((rey >> 8) & (piezasPropias | piezasRivales)) == 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB - 8, QUIET);
    }
    // Capturando hacia abajo
    else if (((rey & fila1) == 0) && (((rey >> 8) & piezasRivales) > 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB - 8, CAPTURE);
    }
    // Movimiento hacia la izquierda
    if (((rey & AFile) == 0) && (((rey << 1) & (piezasPropias | piezasRivales)) == 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB + 1, QUIET);
    }
    // Capturando hacia la izquierda
    else if (((rey & AFile) == 0) && (((rey << 1) & piezasRivales) > 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB + 1, CAPTURE);
    }
    // Movimiento hacia la derecha
    if (((rey & HFile) == 0) && (((rey >> 1) & (piezasPropias | piezasRivales)) == 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB - 1, QUIET);
    }
    // Capturando hacia la derecha
    else if (((rey & HFile) == 0) && (((rey >> 1) & piezasRivales) > 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB - 1, CAPTURE);
    }
    // Movimiento hacia arriba derecha
    if (((rey & fila8) == 0) && ((rey & HFile) == 0) && (((rey << 7) & (piezasPropias | piezasRivales)) == 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB + 7, QUIET);
    }
    // Capturando hacia arriba derecha
    else if (((rey & fila8) == 0) && ((rey & HFile) == 0) && (((rey << 7) & piezasRivales) > 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB + 7, CAPTURE);
    }
    // Movimiento hacia arriba izquierda
    if (((rey & fila8) == 0) && ((rey & AFile) == 0) && (((rey << 9) & (piezasPropias | piezasRivales)) == 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] = operaciones_bit::crearJugada(LSB, LSB + 9, QUIET);
    }
    // Capturando hacia arriba izquierda
    else if (((rey & fila8) == 0) && ((rey & AFile) == 0) && (((rey << 9) & piezasRivales) > 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] =operaciones_bit::crearJugada(LSB, LSB + 9, CAPTURE);
    }
    // Movimiento hacia abajo derecha
    if (((rey & fila1) == 0) && ((rey & HFile) == 0) && (((rey >> 9) & (piezasPropias | piezasRivales)) == 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] =operaciones_bit::crearJugada(LSB, LSB - 9, QUIET);
    }
    // Capturando hacia abajo derecha
    else if (((rey & fila1) == 0) && ((rey & HFile) == 0) && (((rey >> 9) & piezasRivales) > 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] =operaciones_bit::crearJugada(LSB, LSB - 9, CAPTURE);
    }
    // Movimiento hacia abajo izquierda
    if (((rey & fila1) == 0) && ((rey & AFile) == 0) && (((rey >> 7) & (piezasPropias | piezasRivales)) == 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] =operaciones_bit::crearJugada(LSB, LSB - 7, QUIET);
    }
    // Capturando hacia abajo izquierda
    else if (((rey & fila1) == 0) && ((rey & AFile) == 0) && (((rey >> 7) & piezasRivales) > 0)) {
        tablero->cantMovesGenerados[ply]++;
        tablero->movimientos_generados[ply][tablero->cantMovesGenerados[ply]] =operaciones_bit::crearJugada(LSB, LSB - 7, CAPTURE);
    }


};

U64 Rey::generar_movimientos_legales(U64 rey, U64 piezasPropias, U64 piezasRivales, int turno) { // attack mask
    U64 movimientos = 0L;
    int casillaDelRey = operaciones_bit::LSB(rey);
    rey = operaciones_bit::setBit(0L,casillaDelRey,1);

    // Movimiento hacia arriba
    int pos = casillaDelRey+ 8;
    if (((rey & fila8) == 0) && (((rey << 8) & (piezasPropias)) == 0)) {
        movimientos = operaciones_bit::setBit(movimientos, casillaDelRey+ 8, 1);
    }

    // Movimiento hacia abajo
    if (((rey & fila1) == 0) && (((rey >> 8) & (piezasPropias )) == 0)) {
        movimientos = operaciones_bit::setBit(movimientos, casillaDelRey- 8, 1);
    }

    // Movimiento hacia la izquierda
    if (((rey & AFile) == 0) && (((rey << 1) & (piezasPropias )) == 0)) {
        movimientos = operaciones_bit::setBit(movimientos, casillaDelRey+ 1, 1);
    }

    // Movimiento hacia la derecha
    if (((rey & HFile) == 0) && (((rey >> 1) & (piezasPropias) ) == 0)) {
        movimientos = operaciones_bit::setBit(movimientos, casillaDelRey- 1, 1);
    }

    // Movimiento hacia arriba derecha
    if (((rey & fila8) == 0) && ((rey & HFile) == 0) & (((rey << 7) & (piezasPropias )) == 0)) {
        movimientos = operaciones_bit::setBit(movimientos, casillaDelRey+ 7, 1);
    }

    // Movimiento hacia arriba izquierda
    if (((rey & fila8) == 0) && ((rey & AFile) == 0) & (((rey << 9) & (piezasPropias )) == 0)) {
        movimientos = operaciones_bit::setBit(movimientos, casillaDelRey+ 9, 1);
    }

    // Movimiento hacia abajo derecha
    if (((rey & fila1) == 0) && ((rey & HFile) == 0) & (((rey >> 9) & (piezasPropias )) == 0)) {
        movimientos = operaciones_bit::setBit(movimientos, casillaDelRey- 9, 1);
    }

    // Movimiento hacia abajo izquierda
    if (((rey & fila1) == 0) && ((rey & AFile) == 0) & (((rey >> 7) & (piezasPropias )) == 0)) {
        movimientos = operaciones_bit::setBit(movimientos, casillaDelRey- 7, 1);
    }

    return movimientos;
}


