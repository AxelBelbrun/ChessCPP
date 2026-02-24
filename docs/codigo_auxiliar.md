# Módulo de Código Auxiliar

## Descripción General

**Archivo**: `codigo_auxiliar.cpp` / `codigo_auxiliar.h`  
**Propósito**: Este módulo implementa funciones auxiliares para la optimización de parámetros de la función de evaluación del motor de ajedrez mediante técnicas de aprendizaje automático (Stochastic Gradient Descent).

> **Nota**: Este código fue utilizado para el proceso de optimización de parámetros de la función de evaluación.

ales

| Variable---

## Variables Glob | Tipo | Descripción |
|----------|------|-------------|
| `velocidades` | `std::vector<double>(433)` | Vector de 433 elementos inicializado en 0.0 |
| `velocity` | `double` | Almacena una velocidad (valor temporal) |
| `contador` | `int` | Contador para numerar archivos de parámetros guardados |

---

## Clase: `codigo_auxiliar`

### Métodos Públicos

#### 1. `meanSquareError(double k)`

**Propósito**: Calcula el error cuadrático medio (MSE) entre los resultados del motor y los resultados esperados.

**Parámetros**:
- `k`: Factor de escala para la función sigmoide

**Flujo**:
1. Lee posiciones desde archivo `/home/axel/Documentos/posiciones-ML-Motor.txt`
2. Para cada posición:
   - Extrae el resultado esperado del archivo FEN
   - Configura el tablero con la posición
   - Ejecuta `m->quiescence()` para obtener el score del motor
   - Aplica sigmoide: `1 / (1 + exp(-k * score))`
   - Acumula el error usando cross-entropy
3. Retorna el error promedio

**Dependencias externas**: `Motor`, `Tablero`

---

#### 2. `actualizarParametros(std::vector<double>& gradientes, double rate, double epsilon)`

**Propósito**: Actualiza los parámetros de evaluación según los gradientes calculados.

**Parámetros**:
- `gradientes`: Vector de gradientes (tamaño 432)
- `rate`: Learning rate para el actualización
- `epsilon`: Valor pequeño para evitar division por cero

**Estado**: **Código comentado** - La función contiene lógica comentada que actualizaba:
- Tablas de ocupación por tipo de pieza (peón, caballo, alfil, torre, reina, rey)
- Parámetros adicionales (premio por enrocar, castigo por sacar dama, etc.)

**Actualizado por**: No disponible actualmente

---

#### 3. `guardarParametros()`

**Propósito**: Persiste los parámetros actuales de evaluación en un archivo de texto.

**Flujo**:
1. Incrementa el contador de iteraciones
2. Abre archivo `/home/axel/Documentos/parametros/iteracion{N}.txt`
3. Escribe en orden:
   - Occupación peón blanco (64 valores, excluyendo filas 0 y 7)
   - Occupación caballo blanco (64 valores)
   - Occupación alfil blanco (64 valores)
   - Occupación torre blanca (64 valores)
   - Occupación reina blanca (64 valores)
   - Occupación rey blanco medio juego (64 valores)
   - Occupación rey blanco final (64 valores)

**Dependencias externas**: `constantes` (ocupacionPeon, ocupacionCaballo, etc.)

---

#### 4. `obtenerParametros()`

**Propósito**: Recolecta los parámetros actuales de evaluación en un vector.

**Retorna**: `std::vector<double>` con 432 valores:
- Índices 0-47: Ocupación peón (excluyendo casillas iniciales)
- Índices 48-111: Ocupación caballo
- Índices 112-175: Ocupación alfil
- Índices 176-239: Ocupación torre
- Índices 240-303: Ocupación reina
- Índices 304-367: Ocupación rey medio juego
- Índices 368-431: Ocupación rey final

**Dependencias externas**: `constantes`

---

#### 5. `calcularGradiente(std::string fen, double epsilon)`

**Propósito**: Calcula el gradiente de la función de pérdida respecto a cada parámetro.

**Parámetros**:
- `fen`: Notación FEN de la posición
- `epsilon`: Tamaño del paso para diferencias finitas

**Estado**: **Código mayormente comentado** - La función contiene lógica comentada que:
1. Obtiene parámetros actuales
2. Itera sobre cada parámetro, lo perturba con epsilon
3. Calcula diferencia finita
4. Guarda gradiente

**Dependencias externas**: `obtenerParametros()`, `Motor`, `Tablero`, `constantes`

---

#### 6. `SGD(int epochs, double learningRate)`

**Propósito**: Implementa Stochastic Gradient Descent para optimizar los parámetros.

**Parámetros**:
- `epochs`: Número de iteraciones de entrenamiento
- `learningRate`: Tasa de aprendizaje

**Flujo**:
1. Lee posiciones desde archivo `posiciones-ML-Motor.txt`
2. Crea generador de números aleatorios
3. Para cada época:
   - Selecciona posición aleatoria
   - Extrae FEN y resultado esperado
   - Obtiene score del motor mediante `quiescence()`
   - Aplica sigmoide al score
   - Calcula gradiente basado en la diferencia (scorePropio - score)
   - Maneja espejado de casillas para piezas negras
