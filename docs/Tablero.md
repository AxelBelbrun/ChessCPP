# Documentación del Módulo Tablero

## 1. Propósito General

El archivo `Tablero.cpp` (acompañado de `Tablero.h`) constituye el núcleo central del motor de ajedrez. Es responsable de:

- Representar el estado del tablero mediante **bitboards**
- Gestionar el movimiento de piezas
- Manejar jugadas especiales (enroque, captura al paso, promoción)
- Generar movimientos legales
- Calcular ataques y jaques
- Mantener el historial de jugadas para poder deshacerlas
- Gestionar la clave Zobrist para detección de posiciones repetidas

---

## 2. Estructuras de Datos Principales

### 2.1 Bitboards

El tablero utiliza 12 bitboards (tipo `U64`) para representar las piezas:

| Índice | Pieza (Blancas) | Índice | Pieza (Negras) |
|--------|-----------------|--------|----------------|
| 0      | Rey blanco      | 6      | Rey negro      |
| 1      | Dama blanca     | 7      | Dama negra     |
| 2      | Torre blanca    | 8      | Torre negra    |
| 3      | Alfil blanco    | 9      | Alfil negro    |
| 4      | Caballo blanco  | 10     | Caballo negro  |
| 5      | Peón blanco    | 11     | Peón negro     |

### 2.2 Tablas de Movimiento (Magic Bitboards)

```cpp
U64 movimientosDeTorre[64][4096];  // 64 casillas × 4096 configuraciones
U64 movimientosDeAlfil[64][512];  // 64 casillas × 512 configuraciones
U64 movimientosDeCaballo[64];
U64 movimientosDeRey[64];
```

Estas tablas precalculan todos los movimientos posibles para cada pieza en cada casilla, utilizando la técnica de **magic bitboards** para piezas deslizantes (sliding pieces).

### 2.3 Historiales

- `u_short jugadas[512]` - Historial de jugadas realizadas
- `U64 historialZobrist[512]` - Historial de claves Zobrist
- `modificacionBitboard historialBitboards[512]` - Historial de modificaciones a bitboards
- `std::vector<int> historialEnPassant` - Historial de casillas para captura al paso
- `std::stack<derechosDeEnroque> enroques` - Historial de derechos de enroque

---

## 3. Constantes de Piezas (Variables Globales)

```cpp
int PEON_BLANCO = PEON;        // Índice 5
int PEON_NEGRO = PEON + 6;     // Índice 11
int REY_BLANCO = REY;          // Índice 0
int REY_NEGRO = REY + 6;      // Índice 6
// ... y así sucesivamente
```

---

## 4. Funciones Principales (Agrupadas por Responsabilidad)

### 4.1 Inicialización del Tablero

| Función | Descripción |
|---------|-------------|
| `Tablero()` | Constructor - genera tablas de movimientos |
| `actualizarTablero(string)` | Punto de entrada principal para cargar posición |
| `setearPosicionInicial()` | Configura posición inicial estándar del ajedrez |
| `setearPosicionDeFen()` | Parsea notación FEN para posición personalizada |
| `generarTablasDeMovimientos()` | Precalcula todas las tablas de movimientos |
| `generarTablasSliding()` | Genera tablas para torre y alfil |
| `generarTablasNoSliding()` | Genera tablas para caballo, rey y peón |

### 4.2 Movimiento de Piezas

| Función | Descripción |
|---------|-------------|
| `moverPieza(int salida, int llegada, int tipoDeJugada)` | Ejecuta un movimiento en el tablero |
| `moverPiezaTrusted()` | Versión que no valida法律的 (para búsqueda) |
| `deshacerMovimiento()` | Deshace el último movimiento |
| `promocionar()` | Ejecuta promoción de peón |

#### Tipos de Jugada (constantes externas):
- `QUIET` - Movimiento normal
- `CAPTURE` - Captura
- `CHECK` - Jaque
- `CAPTURECHECK` - Captura que da jaque
- `CASTLING` - Enroque
- `ENPASSANT` - Captura al paso
- `PROMOTION` / `PROMOTIONDER` / `PROMOTIONIZQ` - Promoción

