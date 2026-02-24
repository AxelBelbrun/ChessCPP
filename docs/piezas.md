# Documentación del Sistema de Piezas de Ajedrez

## 1. Visión General

Este documento describe el sistema de piezas implementado para el motor de ajedrez ChessCPP. El código utiliza **bitboards** (representación mediante enteros de 64 bits) para representar las casillas del tablero y las posiciones de las piezas.

### 1.1 Convenciones del Tablero

- El tablero se representa con 64 casillas numeradas del 1 al 64
- Las casillas se representan mediante bits en un `U64` (unsigned long long)
- El turno se representa como `int`: 0 para blancas, 1 para negras
- Las piezas propias y rivales se pasan como parámetros separados a las funciones de generación de movimientos

### 1.2 Representación de Piezas

Cada tipo de pieza hereda de una clase base y tiene su propia implementación para generar movimientos legales.

---

## 2. Clase Base: Pieza

**Archivo:** `include/Pieza.h`

### Descripción

Interfaz abstracta que define la estructura común para todas las piezas del juego.

### Métodos Virtuales Puros

```cpp
virtual U64 generar_movimientos_legales(U64 piezas, U64 piezasPropias, U64 piezasRivales, int turno) = 0;
```

- **Parámetros:**
  - `piezas`: Bitboard con la posición de la pieza(s) que debe mover
  - `piezasPropias`: Bitboard con todas las piezas del bando que mueve
  - `piezasRivales`: Bitboard con todas las piezas del bando oponente
  - `turno`: 0 para blancas, 1 para negras

- **Retorna:** Bitboard con todas las casillas destino legales para la pieza

### Notas

- La clase es abstracta (no se puede instanciar directamente)
- Proporciona el destructor virtual por defecto

---

## 3. Clase Base: slidingPiece

**Archivos:** 
- `include/slidingPiece.h`
- `src/slidingPiece.cpp`

### Descripción

Clase abstracta que hereda de `Pieza`. Sirve como base para las piezas "deslizantes" (Torre, Alfil, Dama), que se mueven en línea recta a través del tablero.

### Métodos Virtuales Puros

```cpp
virtual U64 generar_attack_mask(U64 bitboard_pieza) = 0;
```

- **Propósito:** Genera una máscara de ataque en un tablero vacío (sin bloquear por otras piezas)
- **Utilidad:** Usado principalmente para algoritmos de generación de movimientos con tablas de ataque precomputadas

### Jerarquía de Herencia

```
Pieza (abstracta)
    |
    +-- slidingPiece (abstracta)
    |       |
    |       +-- Torre
    |       +-- Alfil
    |       +-- Dama (combina Torre + Alfil)
    |
    +-- Caballo
    +-- Rey
    +-- Peon
```

---

## 4. Peón (Peon)

**Archivos:**
- `include/Peon.h`
- `src/Peon.cpp`

### Descripción

El peón tiene el movimiento más complejo debido a sus reglas especiales:
- Movimiento hacia adelante (dirección取决于 del color)
- Captura diagonal
- Movimiento doble desde posición inicial
- Captura al paso (en passant)
- Promoción al llegar al extremo del tablero

### Constructores y Métodos

#### Constructor
```cpp
Peon();
```

#### Métodos de Generación de Movimientos

**`movimientos_legales`**
```cpp
void movimientos_legales(Tablero* tablero, int ply, U64 peones, U64 piezasPropias, U64 piezasRivales);
```
Genera todos los movimientos legales para los peones dados y los almacena en el tablero.

**`generar_movimientos_legales`**
```cpp
U64 generar_movimientos_legales(U64 peon, U64 piezasPropias, U64 piezasRivales, int turno) override;
```
Retorna un bitboard con los movimientos legales para un peón individual.

**`avances`**
```cpp
static U64 avances(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales, int turno);
```
Calcula los movimientos hacia adelante (simples y dobles desde fila inicial).

**`capturas`**
```cpp
static U64 capturas(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales, int turno);
```
Calcula las capturas diagonales posibles.

#### Métodos de Captura al Paso

**`hayCapturaAlPaso`**
```cpp
static int hayCapturaAlPaso(Tablero* tablero, U64 peonBitboard, int turno);
```
Determina si existe la posibilidad de realizar una captura al paso. Retorna la casilla de captura o 0 si no es posible.

**`agregarCapturasAlPaso`**
```cpp
static void agregarCapturasAlPaso(Tablero* tablero, int ply, U64 peones, int turno);
```
Agrega las jugadas de captura al paso al listado de movimientos.

#### Métodos de Promoción

**`agregarPromociones`**
```cpp
static void agregarPromociones(Tablero* tablero, int ply, U64 bitboardMovimientos);
```
Maneja promociones cuando el peón avanza hasta la última fila.

