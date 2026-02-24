# Módulo de Constantes - Documentación Técnica

## 1. Propósito del Módulo

Este módulo define y exporta todas las constantes globales utilizadas por el motor de ajedrez ChessCPP. El archivo proporciona:

- Definiciones de tipos fundamentales
- Bitboards precalculados para posiciones frecuentes del tablero
- Tablas de evaluación de posición (piece-square tables)
- Datos para generación de movimientos (números mágicos, tablas de ataque)
- Claves Zobrist para hashing de posiciones
- Mapas de conversión entre sistemas de coordenadas

## 2. Alias de Tipos

```cpp
typedef unsigned long long U64;      // Representa un bitboard (64 bits)
typedef u_int8_t bitmask;            // Máscara de bits de 8 bits
```

**Contexto para el agente**: `U64` es el tipo fundamental para representar el estado del tablero mediante bitboards. Cada bit representa una casilla del tablero de ajedrez (64 casillas).

## 3. Constantes de Archivos y Filas (Bitboards)

### 3.1 Archivos (Columnas)

| Variable | Descripción | Representación |
|----------|-------------|-----------------|
| `AFile` | Columna A | Bitboard con bits de la columna a |
| `BFile` | Columna B | Bitboard con bits de la columna b |
| `GFile` | Columna G | Bitboard con bits de la columna g |
| `HFile` | Columna H | Bitboard con bits de la columna h |

### 3.2 Filas

| Variable | Descripción | Fila del tablero |
|----------|-------------|------------------|
| `fila1` | Fila 1 (blancas) | 1 (base) |
| `fila2` | Fila 2 (peones blancos iniciales) | 2 |
| `fila3` | Fila 3 | 3 |
| `fila4` | Fila 4 | 4 |
| `fila5` | Fila 5 | 5 |
| `fila6` | Fila 6 | 6 |
| `fila7` | Fila 7 (peones negros iniciales) | 7 |
| `fila8` | Fila 8 (negras) | 8 |

**Nota**: Estas constantes son bitboards donde cada bit corresponde a una casilla. Por ejemplo, `fila1` tiene los 8 bits menos significativos activados (representando a1-h1).

## 4. Constantes de Evaluación

### 4.1 Pesos de Evaluación del Motor

| Variable | Valor | Descripción |
|----------|-------|-------------|
| `pesoMovilidad` | 0.3 | Peso utilizado en la función de evaluación para favorecer posiciones con mayor movilidad |
| `premioPorEnrocar` | 50 | Premio dado cuando una pieza ha enrocado |
| `castigoPorSacarDama` | 95 | Castigo por sacar la dama过早 (desarrollo temprano) |
| `castigoMultiplesMovimientos` | 95 | Castigo por realizar múltiples movimientos con la misma pieza |
| `premioDesarrolloPiezaMenor` | 30 | Premio por desarrollar piezas menores (caballos, alfiles) |
| `premioEscudoDePeones` | 50 | Premio por mantener un escudo de peones frente al rey |
| `castigoEnroqueSinEscudo` | 60 | Castigo por enrocar sin tener un escudo de peones |
| `castigoPeonBloqueado` | 30 | Castigo por tener un peon bloqueado |

### 4.2 Premio por Peón Pasado

```cpp
int premioPeonPasado[6] = {0, 5, 10, 10, 20, 25};
```

Array indexado por la distancia del peón al borde del tablero (0-5). El índice 0 representa un peón en la séptima/sexta fila (a punto de coronar).

## 5. Valor de Piezas

```cpp
float valorPieza[12] = {
    10000,  900,  500,  325,  301,  100,  // Piezas blancas: Rey, Dama, Torre, Alfil, Caballo, Peón
    -10000, -900, -500, -325, -301, -100  // Piezas negras (valor negativo)
};
```

**Orden de índices** (correlación con el tipo de pieza):
- 0: Rey blanco
- 1: Dama blanca
- 2: Torre blanca
- 3: Alfil blanco
- 4: Caballo blanco
- 5: Peón blanco
- 6-11: Mismas piezas para negro (valores negativos)

## 6. Mapas de Conversión de Coordenadas

### 6.1 NumeroACasilla
Mapa que convierte índices numéricos (1-64) a notación algebraica.

Ejemplos:
- `1 → "h1"`
- `21 → "d3"`
- `64 → "a8"`

### 6.2 CasillaANumero
Mapa inverso que convierte notación algebraica a índice numérico.

Ejemplos:
- `"e4" → 28`
- `"a1" → 8`

