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

---

## 8. Análisis Técnico para Optimización

### 8.1 Complejidad Algorítmica

| Método | Complejidad Temporal | Complejidad Espacial |
|--------|----------------------|---------------------|
| `masksTableroVacio()` | O(64 × C_atacando) | O(64) |
| `masksPiezasBloqueando()` | O(2^n × n) | O(2^n) |
| `obtenerNumeroMagico()` | O(Intentos × 2^n × C_mover) | O(4096) |

**Donde**:
- `n` = número de casillas que pueden bloquear (varía por posición, máximo ~14 para esquinas)
- `C_atacando` = costo de `Torre::generar_attack_mask()`
- `C_mover` = costo de `Torre::generar_movimientos_legales()`

### 8.2 Asignaciones de Memoria Dinámica

**Problemáticas identificadas**:

| Ubicación | Tipo de asignación | Impacto |
|-----------|---------------------|---------|
| Línea 17: `Torre* torre = new Torre()` | `new` en bucle (64 iteraciones) | Fragmentación de heap |
| Línea 52: `Torre* torre = new Torre()` | `new` dentro de función llamada repetidamente | Sin reuse de objeto |
| Línea 14, 34, 48, 49: `std::vector` | Reallocations implícitas | Copies innecesarias |
| Línea 35: `pow(2, contador)` | Uso de `double` para enteros | Imprecisión potencial |

### 8.3 Operaciones Costosas

1. **Línea 35**: `pow(2, contador)` - `pow()` trabaja con `double` y es significativamente más lento que `1ULL << contador`
2. **Línea 74**: Generación de número aleatorio con triple mascarao `mt() & mt() & mt()` - diseñado para setear más bits, pero reduce entropía efectiva
3. **Líneas 77-96**: Bucle anidado con hasta 16,384 iteraciones (2^14 × 10M en worst case)
4. **Línea 80**: Llamada a `generar_movimientos_legales()` repetida por cada configuración - esta función se invoca miles de veces por intento

### 8.4 Cuellos de Botella Identificados

1. **Allocations dinámicas repetidas** (líneas 17, 52): Crear y destruir objetos `Torre` en cada iteración
2. **Recálculo de masksTableroVacio()** (línea 48): Se recalcula en cada llamada a `obtenerNumeroMagico()`, debería calcularse una sola vez
3. **Recálculo de masksPiezasBloqueando()** (línea 49): Mismo problema que arriba
4. **Limpieza de tablaDeHash** (líneas 87-89): Bucle de 4096 iteraciones en cada colisión

### 8.5 Métricas de Rendimiento Esperadas

- **Tiempo por llamada a `masksTableroVacio()`**: ~1-5 ms (64 iteraciones)
- **Tiempo por llamada a `masksPiezasBloqueando()`**: Variable según n (2^n configuraciones)
- **Tiempo por `obtenerNumeroMagico()`**: Minutos a horas (10M intentos × 2^n × C_mover)
- **Promedio de intentos hasta encontrar válido**: Varía por posición; algunas posiciones son más difíciles

### 8.6 Uso de CPU y Cache

- **tablaDeHash[4096]**: 32 KB (4096 × 8 bytes) - cabe en L1 cache
- **Vector masks**: Hasta ~128 KB (2^14 × 8 bytes) - excede L1, entra en L2
- **Operaciones bit**: Todas las operaciones trabajan con registros de 64 bits - cache-friendly
- **Prefetching**: No hay acceso secuencial claro que permita prefetching efectivo

### 8.7 Oportunidades de Optimización Identificadas

| Área | Optimización Potencial | Impacto Estimado |
|------|------------------------|------------------|
| Línea 35 | Reemplazar `pow(2,n)` por `1ULL << n` | 10-50x más rápido |
| Línea 17, 52 | Usar stack allocation o reuse de objeto Torre | Elimina allocation overhead |
| Línea 48-49 | Calcular masksTableroVacio() una vez (static) | Elimina 64 × N llamadas |
| Línea 49 | Calcular masksPiezasBloqueando() una vez por attack_mask | Elimina 2^n × N llamadas |
| Líneas 87-89 | Usar `memset()` para limpiar tablaDeHash | ~10x más rápido |
| Línea 74 | Usar solo `mt()` o `mt() ^ (mt() << 1)` | Mayor entropía con menos operaciones |
| Parámetro `shifteo` | Implementar uso del parámetro | Podría reducir espacio de búsqueda |

### 8.8 Consideraciones de Seguridad y Correctitud

- **Línea 30**: `operaciones_bit::LSB()` debe manejar correctamente el caso cuando `attack_mask = 0`
- **Línea 105**: Retornar `0` como número mágico podría ser problemático - evaluar usar valor centinela
- **Líneas 14, 17**: Rango de casillas 1-64 - verificar que `Torre` use indexación 1-based consistente

### 8.9 Código Muerto y Comentarios

- **Línea 55-68**: Código comentado con estrategias alternativas de generación
- **Línea 53**: Objeto `Tablero` comentado - verificar si es necesario
- **Línea 55**: Parámetro `shifteo` no utilizado - evaluar remover o implementar

---

## 9. Recomendaciones para Refactoring

1. **Extraer cálculos重复itivos**: Masks de tablero vacío y piezas bloqueando deberían calcularse una sola vez y almacenarse
2. **Eliminar allocations dinámicas**: Usar objetos en stack o pooling
3. **Usar operaciones enteras**: Reemplazar `pow()` con shifts
4. **Optimizar limpieza de hash**: Usar `memset` o mantener tabla en stack
5. **Considerar paralelización**: La búsqueda de número mágico es paralelizable (múltiples intentos independientes)
6. **Evaluar pre-cálculo**: Los números mágicos podrían calcularse en tiempo de compilación o en un paso de inicialización separado
