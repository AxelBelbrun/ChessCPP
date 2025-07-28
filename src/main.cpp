
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
