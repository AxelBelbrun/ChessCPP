# Documentación del módulo Jugada

## Propósito

Este módulo define la estructura `Jugada` que representa un movimiento de ajedrez en el sistema. Su función principal es codificar y decodificar jugadas mediante operaciones de bits, permitiendo almacenar toda la información relevante de un movimiento en un único valor de 16 bits (`u_short`).

## Estructura de datos

### Representación binaria del movimiento

El campo `movimiento` (tipo `u_short`, 16 bits) almacena la información del movimiento usando los siguientes bits:

| Rango de bits | Tamaño | Descripción |
|---------------|--------|-------------|
| 0-5           | 6 bits | Posición de origen (casilla de salida) |
| 6-11          | 6 bits | Posición de destino (casilla de llegada) |
| 12-17         | 6 bits | Tipo de jugadatipoDeJugada |

### Enum `tipoDeJugada`

Define los posibles tipos de jugadas reconocidas por el sistema:

```
QUIET, ENPASSANT, CASTLING, PROMOTIONDER, PROMOTIONIZQ, PROMOTION,
CAPTURE, CPIC, CPDC, PROMOTIONCHECK, CAPTURECHECK, CHECK,
CHECKMATE, ENPASSANTCHECK, CASTLINGCHECK
```

## Componentes del módulo

### Header: `include/Jugada.h`

**Estructura `Jugada`** (líneas 12-58)

Contiene:

- `u_short movimiento`: Campo privado que almacena el movimiento codificado.
- Constructor `Jugada(int salida, int llegada, int tipoDeJugada)`: Inicializa el campo `movimiento` usando operaciones OR de bits para empaquetar los tres valores.

**Métodos de acceso (getters)**:

- `int getSalida()`: Extrae los bits 0-5 del movimiento.
- `int getLlegada()`: Extrae los bits 6-11 del movimiento.
- `int getTipoDeJugada()`: Extrae los bits 12-17 del movimiento.
- `int getCasillaDestino()`: Equivalente a `getLlegada()` (implementación en .cpp).

**Método estático**:

- `static u_short stringToUShort(std::string jugada)`: Convierte una representación en string del movimiento a formato `u_short`. El string debe tener formato "ABCD" donde:
  - "AB": casillero de origen (ej: "e2")
  - "CD": casillero de destino (ej: "e4")
  - El quinto carácter indica el tipo de jugadatipoDeJugada como dígito numérico.

**Operadores**:

- `operator==`: Compara si dos jugadas son iguales comparando directamente el campo `movimiento`.

### Implementación: `src/Jugada.cpp`

Contiene la implementación de los métodos definidos fuera de línea:

1. **`getCasillaDestino()`** (línea 18-21): Retorna `movimiento >> 6`, equivalente a `getLlegada()`.

2. **`stringToUShort(std::string)`** (líneas 22-32):
   - Utiliza el mapa `casillaANumero` (definido en `constantes.h`) para convertir notación algebraica (ej: "e2") a números.
   - Construye el `u_short` resultado usando operaciones de bits:
     - `salida` en bits 0-5
     - `llegada << 6` en bits 6-11
     - `tipoDeJugada << 12` en bits 12-17

## Dependencias externas

El módulo depende de:

- **`constantes.h`**: Proporciona el mapa `casallaANumero` y el namespace `constantes`.
- **`Tablero.h`**: Incluido pero no utilizado directamente en el código de Jugada.cpp.

## Consideraciones para refactoring

1. **Duplicación lógica**: `getCasillaDestino()` y `getLlegada()` realizan la misma operación. Consolidar en un solo método.

2. **Validación ausente**: `stringToUShort()` no valida que el string tenga el formato correcto ni que las casillas existan en el mapa.

3. **Código muerto**: El enum `tipoDeJugada` usa valores que podrían no ser necesarios (ej: CPDC, CPIC).

4. **Acoplamiento con constantes**: El método `stringToUShort()` depende fuertemente de que `casillaANumero` exista y contenga las casillas esperadas.

5. **Comentar código**: Hay bloques comentados en ambas líneas (constructor alternativo en .cpp, operadores en .h) que deberían eliminarse o mantenerse explícitamente.

6. **Notación confusa**: Nombres como PROMOTIONDER/PROMOTIONIZQ parecen hacer referencia a direcciones (derecha/izquierda) pero el significado no es claro sin contexto adicional.

## Análisis técnico para optimización

### Características de memoria

| Aspecto | Detalle |
|---------|---------|
| Tamaño de `Jugada` | 2 bytes (`u_short` único) |
| Alineación | 2 bytes (óptimo para arquitecturas de 16/32/64 bits) |
| Padding | Ninguno (estructura minimalista) |
| Sobrecarga por instanciación | 2 bytes por objeto |