### 4.3 Generación de Movimientos

| Función | Descripción |
|---------|-------------|
| `generar_movimientos(int turno, int ply)` | Genera todos los movimientos legales |
| `generar_capturas()` | Genera solo movimientos de captura |
| `obtener_movimientos_*_blanco()` | Genera movimientos de cada pieza blanca |
| `obtener_movimientos_*_negro()` | Genera movimientos de cada pieza negra |
| `bitboard_movimientos_*()` | Calcula bitboard de ataques de cada pieza |

### 4.4 Estado del Juego

| Función | Descripción |
|---------|-------------|
| `reyPropioEnJaque(int color)` | Determina si el rey está en jaque |
| `esJaque()` | Verifica si un movimiento da jaque |
| `chequearEnroqueCorto()` | Valida si es posible enroque corto |
| `chequearEnroqueLargo()` | Valida si es posible enroque largo |
| `enrocar()` | Ejecuta un enroque |

### 4.5 Utilidades de Piezas

| Función | Descripción |
|---------|-------------|
| `piezas_blancas()` | Retorna bitboard de todas las piezas blancas |
| `piezas_negras()` | Retorna bitboard de todas las piezas negras |
| `todas_las_piezas()` | Retorna bitboard de todas las piezas |
| `piezasPropias()` | Piezas del bando actual |
| `piezasRivales()` | Piezas del bando oponente |
| `obtenerTipoDePieza()` | Determina el tipo de pieza en una casilla |
| `esSlidingPiece()` | Indica si una pieza es deslizante |
| `esUnaPromocion()` | Verifica si la jugada es promoción |

### 4.6 Piezas Clavadas

| Función | Descripción |
|---------|-------------|
| `estaClavada()` | Detecta si una pieza está clavada |
| `generarMovimientosPiezaClavada()` | Genera movimientos válidos para pieza clavada |

### 4.7 Representación y Visualización

| Función | Descripción |
|---------|-------------|
| `imprimirTablero()` | Imprime el tablero en consola (formato ASCII) |
| `imprimirFen()` | Imprime la posición en notación FEN |

### 4.8 Zobrist y Detección de Repeticiones

| Función | Descripción |
|---------|-------------|
| `zobristKey()` | Calcula la clave Zobrist de la posición actual |
| `actualizarZobristKey()` | Actualiza la clave después de un movimiento |
| `esTripleRepeticion()` | Detecta si hay triple repetición |

### 4.9 Gestión de Turno y Enroques

| Función | Descripción |
|---------|-------------|
| `cambiarTurno()` | Alterna entre blancas y negras |
| `setTurno()` | Establece el turno |
| `setearEnroque()` | Modifica derechos de enroque |
| `guardarEnroque()` | Guarda el estado actual de enroques |
| `deshacerEnroque()` | Deshace los cambios de enroque |
| `enroqueCortoBlancas()` / `enroqueLargoBlancas()` | Consulta derechos de enroque |
| `enroqueCortoNegras()` / `enroqueLargoNegras()` | Consulta derechos de enroque |

---

## 5. Dependencias Externas

El módulo depende de las siguientes clases y utilidades (implementadas en otros archivos):

### 5.1 Clases de Piezas
- `Rey` - Generación de movimientos del rey
- `Dama` - Generación de movimientos de la dama
- `Torre` - Generación de movimientos de la torre
- `Alfil` - Generación de movimientos del alfil
- `Caballo` - Generación de movimientos del caballo
- `Peón` - Generación de movimientos del peón

### 5.2 Utilidades
- `operaciones_bit` - Operaciones a nivel de bits (setBit, LSB, etc.)
- `constantes` - Constantes del programa (attack masks, magic numbers, etc.)
- `calculadoraMovesTorre` / `calculadoraMovesAlfil` - Generadores de masks para piezas deslizantes
- `Jugada` - Manejo de jugadas codificadas

### 5.3 Tipos de Datos
- `U64` - Entero sin signo de 64 bits (bitboard)
- `u_short` - Entero sin signo para representar jugadas
- `bitmask` - Tipo para máscaras de bits