4. Promedia gradientes por épocas
5. Retorna vector de 432 gradientes

**Cálculo de gradiente** (para cada parámetro i):
- Determina la casilla corresponding al índice i
- Obtiene el tipo de pieza en esa casilla
- Calcula: `gradientes[i] += (scorePropio - score) * hayPieza`

**Dependencias externas**: `Motor`, `Tablero`, `operaciones_bit::espejarCasilla()`, `t.obtenerTipoDePieza()`, `t.piezas_negras()`

---

#### 7. `gradientDescent(int epochs, double learningRate)`

**Declarada en header** pero **no implementada** en el archivo `.cpp`.

---

## Estructura de Parámetros (432 elementos)

| Rango Índices | Tipo de Pieza | Descripción |
|---------------|---------------|-------------|
| 0-47 | Peón | Tablero 8x8 sin filas 0 y 7 (48 casillas) |
| 48-111 | Caballo | Tablero completo (64 casillas) |
| 112-175 | Alfil | Tablero completo (64 casillas) |
| 176-239 | Torre | Tablero completo (64 casillas) |
| 240-303 | Reina | Tablero completo (64 casillas) |
| 304-367 | Rey (MG) | Tablero completo - medio juego (64 casillas) |
| 368-431 | Rey (EG) | Tablero completo - final (64 casillas) |

---

## Notas para Refactoring

1. **Código muerto**: Gran cantidad de lógica comentada que debería eliminarse o restaurarse según necesidad
2. **Rutas hardcodeadas**: 
   - `/home/axel/Documentos/posiciones-ML-Motor.txt`
   - `/home/axel/Documentos/parametros/`
3. **Falta implementación**: `gradientDescent()` no está implementada
4. ** memory**: En `SGD()` se usa `new Motor()` sin smart pointer (línea 410), debe usar `std::make_unique`
5. **Parámetro no usado**: El argumento `learningRate` en `SGD()` no se utiliza actualmente

---

## Análisis Técnico para Optimización

### Complejidad Algorítmica

| Función | Complejidad | Factor dominante |
|---------|-------------|------------------|
| `meanSquareError()` | O(n × Q) | n = número de posiciones, Q = complejidad de `quiescence()` |
| `SGD()` | O(epochs × 432 × Q) | 432 iteraciones internas por época, Q = `quiescence()` |
| `obtenerParametros()` | O(432) | Iteración fija sobre todos los parámetros |
| `guardarParametros()` | O(432) | Escritura de 432 valores a archivo |
| `actualizarParametros()` | O(432) | Actualización de vectores de parámetros |
| `calcularGradiente()` | O(432 × Q) | Diferencias finitas sobre 432 parámetros |

> **Nota**: `quiescence()` es típicamente O(b^d) donde b ≈ branching factor y d ≈ profundidad, lo que convierte estas funciones en las más costosas del módulo.

---

### Operaciones Costosas Identificadas

#### 1. Lectura de archivo en `meanSquareError()` y `SGD()`
```cpp
std::ifstream file("/home/axel/Documentos/posiciones-ML-Motor.txt");
while (getline(file, line)) {
    positions.push_back(line);
}
```
- **Problema**: Lee todo el archivo en memoria en cada llamada
- **Oportunidad**: Cachear posiciones en memoria una sola vez o usar mmap

#### 2. Creación de objetos `Motor` y `Tablero` por iteración
```cpp
std::unique_ptr<Motor> m = std::make_unique<Motor>();
std::unique_ptr<Tablero> t = std::make_unique<Tablero>("position startpos");
```
- **Problema**: En `meanSquareError()` se crea un nuevo Motor y Tablero para cada batch
- **Oportunidad**: Reutilizar objetos, usar object pooling

#### 3. Llamadas a `quiescence()` en loop
```cpp
for (const auto& pos : positions) {
    score = m->quiescence(t.get(), -500000, 500000);
}
```
- **Problema**: Esta es la operación más costosa (búsqueda de motor de ajedrez)
- **Oportunidad**: Reducir llamadas, usar paralelización (ver sección abajo)

#### 4. Escritura a archivo en `guardarParametros()`
```cpp
file.open("/home/axel/Documentos/parametros/iteracion" + std::to_string(contador) + ".txt");
```
- **Problema**: Abre/cierra archivo en cada iteración de entrenamiento
- **Oportunidad**: Usar buffer en memoria, escribir periódicamente

#### 5. Cálculo de gradiente en `SGD()`
```cpp
for (int i = 0; i < 432; i++) {
    // ... operaciones por cada parámetro
}
```
- **Problema**: Loop secuencial sobre 432 parámetros
- **Oportunidad**: Paralelizar con OpenMP o std::parallel_for

---

### Uso de Memoria

