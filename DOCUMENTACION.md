# Documentación Global del Proyecto ChessCPP

## 1. Introducción y Visión General

ChessCPP es un motor de ajedrez escrito en C++ que implementa un sistema completo de representación del tablero, generación de movimientos legales, evaluación de posiciones y búsqueda de la mejor jugada mediante el algoritmo Negamax con diversas optimizaciones. El motor se comunica con interfaces gráficas mediante el protocolo UCI (Universal Chess Interface), lo que permite integrarlo con cualquier cliente de ajedrez compatible.

El proyecto está organizado en múltiples módulos que trabajan de manera coordinada para proporcionar funcionalidad completa de ajedrez. Cada módulo tiene responsabilidades específicas pero se relaciona con los demás a través de interfaces bien definidas, principalmente mediante el uso de bitboards para representar el estado del tablero y tipos de datos compartidos para representar jugadas y posiciones.

La arquitectura general del sistema sigue un patrón de capas donde la capa inferior proporciona utilidades fundamentales de manipulación de bits, sobre la cual se construyen las representaciones del tablero y las piezas, posteriormente el motor de búsqueda utiliza estas abstracciones para tomar decisiones, y finalmente el protocolo UCI actúa como interfaz de comunicación con el exterior.

## 2. Arquitectura del Sistema y Dependencias entre Módulos

### 2.1 Diagrama de Dependencias

El sistema se estructura en varios niveles de abstracción que se relacionan entre sí de manera jerárquica. En el nivel más bajo se encuentran las utilidades fundamentales que proporcionan operaciones a nivel de bits y constantes globales. Sobre estas se construyen las clases que representan las piezas del ajedrez y el tablero. El motor de búsqueda utiliza tanto el tablero como las utilidades para realizar su trabajo, y finalmente el protocolo UCI actúa como capa de comunicación.

La dependencia fundamental fluye de la siguiente manera: el módulo de operaciones_bit y constantes proporcionan las bases sobre las cuales se construyen las piezas individuales y el tablero. El motor depende directamente del tablero para manipular posiciones y de las constantes para los parámetros de evaluación. El protocolo UCI depende tanto del tablero como del motor para interactuar con el usuario.

### 2.2 Estructura de Archivos

El código fuente se organiza en dos directorios principales que contienen los archivos de cabecera en include y los archivos de implementación en src. Cada módulo del sistema tiene su correspondiente archivo de cabecera y archivo de implementación, además de documentación específica en la carpeta docs.

Los módulos principales del proyecto son: operaciones_bit que proporciona utilidades de bajo nivel para manipulación de bits y codificación de jugadas; constantes que define todos los valores globales incluyendo tablas de evaluación posicional y datos necesarios para la generación de movimientos; Jugada que encapsula la estructura de datos para representar un movimiento; Tablero que gestiona el estado completo del juego; las clases de piezas que incluyen Pieza como clase base abstracta, slidingPiece para piezas deslizantes, y las implementaciones específicas de cada tipo de pieza; Motor que implementa el algoritmo de búsqueda y la función de evaluación; Tabla_transposicion que optimiza la búsqueda almacenando posiciones previamente evaluadas; ProtocoloUCI que maneja la comunicación con interfaces gráficas; y codigo_auxiliar que contiene funcionalidades辅助 adicionales para optimización de parámetros.

## 3. Representación del Tablero mediante Bitboards

### 3.1 Concepto de Bitboards

El sistema utiliza bitboards como estructura de datos fundamental para representar el estado del tablero de ajedrez. Un bitboard es un número de 64 bits donde cada bit representa una casilla del tablero, permitiendo realizar operaciones de movimiento y ataque de manera extremadamente eficiente mediante operaciones bit a bit. Esta representación es estándar en motores de ajedrez modernos debido a su eficiencia computacional y facilita la implementación de algoritmos de generación de movimientos.

El tipo U64 definido como unsigned long long se utiliza exclusivamente para representar bitboards en todo el sistema. Las casillas se numeran del 1 al 64, con la convención de que la esquina inferior izquierda (a1) corresponde al bit menos significativo y la esquina superior derecha (a8) corresponde al bit más significativo. Esta numeración permite realizar desplazamientos (shifts) para calcular movimientos de piezas en direcciones específicas.

### 3.2 Bitboards de Piezas

El tablero mantiene separados, uno para cada tipo de pieza 12 bitboards de cada color. Los índices del 0 al 5 corresponden a las piezas blancas: rey, dama, torre, alfil, caballo y peón. Los índices del 6 al 11 corresponden a las mismas piezas para las negras. Esta separación permite acceder rápidamente a todas las piezas de un tipo específico o de un color específico mediante operaciones simples de manipulación de bits.

