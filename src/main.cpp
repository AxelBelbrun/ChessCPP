
#include "ProtocoloUCI.h"
#include <memory>
#include <memory>
#include <iostream>
#include <sstream>
#include "ProtocoloUCI.h"
#include <memory>
#include <iostream>
#include "codigo_auxiliar.h"
#include <sstream>
#include <vector>
#include <fstream>
#include <map>
#include <stdexcept>
#include <chrono>


int main() {
         std::unique_ptr<ProtocoloUCI> protocoloUCI = std::make_unique<ProtocoloUCI>();
         protocoloUCI->comunicacionUCI();

         return 0;

     }

   /* Tablero* t = new Tablero("position startpos");
     Motor* m = new Motor();*/
    /* auto bitboardsOriginales = t->bitboards;
     for (auto x: t->movimientos_generados[0]) {
         std::string a = t->formatearJugada(x);
         t->moverPiezaTrusted(operaciones_bit::getSalida(x), operaciones_bit::getLlegada(x), operaciones_bit::getTipoDeJugada(x));
         if (!t->reyPropioEnJaque(t->_turno)) {

             t->cambiarTurno();

             cout << t->formatearJugada(x) << ": " << m->perft(4, t) << endl;
             m->ply--;

             t->derechosEnroqueAux = 0;
             t->deshacerEnroque();

             t->cambiarTurno();
         }
         t->contadorJugadas--;

         t->numeroDeJugadas--;


         t->bitboards = bitboardsOriginales;
     }*/
    //Controlamos el tiempo que tarda perft:
   /* std::chrono::steady_clock::time_point timeStart = chrono::steady_clock::now();
    cout << m->perft(6, t) << endl;
    std::chrono::steady_clock::time_point timeEnd = chrono::steady_clock::now();
    cout << "Tiempo del test: " << chrono::duration_cast<chrono::seconds>(timeEnd - timeStart).count() << endl;*/
    /*cout << "Ocupacion peon blanco:" << t->ocupacionPiezas[5] << endl;
    cout << "Ocupacion peon negro:" << t->ocupacionPiezas[11] << endl;
    cout << "Ocupacion rey blanco:" << t->ocupacionPiezas[0] << endl;
    cout << "Ocupacion rey negro:" << t->ocupacionPiezas[6] << endl;
    cout << "Ocupacion dama blanco:" << t->ocupacionPiezas[1] << endl;
    cout << "Ocupacion dama negro:" << t->ocupacionPiezas[7] << endl;
    cout << "Ocupacion torre blanco:" << t->ocupacionPiezas[2] << endl;
    cout << "Ocupacion torre negro:" << t->ocupacionPiezas[8] << endl;
    cout << "Ocupacion torre negro:" << t->ocupacionPiezas[8] << endl;
    cout << "Ocupacion alfil blanco:" << t->ocupacionPiezas[3] << endl;
    cout << "Ocupacion alfil negro:" << t->ocupacionPiezas[9] << endl;
    cout << "Ocupacion caballo blanco:" << t->ocupacionPiezas[4] << endl;
    cout << "Ocupacion caballo negro:" << t->ocupacionPiezas[10] << endl;*/






    