**`agregarPromIzq`** / **`agregarPromDer`**
```cpp
static void agregarPromIzq(Tablero* tablero, int ply, U64 bitboardMovimientos);
static void agregarPromDer(Tablero* tablero, int ply, U64 bitboardMovimientos);
```
Manejan promociones por captura diagonal (izquierda/derecha).

#### Métodos Auxiliares de Agregado

**`agregarAvances`**
```cpp
static void agregarAvances(Tablero* tablero, int ply, U64 bitboardMovimientos, int desplazamiento);
```
Agrega movimientos de avance (quiet moves) al tablero.

**`agregarCapturas`**
```cpp
static void agregarCapturas(Tablero* tablero, int ply, U64 bitboardMovimientos, int desplazamiento);
```
Agrega movimientos de captura al tablero.

### Lógica de Movimiento

#### Blancas (turno = 0)
- **Avance simple:** `peon << 8` (una fila hacia adelante)
- **Avance doble:** Desde fila 2, `peon << 16` (dos filas)
- **Captura izquierda:** `peon << 9` (diagonal superior derecha)
- **Captura derecha:** `peon << 7` (diagonal superior izquierda)
- **Promoción:** Llega a fila 8

#### Negras (turno = 1)
- **Avance simple:** `peon >> 8` (una fila hacia atrás)
- **Avance doble:** Desde fila 7, `peon >> 16` (dos filas)
- **Captura izquierda:** `peon >> 9` (diagonal inferior izquierda)
- **Captura derecha:** `peon >> 7` (diagonal inferior derecha)
- **Promoción:** Llega a fila 1

### Representación de Jugadas de Peón

| Tipo de Jugada | Código | Descripción |
|---------------|--------|-------------|
| QUIET | 0 | Avance simple |
| CAPTURE | 7 | Captura diagonal |
| PROMOTION | 5 | Promoción sin captura |
| PROMOTIONIZQ | 4 | Captura + promoción a izquierda |
| PROMOTIONDER | 3 | Captura + promoción a derecha |
| ENPASSANT | 1 | Captura al paso |
| ENPASSANTCHECK | 14 | Captura al paso que da jaque |

---

## 5. Torre (Torre)

**Archivos:**
- `include/Torre.h`
- `src/Torre.cpp`

### Descripción

Hereda de `slidingPiece`. Se mueve en líneas rectas horizontales y verticales.

### Constructores y Métodos

#### Constructor
```cpp
Torre();
```

#### Métodos Principales

**`generar_attack_mask`**
```cpp
U64 generar_attack_mask(U64 torreBitboard) override;
```
Retorna máscara de ataque en tablero vacío.

**`generar_movimientos_legales`**
```cpp
U64 generar_movimientos_legales(U64 torres, U64 piezasPropias, U64 piezasRivales, int turno) override;
```
Genera movimientos legales considerando bloqueo por piezas propias y capturas a piezas rivales.

#### Métodos Privados de Generación de Movimientos

**`movimientos_arriba`**
```cpp
static U64 movimientos_arriba(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
```

**`movimientos_abajo`**
```cpp
static U64 movimientos_abajo(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
```

**`movimientos_izquierda`**
```cpp
static U64 movimientos_izquierda(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
```

**`movimientos_derecha`**
```cpp
static U64 movimientos_derecha(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
```

#### Métodos de Attack Mask

```cpp
static U64 attack_mask_arriba(U64 torre);
static U64 attack_mask_abajo(U64 torre);
static U64 attack_mask_izquierda(U64 torre);
static U64 attack_mask_derecha(U64 torre);
```

### Lógica de Movimiento

1. Itera desde la casilla de salida en la dirección correspondiente
2. Si encuentra una pieza propia: corta el movimiento
3. Si encuentra una pieza rival: agrega la captura y corta
4. Si la casilla está vacía: agrega el movimiento y continúa

---

## 6. Alfil (Alfil)

**Archivos:**
- `include/Alfil.h`
- `src/Alfil.cpp`

### Descripción

Hereda de `slidingPiece`. Se mueve en líneas diagonales.

### Constructores y Métodos

#### Constructor
```cpp
Alfil();
```

#### Métodos Principales

**`generar_attack_mask`**
```cpp
U64 generar_attack_mask(U64 bitboard_alfil) override;
```

**`generar_movimientos_legales`**
```cpp
U64 generar_movimientos_legales(U64 alfil, U64 piezasPropias, U64 piezasRivales, int turno) override;
```

#### Métodos de Movimientos Legales por Dirección

```cpp
static U64 movimientos_legales_nordeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
static U64 movimientos_legales_noroeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
static U64 movimientos_legales_sudeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
static U64 movimientos_legales_sudoeste(int casillaDeSalida, U64 piezasPropias, U64 piezasRivales);
```

