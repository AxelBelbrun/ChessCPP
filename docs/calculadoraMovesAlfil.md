# Módulo calculadoraMovesAlfil

## Propósito

Este módulo implementa el cálculo de **números mágicos** para el alfil en el contexto de la técnica de *magic bitboards* utilizada en motores de ajedrez. Los números mágicos permiten indexar tablas de ataque precalculadas de manera eficiente, transformando cualquier configuración de piezas bloqueantes en un índice único para buscar los movimientos legales del alfil.

## Dependencias Externas

El módulo depende de las siguientes entidades (definidas en otros archivos):

| Entidad | Archivo | Propósito |
|---------|---------|-----------|
| `operaciones_bit` | `operaciones_bit.h` | Utilidades de operaciones bit (LSB, setBit, etc.) |
| `Alfil` | `Alfil.h` | Clase que genera los movimientos legales del alfil |
| `U64` | Typedef | Tipo `unsigned long long` para representar bitboards |

---

## Variables Globales

### `BBits[64]`

```cpp
static int BBits[64];
```

Array estático que define el **número de bits relevantes** para cada casilla del tablero de ajedrez. Los valores representan cuántos cuadrados en las diagonales desde cada posición pueden ser bloqueados por piezas.

**Distribución de valores:**
- **6**: Esquinas (a1, a8, h1, h8)
- **5**: Casillas adyacentes a las esquinas
- **7**: Casillas intermedias de los bordes
- **9**: Centro del tablero (casillas d4, d5, e4, e5)

---

## Funciones

### `masksTableroVacio()`

```cpp
std::vector<U64> masksTableroVacio();
```

**Descripción:** Genera los *attack masks* del alfil para un tablero vacío, es decir, los cuadrados que el alfil atacaría sin piezas bloqueantes en el tablero.

**Retorna:** Vector de 64 elementos donde cada posición `i` contiene un bitboard con los cuadrados que el alfil atacaría desde la casilla `i+1` en un tablero vacío.

**Dependencias internas:**
- `Alfil::generar_attack_mask()` — Calcula los ataques del alfil dado un bitboard

---

### `masksPiezasBloqueando(U64 generar_attack_mask)`

```cpp
std::vector<U64> masksPiezasBloqueando(U64 generar_attack_mask);
```

**Descripción:** Genera todas las posibles configuraciones de piezas que pueden bloquear los movimientos del alfil desde una posición específica.

**Parámetros:**
- `generar_attack_mask`: Bitboard que representa las casillas que el alfil puede atacar (las casillas potencialmente bloqueables)

**Algoritmo:**
1. Extrae las posiciones (LSB) de los bits en el mask de ataque
2. Para cada combinación posible de piezas bloqueantes (2^n combinaciones donde n = número de casillas bloqueables):
   - Genera un mask representando esa configuración de piezas
   - Almacena el mask en el vector resultado

**Retorna:** Vector conteniendo todos los posibles subconjuntos de piezas que pueden bloquear al alfil (2^n elementos).

**Dependencias internas:**
- `operaciones_bit::LSB()` — Extrae el bit menos significativo
- `operaciones_bit::setBit()` — Establece un bit en una posición específica

---

### `obtenerNumeroMagico(int casilla, int shifteo)`

```cpp
U64 obtenerNumeroMagico(int casilla, int shifteo);
```

**Descripción:** Calcula un **número mágico** válido para una casilla específica del alfil. El número mágico debe producir índices únicos para cada configuración diferente de piezas bloqueantes.

**Parámetros:**
- `casilla`: Posición del alfil en el tablero (1-64)
- `shifteo`: Valor de shift para calcular el índice (actualmente **no utilizado** en el código)

**Algoritmo:**
1. Obtiene el mask de tablero vacío para la casilla
2. Genera todas las configuraciones de piezas bloqueantes posibles
3. Itera hasta 100,000 veces:
   - Genera un número aleatorio de 64 bits como candidato
   - Para cada configuración de piezas bloqueantes:
     - Calcula el índice: `(bloqueadores * numeroMagico) >> (64 - 9)` → resultado en rango [0, 511]
     - Genera los movimientos legales del alfil con esas piezas bloqueando
     - Verifica que no haya colisión: el índice debe mapear a los mismos movimientos legales
   - Si todas las configuraciones producen índices únicos → retorna el número mágico
4. Si no encuentra uno en 100,000 intentos, retorna 0

**Retorna:** Un número mágico de 64 bits válido para la casilla dada, o 0 si no se encuentra.

**Nota:** El parámetro `shifteo` está declarado pero no se utiliza en la implementación actual. El shift está hardcodeado como `64 - 9 = 55`, generando una tabla de 512 entradas.

**Dependencias internas:**
- `masksTableroVacio()` — Obtiene el mask base sin bloqueadores
- `masksPiezasBloqueando()` — Genera configuraciones de piezas
- `operaciones_bit::setBit()` — Para crear el bitboard del alfil
- `Alfil::generar_movimientos_legales()` — Genera movimientos legales dados bloqueadores

---

## Notas Técnicas

### Magic Bitboards

Los *magic bitboards* son una técnica de optimización que permite calcular los movimientos del alfil (y la torre) en O(1) mediante tablas de lookup. La fórmula básica es:

```
indice = (bloqueadores * numero_magico) >> (64 - shift)
```

Donde:
- `bloqueadores`: Bits de las piezas que pueden bloquear al alfil
- `numero_magico`: Número precalculado que garantiza índices únicos
- `shift`: Cantidad de bits a desplazar (determina tamaño de tabla)

### Tamaño de Tabla

El código actual utiliza:
- Shift = 9 → Tabla de 2^9 = **512 entradas**
- Esto es suficiente para el alfil, que tiene máximo 13 casillas bloqueables en el centro

### Código Inactivo

El archivo contiene secciones comentadas que parecen ser trabajo en progreso:
- Líneas 61, 64, 67-69, 74-77: Referencias a `Tablero` y distribuciones aleatorias
- Estas no afectan la funcionalidad actual

---

## Integración con el Resto del Programa

```
┌─────────────────────────────────────────────────────────────┐
│                   calculadoraMovesAlfil                     │
├─────────────────────────────────────────────────────────────┤
│  obtenerNumeroMagico()                                       │
│       │                                                      │
│       ├── masksTableroVacio() ──► Alfil::generar_attack_mask │
│       │                                                      │
│       └── masksPiezasBloqueando()                           │
│                │                                             │
│                ├── operaciones_bit::LSB()                    │
│                └── operaciones_bit::setBit()                 │
│                                                              │
│  Salida: Número mágico para indexar tabla de movimientos    │
└─────────────────────────────────────────────────────────────┘
```

Este módulo es parte del sistema de inicialización del motor de ajedrez. Los números mágicos calculados se utilizarían posteriormente para construir tablas de ataque que aceleren el cálculo de movimientos legales durante la búsqueda de mejores jugadas.