Además de los 12 bitboards de piezas, el sistema mantiene bitboards adicionales para representar estados especiales del juego como los derechos de enroque y las casillas disponibles para captura al paso. Estas representaciones permiten verificar rápidamente si un movimiento especial es legal sin necesidad de examinar todo el estado del juego.

## 4. Módulos Fundamentales

### 4.1 Operaciones Bit y Codificación de Jugadas

El módulo operaciones_bit proporciona funciones de bajo nivel que son utilizadas por prácticamente todos los demás módulos del sistema. Las funciones principales incluyen setBit que establece o limpia un bit específico en una posición determinada de un bitboard, y LSB que encuentra y retorna la posición del bit menos significativo activo mientras simultáneamente lo limpia, permitiendo iterar eficientemente sobre todas las piezas de un bitboard.

La codificación de jugadas utiliza 16 bits para almacenar toda la información relevante de un movimiento. Los bits 0-5 almacenan la casilla de origen, los bits 6-11 almacenan la casilla de destino, y los bits 12-15 almacenan el tipo de jugada. Esta codificación permite almacenar jugadas de manera compacta y acceder rápidamente a cualquiera de sus componentes mediante operaciones de desplazamiento y máscara.

Los tipos de jugada definidos en el sistema incluyen: QUIET para movimientos normales sin captura; CAPTURE para capturas de piezas rivales; ENPASSANT para la captura especial de peón al paso; CASTLING para enroques; PROMOTION, PROMOTIONDER y PROMOTIONIZQ para promociones de peón sin captura, con captura a derecha y con captura a izquierda respectivamente; y variantes con CHECK para jugadas que dan jaque.

### 4.2 Constantes Globales y Tablas de Evaluación

El módulo constantes define un extenso conjunto de valores que son utilizados por todo el sistema. Entre las constantes más importantes se encuentran los valores de material de cada pieza, donde el rey tiene un valor de 10000 puntos, la dama 900, la torre 500, el alfil 325, el caballo 301 y el peón 100. Estos valores se utilizan tanto en la evaluación estática como en la detección de finales de partida.

Las tablas de ocupación (piece-square tables) son arrays de 64 valores float que definen la valoración posicional de cada tipo de pieza en cada casilla del tablero. Estas tablas reflejan conocimiento ajedrecístico básico como la preferencia de los caballos por casillas centrales, las torres por columnas abiertas y los peones por avanzar. Existen tablas separadas para el medio juego y el final de partida para el rey, reconociendo que su posicionamiento ideal cambia según la fase del juego.

Las claves Zobrist son una matriz de 12 por 64 números aleatorios de 64 bits utilizados para generar un hash de la posición del tablero. Cada combinación de tipo de pieza y casilla tiene una clave única, y el hash de la posición se calcula mediante XOR de todas las claves correspondientes. Este hashing permite detectar repeticiones de posiciones de manera eficiente y es fundamental para la tabla de transposición.

Los números mágicos (magics) son valores precalculados que se utilizan en el algoritmo de magic bitboards para calcular eficientemente los movimientos de piezas deslizantes como la torre y el alfil. El código en calculadoraMovesTorre y calculadoraMovesAlfil fue utilizado para obtener estos números que posteriormente están hardcodeados en el sistema.

## 5. Sistema de Piezas

### 5.1 Jerarquía de Clases

El sistema de piezas implementa una jerarquía de herencia donde Pieza es la clase base abstracta que define la interfaz común para todas las piezas. De ella hereda slidingPiece que es una clase base abstracta para piezas deslizantes (aquellas que se mueven en línea recta a través del tablero), y las clases concretas Caballo, Rey y Peón que heredan directamente de Pieza.

Las clases Torre y Alfil heredan de slidingPiece, mientras que Dama combina los movimientos de Torre y Alfil mediante composición, instanciando objetos de ambas clases y disjuntando sus resultados. Este diseño demuestra el principio de composición sobre herencia y permite reutilizar código.

**Importante**: La jerarquía de clases con métodos virtuales existe en el código, pero NO se utiliza durante la búsqueda (hot path). Esta decisión de diseño está motivada por rendimiento, ya que las llamadas virtuales tienen overhead.

### 5.2 Generación de Movimientos por Pieza

Cada pieza implementa el método generar_movimientos_legales (definido como virtual puro en la clase base). Sin embargo, durante la generación real de movimientos en el hot path, el código utiliza llamadas estáticas directas en lugar de polimorfismo.

