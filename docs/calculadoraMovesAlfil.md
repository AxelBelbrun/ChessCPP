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

## Información Técnica de Optimización

### Complejidad Algorítmica

| Función | Complejidad | Análisis |
|---------|-------------|----------|
| `masksTableroVacio()` | O(64 × M) | 64 iteraciones, donde M es el coste de `generar_attack_mask()` |
| `masksPiezasBloqueando()` | O(2^n × n) | n = bits bloqueables. Genera 2^n masks, cada uno con n operaciones de setBit |
| `obtenerNumeroMagico()` | O(I × 2^n × M) | I = intentos (max 100,000), 2^n configuraciones, M = coste de generar_movimientos_legales() |

**Detalles:**
- Para el alfil, n varía entre 4-13 dependiendo de la posición:
  - Esquinas: n = 4-6 → 16-64 configuraciones
  - Bordes: n = 7-9 → 128-512 configuraciones
  - Centro: n = 10-13 → 1024-8192 configuraciones

---

### Uso de Memoria

| Estructura | Tamaño | Notas |
|------------|--------|-------|
| `BBits[64]` | 256 bytes | Datos estáticos, siempre en memoria |
| `masksTableroVacio()` | 64 × 8 = **512 bytes** | Vector temporal por llamada |
| `masksPiezasBloqueando()` | 2^n × 8 bytes | Hasta 64KB para centro (8192 × 8) |
| `tablaDeHash[512]` | 512 × 8 = **4 KB** | Tabla temporal en `obtenerNumeroMagico()` |
| `Alfil*` | ~tamaño clase | Objeto dinámico creado/destruido por llamada |

**Notas:**
- `masksTableroVacio()` se recalcula en CADA llamada a `obtenerNumeroMagico()` → podría cachearse
- Los vectores usan overhead adicional de `std::vector` (~24 bytes por vector)

---

### Cuellos de Botella Identificados

#### 1. **Recálculo de `masksTableroVacio()` en cada búsqueda** (línea 56)
```cpp
vector<U64> masksTableroVacio = calculadoraMovesAlfil::masksTableroVacio();
```
Este método itera 64 veces y llama a `generar_attack_mask()` para cada casilla. Se ejecuta **cada vez** que se busca un número mágico. 

**Impacto:** ~64 llamadas a `generar_attack_mask()` × 100,000 intentos = 6.4M llamadas innecesarias.

**Oportunidad:** Calcular una sola vez al inicio y reutilizar.

#### 2. **Creación/destrucción de objetos `Alfil`**
```cpp
auto alfil = new Alfil();  // línea 22
// ... uso ...
delete alfil;              // línea 31
```
```cpp
Alfil* alfil = new Alfil();  // línea 60
// ... uso ...
// NO HAY DELETE → MEMORY LEAK
```

**Problemas:**
- Memory leak en línea 60: `alfil` nunca se libera
- Creación repetida de objetos en `masksTableroVacio()`

#### 3. **Potencia de 2 en tiempo de ejecución** (línea 43)
```cpp
for(int j = 0; j < pow(2, contador); j++)
```
`pow()` es una función floating-point costosa. Debería usar shift: `1 << contador` (con control de overflow).

#### 4. **Búsqueda con máximo fijo de intentos**
```cpp
for(w; w < 100000; w++)
```
- 100,000 es un número arbitrario
- No hay early termination por caso imposible
- Coste variable: algunas casillas pueden necesitar más intentos

#### 5. **Parámetro `shifteo` no utilizado**
El segundo parámetro está declarado pero ignorado:
```cpp
U64 obtenerNumeroMagico(int casilla, int shifteo)  // shifteo = 9 hardcodeado
```

---

### Operaciones Costosas por Categoría

