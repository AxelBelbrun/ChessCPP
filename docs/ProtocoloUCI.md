# Documentación del Módulo ProtocoloUCI

## Descripción General

Este módulo implementa el protocolo UCI (Universal Chess Interface), el estándar de comunicación entre motores de ajedrez e interfaces gráfica de usuario (GUI). El protocolo permite que una GUI envíe comandos al motor y este responda con jugadas y otra información.

**Ubicación de archivos:**
- Header: `include/ProtocoloUCI.h`
- Implementación: `src/ProtocoloUCI.cpp`

---

## Clase: `ProtocoloUCI`

### Atributos Públicos

| Atributo | Tipo | Descripción |
|----------|------|-------------|
| `posicionSeteada` | `bool` | Indica si se ha establecido una posición en el tablero |
| `ready` | `bool` | Indica si el motor está listo para recibir comandos |
| `tablero` | `Tablero*` | Puntero al objeto Tablero que representa el estado del juego |
| `motor` | `Motor*` | Puntero al motor de búsqueda que calcula las mejores jugadas |

---

## Métodos

### Constructor: `ProtocoloUCI()`

Constructor por defecto. Inicializa el objeto sin establecer ningún estado interno particular.

```cpp
ProtocoloUCI();
```

---

### `comunicacionUCI()`

**Propósito:** Bucle principal de comunicación que escucha y procesa comandos UCI desde stdin.

**Comandos soportados:**

| Comando | Acción |
|---------|--------|
| `uci` | Inicializa el juego y responde con información del motor |
| `isready` | Responde con `readyok` |
| `ucinewgame` | Inicializa un nuevo juego |
| `position` | Establece la posición en el tablero |
| `go` | Inicia la búsqueda de la mejor jugada |
| `quit` | Finaliza el programa liberando memoria |

**Flujo:**
1. Lee líneas de entrada estándar en un bucle infinito
2. Identifica el comando y redirige al método correspondiente
3. En `quit`, libera memoria del motor, tabla de transposición y tablero

---

### `inputUCINewGame()`

**Propósito:** Inicializa un nuevo juego creando nuevas instancias de Motor y Tablero.

**Acciones:**
1. Crea un nuevo objeto `Motor` y lo asigna a `this->motor`
2. Crea un nuevo objeto `Tablero` y lo asigna a `this->tablero`
3. Asigna el tablero a la tabla de transposición del motor
4. Marca `tablaInicializada` del motor como true
5. Resetea `posicionSeteada` a false

---

### `inputPosition(std::string input)`

**Propósito:** Procesa el comando UCI `position` para establecer la posición del tablero.

**Comando UCI:** `position [fen <fenstring> | startpos] moves <move1> ... <moveN>`

**Comportamiento:**
- Llama a `tablero->actualizarTablero(input)` para procesar el string de posición
- Asigna el tablero a la tabla de transposición del motor
- Marca `posicionSeteada` como true

**Nota:** Existe código comentado que manejaba parseo manual de FEN y jugadas históricas. La implementación actual delega en `actualizarTablero()`.

---

### `inputGo(string input)`

**Propósito:** Procesa el comando UCI `go` para iniciar la búsqueda de la mejor jugada.

**Parámetros de tiempo extraídos:**
- `wtime` / `btime`: Tiempo restante para blancas/negras (en ms)
- `winc` / `binc`: Incremento por jugada para blancas/negras (en ms)

**Fases del algoritmo:**

#### 1. Cálculo de tiempo disponible
- Determina tiempo e incremento según el bando actual
- Si el incremento es mayor a 0 y el tiempo remaining <= 5000ms, usa 75% del incremento
- Calcula tiempo por jugadas: `tiempo / 20 + incremento / 2`

#### 2. Búsqueda iterativa (profundidad incremental)
- Itera profundidades de 1 a 49
- Por cada profundidad:
  - Llama a `motor->negamax(tablero, profundidad, ...)` para buscar
  - Calcula Effective Branching Factor (EBF) para debugging
  - Verifica si la mejor jugadas se mantiene estable por múltiples niveles (ply)
  - Controla si se alcanzó el tiempo límite

