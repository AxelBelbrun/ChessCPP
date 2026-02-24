# Documentación del Motor de Ajedrez

## Descripción General

El archivo `Motor.cpp` contiene la implementación del motor de búsqueda y evaluación de ajedrez. Esta clase encapsula toda la lógica de:
- **Evaluación estática** de posiciones
- **Búsqueda de movimientos** usando el algoritmo Negamax con podas
- **Quiescence search** para evitar el problema del horizonte
- **Tablas de ocupación** (positional tables) para evaluación posicional
- **Gestión de la tabla de transposición**

---

## Constructor

### Motor()
Inicializa el motor creando una nueva tabla de transposición y configurando el estado inicial.

```cpp
Motor();
```

**Comportamiento:**
- Instancia una nueva `Tabla_transposicion`
- In = -1`
- Marca `tablaInicializa `plyicializada = false`
- Inicializa la matriz de `killerMove` con valor -1

---

## Métodos de Evaluación

### float valoracion(Tablero* tablero)
Función principal de evaluación que retorna un valor numérico representando la ventaja de la posición.

```cpp
float valoracion(Tablero* tablero);
```

**Componentes evaluados:**
| Componente | Descripción |
|------------|-------------|
| Material | Suma del valor de todas las piezas (diferencia blancas - negras) |
| Peones bloqueados | Penalización por peones bloqueados en d4/d5/e4/e5 |
| Enroque | Premio por haber enrocado |
| Seguridad del rey | Evaluación del escudo de peones alrededor del rey |
| Movilidad | Número de movimientos legales disponibles |
| Peones pasados | Premio por peones que no pueden ser bloqueados |

**Características especiales:**
- Calcula dos valores: evaluación de medio juego y final
- Interpola entre ambos según la fase del juego (calculada por `calcularFaseDeJuego`)
- Ajusta el signo según el bando a mover (siempre retorna ventaja para blancas)

---

### float valoracionMovilidad(Tablero* tablero)
Evalúa la movilidad simple del bando actual.

```cpp
float valoracionMovilidad(Tablero* tablero);
```

**Retorna:** Cantidad de movimientos legales (positiva para blancas, negativa para negras)

---

### float seguridadDelRey(Tablero* tablero)
Evalúa la seguridad del rey calculando el escudo de peones.

```cpp
float seguridadDelRey(Tablero* tablero);
```

**Lógica:**
- Para rey en esquina flanco rey (casillas e1/g1): evalúa peones en f2/g2/h2
- Para rey en esquina flanco dama (casillas a1/c1): evalúa peones en a2/b2/c2
- Lo mismo para negras en lascasillas superiores
- **Retorna:** Premio positivo para blancas, negativo para negras

---

### float contarMaterialSinPeones(Tablero* tablero)
Cuenta el valor material de todas las piezas excepto peones.

```cpp
float contarMaterialSinPeones(Tablero* tablero);
```

**Nota:** Itera sobre los bitboards de reyes, damas, torres, alfiles y caballos.

---

### int calcularFaseDeJuego(Tablero* tablero)
Determina en qué fase del juego se encuentra la partida.

```cpp
int calcularFaseDeJuego(Tablero* tablero);
```

**Cálculo:**
- Fase máxima = 24 (todas las piezas menores + damas)
- Cada pieza tiene un peso: Caballo=1, Alfil=1, Torre=2, Dama=4
- **Retorna:** Valor 0-256 representando el porcentaje de fase de medio juego

---

## Métodos de Tablas de Ocupación (Positional Tables)

Estas funciones evaluan la posición de cada tipo de pieza usando tablas predefinidas.

### static float calcularOcupacion(Tablero* tablero, int color)
Función genérica de ocupación (actualmente no utilizada).

```cpp
static float calcularOcupacion(Tablero* tablero, int color);
```

---

### static float calcularOcupacionPeon(Tablero* tablero, int color)
Evalúa la posición de los peones usando tabla de ocupación de peón.

---

### static float calcularOcupacionCaballo(Tablero* tablero, int color)
Evalúa la posición de los caballos usando tabla de ocupación.

---

### static float calcularOcupacionAlfil(Tablero* tablero, int color)
Evalúa la posición de los alfiles usando tabla de ocupación.

---

### static float calcularOcupacionTorre(Tablero* tablero, int color)
Evalúa la posición de las torres usando tabla de ocupación.

---

### static float calcularOcupacionReina(Tablero* tablero, int color)
Evalúa la posición de las damas usando tabla de ocupación.

---

### static float calcularOcupacionRey(Tablero* tablero, int color)
Evalúa la posición del rey. **Nota:** Usa tablas diferentes según si es endgame o medio juego (variable `endgame`).

---

## Métodos de Búsqueda

### float negamax(Tablero* tablero, int profundidad, float alfa, float beta, bool esRaiz)
Implementación principal del algoritmo de búsqueda Negamax con múltiples optimizaciones.