El razonamiento detrás de esta decisión de rendimiento es que la generación de movimientos se ejecuta millones de veces durante la búsqueda, y el overhead de las llamadas virtuales (vtable lookup) sería significativo.

**Durante inicialización** (NO es hot path): El Tablero crea objetos de cada tipo de pieza y utiliza llamadas via puntero para precalcular las tablas de movimientos.

**Durante la búsqueda** (hot path): El código utiliza llamadas estáticas directas:

- Para **Rey, Caballo y Peón**: llamadas estáticas como `Rey::generar_movimientos_legales()`, `Caballo::generar_movimientos_legales()`, `Peon::generar_movimientos_legales()`

- Para **Torre, Alfil y Dama**: el sistema accede directamente a tablas precalculadas mediante indexación con magic bitboards, sin llamar a ningún método de clase

El Peón tiene la lógica de movimiento más compleja debido a sus reglas especiales. Su método de avances calcula los movimientos hacia adelante considerando el movimiento simple desde cualquier posición y el movimiento doble desde la fila inicial. El método de capturas calcula las capturas diagonales a izquierda y derecha, considerando tanto capturas normales como la captura al paso. La promoción se maneja separadamente cuando el peón alcanza la octava (para blancas) o primera (para negras) fila.

La Torre y el Alfil implementan métodos para cada una de sus direcciones de movimiento, iterando desde la casilla de salida en la dirección correspondiente hasta encontrar una pieza que bloquee el movimiento. Si encuentran una pieza propia, el movimiento se corta; si encuentran una pieza rivales, agregan la captura y cortan; si la casilla está vacía, agregan el movimiento y continúan.

El Caballo utiliza un conjunto de funciones estáticas predefinidas que calculan sus ocho posibles movimientos en forma de L, utilizando máscaras de archivos para evitar movimientos que salgan del tablero. El Rey se mueve una casilla en cualquier dirección, con verificación de bordes similar a la del caballo pero para las ocho direcciones cardinales y diagonales.

### 5.3 Piezas Clavadas y Validación de Jaque

El módulo Tablero proporciona funciones para detectar piezas clavadas, es decir, piezas que no pueden moverse porque expondrían a su propio rey a jaque. La función estaClavada determina si una pieza específica está clavada, y generarMovimientosPiezaClavada genera los movimientos válidos considerando esta restricción.

La validación de jaque es fundamental para la generación de movimientos legales. El método reyPropioEnJaque del Tablero determina si el rey del bando actual está bajo ataque, y esJaque verifica si un movimiento específico expone al rey propio a jaque, lo cual invalidaría el movimiento.

## 6. El Módulo Tablero

### 6.1 Responsabilidades Principales

La clase Tablero es el núcleo central del sistema y encapsula todo el estado del juego. Sus responsabilidades incluyen la representación del estado del tablero mediante bitboards, la gestión del turno actual, el manejo de movimientos especiales como enroques y capturas al paso, la generación de movimientos legales, el cálculo de ataques y jaques, el mantenimiento del historial de jugadas para poder deshacerlas, y la gestión de la clave Zobrist para detección de repeticiones.

El tablero mantiene un historial de las últimas 512 jugadas realizadas, permitiendo deshacer movimientos durante la búsqueda. También mantiene un historial de las claves Zobrist para detectar triple repetición, una situación de tablas por repetición de posición.

### 6.2 Métodos de Movimiento

El método moverPieza es la función principal para ejecutar un movimiento en el tablero. Este método valida que el movimiento sea legal antes de ejecutarlo, actualizando los bitboards de las piezas afectadas, el turno, los derechos de enroque y la clave Zobrist.

El método moverPiezaTrusted es una versión optimizada que omite la validación legal y se utiliza durante la búsqueda donde ya se garantiza que el movimiento es legal por construcción. El método deshacerMovimiento restaura el estado anterior del tablero deshaciendo todos los cambios realizados por el último movimiento.

La promoción de peones se maneja mediante el método promocionar que reemplaza el peón en la casilla de destino con la pieza seleccionada (dama, torre, alfil o caballo). Los métodos setearPosicionInicial y setearPosicionDeFen permiten establecer la posición inicial estándar del ajedrez o una posición personalizada mediante notación FEN.

### 6.3 Tablas de Movimiento Precalculadas

El sistema utiliza tablas de movimiento precalculadas para la generación de movimientos de piezas deslizantes. Las tablas movimientosDeTorre de 64 por 4096 elementos y movimientosDeAlfil de 64 por 512 elementos almacenan los movimientos posibles para cada combinación de casilla y configuración de piezas bloqueantes.