**Nota**: El sistema de numeración es little-endian desde la perspectiva del usuario, donde:
- Las casillas de la fila 1 (a1-h1) son 1-8
- Las casillas de la fila 8 (a8-h8) son 57-64

## 7. Tablas de Casillas Centrales

El código define tablas de 64 valores float para evaluar la importancia de controlar casillas centrales según el tipo de pieza y color:

### 7.1 Tablas de Caballos
- `casillasCentralesCaballoBlanco[64]`: Preferencias para caballo blanco
- `casillasCentralesCaballoNegro[64]`: Preferencias para caballo negro

### 7.2 Tablas de Peones
- `casillasCentralesPeonBlanco[64]`: Preferencias para peón blanco
- `casillasCentralesPeonNegro[64]`: Preferencias para peón negro

### 7.3 Tablas de Alfiles
- `casillasCentralesAlfilBlanco[64]`: Preferencias para alfil blanco
- `casillasCentralesAlfilNegro[64]`: Preferencias para alfil negro

**Patrón general**: Los valores más altos están en las casillas centrales d4, e4, d5, e5, decreciendo hacia los bordes.

## 8. Tablas de Ocupación (Piece-Square Tables)

Tablas utilizadas en la función de evaluación estática para dar valores posicionales a las piezas en cada casilla del tablero:

### 8.1 Tablas por Tipo de Pieza

| Tabla | Descripción |
|-------|-------------|
| `ocupacionPeon[64]` | Valores posicionales para peones |
| `ocupacionCaballo[64]` | Valores posicionales para caballos |
| `ocupacionAlfil[64]` | Valores posicionales para alfiles |
| `ocupacionTorre[64]` | Valores posicionales para torres |
| `ocupacionReina[64]` | Valores posicionales para reinas |

### 8.2 Tablas del Rey (Por Fase)

| Tabla | Uso |
|-------|-----|
| `ocupacionReyMedioJuego[64]` | Evaluación del rey en middlegame |
| `ocupacionReyFinal[64]` | Evaluación del rey en endgame |

**Nota**: Estas tablas son simétricas para ambos colores. El código debe rotar la tabla para piezas negras (usando `casillasEspejadas`).

## 9. Casillas Espejadas

```cpp
int casillasEspejadas[64]
```

Array de 64 elementos que mapea cada casilla a su equivalente espejeado verticalmente. Se utiliza para convertir las tablas de ocupación de piezas blancas a negras sin necesidad de definir tablas separadas.

Ejemplo: La posición de un peón blanco en e4 (índice 28) corresponde a e5 (índice 36) para negras.

## 10. Números Mágicos (Magics)

### 10.1 Magics para Torre
```cpp
U64 magicsParaTorre[64]
```
Array de 64 números mágicos utilizados en el algoritmo de generación de movimientos de torre con tablas de ataque precalculadas.

### 10.2 Magics para Alfil
```cpp
U64 magicsParaAlfil[64]
```
Array de 64 números mágicos utilizados en el algoritmo de generación de movimientos de alfil con tablas de ataque precalculadas.

**Contexto para el agente**: Estos magics se usan en combinación con el algoritmo de "magic bitboards" para calcular de manera eficiente los movimientos posibles de torres y alfiles usando lookups en tablas precalculadas.

## 11. Bits Relevantes

### 11.1 Bishop Relevant Bits
```cpp
const int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};
```

Número de bits relevantes para el cálculo de movimientos del alfil en cada casilla. Los valores van de 5 a 9.

### 11.2 Rook Relevant Bits
```cpp
const int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    // ... (continúa)
};
```

Número de bits relevantes para el cálculo de movimientos de la torre en cada casilla. Los valores van de 10 a 12.

## 12. Tablas de Ataque Precalculadas

### 12.1 Attack Masks
```cpp
extern std::vector<U64> attackMasksTorre;
extern std::vector<U64> attackMasksAlfil;
```

**Inicialización** (líneas 1124-1127):
```cpp
calculadoraMovesTorre* calculadoraTorre = new calculadoraMovesTorre();
calculadoraMovesAlfil* calculadoraAlfil = new calculadoraMovesAlfil();
std::vector<U64> attackMasksTorre = calculadoraTorre->masksTableroVacio();
std::vector<U64> attackMasksAlfil = calculadoraAlfil->masksTableroVacio();
```

**Nota para el agente**: Estas tablas son generadas dinámicamente al iniciar el programa mediante las clases `calculadoraMovesTorre` y `calculadoraMovesAlfil`. El método `masksTableroVacio()` calcula los movimientos de cada pieza en tablero vacío.

### 12.2 Tabla de Movimientos de Torre
```cpp
U64 movimientosDeTorre[64][4096] = {0};
```

