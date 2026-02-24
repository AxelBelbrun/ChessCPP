# Documentación del Módulo `operaciones_bit`

## Descripción General

El módulo `operaciones_bit` (definido en `operaciones_bit.h` e implementado en `operaciones_bit.cpp`) contiene funciones de bajo nivel para manipulación de bits y codificación de jugadas de ajedrez. Este módulo es utilizado como utilidad fundamental por otras partes del programa para representar y manipular el estado del tablero y los movimientos.

**Namespace:** `operaciones_bit`

**Tipos definidos:**
- `U64`: `unsigned long long` (64 bits) - representa casillas del tablero
- `int8`: `u_int8_t` (8 bits sin signo)

---

## Funciones

### 1. `setBit`

```cpp
U64 setBit(U64 num, int pos, int bit);
```

Establece un bit específico en una posición determinada de un número de 64 bits.

**Parámetros:**
- `num`: Número de 64 bits donde se modificará el bit
- `pos`: Posición del bit (1-64, donde 1 es el bit menos significativo)
- `bit`: Valor a establecer (0 o 1)

**Retorna:**
- Nuevo valor de `U64` con el bit modificado en la posición especificada

**Comportamiento:**
- Si `bit == 0`: Pone el bit en 0 usando máscara AND con negación
- Si `bit == 1`: Pone el bit en 1 usando máscara OR

---

### 2. `LSB`

```cpp
int LSB(U64 &num);
```

Encuentra y retorna la posición del bit menos significativo que está activado (tiene valor 1), y simultáneamente lo establece a 0.

**Parámetros:**
- `num`: Referencia a un número de 64 bits (se modifica internamente)

**Retorna:**
- Posición del bit menos significativo encontrado (1-64), o 0 si no hay bits activos

**Comportamiento:**
- Utiliza `__builtin_ffsll()` para encontrar el primer bit activo
- Limpia ese bit usando la técnica `num & (num - 1)` (BQT)

---

### 3. `crearJugada`

```cpp
u_short crearJugada(int salida, int llegada, int tipoDeJugada);
```

Codifica una jugad de ajedrez en un entero de 16 bits compactando la información.

**Parámetros:**
- `salida`: Casilla de origen (1-64)
- `llegada`: Casilla de destino (1-64)
- `tipoDeJugada`: Tipo de movimiento (0-15)

**Retorna:**
- `u_short` (16 bits) con la jugad codificada

**Estructura del encoding (16 bits):**
| Bits | Campo |
|------|-------|
| 0-5 (6 bits) | Casilla de salida (0-63, almacenado como `salida - 1`) |
| 6-11 (6 bits) | Casilla de llegada (0-63, almacenado como `llegada - 1`) |
| 12-15 (4 bits) | Tipo de jugada (0-15) |

---

### 4. `getLlegada`

```cpp
int getLlegada(u_short movimiento);
```

Extrae la casilla de destino de una jugada codificada.

**Parámetros:**
- `movimiento`: Jugada codificada (16 bits)

**Retorna:**
- Casilla de llegada (1-64), o 1 si es captura al paso especial

**Nota:** Maneja un caso especial para captura al paso donde la posición efectiva es 1

---

### 5. `getSalida`

```cpp
int getSalida(u_short movimiento);
```

Extrae la casilla de origen de una jugada codificada.

**Parámetros:**
- `movimiento`: Jugada codificada (16 bits)

**Retorna:**
- Casilla de salida (1-64)

---

### 6. `getTipoDeJugada`

```cpp
int getTipoDeJugada(u_short movimiento);
```

Extrae el tipo de jugad de una jugada codificada.

**Parámetros:**
- `movimiento`: Jugada codificada (16 bits)

**Retorna:**
- Tipo de jugad (0-15)

---

### 7. `espejarCasilla`

```cpp
int espejarCasilla(int casilla);
```

Refleja una casilla horizontalmente en el tablero (simetría de espejo).

**Parámetros:**
- `casilla`: Número de casilla (1-64)

**Retorna:**
- Casilla espejada correspondiente

**Dependencia:** Utiliza el array `constantes::casillasEspejadas` definido en `constantes.h`

