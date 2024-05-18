

#include "TablaTrans.h"

TablaTrans::TablaTrans() {
    this->tamanoTabla = 20833333;
    count = 0;
}

void TablaTrans::asignarTablero(Tablero *tablero) {
    this->tablero = tablero;
}

void TablaTrans::insertar(U64 clave, float valor, int profundidad, int tipo, u_short jugada) {

    if (tabla[index()] != nullptr) {
            if (tabla[index()]->profundidad > profundidad) {
                return;
            }
        }
    entrada* e = new entrada();
    e->clave = clave;
    e->valor = valor;
    e->profundidad = profundidad;
    e->tipo = tipo;
    e->jugada = jugada;
    delete tabla[index()];
    tabla[index()] = e;
    count++;
}

U64 TablaTrans::index() {
    return tablero->zobristKey() % (tamanoTabla);
}

u_short TablaTrans::obtenerMovimientoGuardado() {
    return tabla[index()]->jugada;
}

TablaTrans::entrada TablaTrans::obtenerEntrada() {
    if (tabla[index()] != nullptr) {
        return *tabla[index()];
    } else {
        entrada e;
        e.valor = 0;
        e.profundidad = 0;
        e.tipo = 0;
        e.jugada = 0;
        return e;
    }
}

void TablaTrans::limpiarTabla() {
    for (int i = 0; i < tamanoTabla; i++) {
        delete tabla[i];
        tabla[i] = nullptr;
    }
    count = 0;
}