Tabla de 64 × 4096 elementos que almacena los movimientos posibles de las torres para todas las ocupaciones relevantes. Se inicializa en cero (lazy initialization).

## 13. Claves Zobrist

```cpp
U64 zobristKeys[12][64]
```

Matriz de 12 × 64 claves Zobrist utilizadas para el hashing de posiciones. Cada combinación de tipo de pieza (12 tipos) y casilla (64) tiene una clave única aleatoria de 64 bits.

**Índices de piezas** (primera dimensión):
- 0-5: Piezas blancas (Rey, Dama, Torre, Alfil, Caballo, Peón)
- 6-11: Piezas negras (mismo orden)

Estas claves se combinan con XOR para generar un hash de la posición del tablero.

## 14. Constantes de Enroque (Bitboards)

```cpp
U64 enroqueLargoNegro = 12671771075561093284ULL;
U64 enroqueCortoNegro = 9649101450678681033ULL;
U64 enroqueLargoBlanco = 15793078621052637098ULL;
U64 enroqueCortoBlanco = 4072072696291369701ULL;
```

Bitboards que representan las casillas involucradas en cada tipo de enroque. Se utilizan para verificar la legitimidad del enroque y para actualizar el estado del tablero.

## 15. Otras Constantes de Posición

### 15.1 En Passant
```cpp
U64 enPassant[8] = {
    15302557312493823468ULL,
    17356847164896751443ULL,
    // ... (6 más)
};
```

Array de 8 bitboards, uno por cada columna del tablero. Representa las casillas de captura "en passant" disponibles.

### 15.2 Turno
```cpp
U64 mueveElNegro = 9881156418124848949ULL;
```

Bitboard que indica si es el turno de las negras (usado en hashing).

## 16. Dependencias Externas

El archivo `constantes.cpp` incluye y utiliza:

- `calculadoraMovesTorre`: Clase para calcular masks de ataque de torres
- `calculadoraMovesAlfil`: Clase para calcular masks de ataque de alfiles

**Nota**: Estas clases son instanciadas en tiempo de ejecución para inicializar las tablas de ataque.

## 17. Notas para Refactoring

1. **Valores hardcodeados**: Los valores de evaluación están hardcodeados. Un refactoring podría:
   - Moverlos a un archivo de configuración externo
   - Hacerlos ajustables en tiempo de ejecución

2. **Inicialización perezosa**: La tabla `movimientosDeTorre[64][4096]` se inicializa en cero pero no se llena. Esto sugiere que la población de la tabla ocurre lazily o en otro lugar del código.

3. **Memory leak**: Las instancias `calculadoraTorre` y `calculadoraAlfil` se crean con `new` pero nunca se eliminan. En un refactoring debería considerarse:
   - Usar punteros inteligentes
   - Hacerlos estáticos
   - Eliminarlos al final de la inicialización

4. **Simetría de tablas**: Varias tablas (como las de ocupación) podrían generarse automáticamente a partir de una sola tabla espejeada, reduciendo el código y potenciales errores.

---

## 18. Análisis de Uso de Memoria

### 18.1 Tablas Estáticas

| Estructura | Tipo | Tamaño | Memoria |
|------------|------|--------|---------|
| `valorPieza[12]` | float | 12 | 48 bytes |
| `casillasEspejadas[64]` | int | 64 | 256 bytes |
| `premioPeonPasado[6]` | int | 6 | 24 bytes |
| `casillasCentrales*` (6 tablas) | float[64] | 6 × 64 | 1,536 bytes |
| `ocupacion*` (7 tablas) | float[64] | 7 × 64 | 1,792 bytes |
| `magicsParaTorre[64]` | U64 | 64 | 512 bytes |
| `magicsParaAlfil[64]` | U64 | 64 | 512 bytes |
| `zobristKeys[12][64]` | U64 | 768 | 6,144 bytes |
| `bishop_relevant_bits[64]` | const int | 64 | 256 bytes |
| `rook_relevant_bits[64]` | const int | 64 | 256 bytes |
| `enPassant[8]` | U64 | 8 | 64 bytes |
| Constantes de enroque (4) | U64 | 4 | 32 bytes |
| Bitboards de archivos/filas | U64 | 8 | 64 bytes |

### 18.2 Tablas Dinámicas

| Estructura | Dimensiones | Memoria Estimada |
|------------|-------------|------------------|
| `attackMasksTorre` | 64 × U64 | 512 bytes (para tablero vacío) |
| `attackMasksAlfil` | 64 × U64 | 512 bytes (para tablero vacío) |
| `movimientosDeTorre[64][4096]` | 64 × 4096 × U64 | 2,097,152 bytes (2 MB) |