---

### 8. `random_custom`

```cpp
U64 random_custom();
```

Genera un número aleatorio de 64 bits.

**Parámetros:** Ninguno

**Retorna:**
- `U64` con valor aleatorio

**Nota:** La implementación no está visible en este módulo (definida en header, implementación externa)

---

## Dependencias Externas

- **Header `constantes.h`**: Provee el array `constantes::casillasEspejadas` usado por `espejarCasilla`
- **Funciones del compilador**: `__builtin_ffsll()` (GCC/Clang) para finding least significant bit

---

## Casos de Uso Típicos

1. **Manipulación del tablero**: Usar `setBit` y `LSB` para activar/desactivar casillas y recorrer bits eficientemente
2. **Codificación de movimientos**: Usar `crearJugada` para compactar jugadas, y las funciones `get*` para decodificarlas
3. **Simetría del tablero**: Usar `espejarCasilla` para operaciones con tableros espejados
4. **Generación aleatoria**: Usar `random_custom` para valores de hash aleatorios

---

## Notas para Refactoring

- La función `espejarCasilla` tiene acoplamiento con el módulo `constantes.h` - considerar pasar la tabla como parámetro
- El caso especial en `getLlegada` (línea 36-38) para "captura al paso" debería documentarse mejor o usar constantes nombradas
- Los tipos `int8` y `U64` son fundamentales para todo el programa - considerar si pertenecen a este módulo o a uno más global
- Las operaciones de jugadas usan magic numbers (6, 12, 63, 15) - idealmente deberían ser constantes con nombres descriptivos

---

## Información Técnica para Optimización

### Complejidad Algorítmica

| Función | Complejidad | Notas |
|---------|-------------|-------|
| `setBit` | O(1) | Operaciones bitwise de costo constante |
| `LSB` | O(1) | `__builtin_ffsll` es O(1) en práctica (instrucción CPU) |
| `crearJugada` | O(1) | 3 operaciones bitwise |
| `getLlegada` | O(1) | 2 shifts + 1 AND + 1 suma + condición |
| `getSalida` | O(1) | 1 AND + 1 suma |
| `getTipoDeJugada` | O(1) | 1 shift + 1 AND |
| `espejarCasilla` | O(1) | Acceso a array con índice |
| `random_custom` | O(1) | Depende de implementación (posiblemente RNG) |

---

### Análisis de Rendimiento por Función

#### `setBit`
- **Operaciones realizadas:** 1 shift, 1 negación (opcional), 1 AND/OR
- **Instrucciones CPU típicas:** `shl`, `not`, `and`, `or`
- **Costo estimado:** 3-5 ciclos de CPU
- **Observaciones:** 
  - La función recibe `int pos` pero calcula `(pos-1)` - posible pérdida de rendimiento por resta
  - El parámetro `bit` es `int` pero solo se usa como 0/1 - podría ser `bool`
  - **Optimización potencial:** Si `pos` es 1-indexed, convertir a 0-indexed en caller para eliminar resta

#### `LSB`
- **Operaciones realizadas:** `__builtin_ffsll()` + 1 resta + 1 AND
- **Instrucciones CPU típicas:** `tzcnt` o `bsf` (x86), `rbit` + `clz` (ARM)
- **Costo estimado:** 1-3 ciclos de CPU (instrucción nativa)
- **Observaciones:**
  - `__builtin_ffsll` se compila a instrucción CPU nativa (`BSF` o `TZCNT` en x86)
  - La técnica `num & (num - 1)` es una de las formas más eficientes de clear lowest bit
  - **Posible mejora:** Usar directamente `__builtin_ctzll()` si no se necesita modificar `num`
  - **Riesgo:** Retorna 0 cuando `num == 0` - verificar que callers manejen este caso

#### `crearJugada`
- **Operaciones realizadas:** 3 operaciones bitwise (2 shifts, 2 OR)
- **Costo estimado:** 3-5 ciclos de CPU
- **Observaciones:**
  - Usa `salida - 1` y `llegada - 1` para convertir de 1-indexed a 0-indexed
  - **Optimización potencial:** Si los argumentos ya vienen en 0-indexed, eliminar restas
  - **Observación:** Inicializa `jugada = 0` innecesario (OR con 0 no cambia valor)

