
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

  /* Tablero* t = new Tablero("position fen k7/8/8/5P2/8/8/8/K7 b - -");
   Motor* m = new Motor();

   //Controlamos el tiempo que tarda perft:
    std::chrono::steady_clock::time_point timeStart = chrono::steady_clock::now();
    m->perft(8, t);
    std::chrono::steady_clock::time_point timeEnd = chrono::steady_clock::now();
    cout << "Ocupacion peon blanco:" << t->ocupacionPiezas[5] << endl;
    cout << "Ocupacion peon negro:" << t->ocupacionPiezas[11] << endl;
    cout << "Ocupacion rey blanco:" << t->ocupacionPiezas[0] << endl;
    cout << "Ocupacion rey negro:" << t->ocupacionPiezas[6] << endl;
    cout << "Ocupacion dama blanco:" << t->ocupacionPiezas[1] << endl;
    cout << "Ocupacion dama negro:" << t->ocupacionPiezas[7] << endl;
    cout << "Ocupacion torre blanco:" << t->ocupacionPiezas[2] << endl;
    cout << "Ocupacion torre negro:" << t->ocupacionPiezas[8] << endl;
    cout << "Ocupacion alfil blanco:" << t->ocupacionPiezas[3] << endl;
    cout << "Ocupacion alfil negro:" << t->ocupacionPiezas[9] << endl;
    cout << "Ocupacion caballo blanco:" << t->ocupacionPiezas[4] << endl;
    cout << "Ocupacion caballo negro:" << t->ocupacionPiezas[10] << endl;*/





    