### 18.3 Uso Total de Memoria

El consumo de memoria del módulo de constantes es aproximadamente **2.1 MB**, siendo la tabla `movimientosDeTorre` el mayor consumidor (~2 MB).

### 18.4 Notas de Optimización de Memoria

- **Segmentación de memoria**: La tabla `movimientosDeTorre` podría dividirse en chunks pequeños para mejor más localización de cache.
- **Compresión**: Los `float` de las tablas de evaluación podrían convertirse a `int16_t` si se escala adecuadamente, reduciendo ~50% del espacio.
- **Inicialización lazy**: La tabla de movimientos de torre ocupa 2 MB pero no se utiliza activamente (inicializada en cero).

---

## 19. Complejidad Algorítmica de Operaciones Clave

### 19.1 Operaciones con Tablas de Evaluación

| Operación | Complejidad | Notas |
|-----------|-------------|-------|
| Lookup de piece-square | O(1) | Acceso directo por índice |
| Evaluación de posición | O(1) por pieza | ~30 piezas máximo × O(1) |
| Espejeado de tablas | O(64) | Solo una vez por color |

### 19.2 Operaciones con Magics

| Operación | Complejidad | Notas |
|-----------|-------------|-------|
| Generación magic move | O(1) | Lookup en tabla precalculada |
| Cálculo de índice | O(1) | `(occupancy * magic) >> (64 - relevant_bits)` |
| Inicialización de tablas | O(2^R) | R = bits relevantes (5-12 según pieza/posición) |

### 19.3 Operaciones con Zobrist

| Operación | Complejidad | Notas |
|-----------|-------------|-------|
| Hash de posición | O(N) | N = número de piezas en tablero (~30 max) |
| Actualización incremental | O(1) por movimiento | XOR de clave old + new |

### 19.4 Mapas de Conversión

| Operación | Complejidad | Notas |
|-----------|-------------|-------|
| `casillaANumero["e4"]` | O(log M) | M = tamaño del map (~64), podría ser O(1) con array |
| `NumeroACasilla[28]` | O(log M) | Mismo caso |

**Nota de optimización**: Los `std::map` podrían reemplazarse por arrays estáticas para acceso O(1).

---

## 20. Patrones de Acceso a Memoria y Cache Locality

### 20.1 Evaluación de Posición

El patrón de acceso más frecuente en la función de evaluación es:

```cpp
evaluacion += ocupacionPeon[casilla];
```

- **Tipo de acceso**: Secuencial para piezas del mismo tipo
- **Spatial locality**: Alta - las casillas contiguas en el tablero suelen procesarse consecutivamente
- **Temporal locality**: Alta - la misma pieza se evalúa múltiples veces durante el search

### 20.2 Tablas de Magics

- **Tipo de acceso**: Aleatorio basado en occupancy
- **Cache line**: 64 bytes típicos → 8 U64 por línea
- **Optimización**: Los magics de piezas similares podrían agruparse para mejor prefetching

### 20.3 Zobrist Hashing

- **Patrón**: Acceso aleatorio pero predecible (basado en posición de pieza)
- **Optimización**: Las claves Zobrist podrían alinearse a líneas de cache para reducir misses

### 20.4 Recomendaciones para Mejor Cache

1. **Alinear tablas a 64 bytes**: Todas las tablas de 64 elementos de U64 deberían estar alineadas
2. **Ordenar por frecuencia de acceso**: Poner tablas más usadas (peones, reyes) al inicio
3. **Prefetching**: Considerar prefetch de tablas antes de bucles de evaluación intensivos

---

## 21. Oportunidades de Optimización de Rendimiento

### 21.1 Optimizaciones de Memoria

| Optimización | Impacto Estimado | Complejidad |
|--------------|------------------|--------------|
| Reemplazar `std::map` con array[64] | 10-15% (en parsing UCI) | Baja |
| Usar `int16_t` para tablas de evaluación | 50% reducción memoria | Media |
| Eliminar `movimientosDeTorre` no usada | -2 MB | Baja |
| Usar punteros inteligentes para calculadoras | Previene leaks | Baja |

### 21.2 Optimizaciones Computacionales

| Optimización | Impacto Estimado | Notas |
|--------------|------------------|-------|
| Precomputar tablas espejeadas | O(64) → O(0) para negras | Solo inicialización |
| SIMD para evaluación | 2-4x speedup | Requiere cambios en función de evaluación |
| Loop unrolling en evaluación | 10-20% | Dependiente del compilador |