#### 3. Criterio de parada temprana
- **Etapa de finales** (≤6 piezas materiales): estable por 12 jugadas
- **Etapa media**: estable por 5 jugadas
- Además requiere que la evaluación haya mejorado significativamente (eval - maxEval2 >= 100)

#### 4. Formateo de la jugadas
- Utiliza `formatearJugada()` para convertir el código interno a notación UCI (ej: "e2e4")
- Maneja especialmente las promociones calculando la posición de origen correcta

#### 5. Actualización post-búsqueda
- Ejecuta la jugadas en el tablero interno
- Imprime el tablero actualizado
- Registra la posición en la tabla de repeticiones
- Limpia los movimientos generados
- Detecta condiciones de victoria (jaque mate)

**Salida:**
- `bestmove <jugada>` - La mejor jugadas encontrada
- `profundidad alcanzada: X` - Profundidad final de búsqueda
- `cantidad de nodos buscados: X` - Nodos explorados

---

### `formatearJugada(u_short jugada)`

**Propósito:** Convierte una jugadas codificada internamente a notación UCI estándar.

**Parámetros:**
- `jugada`: Código de 16 bits que contiene tipo, posición origen y destino

**Tipos de jugadas manejadas:**
- **Promociones** (`PROMOTION`, `PROMOTIONCHECK`): Calcula posición origen restando 8 al destino
- **Promociones derecha** (`PROMOTIONDER`, `CPDC`): Calcula restando 7
- **Promociones izquierda** (`PROMOTIONIZQ`, `CPIC`): Calcula restando 9
- **Jugadas normales**: Extrae directamente origen y destino

**Retorna:** String en formato UCI (ej: "e7e8q" para promoción a dama)

---

### `generarJugadaString(int piezaAPromover, int salida, int llegada)`

**Propósito:** Genera el string de promoción con la notación de la pieza promocionada.

**Piezas soportadas:**
| Código | Letera UCI | Pieza |
|--------|------------|-------|
| `DAMA` | `q` | Dama (Queen) |
| `TORRE` | `r` | Torre (Rook) |
| `ALFIL` | `b` | Alfil (Bishop) |
| `CABALLO` | `n` | Caballo (Knight) |

**Retorna:** String en formato "origen-destino+pieza" (ej: "e7e8q")

---

### `inputSetOption(string inputString)`

**Propósito:** Procesa el comando UCI `setoption`.

**Estado actual:** Método vacío (sin implementación). Reserved para opciones futuras del motor como:
- Hash size
- Nivel de dificultad
- Número de threads

---

## Formato de Jugadas Internas

El motor utiliza un sistema de codificación de jugadas basado en bits:

```
[XXXX XXXX XXXX XXXX] (16 bits)
|-----------|---|---|
    Tipo    |dst |src|
```

- **Tipo de jugadas**: Identifica si es normal, captura, promoción, enroque, etc.
- **src (origen)**: Casilla de origen (0-63)
- **dst (destino)**: Casilla de destino (0-63)

Para decodificar se utilizan funciones de `operaciones_bit`:
- `getTipoDeJugada(jugada)`
- `getSalida(jugada)`
- `getLlegada(jugada)`

---

## Constantes Utilizadas

Definidas en `constantes.h`:
- `NumeroACasilla[]`: Array de mapeo de índices 0-63 a coordenadas algebraicas ("a1", "h8", etc.)
- Tipos de jugadas: `PROMOTION`, `PROMOTIONCHECK`, `PROMOTIONDER`, `CPDC`, `PROMOTIONIZQ`, `CPIC`
- Piezas: `DAMA`, `TORRE`, `ALFIL`, `CABALLO`

---

## Dependencias Externas

| Módulo | Función |
|--------|---------|
| `Tablero` | `actualizarTablero()`, `moverPieza()`, `imprimirTablero()`, `_turno`, `zobrist`, etc. |
| `Motor` | `negamax()`, `bestMove`, `tiempoDisponible`, `nodos`, etc. |
| `operaciones_bit` | `getTipoDeJugada()`, `getSalida()`, `getLlegada()` |
| `constantes` | `NumeroACasilla`, constantes de piezas y tipos de jugadas |

---

## Notas de Implementación

1. **Gestión de memoria**: El destructor del comando `quit` libera explícitamente `motor->TT`, `motor` y `tablero` con `delete`.