La estructura ocupa exactamente 2 bytes, lo cual es óptimo para almacenar los tres componentes del movimiento. El diseño es más eficiente que usar tres campos separados (que ocuparía 12 bytes con padding típico de 32 bits).

### Operaciones bit a nivel de Assembly

Los getters utilizan operaciones extremadamente eficientes a nivel de CPU:

| Método | Operación | Latencia típica (ciclos) |
|--------|-----------|------------------------|
| `getSalida()` | `AND` con mask 0x3F | 1 ciclo |
| `getLlegada()` | Shift right 6 + `AND` | 1 ciclo |
| `getTipoDeJugada()` | Shift right 12 + `AND` | 1 ciclo |
| `getCasillaDestino()` | Shift right 6 | 1 ciclo |

**Consideración crítica**: `getLlegada()` y `getCasillaDestino()` realizan exactamente la misma operación (`movimiento >> 6`), pero la primera aplica una máscara adicional que es redundante dado el rango de bits utilizado. Esta máscara `& 0x3F` no afecta el resultado pero consume un ciclo de CPU adicional.

### Constructor inline

El constructor `Jugada(int salida, int llegada, int tipoDeJugada)` está definido inline en el header. El compilador puede:
- **Inlinearlo completamente**: Eliminando la llamada de función
- **Usar registros**: Los tres parámetros caben en registros (RDI, RSI, EDX en x86-64)
- **Optimizar shifts compuestos**: `llegada << 6` y `tipoDeJugada << 12` pueden ejecutarse en paralelo en CPUs con ejecución fuera de orden

### Análisis de `stringToUShort()`

Esta función presenta el mayor costo computacional del módulo:

| Operación | Costo relativo |
|-----------|-----------------|
| `jugada.substr(0,2)` | Asignación heap (≈50-100 ciclos) |
| `jugada.substr(2,2)` | Asignación heap (≈50-100 ciclos) |
| `casillaANumero.at()` | Hash lookup (≈10-30 ciclos) |
| `jugada[4] - '0'` | Acceso array + resta (1 ciclo) |
| Operaciones bit | Negligible (1 ciclo) |

**Cuellos de botella identificados**:
1. **Allocaciones de memoria**: `substr()` crea nuevos objetos `std::string` en cada llamada
2. **Hash map lookup**: `std::map::at()` tiene complejidad O(log n), podría optimizarse con unordered_map
3. **Sin validación**: No hay checks de longitud, causando undefined behavior con strings cortos

### Potencial de optimización

#### 1. Eliminación de asignaciones heap
```cpp
// Actual (costoso):
jugada.substr(0,2)  // allocates new string

// Optimizable con string_view:
std::string_view(jugada.data(), 2)  // no allocation
```

#### 2. Lookuptable para conversión string→número
El mapa `casillaANumero` con 64 entradas puede convertirse en un array estático indexado directamente, eliminando el hash lookup:
```cpp
// En lugar de map lookup:
int salida = casillaANumero.at(jugada.substr(0,2));

// Array indexable:
int salida = charToSquare[jugada[0]-'a'][jugada[1]-'1'];  // O(1) directo
```

#### 3. Unificación de getters redundantes
`getLlegada()` y `getCasillaDestino()` pueden fusionarse. La máscara en `getLlegada()` es redundante dado que el shift ya posiciona los bits correctamente.

#### 4. Pre-computación de masks
Los masks `0x3F` (63) pueden declararse como `constexpr` para permitir optimización en tiempo de compilación.

### Consideraciones de caché

- **Footprint mínimo**: 2 bytes por `Jugada` permite almacenar hasta ~500 jugadas en una caché L1 típico (1KB)
- **Spatial locality**: Almacenar arrays de `Jugada` es extremadamente cache-friendly
- **Sin false sharing**: En entornos multithreaded, el tamaño de 2 bytes minimiza contenido entre líneas de caché

### Observaciones para análisis de oportunidades

1. **Hot path**: Los getters son parte del hot path del motor de búsqueda. Optimizarlos (especialmente eliminar la máscara redundante) tiene impacto directo en NPS (nodes per second).

2. **String parsing es el bottleneck**: La conversión de string a `u_short` es 50-100x más costosa que las operaciones bit. Priorizar optimización aquí si el perfilado muestra lentitud en parsing UCI.

3. **Memoria vs velocidad**: El diseño actual prioriza espacio (2 bytes). Si el perfilado indica que los getters son críticos,可以考虑 convertir a registros separados (6+6+6 = 18 bits, 4 bytes con padding) para eliminar shifts.

4. **Sin SIMD potencial**: Por el tamaño mínimo y naturaleza escalar, no hay oportunidad de vectorización SIMD en este módulo.

5. **Trade-off diseño**: El uso de bit-packing es apropiado para este caso de uso. Alternativas como campos separados simplificarían el código pero aumentarían uso de memoria 4x-6x.
