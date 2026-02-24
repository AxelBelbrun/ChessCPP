# Módulo de Código Auxiliar

## Descripción General

**Archivo**: `codigo_auxiliar.cpp` / `codigo_auxiliar.h`  
**Propósito**: Este módulo implementa funciones auxiliares para la optimización de parámetros de la función de evaluación del motor de ajedrez mediante técnicas de aprendizaje automático (Stochastic Gradient Descent).

> **Nota**: Este código fue utilizado para el proceso de optimización de parámetros de la función de evaluación.

ales

| Variable---

## Variables Glob | Tipo | Descripción |
|----------|------|-------------|
| `velocidades` | `std::vector<double>(433)` | Vector de 433 elementos inicializado en 0.0 |
| `velocity` | `double` | Almacena una velocidad (valor temporal) |
| `contador` | `int` | Contador para numerar archivos de parámetros guardados |

---

## Clase: `codigo_auxiliar`

### Métodos Públicos

#### 1. `meanSquareError(double k)`

**Propósito**: Calcula el error cuadrático medio (MSE) entre los resultados del motor y los resultados esperados.

**Parámetros**:
- `k`: Factor de escala para la función sigmoide

**Flujo**:
1. Lee posiciones desde archivo `/home/axel/Documentos/posiciones-ML-Motor.txt`
2. Para cada posición:
   - Extrae el resultado esperado del archivo FEN
   - Configura el tablero con la posición
   - Ejecuta `m->quiescence()` para obtener el score del motor
   - Aplica sigmoide: `1 / (1 + exp(-k * score))`
   - Acumula el error usando cross-entropy
3. Retorna el error promedio

**Dependencias externas**: `Motor`, `Tablero`

---

#### 2. `actualizarParametros(std::vector<double>& gradientes, double rate, double epsilon)`

**Propósito**: Actualiza los parámetros de evaluación según los gradientes calculados.

**Parámetros**:
- `gradientes`: Vector de gradientes (tamaño 432)
- `rate`: Learning rate para el actualización
- `epsilon`: Valor pequeño para evitar division por cero

**Estado**: **Código comentado** - La función contiene lógica comentada que actualizaba:
- Tablas de ocupación por tipo de pieza (peón, caballo, alfil, torre, reina, rey)
- Parámetros adicionales (premio por enrocar, castigo por sacar dama, etc.)

**Actualizado por**: No disponible actualmente

---

#### 3. `guardarParametros()`

**Propósito**: Persiste los parámetros actuales de evaluación en un archivo de texto.

**Flujo**:
1. Incrementa el contador de iteraciones
2. Abre archivo `/home/axel/Documentos/parametros/iteracion{N}.txt`
3. Escribe en orden:
   - Occupación peón blanco (64 valores, excluyendo filas 0 y 7)
   - Occupación caballo blanco (64 valores)
   - Occupación alfil blanco (64 valores)
   - Occupación torre blanca (64 valores)
   - Occupación reina blanca (64 valores)
   - Occupación rey blanco medio juego (64 valores)
   - Occupación rey blanco final (64 valores)

**Dependencias externas**: `constantes` (ocupacionPeon, ocupacionCaballo, etc.)

---

#### 4. `obtenerParametros()`

**Propósito**: Recolecta los parámetros actuales de evaluación en un vector.

**Retorna**: `std::vector<double>` con 432 valores:
- Índices 0-47: Ocupación peón (excluyendo casillas iniciales)
- Índices 48-111: Ocupación caballo
- Índices 112-175: Ocupación alfil
- Índices 176-239: Ocupación torre
- Índices 240-303: Ocupación reina
- Índices 304-367: Ocupación rey medio juego
- Índices 368-431: Ocupación rey final

**Dependencias externas**: `constantes`

---

#### 5. `calcularGradiente(std::string fen, double epsilon)`

**Propósito**: Calcula el gradiente de la función de pérdida respecto a cada parámetro.

**Parámetros**:
- `fen`: Notación FEN de la posición
- `epsilon`: Tamaño del paso para diferencias finitas

**Estado**: **Código mayormente comentado** - La función contiene lógica comentada que:
1. Obtiene parámetros actuales
2. Itera sobre cada parámetro, lo perturba con epsilon
3. Calcula diferencia finita
4. Guarda gradiente

**Dependencias externas**: `obtenerParametros()`, `Motor`, `Tablero`, `constantes`

---

#### 6. `SGD(int epochs, double learningRate)`

**Propósito**: Implementa Stochastic Gradient Descent para optimizar los parámetros.

**Parámetros**:
- `epochs`: Número de iteraciones de entrenamiento
- `learningRate`: Tasa de aprendizaje

**Flujo**:
1. Lee posiciones desde archivo `posiciones-ML-Motor.txt`
2. Crea generador de números aleatorios
3. Para cada época:
   - Selecciona posición aleatoria
   - Extrae FEN y resultado esperado
   - Obtiene score del motor mediante `quiescence()`
   - Aplica sigmoide al score
   - Calcula gradiente basado en la diferencia (scorePropio - score)
   - Maneja espejado de casillas para piezas negras
4. Promedia gradientes por épocas
5. Retorna vector de 432 gradientes

**Cálculo de gradiente** (para cada parámetro i):
- Determina la casilla corresponding al índice i
- Obtiene el tipo de pieza en esa casilla
- Calcula: `gradientes[i] += (scorePropio - score) * hayPieza`

**Dependencias externas**: `Motor`, `Tablero`, `operaciones_bit::espejarCasilla()`, `t.obtenerTipoDePieza()`, `t.piezas_negras()`

---

#### 7. `gradientDescent(int epochs, double learningRate)`

**Declarada en header** pero **no implementada** en el archivo `.cpp`.

---

## Estructura de Parámetros (432 elementos)

| Rango Índices | Tipo de Pieza | Descripción |
|---------------|---------------|-------------|
| 0-47 | Peón | Tablero 8x8 sin filas 0 y 7 (48 casillas) |
| 48-111 | Caballo | Tablero completo (64 casillas) |
| 112-175 | Alfil | Tablero completo (64 casillas) |
| 176-239 | Torre | Tablero completo (64 casillas) |
| 240-303 | Reina | Tablero completo (64 casillas) |
| 304-367 | Rey (MG) | Tablero completo - medio juego (64 casillas) |
| 368-431 | Rey (EG) | Tablero completo - final (64 casillas) |

---

## Notas para Refactoring

1. **Código muerto**: Gran cantidad de lógica comentada que debería eliminarse o restaurarse según necesidad
2. **Rutas hardcodeadas**: 
   - `/home/axel/Documentos/posiciones-ML-Motor.txt`
   - `/home/axel/Documentos/parametros/`
3. **Falta implementación**: `gradientDescent()` no está implementada
4. ** memory**: En `SGD()` se usa `new Motor()` sin smart pointer (línea 410), debe usar `std::make_unique`
5. **Parámetro no usado**: El argumento `learningRate` en `SGD()` no se utiliza actualmente