2. **Búsqueda incremental**: El motor busca en profundidades crecientes (1-50) hasta agotar el tiempo o encontrar una jugadas estable.

3. **Control de tiempo adaptativo**: Utiliza heurísticas para calcular el tiempo por jugadas y detener la búsqueda prematuramente si la mejor jugadas es consistente.

4. **Tabla de repeticiones**: Mantiene un historial de posiciones Zobrist para detectar repeticiones (utilizado para evitar loops infinitos).

5. **Detección de final de juego**: Al buscar, verifica si hayMaterial suficiente para continuar. Con ≤6 piezas considera que es final de partida.

---

## Análisis Técnico para Optimización

### Métricas de Rendimiento Recopiladas

El módulo收集a las siguientes métricas durante la búsqueda:

| Métrica | Variable | Descripción |
|---------|----------|-------------|
| Nodos explorados | `motor->nodos` | Contador global de nodos visitados |
| Nodos por iteración | `motor->nodosBusqueda` | Nodos de la profundidad actual |
| Profundidad alcanzada | `prof` (local) | Profundidad final de la búsqueda |
| EBF | `motor->nodos / nodosIteracionAnterior` | Effective Branching Factor |
| Tiempo transcurrido | `tiempoTranscurrido` | Milisegundos desde inicio de búsqueda |
| Éxitos Null Move | `motor->exitosNull` | Contador de cortocircuitos exitosos |

**Nota:** El EBF se calcula solo desde profundidad 2 en adelante (línea 285-288).

---

### Gestión de Tiempo: Detalle Técnico

#### Extracción de Parámetros UCI

```cpp
// Líneas 244-258: Parseo de tiempos UCI
if (tablero->_turno == 0) {  // Blanco
    wt = stod(input.substr(pos + 6));   // wtime
    winc = stod(input.substr(pos + 5)); // winc
} else {  // Negro
    bt = stod(input.substr(pos + 6));   // btime
    binc = stod(input.substr(pos + 5)); // binc
}
```

**Observaciones de optimización:**
- Usa `stod()` (double) para parsing, posible overhead vs `stoll()` (integer)
- No hay validación de rango de los valores extraídos
- El parsing es O(n) donde n = longitud del string input

#### Cálculo de Tiempo Disponible

```cpp
// Líneas 264-269
if (incremento > 0 && (tiempo <= 5000)){
    tiempo = (incremento*75)/100;  // Ajuste si tiempo bajo con incremento
}
motor->tiempoDisponible = tiempo / 20 + incremento / 2;
```

**Fórmula actual:**
- **Tiempo base**: `tiempo / 20` (asume 20 jugadas restantes)
- **Con incremento**: `+ incremento / 2` (usa la mitad del incremento)

**Posibles optimizaciones:**
- Hardcodear 20 jugadas restantes es una heurística rígida
- No considera fase de la partida (aperturas tienen más jugadas restantes)
- No diferencia entre time control sudden death vs Fischer clock

---

### Bucle de Iterative Deepening: Análisis

#### Estructura del Bucle (líneas 279-346)

```
while (i < 50) {
    1. Reset de contadores por profundidad
    2. Llamada a negamax()
    3. Cálculo de esFinal (material <= 6)
    4. Verificación de estabilidad (plySeguidosComoMejorJugada)
    5. Verificación de tiempo
    6. Incremento de profundidad
}
```

#### Costos por Iteración

| Operación | Complejidad | Notas |
|-----------|-------------|-------|
| `negamax()` | Variable (típicamente O(b^d)) | b≈30, d=profundidad |
| `formatearJugada()` | O(1) | Bit operations |
| Cálculo EBF | O(1) | Solo debug |
| Verificación tiempo | O(1) | chrono overhead |
| Detección final | O(1) | Contadores de piezas |

#### Variables de Estabilidad (líneas 305-321)

```cpp
int plySegunEtapa = esFinal ? 12 : 5;

if (i >= 5) {
    if (mejorJugadaTemporal == motor->bestMove) {
        plySeguidosComoMejorJugada++;  // Comparación u_short
    } else {
        plySeguidosComoMejorJugada = 0;
    }
    if ((plySeguidosComoMejorJugada == plySegunEtapa) && (eval - motor->maxEval2 >= 100)) {
        romper = true;
    }
}
```