| Operación | Coste Aproximado | Frecuencia | Impacto |
|-----------|------------------|------------|---------|
| `pow(2, n)` | ~50-100 ciclos | Por cada `masksPiezasBloqueando()` | Bajo-Medio |
| `generar_movimientos_legales()` | Alto | 2^n × intentos | **Muy Alto** |
| `mt()` (random 64-bit) | ~20-30 ciclos | intentos × 1 | Bajo |
| Multiplicación U64 | ~20-40 ciclos | 2^n × intentos | Medio |
| Shift U64 | ~1 ciclo | 2^n × intentos | Bajo |
| `std::vector::push_back()` | Variable | 2^n veces | Medio |

---

### Ineficiencias de Código

#### A. Vector no inicializado con tamaño exacto
```cpp
vector<U64> masks;
for(int j = 0; j < pow(2, contador); j++){
    masks.push_back(0L);  // push_back con reallocation
```
**Mejora:** `vector<U64> masks(1 << contador, 0);`

#### B. Uso inconsistente de tipos
```cpp
vector<U64> masks;           // línea 42
vector<U64> masksTableroVacio = ...;  // línea 56
vector<u_short> a;           // línea 81 (definida pero no usada)
```
- `vector<U64>` vs `vector<u_short>` - sin consistencia

#### C. Tabla de hash no limpiada eficientemente
```cpp
for (int j = 0; j < 512; j++) {
    tablaDeHash[j] = 0;  // Loop manual
}
```
**Mejora:** `memset(tablaDeHash, 0, sizeof(tablaDeHash));`

#### D. Variable no utilizada
```cpp
vector<u_short> a;  // línea 81 - declarada pero nunca usada
```

---

### Métricas de Rendimiento Estimadas

Asumiendo las siguientes operaciones base:
- `generar_movimientos_legales()`: ~500 ciclos
- Iteración media de configuraciones: ~256 (nivel centro)

| Escenario | Iteraciones | Coste Estimado |
|-----------|-------------|----------------|
| Mejor caso (esquina) | 1-10 | 64 × 500 × 10 = **320K ciclos** |
| Caso promedio | 1000 | 64 × 500 × 1000 = **32M ciclos** |
| Peor caso (centro) | 100,000 | 64 × 500 × 100,000 = **3.2G ciclos** |

**Nota:** Estas son estimaciones burdas; el rendimiento real depende fuertemente de `generar_movimientos_legales()`.

---

### Oportunidades de Optimización Identificadas

#### Prioridad Alta
1. **Cachear `masksTableroVacio()`** como variable estática o miembro de clase
2. **Corregir memory leak** en `obtenerNumeroMagico()` (línea 60)
3. **Reutilizar objeto `Alfil`** en lugar de crear/destruir por llamada

#### Prioridad Media
4. **Reemplazar `pow(2, n)`** por `1 << n` (con validación de n < 63)
5. **Usar `memset`** para limpiar tablaDeHash
6. **Hacer `shifteo` operativo** o eliminar el parámetro muerto
7. **Pre-calcular tamaño de vector** en `masksPiezasBloqueando()`

#### Prioridad Baja
8. **Eliminar variable `a` no utilizada** (línea 81)
9. **Usar `reserve()`** en vectores si se conoce el tamaño
10. **Considerar paralelización** para buscar magic numbers de 64 casillas

---

### Consideraciones para Refactoring

1. **Separación de responsabilidades**: La clase actualmente mezcla:
   - Cálculo de masks
   - Búsqueda de números mágicos
   - Generación de ataques
   
   Podría dividirse en clases especializadas.

2. **Valores hardcodeados a constantes**:
   ```cpp
   const int MAX_INTENTOS = 100000;
   const int SHIFT_ALFIL = 9;
   const int TABLA_HASH_SIZE = 512;
   ```

3. **Patrón de diseño**: Considerar singleton para la calculadora mágica, manteniendo caches de masks.

4. **Thread-safety**: Si se paraleliza la búsqueda de números mágicos para múltiples casillas, habría que considerar sincronización.

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
