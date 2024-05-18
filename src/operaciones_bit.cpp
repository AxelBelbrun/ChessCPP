#include "operaciones_bit.h"

U64 operaciones_bit::setBit(U64 num, int pos, int bit){
        if (bit == 0){
            U64 bitmask = ~(1ULL << (pos-1));
            num &= bitmask;
        }
        else{
            U64 bitmask = 1ULL << (pos-1);
            num |= bitmask;
        }
        return num;
    }

    U64 operaciones_bit::generador_numeros_random(){
        U64 u1, u2, u3, u4;
        u1 = (U64)(operaciones_bit::random_custom()) & 0xFFFF; u2 = (U64)(operaciones_bit::random_custom()) & 0xFFFF;
        u3 = (U64)(operaciones_bit::random_custom()) & 0xFFFF; u4 = (U64)(operaciones_bit::random_custom()) & 0xFFFF;
        return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}
    U64 operaciones_bit:: random_custom(){
       U64 numero = semilla;
       numero ^= numero << 13;
        numero ^= numero >> 17;
        numero ^= numero << 5;
        semilla = numero;
        return numero;
    }

    // Devuelve la posición del dígito menos significativo de num y además setea en 0 al dígito en cuestión.
    int operaciones_bit::LSB(U64 &num) {
        int res = __builtin_ffsll(num);
        num = num & (num - 1);

        return res;

    }

u_short operaciones_bit::crearJugada(int salida, int llegada, int tipoDeJugada){
    u_short jugada = 0;
    jugada = jugada | (salida - 1);

    //Caso especial cuando la casilla de llegada es h1 y es un movimiento QUIET. Si esto sucede
    // se rompe el formato de la jugada, por lo que se cambia la casilla de llegada a 12 y el tipo de jugada a 100
    if(llegada == 1 && tipoDeJugada == 0){
        llegada = 12;
        tipoDeJugada = 14;
    }
    jugada = jugada | ((llegada - 1) << 6);
    jugada = jugada | (tipoDeJugada << 12);
    return jugada;
}

int operaciones_bit::getLlegada(u_short movimiento){
    if((((movimiento >> 6) & 63) + 1 == 12) && ((int) ((movimiento >> 12) & 15)) == 14){
        return 1;
    }
    return ((movimiento >> 6) & 63) + 1;
}

int operaciones_bit::getSalida(u_short movimiento){
    return ((movimiento & 63) + 1);
}

int operaciones_bit::getTipoDeJugada(u_short movimiento){
    if(((movimiento >> 12) & 15) == 14){
        return 0;
    }

    return (movimiento >> 12) & 15;
}
int operaciones_bit:: contadorDe1s(U64 num){
    int contador = 0;
    while(num > 0){
        int LSB = operaciones_bit::LSB(num);
        contador++;
    }
    return contador;
}

int operaciones_bit::espejarCasilla(int casilla){
        casilla--;
        return ((9 - (casilla/8) + 1) * (casilla % 8)+1);

}