#### Métodos de Attack Mask por Dirección

```cpp
static U64 attack_mask_nordeste(U64 alfil);
static U64 attack_mask_noroeste(U64 alfil);
static U64 attack_mask_sudeste(U64 alfil);
static U64 attack_mask_sudoeste(U64 alfil);
```

### Direcciones y Desplazamientos

| Dirección | Desplazamiento |
|-----------|----------------|
| Noreste | +7 |
| Noroeste | +9 |
| Sureste | -9 |
| Sudoeste | -7 |

---

## 7. Caballo (Caballo)

**Archivos:**
- `include/Caballo.h`
- `src/Caballo.cpp`

### Descripción

Hereda directamente de `Pieza`. El caballo se mueve en forma de "L" (2+1 casillas). Es la única pieza que puede saltar sobre otras.

### Constructores y Métodos

#### Constructor
```cpp
Caballo();
```

#### Métodos Principales

**`obtener_movimientos`**
```cpp
void obtener_movimientos(Tablero* tablero, int ply, U64 piezas, U64 piezasPropias, U64 piezasRivales);
```
Genera todos los movimientos de caballo y los almacena en el tablero.

**`generar_movimientos_legales`**
```cpp
U64 generar_movimientos_legales(U64 bitboard_caballo, U64 piezasPropias, U64 piezasRivales, int turno) override;
```
Retorna bitboard con movimientos legales.

**`capturas`**
```cpp
static void capturas(Tablero* tablero, std::vector<u_short>* movimientos, U64 piezas, U64 piezasPropias, U64 piezasRivales);
```
Genera solo movimientos de captura para los caballos.

### Funciones Auxiliares de Movimiento

Implementadas como funciones estáticas anónimas que calculan los 8 posibles movimientos del caballo:

```cpp
static U64 noNoEa(U64 b)  { return (b << 17) & ~HFile; }  // 2 arriba, 1 derecha
static U64 noEaEa(U64 b)  { return (b << 10) & ~HFile & ~GFile; } // 1 arriba, 2 derecha
static U64 soEaEa(U64 b)  { return (b >>  6) & ~HFile & ~GFile; } // 1 abajo, 2 derecha
static U64 soSoEa(U64 b)  { return (b >> 15) & ~HFile; }  // 2 abajo, 1 derecha
static U64 noNoWe(U64 b)  { return (b << 15) & ~AFile; }  // 2 arriba, 1 izquierda
static U64 noWeWe(U64 b)  { return (b <<  6) & ~BFile & ~AFile; } // 1 arriba, 2 izquierda
static U64 soWeWe(U64 b)  { return (b >> 10) & ~BFile & ~AFile; } // 1 abajo, 2 izquierda
static U64 soSoWe(U64 b)  { return (b >> 17) & ~AFile; }  // 2 abajo, 1 izquierda
```

### Representación de Jugadas de Caballo

| Tipo de Jugada | Código | Descripción |
|---------------|--------|-------------|
| QUIET | 0 | Movimiento sin captura |
| CAPTURE | 7 | Captura de pieza rival |

---

## 8. Rey (Rey)

**Archivos:**
- `include/Rey.h`
- `src/Rey.cpp`

### Descripción

Hereda directamente de `Pieza`. El rey se mueve una casilla en cualquier dirección. No tiene implementación completa de enroque en este módulo.

### Constructores y Métodos

#### Constructor
```cpp
Rey();
```

#### Métodos Principales

**`movimientos_legales`**
```cpp
void movimientos_legales(Tablero* tablero, int ply, U64 rey, U64 piezasPropias, U64 piezasRivales);
```
Genera todos los movimientos legales del rey.

**`generar_movimientos_legales`**
```cpp
U64 generar_movimientos_legales(U64 rey, U64 piezasPropias, U64 piezasRivales, int turno) override;
```
Retorna bitboard con los 8 posibles movimientos (attack mask).

**`capturas`**
```cpp
static void capturas(Tablero* tablero, std::vector<u_short>* movimientos, U64 rey, U64 piezasPropias, U64 piezasRivales);
```
Genera solo movimientos de captura del rey.

### Direcciones de Movimiento

El rey puede moverse a las 8 casillas adyacentes:
- Arriba: +8
- Abajo: -8
- Izquierda: +1
- Derecha: -1
- Arriba-derecha: +7
- Arriba-izquierda: +9
- Abajo-derecha: -9
- Abajo-izquierda: -7

### Restricciones

Se verifican los bordes del tablero usando máscaras de archivo (AFile, HFile) y fila (fila1, fila8).

---

## 9. Dama (Dama)

**Archivos:**
- `include/Dama.h`
- `src/Dama.cpp`

