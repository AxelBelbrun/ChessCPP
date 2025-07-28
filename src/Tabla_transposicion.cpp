

#include "Tabla_transposicion.h"

Tabla_transposicion::Tabla_transposicion() {
    this->tamanoTabla = 419430;
    count = 0;
}

void Tabla_transposicion::asignarTablero(Tablero *tablero) {
    this->tablero = tablero;
}

void Tabla_transposicion::insertar(U64 clave, float valor, int profundidad, int tipo, u_short jugada) {

    if (tabla[index(clave)] != nullptr) {
            if (tabla[index(clave)]->profundidad > profundidad) {
                return;
            }
        }
    entrada* e = new entrada();
    e->clave = clave;
    e->valor = valor;
    e->profundidad = profundidad;
    e->tipo = tipo;
    e->jugada = jugada;
    delete tabla[index(clave)];
    tabla[index(clave)] = e;
    count++;
}

U64 Tabla_transposicion::index(U64 clave) {
    return clave % (tamanoTabla);
}

u_short Tabla_transposicion::obtenerMovimientoGuardado(U64 clave) {
    return tabla[index(clave)]->jugada;
}

Tabla_transposicion::entrada Tabla_transposicion::obtenerEntrada(U64 clave) {
    if (tabla[index(clave)] != nullptr) {
        return *tabla[index(clave)];
    } else {
        entrada e;
        e.valor = 0;
        e.profundidad = 0;
        e.tipo = 0;
        e.jugada = 0;
        return e;
    }
}

void Tabla_transposicion::limpiarTabla() {
    for (int i = 0; i < tamanoTabla; i++) {
        delete tabla[i];
        tabla[i] = nullptr;
    }
    count = 0;
}