Estas tablas se generan utilizando la técnica de magic bitboards, donde los números mágicos precalculados permiten indexar directamente la tabla correcta mediante una operación de multiplicación y desplazamiento. La generación de estas tablas ocurre durante la inicialización del programa en el constructor del Tablero.

## 7. El Motor de Búsqueda y Evaluación

### 7.1 Algoritmo Negamax con Podas

El motor implementa el algoritmo Negamax con múltiples optimizaciones que son fundamentales para lograr un rendimiento aceptable en la búsqueda de mejores jugadas. El método negamax es la implementación principal que recibe la profundidad de búsqueda restante, los valores alfa y beta para la poda, y un indicador de si es la llamada raíz.

Las optimizaciones implementadas incluyen: la tabla de transposición que consulta y almacena posiciones previamente evaluadas; el null move pruning que reduce la profundidad cuando la posición es claramente buena; los killer moves que mantienen movimientos quietos que han causado podas beta; el late move reduction (LMR) que reduce la profundidad de movimientos tardíos poco prometedores; el futility pruning que poda movimientos estáticos en profundidad uno; y el ordenamiento de movimientos que prioriza los mejores movimientos antes de buscar en profundidad.

### 7.2 Función de Evaluación Estática

La función valoracion es la función principal de evaluación que retorna un valor numérico representando la ventaja de la posición. Evalúa múltiples componentes del juego: el material total sumando el valor de todas las piezas; los peones bloqueados aplicando una penalización por peones en casillas centrales avanzados; el enroque premiando a los bandos que han enrocado; la seguridad del rey evaluando el escudo de peones alrededor del rey; la movilidad contando el número de movimientos legales disponibles; y los peones pasados premiando a los peones que no pueden ser bloqueados.

La evaluación calcula dos valores separados: uno para el medio juego y otro para el final de partida. Estos valores se interpolan según la fase del juego calculada mediante el método calcularFaseDeJuego, que considera qué piezas aún permanecen en el tablero. La fase máxima es 24 puntos (caballo=1, alfil=1, torre=2, dame=4 por bando).

### 7.3 Tablas de Ocupación

El motor utiliza las tablas de ocupación definidas en constantes.h para evaluar la posición de cada tipo de pieza. Las funciones calcularOcupacionPeon, calcularOcupacionCaballo, calcularOcupacionAlfil, calcularOcupacionTorre, calcularOcupacionReina y calcularOcupacionRey summationan los valores de las tablas para las piezas presentes en el tablero.

Para el rey existen dos tablas separadas: ocupacionReyMedioJuego para evaluar la seguridad del rey en el medio juego y ocupacionReyFinal para el final donde el rey debe ocupar el centro activamente.

### 7.4 Búsqueda de Quiescencia

El método quiescence implementa una búsqueda de quiescencia que evita el problema del horizonte, donde el motor podría hacer un movimiento catastrófico justo más allá de la profundidad de búsqueda principal. Esta búsqueda solo considera capturas y promociones, ordenándolas mediante evaluación de intercambio estático (SEE), y continúa hasta que la posición está "tranquila" sin capturas disponibles.

## 8. Tabla de Transposición

### 8.1 Propósito y Estructura

La tabla de transposición es una estructura de datos fundamental para la eficiencia del motor de búsqueda. Almacena posiciones del tablero que ya han sido evaluadas junto con su mejor movimiento, permitiendo evitar recalcular posiciones repetidas y podar ramas del árbol de búsqueda cuando se encuentra una posición que ya fue evaluada en profundidad igual o mayor.

Cada entrada de la tabla contiene: la clave Zobrist de la posición como identificador; el valor evaluado; la profundidad a la que se evaluó; el tipo de resultado (EXACTO si el valor está dentro del intervalo alfa-beta, BETA si causó poda beta, o ALFA si solo se mejoró alfa); y el mejor movimiento encontrado para esa posición.

### 8.2 Política de Reemplazo

El tamaño de la tabla está fijo en aproximadamente 1.68 millones de entradas (4 * 419430). Cuando ocurre una colisión (dos posiciones diferentes mapean al mismo índice), la política de reemplazo reemplaza la entrada existente si: el slot está vacío; es una colisión con diferente clave; la nueva profundidad es mayor; o es la misma profundidad pero con un tipo "más fuerte" (EXACTO > BETA > ALFA).

La tabla proporciona métodos para insertar nuevas entradas (insertar), recuperar el mejor movimiento guardado (obtenerMovimientoGuardado), recuperar la entrada completa (obtenerEntrada) y limpiar todas las entradas (limpiarTabla).

## 9. Protocolo UCI