| Estructura | Tamaño | Notas |
|------------|--------|-------|
| `velocidades` | 433 × 8 bytes ≈ 3.4 KB | Vector global sin uso aparente |
| `positions` (en memoria) | n × ~100 bytes | Depende del archivo de posiciones |
| `gradientes` | 432 × 8 bytes ≈ 3.4 KB | Vector local en SGD |
| `parametros` | 432 × 8 bytes ≈ 3.4 KB | Retorno de obtenerParametros |
| `variacionesRandom` | 432 × 8 bytes ≈ 3.4 KB | Creado pero no utilizado |

**Total estimado por ejecución**: ~10-50 KB + contenido del archivo de posiciones

---

### Oportunidades de Paralelización

#### Nivel 1: Paralelización de posiciones (`meanSquareError`, `SGD`)
```cpp
// Patrón posible: Parallel for sobre posiciones
#pragma omp parallel for
for (size_t i = 0; i < positions.size(); i++) {
    // procesar positions[i]
}
```
- **Beneficio potencial**: Lineal en número de cores
- **Sin dependencias entre iteraciones** (excepto acumulación de gradiente)

#### Nivel 2: Paralelización de gradientes (`SGD`, `calcularGradiente`)
```cpp
// 432 iteraciones independientes
#pragma omp parallel for
for (int i = 0; i < 432; i++) {
    gradientes[i] = calcularGradienteParametro(i);
}
```
- **Beneficio potencial**: speedup de hasta 432x (teórico)
- **Requiere**: Reducción thread-safe para gradientes

#### Nivel 3: SIMD para operaciones vectoriales
- Las operaciones sobre vectores de 432 elementos pueden beneficiarse de SIMD
- Especialmente: `scorePropio - score`, multiplicaciones por `hayPieza`

---

### Cuellos de Botella Identificados

1. **`quiescence()` call** (líneas 51, 436)
   -占比: ~90% del tiempo de ejecución
   - Tipo: Algorítmico (búsqueda de ajedrez)
   - Optimización: Reducir llamadas, mejor poda alfa-beta

2. **I/O de archivo** (líneas 27, 397)
   -占比: ~5-10% del tiempo total
   - Optimización: Cache en memoria

3. **String operations** (líneas 44-46, 429-431)
   - `substr()` llamado repetidamente
   - Optimización: Preprocesar FEN y resultados

---

### Dependencias entre Funciones

```
meanSquareError
├── → Tablero::configurarFen()
├── → Motor::quiescence()
└── → obtenerParametros() [indirecto via Motor]

SGD
├── → obtenerParametros()
├── → Tablero (creación por iteración)
├── → Motor::quiescence()
├── → Tablero::obtenerTipoDePieza()
├── → Tablero::piezas_negras()
└── → operaciones_bit::espejarCasilla()

calcularGradiente
├── → obtenerParametros()
├── → Motor::quiescence()
└── → constantes (lectura/escritura)

guardarParametros
└── → constantes (lectura)

actualizarParametros
└── → constantes (escritura)

obtenerParametros
└── → constantes (lectura)
```

---

### Patrones de Diseño Observados

1. **Factory/Builder**: Uso de `std::make_unique<Motor>()` para creación de objetos
2. **Strategy**: La función de evaluación parametrizable permite diferentes estrategias
3. **Template Method**: Patrón implícito en el cálculo de gradientes (iterar → perturbar → evaluar → restaurar)
4. **Batch Processing**: `meanSquareError` procesa múltiples posiciones en batch

---

### Métricas Sugeridas para Profiling

| Métrica | Herramienta | Punto de medición |
|---------|-------------|-------------------|
| Tiempo por posición | `std::chrono` | Around line 51, 436 |
| Tiempo de I/O | `strace` / `perf` | Lectura de archivo |
| Uso de memoria | `valgrind --tool=massif` | Todo el módulo |
| Cache misses | `perf stat` | Acceso a `constantes::*` |
| CPU utilization | `htop` / `perf` | Durante SGD |

---

### Flags de Compilación Recomendados

```bash
# Optimización para velocidad
-O3 -march=native -mtune=native

# Paralelización
-fopenmp  # Para #pragma omp parallel

# Vectorización
-ftree-vectorize -ffast-math

# Ejemplo completo
g++ -O3 -march=native -mtune=native -fopenmp -ftree-vectorize -ffast-math -o motor_optimizado src/*.cpp
```

---

### Referencias Cruzadas con Otros Módulos

| Módulo | Función llamada | Frecuencia |
|--------|-----------------|------------|
| `Motor` | `quiescence()` | Alta (por posición) |
| `Tablero` | `configurarFen()` | Alta |
| `Tablero` | `obtenerTipoDePieza()` | 432 × epochs |
| `Tablero` | `piezas_negras()` | 432 × epochs |
| `operaciones_bit` | `espejarCasilla()` | Ocasional |
| `constantes` | Tablas de ocupación | Lectura/escritura frecuente |

---

*Documento generado para análisis de oportunidades de optimización del motor de ajedrez ChessCPP*
