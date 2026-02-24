

#include "Tabla_transposicion.h"

Tabla_transposicion::Tabla_transposicion() {
    this->tamanoTabla = 33554432;
    count = 0;
}

void Tabla_transposicion::asignarTablero(Tablero *tablero) {
    this->tablero = tablero;
}

void Tabla_transposicion::insertar(U64 clave, float valor, int profundidad, int tipo, u_short jugada) {

    size_t idx = index(clave);
    entrada& slot = tabla[idx];

    // Política mejorada de reemplazo:
    // Reemplazar si:
    // - slot vacío (clave=0)
    // - o profundidad menor
    // - o misma profundidad pero tipo menos fuerte (exacto > beta > alpha)
    // - o diferente clave (colisión)

    bool reemplazar = false;
    if (slot.clave == 0) {
        reemplazar = true;
    } else if (slot.clave != clave) {
        reemplazar = true; // colisión, reemplazamos
    } else if (slot.profundidad < profundidad) {
        reemplazar = true;
    } else if (slot.profundidad == profundidad && slot.tipo < tipo) {
        reemplazar = true;
    }

    if (!reemplazar) return;
    entrada e;
    e.clave = clave;
    e.valor = valor;
    e.profundidad = profundidad;
    e.tipo = tipo;
    e.jugada = jugada;
    tabla[idx] = e;
    count++;
}

U64 Tabla_transposicion::index(U64 clave) {
    return clave & (tamanoTabla - 1);
}

u_short Tabla_transposicion::obtenerMovimientoGuardado(U64 clave) {
    return tabla[index(clave)].jugada;
}

Tabla_transposicion::entrada Tabla_transposicion::obtenerEntrada(U64 clave) {
    if (tabla[index(clave)].clave != 0) {
        return tabla[index(clave)];
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
        tabla[i] = entrada();
    }
    count = 0;
}