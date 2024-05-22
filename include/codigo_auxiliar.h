#ifndef CHESSCPP_CODIGO_AUXILIAR_H
#define CHESSCPP_CODIGO_AUXILIAR_H

#include <vector>
#include <string>

class codigo_auxiliar {
public:
    double meanSquareError(double k);
    void actualizarParametros(std::vector<double> &x, double rate, double epsilon);
    void guardarParametros();
    std::vector<double> obtenerParametros();
    std::vector<double> calcularGradiente(std::string fen, double epsilon);
    std::vector<double> SGD(int epochs, double learningRate);
};

#endif //CHESSCPP_CODIGO_AUXILIAR_H