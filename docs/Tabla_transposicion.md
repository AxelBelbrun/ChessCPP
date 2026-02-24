# Tabla de Transposición

## Propósito

Este módulo implementa una **tabla de transposición** (transposition table), una estructura de datos fundamental en motores de ajedrez para optimizar la búsqueda. Su función principal es almacenar posiciones del tablero que ya han sido evaluadas junto con su mejor movimiento, permitiendo:

- Evitar recalcular posiciones ya analizadas (transposiciones)
- Recortar ramas del árbol de búsqueda (beta cutoff)
- Mejorar la eficiencia del algoritmo Minimax con poda alfa-beta

## Archivos

- **Header**: `include/Tabla_transposicion.h`
- **Implementación**: `src/Tabla_transposicion.cpp`

## Dependencias

- `Tablero.h` - Proporciona el tipo `Tablero*` utilizado internamente

## Estructura de Datos

### Entrada (`entrada`)

Cada posición se almacena en una estructura con los siguientes campos:

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `clave` | `U64` | Hash de la posición del tablero |
| `valor` | `float` | Valor evaluado de la posición |
| `profundidad` | `int` | Profundidad a la que se evaluó la posición |
| `tipo` | `int` | Tipo de resultado (EXACTO, ALFA, BETA) |
| `jugada` | `u_short` | Mejor movimiento encontrado para esta posición |

### Tabla Principal

```cpp
entrada tabla[4 * 419430];
```

- Tamaño fijo de aproximadamente **1,677,720 entradas**
- **Importante**: El tamaño actual (4 * 419430 = 1,677,720) **NO es potencia de 2**
- Indexación mediante módulo: `index = clave % tamanoTabla` (operación lenta)
- Si se usara tamaño de potencia de 2, se podría usar: `clave & (tamanoTabla - 1)` (mucho más rápido)

## API Pública

### Constructor

```cpp
Tabla_transposicion();
```

Inicializa la tabla con tamaño fijo y reinicia el contador.

---

### `asignarTablero`

```cpp
void asignarTablero(Tablero* tablero);
```

Asocia un puntero al tablero actual. No se utiliza para operaciones de la tabla en sí.

**Parámetros:**
- `tablero`: Puntero al objeto `Tablero` del juego

---

### `insertar`

```cpp
void insertar(U64 clave, float valor, int profundidad, int tipo, u_short jugaba);
```

Inserta o actualiza una entrada en la tabla de transposición.

**Parámetros:**
- `clave`: Hash Zobrist de la posición
- `valor`: Valor evaluado de la posición
- `profundidad`: Profundidad de búsqueda
- `tipo`: Tipo de nodo (EXACTO, BETA, ALFA)
- `jugada`: Movimiento codificado

**Política de Reemplazo:**

La entrada existente se reemplaza si:
1. El slot está vacío (`clave == 0`)
2. Es una colisión (diferente clave)
3. La nueva profundidad es mayor
4. Misma profundidad pero tipo "más fuerte" (EXACTO > BETA > ALFA)

---

### `index`

```cpp
U64 index(U64 clave);
```

Calcula el índice en la tabla usando módulo del tamaño.

**Parámetros:**
- `clave`: Hash de la posición

**Retorna:** Índice en el rango `[0, tamanoTabla)`

---

### `obtenerMovimientoGuardado`

```cpp
u_short obtenerMovimientoGuardado(U64 clave);
```

Recupera el mejor movimiento almacenado para una posición.

**Parámetros:**
- `clave`: Hash de la posición

**Retorna:** Movimiento codificado o `0` si no existe entrada

---

### `obtenerEntrada`

```cpp
entrada obtenerEntrada(U64 clave);
```

Recupera la entrada completa asociada a una posición.

**Parámetros:**
- `clave`: Hash de la posición

**Retorna:** Entrada almacenada o entrada vacía si no existe

---

### `limpiarTabla`

```cpp
void limpiarTabla();
```

Reinicia todas las entradas de la tabla a valores por defecto y reinicia el contador.

## Atributos Públicos

| Atributo | Tipo | Descripción |
|----------|------|-------------|
| `tablero` | `Tablero*` | Puntero al tablero asociado |
| `tamanoTabla` | `int` | Tamaño de la tabla |
| `count` | `U64` | Contador de entradas insertadas |
| `tabla` | `entrada[]` | Array de entradas |

## Notas para Refactoring

1. **Tamaño fijo**: La tabla tiene un tamaño hardcodeado (`4 * 419430 = 1,677,720`). 
   - **CRÍTICO**: Este tamaño NO es potencia de 2, por lo que usa `%` (división lenta)
   - Recomendación: Cambiar a potencia de 2 (ej: 2^21 = 2,097,152) y usar `&` (más rápido)

2. **Array vs contenedor**: Usa un array estático en lugar de contenedores estándar (std::vector, std::unordered_map). Esto puede optimizarse.

3. **Política de reemplazo**: La política actual es simple pero podría mejorarse con reemplazo por profundidad o reemplazos diferidos.

4. **Colisiones**: No hay manejo de colisiones más allá del reemplazo básico. No se verifica si la clave coincide exactamente (solo se verifica si el slot está vacío o es colisión).

5. **Tipado**: El tipo `tipo` (int) debería ser un enum para mayor claridad.

6. **Valores nulos**: Se usa `clave = 0` para indicar slot vacío. Esto asume que 0 nunca será un hash válido.

7. **Precisión**: El campo `valor` usa `float`. Podría considerarse `double` para mayor precisión en evaluaciones.