#### `getLlegada`
- **Operaciones realizadas:** 1 shift, 1 AND con 63, 1 suma, 1 shift, 1 AND con 15, 1 comparación
- **Costo estimado:** 5-8 ciclos de CPU
- **Observaciones:**
  - Caso especial para "captura al paso" añade branching непредсказуемо
  - **Problema de rendimiento:** El branching impredecible puede costar 10-20 ciclos si no se predice bien
  - La máscara `& 63` es equivalente a `& 0x3F` - el compilador debería optimizarlo
  - **Optimización potencial:** Extraer el tipo de jugada primero para evitar trabajo extra en caso especial

#### `getSalida`
- **Operaciones realizadas:** 1 AND con 63, 1 suma
- **Costo estimado:** 2-3 ciclos de CPU
- **Observaciones:** Muy eficiente, una de las funciones más rápidas

#### `getTipoDeJugada`
- **Operaciones realizadas:** 1 shift, 1 AND con 15
- **Costo estimado:** 2-3 ciclos de CPU
- **Observaciones:** Muy eficiente, usar `& 15` en lugar de `& 0xF` es idiomático

#### `espejarCasilla`
- **Operaciones realizadas:** 1 resta, 1 acceso a array, 1 resta
- **Costo estimado:** 3-5 ciclos (depende de cache)
- **Observaciones:**
  - Acceso a memoria - el array debe estar en cache para máximo rendimiento
  - **Optimización potencial:** Si se usa frecuentemente, podría inlinearse con cálculo directo: `casilla ^ 7` (para filas horizontales) - verificar mapeo del array

---

### Patrones de Uso y Frecuencia

Basado en el diseño del módulo, las funciones más críticas para optimización son:

1. **`LSB`** - Probablemente la función más llamada en generación de movimientos (recorrer bits de atacantes/tablero)
2. **`setBit`** - Alta frecuencia en manipulación del tablero
3. **`crearJugada` / `getSalida` / `getLlegada`** - Frecuentes en generación y ejecución de jugadas

### Problemas de Rendimiento Identificados

| Problema | Ubicación | Impacto | Sugerencia |
|----------|-----------|---------|------------|
| Magic numbers sin nombrar | Líneas 27-31, 36-39, 43, 49 | Mantenibilidad, posible error | Definir constantes `SALIDA_BITS`, `LLEGADA_SHIFT`, etc. |
| Conversiones 1-indexed a 0-indexed | Múltiples funciones | Resta adicional por llamada | Estandarizar a 0-indexed internamente |
| Branch impredecible | `getLlegada` líneas 36-38 | 10-20 ciclos si mispredict | Reestructurar para evitar branch o usar lookup table |
| Acoplamiento con `constantes.h` | `espejarCasilla` | Cache misses si header cambia | Pasar tabla como parámetro o hacer lookup inline |

---

### Consideraciones de Portabilidad

- `__builtin_ffsll()` es extensión de GCC/Clang - disponible en MSVC como `_BitScanForward64`
- `U64` es `unsigned long long` - garantizado como 64 bits por estándar C++
- El shift `1ULL << (pos-1)` asegura promoción a 64 bits antes del shift

---

### Inline y Compilación

- Todas las funciones son candidatas para `inline` o `static inline` 
- Considerar marcar como `inline` en header si son llamadas frecuentemente
- El compilador puede no inlinear si no está declarado en el mismo archivo de compilación

---

### Notas Adicionales para Análisis

1. **Hot path**: `LSB` es la función más crítica - cualquier mejora aquí tiene máximo impacto
2. **Bitboard operations**: Este módulo es típico de motores de ajedrez con representación bitboard - verificar si hay más operaciones bitboard que podrían agregarse
3. **Memory locality**: `espejarCasilla` accede a array - verificar tamaño y cache locality
4. **Branch prediction**: El caso especial en `getLlegada` es unpredictable - evaluar frecuencia real de "captura al paso"