### 9.1 Comunicación con Interfaces Gráficas

El módulo ProtocoloUCI implementa el protocolo Universal Chess Interface que permite la comunicación entre el motor y interfaces gráficas de ajedrez. El protocolo funciona mediante stdin y stdout, donde la GUI envía comandos al motor y este responde con información y jugadas.

El bucle principal comunicacionUCI lee comandos de entrada estándar y los procesa correspondientemente. Los comandos soportados incluyen: uci que inicializa el protocolo y responde con información del motor; isready que responde con readyok indicando que el motor está listo; ucinewgame que inicializa un nuevo juego; position que establece la posición del tablero ya sea desde la posición inicial (startpos) o mediante una cadena FEN; go que inicia la búsqueda de la mejor jugada; y quit que termina el programa.

### 9.2 Procesamiento del Comando Go

El comando go tiene múltiples parámetros que el motor utiliza para controlar la búsqueda. Los parámetros de tiempo incluyen wtime y btime para el tiempo restante de blancas y negras en milisegundos, y winc y binc para el incremento por movimiento.

El algoritmo de búsqueda implementado en inputGo funciona de manera iterativa: comienza calculando el tiempo disponible según los parámetros recibidos; luego itera profundidades crecientes desde 1 hasta 49; en cada profundidad ejecuta negamax para encontrar la mejor jugada; verifica si la mejor jugada se mantiene estable durante múltiples niveles (ply) para determinar si puede detenerse anticipadamente; y si el tiempo se agota o la jugada es suficientemente estable, reporta el resultado.

El criterio de parada temprana distingue entre el medio juego y el final: en finales (6 o menos piezas) requiere que la jugada sea estable durante 12 ply; en el medio juego requiere 5 ply de estabilidad. También requiere que la evaluación haya mejorado significativamente.

### 9.3 Formato de Jugadas

Las jugadas se formatean utilizando el método formatearJugada que convierte el código interno de 16 bits a la notación UCI estándar (por ejemplo, "e2e4"). Para promociones, el formato incluye la pieza promocionada (q para dama, r para torre, b para alfil, n para caballo) como en "e7e8q".

## 10. Interacciones entre Módulos

### 10.1 Flujo de Ejecución Típico

Cuando el usuario inicia el motor y envía el comando position para establecer una posición, el módulo ProtocoloUCI recibe el comando y llama a Tablero::actualizarTablero() que parsea la posición (ya sea startpos o FEN) y configura los bitboards correspondientes. El tablero también inicializa la clave Zobrist de la posición.

Cuando el usuario envía el comando go, ProtocoloUCI calcula el tiempo disponible y llama repetidamente a Motor::negamax() con profundidades crecientes. En cada llamada, negamax utiliza Tablero para generar movimientos legales, realizar movimientos y deshacerlos durante la búsqueda, y consultar la tabla de transposición para posiciones ya evaluadas.

La función de evaluación en Motor utiliza las tablas de ocupación de constantes, el material del Tablero, y diversas heurísticas para calcular el valor de cada posición. El resultado se interpola entre medio juego y final según la fase del juego.

### 10.2 Generación de Movimientos durante la Búsqueda

Durante la búsqueda, el flujo de generación de movimientos es el siguiente: para cada posición, el motor llama a Tablero::generar_movimientos() que itera sobre todos los tipos de piezas del bando que mueve; para cada pieza, llama al método correspondiente de la clase de pieza (por ejemplo, Torre::generar_movimientos_legales()); cada método de pieza itera sobre sus piezas individuales usando los bitboards, calculando los movimientos legales considerando bloqueos por piezas propias y capturas a piezas rivales.

Los movimientos generados se almacenan en una estructura temporal del Tablero, se ordenan según heurísticas (capturas primero, luego killer moves, etc.), y se procesan en el algoritmo de búsqueda. Para cada movimiento, el motor llama a Tablero::moverPiezaTrusted() para ejecutar el movimiento, llama recursivamente a negamax() para la siguiente profundidad, y luego llama a Tablero::deshacerMovimiento() para restaurar la posición.

### 10.3 Uso de la Tabla de Transposición

En cada nodo de la búsqueda, antes de generar movimientos, el motor consulta la tabla de transposición usando la clave Zobrist actual del tablero. Si encuentra una entrada válida con profundidad suficiente, puede usar directamente el valor y el mejor movimiento guardados, evitando explorar subtreees innecesarios.

Al finalizar la evaluación de una posición, el motor inserta una nueva entrada en la tabla de transposición con el valor calculado, la profundidad, el tipo de nodo (EXACTO, ALFA o BETA), y el mejor movimiento encontrado.

