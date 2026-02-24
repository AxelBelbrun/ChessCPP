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