---

## 6. Notas para Refactoring

1. **Métodos duplicates**: Existen múltiples versiones de métodos como `obtener_movimientos_dama_blanca` vs `bitboard_movimientos_dama_blanca` que podrían refactorizarse.

2. **Código muerto**: Hay fragmentos comentados relacionados con material y evaluación que podrían eliminarse.

3. **Lógica de promoción duplicada**: La lógica de promoción aparece múltiples veces en `moverPieza` y `moverPiezaTrusted`.

4. **Enumerados**: Los tipos de jugada (`QUIET`, `CAPTURE`, etc.) deberían documentarse mejor en `constantes.h`.

5. **Acoplamiento**: La clase `Tablero` tiene muchas responsabilidades; podría dividirse en clases más pequeñas (ej: `GameState`, `MoveGenerator`, `PositionHistory`).

6. **Constantes mágicas**: Hay valores hardcodeados (como códigos de casillas para enroques) que deberían parametrizarse.

---

## 8. Información Técnica para Optimización

### 8.1 Memoria

| Estructura | Tamaño | Descripción |
|------------|--------|-------------|
| `movimientosDeTorre[64][4096]` | 64 × 4096 × 8 = **2 MB** | Tabla de movimientos de torre |
| `movimientosDeAlfil[64][512]` | 64 × 512 × 8 = **256 KB** | Tabla de movimientos de alfil |
| `movimientosDeCaballo[64]` | 64 × 8 = **512 bytes** | Tabla de movimientos de caballo |
| `movimientosDeRey[64]` | 64 × 8 = **512 bytes** | Tabla de movimientos de rey |
| `bitboards[12]` | 12 × 8 = **96 bytes** | Estado actual del tablero |
| `historialZobrist[512]` | 512 × 8 = **4 KB** | Historial de posiciones |
| `historialBitboards[512]` | 512 × ~40 bytes | Historial de cambios |
| `jugadas[512]` | 512 × 2 = **1 KB** | Historial de jugadas |
| `ocupacionPiezas[14]` | 14 × 4 = **56 bytes** | Evaluación de posición |

**Total aproximado**: ~2.3 MB en tablas estáticas

### 8.2 Complejidad Algorítmica

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

### 8.3 Operaciones Bit Frecuentes

El código utiliza extensivamente operaciones bit en `U64`:

| Operación | Frecuencia | Notas |
|-----------|------------|-------|
| `operaciones_bit::setBit()` | Muy alta | Establece un bit |
| `operaciones_bit::LSB()` | Muy alta | Obtiene el bit menos significativo |
| `operaciones_bit::getSalida()` | Alta | Extrae casilla de salida de `u_short` |
| `operaciones_bit::getLlegada()` | Alta | Extrae casilla de llegada de `u_short` |
| `operaciones_bit::getTipoDeJugada()` | Alta | Extrae tipo de movimiento |
| `operaciones_bit::crearJugada()` | Alta | Codifica jugadas |

**Nota**: Estas operaciones están definidas en `operaciones_bit`. La eficiencia de `LSB()` es crítica - debería usar intrinsics como `__builtin_ctzll()` o instrucciones CPU (`TZCNT`/`BSF`).

### 8.4 Tablas de Magic Bitboards

El código implementa el sistema de **magic bitboards**:

```
Key = (occupancy * magic_number) >> (64 - bits)
Lookup = movimientos[casilla][key]
```

| Pieza | Bits de índice | Configuraciones | Tamaño tabla |
|--------|----------------|-----------------|--------------|
| Torre | 12 | 4096 | 64 × 4096 = 262,144 entradas |
| Alfil | 9 | 512 | 64 × 512 = 32,768 entradas |

**Constantes magic** (en `constantes`):
- `magicsParaTorre[64]`
- `magicsParaAlfil[64]`

### 8.5 Puntos de Acceso Calientes (Hot Paths)

Estos métodos se llaman frecuentemente durante la búsqueda:

1. **`moverPieza()`** - Llamado miles de veces por el motor de búsqueda
2. **`deshacerMovimiento()`** - Debe ser muy rápido para el algoritmo de recursión
3. **`reyPropioEnJaque()`** - Llamado para validar movimientos legales
4. **`generar_movimientos()`** - Genera todos los movimientos posibles
5. **`bitboard_movimientos_*()`** - Consulta tablas para ataques
6. **`piezas_blancas()` / `piezas_negras()`** - Calcula union de bitboards

### 8.6 Evaluación de Posición (Ocupación)

El sistema mantiene valores de **ocupación** actualizados para la función de evaluación:

```cpp
int ocupacionPiezas[14];  // Índices 0-5: blancas, 6-11: negras, 12-13: reyes (medio/final)
```

Tablas de lookup (definidas en `constantes`):
- `ocupacionPeon[64]`
- `ocupacionCaballo[64]`
- `ocupacionAlfil[64]`
- `ocupacionTorre[64]`
- `ocupacionReina[64]`
- `ocupacionReyMedioJuego[64]`
- `ocupacionReyFinal[64]`

Funciones de actualización:
- `calcularOcupacion()` - Recalcula toda la ocupación
- `actualizarOcupacion(u_short)` - Actualiza incrementalmente
- `contarMaterialSinPeones()` - Cuenta material sin peones

### 8.7 Zobrist Key

Sistema de hashing para detección de repeticiones:

- **Tabla de claves**: `zobristKeys[12][64]` - Una clave por cada pieza en cada casilla
- **Claves adicionales**: Para turno, enroques, y en-passant
- **Actualización**: `actualizarZobristKey()` modifica la clave en lugar de recalcularla completamente

### 8.8 Iteración sobre Bitboards

Patrón común en el código:

```cpp
// Iterar sobre todas las piezas de un tipo
U64 bitboard = bitboards[INDICE];
while (bitboard > 0) {
    int casilla = operaciones_bit::LSB(bitboard);
    // procesar...
    bitboard &= bitboard - 1;  // Clear LSB
}
```

Esta técnica (clear LSB) es O(n) donde n = número de piezas.

### 8.9 includes Relevantes para Optimización

```cpp
#include <x86intrin.h>  // Intrinsics de CPU (potencialmente usado en operaciones_bit)
```

---

## 9. Oportunidades de Optimización Identificadas

### 9.1 Alta Prioridad

1. **Reemplazar `obtenerTipoDePieza()`**: Actualmente itera sobre 12 bitboards. Puede optimizarse usando lookup tables o manteniendo un array auxiliar.

2. **Cachear `piezas_blancas()` y `piezas_negras()`**: Se calculan frecuentemente mediante OR de 6 bitboards cada vez.

3. **Optimizar `reyPropioEnJaque()`**: Función crítica con múltiples branches. Puede vectorizarse.

4. **Inlining de operaciones bit**: Asegurar que `setBit`, `LSB`, etc. estén inlineadas.

### 9.2 Media Prioridad

1. **Comprimir estructuras de historial**: `modificacionBitboard` tiene ~40 bytes; puede reducirse.

2. **Eliminar código muerto**: Hay funciones comentadas que aumentan el binary size.

3. **Prefetching de tablas magic**: Las tablas de movimientos son grandes; pueden beneficiarse de prefetch.

4. **SIMD para evaluación**: Las funciones de `calcularOcupacion*()` podrían vectorizarse.

### 9.3 Baja Prioridad

1. **Separar responsabilidades**: La clase `Tablero` hace demasiado; separar en módulos permite mejor cacheo.

2. **Memory pooling**: Los `new` de piezas en `generarTablasNoSliding()` podrían reemplazarse con objetos estáticos.

---

## 10. Benchmarks Sugeridos

Para optimizar este módulo, se recomienda medir:

1. **Nodos por segundo** en búsqueda de profundidad fija
2. **Tiempo de `generar_movimientos()`** por posición
3. **Tiempo de `moverPieza()` + `deshacerMovimiento()`** (par)
4. **Tiempo de `reyPropioEnJaque()`**
5. **Cache hit rate** de tablas magic
6. **Tiempo de inicialización** del motor (generación de tablas)