**Características actuales:**
- **Threshold etapas**: 5 ply (medio juego) vs 12 ply (final)
- **Mejora mínima requerida**: 100 centipawns
- **Inicio de verificación**: Solo desde profundidad 5

**Oportunidades de optimización:**
- La comparación `mejorJugadaTemporal == motor->bestMove` ya fue optimizada a comparación directa de `u_short` (antes usaba `formatearJugada()` generando strings)
- `eval - motor->maxEval2 >= 100` es unmagic number que podría parametrizarse

---

### Asignación de Memoria

#### En `inputUCINewGame()` (líneas 127-139)

```cpp
motor = new Motor();
tablero = new Tablero();
motor->TT->asignarTablero(tablero);
```

**Patrón actual:**
- `new` para Motor y Tablero en cada `ucinewgame`
- **Memory leak potencial**: No hay `delete` de las instancias anteriores
- La tabla de transposición (`motor->TT`) se asigna dentro del constructor de Motor

#### En `comunicacionUCI()` con `quit` (líneas 113-118)

```cpp
delete motor->TT;
delete motor;
delete tablero;
exit(0);
```

**Observaciones:**
- El `exit(0)` termina el programa abruptamente
- No hay destructor de ProtocoloUCI que limpie recursos

---

### Tabla de Repeticiones

#### Actualización post-búsqueda (líneas 421-426)

```cpp
motor->index_repeticion = tablero->contadorJugadas;
U64 claveActual = tablero->zobrist;
motor->tabla_de_repeticiones[motor->index_repeticion] = claveActual;
for(int i = 0; i < 256; i++){
    tablero->cantMovesGenerados[i] = -1;
}
```

**Costos:**
- Limpieza de `cantMovesGenerados`: O(256) = O(1) constante
- Array `tabla_de_repeticiones`: Tamaño fijo (probablemente 256)

---

### Variables de Estado Relevantes

| Variable | Tipo | Uso | ¿Constante? |
|----------|------|-----|-------------|
| `i` | int | Profundidad actual | No |
| `mejorJugadaTemporal` | u_short | Mejor move iteración anterior | No |
| `plySeguidosComoMejorJugada` | int | Contador de estabilidad | No |
| `prof` | int | Profundidad final | No |
| `softLimit` / `hardLimit` | long long | Límites de tiempo (propuesto) | No |

---

###潜在 Bottlenecks (Puntos de Congestión)

1. **String parsing en `inputGo()`**
   - `input.find()` llamado múltiples veces
   - `stod()` para conversión de strings
   - Potencial: caching de parámetros si el mismo input se procesa múltiples veces

2. **Cálculo de EBF por iteración**
   - `cout << "EBF: " << ... << endl;` en línea 287
   - I/O bloqueante en cada profundidad
   - **Recomendación**: Remover o condicionalizar para producción

3. **Detección de final de juego**
   - Cálculo de piezas en cada iteración (líneas 300-303)
   - No hay cache del resultado entre iteraciones
   - El material solo cambia cuando se ejecuta una jugadas

4. **Limpieza de cantMovesGenerados**
   - Siempre ejecuta 256 iteraciones sin verificar si es necesario

5. **Impresión del tablero post-jugada**
   - `tablero->imprimirTablero()` en línea 408
   - I/O costoso que retrasa la respuesta UCI

---

### Recomendaciones de Optimización para Futuros Agentes

1. **Time Manager profesional**: Implementar límites suaves y duros como se describió en los requisitos técnicos

2. **Parametrización de umbrales**:
   - Extraer "100" (mejora mínima) a constante
   - Extraer "5" y "12" (ply de estabilidad) a configuración

3. **Remover debug output**:
   - EBF print en producción
   - `exitosNull` print post-búsqueda
   - Tablero post-jugada

4. **Optimización de memoria**:
   - Pool de objetos Motor/Tablero en vez de `new` por juego
   - Considerar arena allocation

5. **Cache de cálculos**:
   - Material total (esFinal) entre iteraciones
   - Parámetros de tiempo calculados una sola vez