```cpp
float negamax(Tablero* tablero, int profundidad, float alfa, float beta, bool esRaiz);
```

**Parámetros:**
- `tablero`: Posición actual
- `profundidad`: Profundidad de búsqueda restante
- `alfa`: Mejor valor asegurable para el bando maximizador
- `beta`: Valor que causa poda (límite del bando minimizador)
- `esRaiz`: Indica si es la llamada raíz

**Optimizaciones implementadas:**
1. **Tabla de Transposición**: Consulta y almacenamiento de posiciones previamente evaluadas
2. **Null Move Pruning**: Reduce profundidad cuando la posición es claramente buena
3. **Killer Moves**: Mantiene movimientos quietos que causan betas
4. **Late Move Reduction (LMR)**: Reduce profundidad de movimientos tardíos poco prometedores
5. **Futility Pruning**: poda movimientos estáticos en profundidad 1
6. **Ordenamiento de movimientos**: Prioriza mejores movimientos antes de buscar

**Retorna:** Mejor valor encontrado para la posición

---

### float quiescence(Tablero* tablero, float alfa, float beta)
Búsqueda de quiescencia para evitar el problema del horizonte.

```cpp
float quiescence(Tablero* tablero, float alfa, float beta);
```

**Comportamiento:**
- Solo considera capturas (y promociones)
- Ordena capturas por SEE (Static Exchange Evaluation)
- Evita que el motor faça movimientos catastrophicos más allá del horizonte de búsqueda

---

### U64 perft(int profundidad, Tablero* tablero)
Función de verificación y depuración que cuenta todos los nodos leaf.

```cpp
U64 perft(int profundidad, Tablero* tablero);
```

**Uso típico:** Verificar que la generación de movimientos es correcta

---

## Métodos Auxiliares

### bool esRepeticion(U64 zobristActual)
Verifica si la posición actual ha ocurrido tres veces (regla de triple repetición).

```cpp
bool esRepeticion(U64 zobristActual);
```

---

### float calcularDesarrollo(Tablero* tablero, int color)
Evalúa el desarrollo de las piezas menores en la apertura.

```cpp
float calcularDesarrollo(Tablero* tablero, int color);
```

**Premio:** Piezas menores que han abandonado su posición inicial

---

### int porcentajeTabla()
Retorna el porcentaje de ocupación de la tabla de transposición.

```cpp
int porcentajeTabla();
```

---

## Variables Miembro Relevantes

### De búsqueda
| Variable | Tipo | Descripción |
|----------|------|-------------|
| `bestMove` | u_short | Mejor movimiento encontrado |
| `killerMove[2][128]` | u_short | Matriz de killer moves por ply |
| `TT` | Tabla_transposicion* | Tabla de transposición |
| `nodos` | int | Nodos explorados |
| `hashHits` | int | Hits en tabla de transposición |
| `stopSearch` | bool | Flag para detener búsqueda |
| `tiempoDisponible` | double | Tiempo máximo de búsqueda (ms) |

### De evaluación
| Variable | Tipo | Descripción |
|----------|------|-------------|
| `endgame` | static bool | Flag de endgame |
| `pesoMaterialPeonM/F` | float | Pesos de material peón medio/final |
| `pesoOcupacion*` | float | Pesos de tablas de posición |
| `premioEnroqueM/F` | float | Premio por enroque |
| `castigoPeonBloqueado*` | float | Castigo por peón bloqueado |
| `seguridadDelRey*` | float | Peso de seguridad del rey |
| `movilidad*` | float | Peso de movilidad |
| `peonesPasados*` | float | Peso de peones pasados |

### De repetición
| Variable | Tipo | Descripción |
|----------|------|-------------|
| `tabla_de_repeticiones[512]` | U64 | Historial de posiciones |
| `index_repeticion` | int | Índice actual en tabla de repeticiones |

---

## Constantes de Fase del Juego

Definidas en `Motor.h`:

```cpp
const int peso_fase_caballo = 1;
const int peso_fase_alfil = 1;
const int peso_fase_torre = 2;
const int peso_fase_dama = 4;
const int fase_maxima = 24;       // Por bando
const int fase_maxima_total = 48; // Ambos bandos
```

---

## Notas para Refactoring

1. **Código muerto**: Existen múltiples secciones comentadas que podrían eliminarse o refactorizarse
2. **Duplicación**: Las funciones de calcularOcupacion* tienen estructura similar - podrían refactorizarse
3. **Magic numbers**: Hay varios números mágicos (ej. casillas hardcodeadas para peones bloqueados)
4. **Separación de responsabilidades**: La evaluación podría separarse en una clase dedicada
5. **Parámetros hardcodeados**: Los pesos de evaluación podrían externalizarse a un archivo de configuración