### 21.3 Precomputación

Las siguientes operaciones podrían precomputarse en tiempo de compilación:

1. **Mapas de conversión**: Cambiar `std::map` a arrays estáticas
2. **Tablas espejeadas**: Generar automáticamente las versiones para negras
3. **Valores absolutos**: `valorPieza` tiene valores negativos redundantes - podrían calcularse en tiempo de ejecución

### 21.4 Constexpr y Compile-Time

El código actual tiene varios elementos que podrían marcarse como `constexpr`:

- `bishop_relevant_bits` y `rook_relevant_bits` (ya son `const`)
- Tablas de evaluación podrían ser `constexpr`
- Valores de piezas

Esto permitiría optimización por el compilador y detección de errores en tiempo de compilación.

---

## 22. Operaciones con Potencial de Cuello de Botella

### 22.1 Generación de Movimientos (Magics)

La generación de movimientos es el hotspot principal en motores de ajedrez. El flujo actual es:

1. **Cálculo de ocupación relevante**: `occupancy & magic_mask`
2. **Índice**: `(occupancy * magic) >> (64 - relevant_bits)`
3. **Lookup**: `attack_table[index]`

**Tiempos típicos** (por operación):
- Acceso a tabla: ~1-3 nanosegundos (L1 cache)
- Cache miss: ~50-100 nanosegundos

### 22.2 Evaluación

La función de evaluación itera sobre ~30 piezas haciendo lookups en tablas de 64 elementos. El acceso es predecible pero podría beneficiarse de:

- ** batching**: Procesar todas las piezas de un tipo antes de pasar al siguiente
- **Estructura de datos contigua**: Mantener piezas agrupadas por tipo

### 22.3 Hashing Zobrist

El hashing se actualiza incrementalmente durante la generación de movimientos:

- **Por movimiento**: 1-2 XOR operations
- **Por posición completa**: O(30) para reconstruir desde cero

**No es bottleneck** - las operaciones son O(1) y muy rápidas.

---

## 23. Benchmarks de Referencia Sugeridos

Para identificar hotspots y validar optimizaciones, se sugieren los siguientes benchmarks:

### 23.1 Micro-benchmarks

| Operación | Métrica | Target |
|-----------|---------|--------|
| Lookup piece-square | ns/op | < 2 ns |
| Generación move (magic) | ns/op | < 5 ns |
| Actualización Zobrist | ns/op | < 1 ns |
| Conversión coordenada | ns/op | < 10 ns |

### 23.2 Macro-benchmarks

| Escenario | Métrica |
|-----------|---------|
| NPS (Nodes Per Second) | Millones/segundo |
| Profundidad alcanzada en 10s | Ply |
| Evaluación por segundo | Millones/segundo |

### 23.3 Herramientas Recomendadas

- **Google Benchmark**: Para micro-benchmarks
- **Valgrind/Callgrind**: Análisis de profiling
- **perf**: Profiling en Linux
- **Intel VTune**: Análisis de cache y vectorización

---

## 24. Consideraciones de Paralelización

### 24.1 Secciones Paralelizables

La evaluación y generación de movimientos son inherentemente paralelizables:

1. **Evaluación de hijos**: En alpha-beta, múltiples hijos pueden evaluarse en paralelo
2. **Move ordering**: Parallel computation de scores
3. **Tabla de transposición**: Acceso concurrenteo (necesita sincronización)

### 24.2 Secciones No Paralizables

1. **Zobrist hashing**: Acceso secuencial al estado del tablero
2. **Constantes globales**: Solo lectura, thread-safe

### 24.3 Impacto en Constantes

Las constantes definidas en este módulo son **thread-safe** para lectura concurrente. No requieren sincronización.

---

## 25. Glosario de Términos

| Término | Definición |
|---------|------------|
| **Bitboard** | Representación del tablero usando 64 bits |
| **Magic bitboards** | Técnica de generación de movimientos usando multiplicación y tablas |
| **Piece-square table** | Tabla que otorga valores posicionales a piezas |
| **Zobrist hashing** | Hash aditivo para posiciones de ajedrez |
| **Enroque** | Movimiento especial del rey y torre |
| **En passant** | Captura especial de peón |
| **Move generation** | Generación de movimientos legales |
| **Eval** | Función de evaluación estática |
| **NPS** | Nodos por segundo (métrica de performance) |
| **Cache line** | Unidad de transferencia de cache (~64 bytes) |
| **Spatial locality** | Patrón de acceso a memoria adyacente |
| **Temporal locality** | Reutilización de datos recently accessed |
