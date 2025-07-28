#ifndef CHESSCPP_CABALLO_H
#define CHESSCPP_CABALLO_H
#include "Pieza.h"
#include "Tablero.h"
#include "Jugada.h"
#include "operaciones_bit.h"

class Caballo: public Pieza {
public:
     Caballo();
     //Esta función tiene que devolver una lista de jugadas (en realidad son punteros a jugadas pero
     // es lo mismo que hace JAVA por defecto)
     void obtener_movimientos(Tablero* tablero, int ply ,U64 piezas,
                                             U64 piezasPropias, U64 piezasRivales);
      U64 generar_movimientos_legales(U64 bitboard_caballo, U64 piezasPropias, U64 piezasRivales, int turno) override;
     static void capturas(Tablero* tablero,  std::vector<u_short>* movimientos,U64 piezas,
                          U64 piezasPropias, U64 piezasRivales);

};


#endif //CHESSCPP_CABALLO_H