---

## Análisis Técnico de Rendimiento

### Uso de Memoria

**Tamaño de la estructura `entrada`:**

| Campo | Tipo | Tamaño (bytes) |
|-------|------|----------------|
| `clave` | `U64` | 8 |
| `valor` | `float` | 4 |
| `profundidad` | `int` | 4 |
| `tipo` | `int` | 4 |
| `jugada` | `u_short` | 2 |
| **Total (sin padding)** | | **22** |
| **Con padding (alineación)** | | **24** (múltiplo de 8) |

**Memoria total de la tabla:**
- Entradas: `1,677,720 × 24 bytes ≈ 40.27 MB`
- Sobrecarga adicional: despreciable (array estático)

### Complejidad Algorítmica

| Operación | Complejidad | Notas |
|-----------|-------------|-------|
| `index()` | O(1) | Una operación módulo |
| `insertar()` | O(1) | Acceso directo al slot |
| `obtenerMovimientoGuardado()` | O(1) | Una lectura |
| `obtenerEntrada()` | O(1) | Una comparación + lectura |
| `limpiarTabla()` | O(n) | Itera sobre todas las entradas |

### Análisis de Cache (CPU Cache)

**Patrón de acceso esperado:**
- Búsquedas: Acceso aleatorio basado en hash
- Inserciones: Acceso aleatorio
- El tamaño de la tabla (≈40 MB) excede la L3 cache típica (8-30 MB)
- Cada acceso causará likely un cache miss

**Impacto:**
- Alto rate de cache misses debido al acceso aleatorio
- La tabla compite con otras estructuras en memoria (tablero, pila de movimientos)

### Costos de las Operaciones Críticas

**Operación `%` (módulo):**
- En x86-64: instrucción `div` o `idiv` - latencia de 20-80 ciclos
- Alternativa con potencia de 2: `and` - latencia de 1 ciclo

**Comparación:**
| Operación | Ciclos (x86-64) |
|-----------|-----------------|
| `div` (64-bit) | 20-80 |
| `and` / `bitwise` | 1 |
| Acceso a memoria (RAM) | 100-300 ns |

### Política de Reemplazo - Análisis Detallado

La política actual tiene las siguientes características:

1. **Reemplazo por clave cero**: Asume que `clave = 0` indica slot vacío
2. **Reemplazo por colisión**: Cualquier clave diferente reemplaza
3. **Reemplazo por profundidad**: Prefiere entradas de mayor profundidad
4. **Reemplazo por tipo**: EXACTO > BETA > ALFA

**Problemas identificados:**
- No hay verificación de "generación" (evitar reemplazar entradas recientes)
- No hay mecanismo de "aging" para entradas antiguas
- No hay protección contra hash collision attacks
- La condición `slot.clave != clave` siempre reemplaza aunque la entrada sea más valiosa

### Acceso a Memoria y Branching

**Patrón de branching en `insertar()`:**
```cpp
if (slot.clave == 0)          // branch 1: typically false
    reemplazar = true;
else if (slot.clave != clave) // branch 2: typically false (good case)
    reemplazar = true;
else if (slot.profundidad < profundidad) // branch 3
    reemplazar = true;
else if (...) // branch 4
    reemplazar = true;
```

**Predicción de branch:**
- Branch 1: Falso la mayor parte del tiempo (tabla en uso) → bien predicho
- Branch 2: Falso cuando hay hit → bien predicho
- Branch 3/4: Variable según distribución de profundidades

### Tamaño de Tabla - Análisis

**Tamaño actual:**
- `4 * 419430 = 1,677,720` entradas
- No es potencia de 2

**Consideraciones para selección de tamaño:**
- Tablas típicas en motores: 1MB - 64MB
- El tamaño debe ser potencia de 2 para indexing rápido
- Debe caber en L3 cache si es posible (menor latency)
- Debe ser suficiente para la profundidad de búsqueda típica

**Valores típicos:**
| Profundidad típica | Entradas necesarias (estimado) |
|--------------------|----------------------------------|
| 3-4 ply | 100,000 - 500,000 |
| 6-8 ply | 1,000,000 - 4,000,000 |
| 10+ ply | 10,000,000+ |

### Alternativas de Implementación

| Enfoque | Ventajas | Desventajas |
|---------|----------|-------------|
| Array estático (actual) | Sin overhead, acceso rápido | Tamaño fijo, colisiones simples |
| std::vector | Dinámico | Overhead de STL |
| std::unordered_map | Hash robusto | Overhead significativo |
| Bucket-based (clusters) | Mejor manejo de colisiones | Más complejo |
| Two-level table ( generational) | Protege entradas recientes | Más memoria |

### Observaciones sobre el Tipo `float`

- `float`: 32 bits, precisión de ~7 dígitos decimales
- `double`: 64 bits, precisión de ~15 dígitos decimales
- En evaluaciones de ajedrez, la precisión extra raramente es necesaria
- `float` es más cache-friendly (4 bytes vs 8 bytes)
- Sin embargo, el padding ya lleva la estructura a 24 bytes

### Métricas Sugeridas para Profiling

Para evaluar el rendimiento real de esta tabla:

1. **Hit rate**: `hits / (hits + misses)`
2. **Tiempo de acceso promedio**: ns por operación
3. **Cache miss rate**: medido con perf/counters
4. **Colisiones efectivas**: cuántos replacements por inserción
5. **Distribución de tipos**: cuántos EXACTO vs ALFA vs BETA