### Descripción

Hereda de `slidingPiece`. La dame combina los movimientos de la torre y el alfil (movimiento en línea en cualquier dirección).

### Constructores y Métodos

#### Constructor
```cpp
Dama();
```

#### Métodos Principales

**`generar_movimientos_legales`**
```cpp
U64 generar_movimientos_legales(U64 dama, U64 piezasPropias, U64 piezasRivales, int turno) override;
```
Implementación: combina los movimientos de Torre y Alfil instanciando ambas piezas y disjuntando sus resultados.

**`generar_attack_mask`**
```cpp
U64 generar_attack_mask(U64 bitboard_dama) override;
```
Implementación: combina los attack masks de Torre y Alfil.

### Implementación

```cpp
U64 Dama::generar_movimientos_legales(U64 dama, U64 piezasPropias, U64 piezasRivales, int turno) {
    U64 movimientos = 0L;
    Alfil* alfil = new Alfil();
    Torre* torre = new Torre();
    movimientos = movimientos | alfil->generar_movimientos_legales(dama, piezasPropias, piezasRivales, 0);
    movimientos = movimientos | torre->generar_movimientos_legales(dama, piezasPropias, piezasRivales, 0);
    delete alfil;
    delete torre;
    return movimientos;
}
```

### Nota de Diseño

La Dama reutiliza la lógica de Torre y Alfil en lugar de tener implementación propia, lo cual demuestra composición sobre herencia.

---

## 10. Constantes Utilizadas

**Archivo:** `include/constantes.h`

### Máscaras de Archivos y Filas

| Constante | Descripción |
|-----------|-------------|
| `AFile`, `BFile`, `GFile`, `HFile` | Máscaras de columnas del tablero |
| `fila1` - `fila8` | Máscaras de filas del tablero |
| `fila2`, `fila7` | Filas iniciales de peones (blancas/negras) |
| `fila3` - `fila6` | Filas intermedias |

### Tipos de Jugada (enum)

```cpp
enum tipoDeJugada {
    QUIET,           // 0: Movimiento normal
    ENPASSANT,       // 1: Captura al paso
    CASTLING,        // 2: Enroque
    PROMOTIONDER,    // 3: Promoción capturando a derecha
    PROMOTIONIZQ,    // 4: Promoción capturando a izquierda
    PROMOTION,       // 5: Promoción sin capturar
    CAPTURE,         // 7: Captura normal
    CPIC,            // ?: Captura en passant con jaque
    CPDC,            // ?: Captura derecha con jaque
    PROMOTIONCHECK,  // ?: Promoción que da jaque
    CAPTURECHECK,    // ?: Captura que da jaque
    CHECK,           // ?: Movimiento que da jaque
    CHECKMATE,       // ?: Jaque mate
    ENPASSANTCHECK   // ?: Captura al paso con jaque
};
```

---

## 11. Notas de Diseño y Reflexiones

### 11.1 Patrones de Diseño Observados

1. **Herencia:** Uso de clase abstracta base (`Pieza`) con implementaciones concretas
2. **Composición:** `Dama` usa `Torre` y `Alfil` en lugar de heredar de ambas
3. **Métodos Factory:** No hay factory explícito, las piezas se instancian directamente

### 11.2 Código Muerto / Comentarizado

Existe código commented-out extenso en `Peon.cpp` que parece ser una implementación alternativa más antigua. Esto debería evaluarse durante el refactoring.

### 11.3 Áreas de Mejora Identificadas

1. **Inconsistencia en interfaces:** 
   - Algunas piezas usan `generar_movimientos_legales` que retorna `U64`
   - Otras usan `movimientos_legales` que modifica directamente el `Tablero`
   
2. **Gestión de memoria en Dama:**
   - Crea y destruye objetos `Alfil` y `Torre` en cada llamada
   - Podría usar objetos estáticos o métodos estáticos

3. **Falta de enroque:**
   - No hay implementación de movimientos de enroque en `Rey`
   - El enum tiene `CASTLING` pero no se utiliza

4. **Validación de jaque:**
   - Hay código comentarizado relacionado con validación de jaque
   - La validación parece delegarse a `Tablero::esJaque`

---

## 12. Glosario de Términos

| Término | Definición |
|---------|------------|
| Bitboard | Representación del tablero usando 64 bits |
| LSB | Least Significant Bit - bit menos significativo |
| Attack mask | Casillas que una pieza ataca en tablero vacío |
| Ply | Un medio-movimiento (un turno) |
| Piezas propias | Piezas del bando que tiene el turno |
| Piezas rivales | Piezas del oponente |
| Quiet move | Movimiento sin captura ni jaque |

---

*Documento generado para facilitar el refactoring del sistema de piezas de ChessCPP.*
