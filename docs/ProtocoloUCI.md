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