### 10.4 Actualización de Zobrist

Cada vez que se realiza un movimiento en el Tablero, la clave Zobrist se actualiza mediante XOR con las claves correspondientes al movimiento. Esto incluye la pieza que se mueve (XOR con la clave de esa pieza en la nueva posición), la pieza capturada si la hay (XOR con la clave de esa pieza en la posición de captura para quitarla), y el turno (XOR con la clave de turno si cambia).

Esta actualización incremental permite mantener el hash de la posición sin necesidad de recalcularlo completamente después de cada movimiento, lo cual es crucial para el rendimiento de la tabla de transposición.

## 11. Análisis Técnico de Rendimiento

Esta sección proporciona información detallada para identificar oportunidades de optimización en el proyecto.

### 11.1 Complejidad Algorítmica por Componente

#### Motor de Búsqueda (Motor.cpp)

| Función | Complejidad | Notas |
|---------|-------------|-------|
| `negamax()` | O(b^d) | b = branching factor (~30-40), d = profundidad |
| `quiescence()` | O(b^q) | q = profundidad de capturas (típicamente 1-3) |
| `perft()` | O(b^d) | Idéntico a negamax pero sin podas |
| `valoracion()` | O(1) | Acceso directo a bitboards, operaciones de popcount |
| `calcularFaseDeJuego()` | O(1) | Contadores directos |
| `seguridadDelRey()` | O(1) | Operaciones bitwise sobre rangos fijos |
| `esRepeticion()` | O(n) | n = historial de repeticiones (máx 512) |
| `contarMaterialSinPeones()` | O(n) | n = número de piezas (máx 30) |

#### Tablero (Tablero.cpp)

| Operación | Complejidad | Notas |
|-----------|-------------|-------|
| `piezas_blancas()` / `piezas_negras()` | O(6) | Itera sobre 6 bitboards |
| `todas_las_piezas()` | O(1) | OR de dos llamadas |
| `obtenerTipoDePieza()` | O(12) | Itera sobre todos los bitboards |
| `generar_movimientos()` | O(1) por movimiento | Lookup en tablas precalculadas |
| `reyPropioEnJaque()` | O(1) a O(n) | Verifica múltiples fuentes de jaque |
| `estaClavada()` | O(n) | Itera sobre piezas deslizantes rivales |
| `zobristKey()` | O(n) | Itera sobre todas las piezas en el tablero |
| `moverPieza()` | O(12) | Busca bitboard de origen y destino |

#### Operaciones Bit (operaciones_bit.cpp)

| Función | Complejidad | Notas |
|---------|-------------|-------|
| `setBit` | O(1) | Operaciones bitwise de costo constante |
| `LSB` | O(1) | `__builtin_ffsll` es O(1) en práctica (instrucción CPU) |
| `crearJugada` | O(1) | 3 operaciones bitwise |
| `getLlegada` | O(1) | 2 shifts + 1 AND + 1 suma + condición |
| `getSalida` | O(1) | 1 AND + 1 suma |
| `getTipoDeJugada` | O(1) | 1 shift + 1 AND |

#### Sistema de Piezas

| Pieza | Complejidad | Factores |
|-------|-------------|----------|
| **Caballo** | O(1) | 8 movimientos máximos, cálculo directo con shifts y máscaras |
| **Rey** | O(1) | 8 movimientos máximos, cálculo directo |
| **Torre** | O(n) | Donde n = número de casillas hasta el borde o siguiente pieza (máx 7) |
| **Alfil** | O(n) | Donde n = número de casillas hasta el borde o siguiente pieza (máx 7) |
| **Dama** | O(n) | Combinación de Torre + Alfil |
| **Peón** | O(1) a O(2) | Movimientos limitados, pero con múltiples verificaciones condicionales |

### 11.2 Operaciones Costosas Identificadas

#### Motor - Popcount en seguridadDelRey() (líneas 338-339, 346-347, 373-374, 394-395)
```cpp
int cantPeonesEscudoUna = __builtin_popcountll(escudoDePeonesAUnaCasilla);
int cantPeonesEscudoDos = __builtin_popcountll(escudoDePeonesADosCasillas);
```
- **Frecuencia**: Llamada 4 veces por evaluación
- **Costo**: ~3-5 ciclos por llamada en CPUs modernas

#### Motor - Bubble sort en quiescence() (líneas 852-859)
```cpp
for (int i = 0; i <= tablero->cantMovesGenerados[ply]; i++) {
    for (int j = i + 1; j <= tablero->cantMovesGenerados[ply]; j++) {
        // swap
    }
}
```
- **Frecuencia**: En cada nodo de quiescence
- **Costo**: O(n²) para ordenar capturas

