# Documentación de calculadoraMovesTorre

## 1. Propósito del Módulo

Este módulo tiene como objetivo calcular **números mágicos** (magic numbers) para la pieza **Torre** en el contexto de un motor de ajedrez. Los números mágicos son utilizados en la técnica de *magic bitboards* para acelerar la generación de movimientos de piezas deslizantes (sliding pieces) mediante tablas de búsqueda (hash tables).

## 2. Dependencias Externas

El módulo depende de los siguientes componentes (definidos en otros archivos):

- **`operaciones_bit`**: Utilizada para operaciones a nivel de bits (setBit, LSB, etc.)
- **`Torre`**: Clase que representa la pieza Torre y proporciona métodos para generar attack masks y movimientos legales
- **Tipo `U64`**: Representa un unsigned long long (64 bits), usado para representar tableros de ajedrez

## 3. Clase: `calculadoraMovesTorre`

### 3.1 Métodos Públicos

#### `std::vector<U64> masksTableroVacio()`

**Descripción**: Genera los *attack masks* para todas las 64 casillas del tablero cuando no hay piezas bloqueando los movimientos de la torre.

**Retorna**: Un vector de 64 elementos, donde cada elemento contiene un bitboard con las casillas que la torre podría atacar desde esa posición en un tablero vacío.

**Comportamiento**:
1. Itera sobre todas las casillas del 1 al 64
2. Crea un bitboard con un solo bit activo en la posición actual
3. Utiliza `Torre::generar_attack_mask()` para obtener los movimientos posibles
4. Almacena el resultado en el vector

---

#### `static std::vector<U64> masksPiezasBloqueando(U64 generar_attack_mask)`

**Descripción**: Genera todas las posibles configuraciones de piezas que podrían estar bloqueando los movimientos de la torre desde una posición dada.

**Parámetros**:
- `generar_attack_mask`: Bitboard que representa las casillas que la torre puede atacar (usado para determinar el número de piezas que pueden bloquear)

**Retorna**: Un vector conteniendo todos los posibles subconjuntos de piezas bloqueantes (2^n configuraciones, donde n es el número de casillas que pueden contener piezas bloqueantes)

**Comportamiento**:
1. Extrae las posiciones de los bits activos del attack_mask
2. Para cada configuración posible (0 a 2^n - 1), genera un bitboard con las piezas en las posiciones correspondientes
3. Utiliza operaciones de bits para construir cada configuración

---

#### `U64 obtenerNumeroMagico(int casilla, int shifteo)`

**Descripción**: Calcula un número mágico válido para la posición de torre especificada. Un número mágico válido es aquel que produce índices únicos en la tabla de hash para cada configuración diferente de piezas bloqueantes.

**Parámetros**:
- `casilla`: Posición de la torre en el tablero (1-64)
- `shifteo`: Valor de desplazamiento (actualmente no utilizado en el código)

**Retorna**: Un número mágico de 64 bits que producecolisiones en la tabla de hash, o 0 si no se encuentra uno después de 10 millones de intentos.

**Comportamiento**:
1. Obtiene los masks de tablero vacío para la posición dada
2. Genera todas las configuraciones de piezas bloqueantes posibles
3. Itera hasta 10 millones de veces:
   - Genera un número aleatorio de 64 bits
   - Para cada configuración de piezas bloqueantes:
     - Calcula el índice en la tabla de hash usando la fórmula: `(bloqueos * numeroMagico) >> (64 - 12)`
     - Obtiene los movimientos legales para esa configuración
     - Verifica si el índice está vacío o contiene los mismos movimientos
     - Si hay colisión (movimientos diferentes), reinicia la tabla y prueba otro número
4. Retorna el número mágico si se encuentra uno válido

---

## 4. Algoritmo de Generación de Números Mágicos

### 4.1 Concepto

Los números mágicos permiten indexar una tabla de hash de tamaño fijo (4096 entradas = 2^12) usando el producto de bits de piezas bloqueantes y el número mágico, desplazado para obtener un índice de 12 bits.

### 4.2 Fórmula de Indexación

```
indice = (piezas_bloqueantes * numero_magico) >> (64 - 12)
```

### 4.3 Criterio de Validez

Un número mágico es válido si y solo si produce un índice único para cada configuración diferente de piezas bloqueantes que resulta en movimientos legales distintos.

## 5. Limitaciones y Notas

- El método `obtenerNumeroMagico` tiene un límite de 10 millones de intentos para encontrar un número válido
- El parámetro `shifteo` en `obtenerNumeroMagico` no se utiliza actualmente en el código
- El código contiene secciones comentadas que sugieren experimentación previa con distribuciones aleatorias y estrategias alternativas de generación
- La tabla de hash tiene un tamaño fijo de 4096 entradas
- Hay objetos `Torre` y `Tablero` creados con `new` pero `Tablero` está comentado

## 6. Estructura de Datos

| Estructura | Tipo | Descripción |
|------------|------|-------------|
| `U64` | `unsigned long long` | Bitboard de 64 bits |
| `tablaDeHash` | `U64[4096]` | Tabla de búsqueda para movimientos precalculados |
| `máscaras` | `vector<U64>` | Colección de bitboards |

## 7. Flujo de Uso Típico

```
1. Llamar a masksTableroVacio() para obtener attack masks base
2. Llamar a masksPiezasBloqueando() con un attack mask específico
3. Llamar a obtenerNumeroMagico() para encontrar el número mágico válido
4. Usar el número mágico en tiempo de ejecución para indexar tablas de movimientos
```
