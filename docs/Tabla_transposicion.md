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