#### Motor - Chequeo de tiempo (línea 598)
```cpp
if (nodosBusqueda == 2048) {
    // verificación de tiempo
}
```
- **Frecuencia**: Cada 2048 nodos
- **Costo**: Llamada a steady_clock (puede ser costoso)

#### Operaciones Bit - getLlegada con branch impredecible
```cpp
// Líneas 36-38: caso especial para "captura al paso"
```
- **Problema**: Branch impredecible puede costar 10-20 ciclos si no se predice bien

#### Piezas - Dama crea objetos dinámicos (Dama.cpp líneas 17-22)
```cpp
Alfil* alfil = new Alfil();
Torre* torre = new Torre();
// ... uso ...
delete alfil;
delete torre;
```
- **Problema**: 2 allocations + 2 deallocations por llamada

### 11.3 Uso de Memoria

#### Tablero
| Estructura | Tamaño | Descripción |
|------------|--------|-------------|
| `movimientosDeTorre[64][4096]` | 2 MB | Tabla de movimientos de torre |
| `movimientosDeAlfil[64][512]` | 256 KB | Tabla de movimientos de alfil |
| `movimientosDeCaballo[64]` | 512 bytes | Tabla de movimientos de caballo |
| `movimientosDeRey[64]` | 512 bytes | Tabla de movimientos de rey |
| `bitboards[12]` | 96 bytes | Estado actual del tablero |
| `historialZobrist[512]` | 4 KB | Historial de posiciones |
| `historialBitboards[512]` | ~20 KB | Historial de cambios |
| `jugadas[512]` | 1 KB | Historial de jugadas |
| **Total aproximado** | ~2.3 MB | En tablas estáticas |

#### Motor
| Estructura | Tamaño | Notas |
|------------|--------|-------|
| `Tabla_transposicion` | ~40 MB | 1,677,720 entradas × 24 bytes |
| `killerMove[2][128]` | 512 bytes | Fijo |
| `tabla_de_repeticiones[512]` | 4 KB | Fijo |
| `prioridades[256][256]` | 256 KB | Matriz de ordenamiento |
| `puntajeCaptura[256][256]` | 256 KB | Duplicado en quiescence |

#### Tabla de Transposición - Estructura de Entrada
| Campo | Tipo | Tamaño (bytes) |
|-------|------|----------------|
| `clave` | `U64` | 8 |
| `valor` | `float` | 4 |
| `profundidad` | `int` | 4 |
| `tipo` | `int` | 4 |
| `jugada` | `u_short` | 2 |
| **Total (con padding)** | | **24** |

**Problema crítico**: El tamaño actual (4 * 419430 = 1,677,720) NO es potencia de 2, por lo que usa `%` (división lenta, 20-80 ciclos) en lugar de `&` (1 ciclo).

### 11.4 Puntos de Acceso Calientes (Hot Paths)

Los siguientes métodos se llaman frecuentemente durante la búsqueda:

1. **`moverPieza()`** - Llamado miles de veces por el motor de búsqueda
2. **`deshacerMovimiento()`** - Debe ser muy rápido para el algoritmo de recursión
3. **`reyPropioEnJaque()`** - Llamado para validar movimientos legales
4. **`generar_movimientos()`** - Genera todos los movimientos posibles
5. **`bitboard_movimientos_*()`** - Consulta tablas para ataques
6. **`piezas_blancas()` / `piezas_negras()`** - Calcula union de bitboards
7. **`LSB()`** - Itera sobre bits de atacantes/tablero
8. **`setBit()`** - Alta frecuencia en manipulación del tablero
9. **`crearJugada()` / `getSalida()` / `getLlegada()`** - Frecuentes en generación y ejecución

### 11.5 Thread Safety

**Sin sincronización**: Este código es **single-threaded** por diseño.
- `ply` es global y se modifica durante la recursión
- `tabla_de_repeticiones` se modifica sin locks
- `killerMove` se actualiza sin sincronización

**Para paralelización futura**:
- Necesitaría copiar `ply` por thread
- Tabla de transposición requerirá locks o versioning
- Killer moves necesitarán ser thread-local

### 11.6 Constantes de Rendimiento del Motor

```cpp
// En negamax()
int reduccion_nula = (profundidad > 6) ? 4 : 3;  // Null move pruning
int reduccion = 1;
if (cantidadDeMovesBuscados > 5) reduccion = 2;
if (profundidad > 6 && cantidadDeMovesBuscados > 8) reduccion = 3;
// Futility
if (evalEstatico + 150 <= alfa) continue;
```

