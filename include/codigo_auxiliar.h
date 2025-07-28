#ifndef CHESSCPP_CODIGO_AUXILIAR_H
#define CHESSCPP_CODIGO_AUXILIAR_H

#include <vector>
#include <string>

class codigo_auxiliar {
public:
    static double meanSquareError(double k);
    static void actualizarParametros(std::vector<double> &x, double rate, double epsilon);
    static void guardarParametros();
    static std::vector<double> obtenerParametros();
    static std::vector<double> calcularGradiente(std::string fen, double epsilon);
    static std::vector<double> SGD(int epochs, double learningRate);
    static void gradientDescent(int epochs, double learningRate);
};

#endif //CHESSCPP_CODIGO_AUXILIAR_H