### 11.7 Métricas Sugeridas para Profiling

Para evaluar el rendimiento real del motor:

1. **Nodos por segundo** (NPS) en búsqueda de profundidad fija
2. **Hit rate de tabla de transposición**: `hits / (hits + misses)`
3. **Tiempo de `generar_movimientos()`** por posición
4. **Tiempo de `moverPieza()` + `deshacerMovimiento()`** (par)
5. **Tiempo de `reyPropioEnJaque()`**
6. **Cache hit rate** de tablas magic
7. **Tiempo de inicialización** del motor
8. **Branch misprediction rate**

### 11.8 Branching Factor Observado

- **Profundidad 1**: ~20-30 movimientos legales
- **Profundidad típica de búsqueda**: 6-12 ply
- **Nodos por segundo objetivo**: >100,000 nps (típico para motores competitivos)

---

## 12. Glosario de Términos

Los siguientes términos son fundamentales para entender el sistema:

Bitboard es la representación del tablero mediante 64 bits donde cada bit corresponde a una casilla. Zobrist Key es el hash de la posición calculado mediante XOR de claves Zobrist de cada pieza en su posición actual. Ply es un medio-movimiento, es decir, el movimiento de un solo bando. El algoritmo Negamax es una variante de Minimax que simplifica el código aprovechando que max(a,b) = -min(-a,-b). La poda alfa-beta es una técnica que elimina ramas del árbol de búsqueda que no pueden mejorar el resultado. Los Killer Moves son movimientos quietos que han causado podas beta y se priorizan en ordenamiento. El problema del horizonte surge cuando el motor hace un movimiento catastrófico justo más allá de la profundidad de búsqueda. Magic Bitboards es una técnica de optimización para calcular ataques de piezas deslizantes mediante tablas precalculadas indexadas con números mágicos.

## 13. Estructura de Datos Resumen

La siguiente tabla resume las principales estructuras de datos utilizadas en el proyecto:

El Tablero mantiene 12 bitboards de piezas más bitboards de enroque y en passant, la clave Zobrist actual, el turno actual, y el historial de 512 jugadas y claves Zobrist. El Motor mantiene la tabla de transposición, la tabla de killer moves de 2 por 128, contadores de nodos y hash hits, y los parámetros de evaluación. La Jugada codifica origen (6 bits), destino (6 bits) y tipo (4 bits) en 16 bits. La Tabla de Transposición mantiene un array de aproximadamente 1.68 millones de entradas, cada una con clave, valor, profundidad, tipo y jugada. Las Constantes mantienen valores de piezas, tablas de ocupación de 64 valores cada una, claves Zobrist de 12 por 64, y los números mágicos para torres y alfiles.

## 14. Notas Adicionales de Implementación

### 14.1 Detección de Final de Juego

El motor detecta cuando la partida está en fase de final de juego mediante el método contarMaterialSinPeones que suma el valor de todas las piezas excepto peones. Cuando este valor es menor o igual a un umbral, el motor considera que está en modo "endgame" y ajusta sus evaluaciones y comportamiento de búsqueda correspondientemente.

### 14.2 Repeticiones y Tablas

El sistema mantiene un historial de las últimas 512 claves Zobrist en tabla_de_repeticiones. El método esRepeticion verifica si la posición actual ha ocurrido tres veces (contando la posición actual), lo cual es una condición de tablas por triple repetición según las reglas del ajedrez.

### 14.3 Código Auxiliar para Optimización de Parámetros

El módulo codigo_auxiliar contiene funcionalidades para optimizar los parámetros de la función de evaluación mediante aprendizaje automático. El código auxiliar SGD fue diseñado para este propósito, utilizando un archivo de posiciones en /home/axel/Documentos/posiciones-ML-Motor.txt para entrenamiento.

### 14.4 Generación de Tablas Magic

Las tablas de attack masks para piezas deslizantes se precalculan al inicio del programa utilizando las clases calculadoraMovesTorre y calculadoraMovesAlfil. Estas clases fueron utilizadas para obtener los números mágicos que posteriormente están hardcodeados en constantes.h. La generación ocurre una sola vez durante la inicialización del programa.

### 14.5 Inicialización del Motor

El constructor del Motor realiza las siguientes operaciones:
- Instancia una nueva Tabla_transposicion
- Inicializa bestMove a -1
- Marca tablaInicializada como false
- Inicializa la matriz de killerMove con valor -1

---

*Documento generado automáticamente a partir de la documentación de los módulos individuales del proyecto ChessCPP.*
