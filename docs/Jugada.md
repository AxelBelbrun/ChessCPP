# Documentación del módulo Jugada

## Propósito

Este módulo define la estructura `Jugada` que representa un movimiento de ajedrez en el sistema. Su función principal es codificar y decodificar jugadas mediante operaciones de bits, permitiendo almacenar toda la información relevante de un movimiento en un único valor de 16 bits (`u_short`).

## Estructura de datos

### Representación binaria del movimiento

El campo `movimiento` (tipo `u_short`, 16 bits) almacena la información del movimiento usando los siguientes bits:

| Rango de bits | Tamaño | Descripción |
|---------------|--------|-------------|
| 0-5           | 6 bits | Posición de origen (casilla de salida) |
| 6-11          | 6 bits | Posición de destino (casilla de llegada) |
| 12-17         | 6 bits | Tipo de jugadatipoDeJugada |

### Enum `tipoDeJugada`

Define los posibles tipos de jugadas reconocidas por el sistema:

```
QUIET, ENPASSANT, CASTLING, PROMOTIONDER, PROMOTIONIZQ, PROMOTION,
CAPTURE, CPIC, CPDC, PROMOTIONCHECK, CAPTURECHECK, CHECK,
CHECKMATE, ENPASSANTCHECK, CASTLINGCHECK
```

## Componentes del módulo

### Header: `include/Jugada.h`

**Estructura `Jugada`** (líneas 12-58)

Contiene:

- `u_short movimiento`: Campo privado que almacena el movimiento codificado.
- Constructor `Jugada(int salida, int llegada, int tipoDeJugada)`: Inicializa el campo `movimiento` usando operaciones OR de bits para empaquetar los tres valores.

**Métodos de acceso (getters)**:

- `int getSalida()`: Extrae los bits 0-5 del movimiento.
- `int getLlegada()`: Extrae los bits 6-11 del movimiento.
- `int getTipoDeJugada()`: Extrae los bits 12-17 del movimiento.
- `int getCasillaDestino()`: Equivalente a `getLlegada()` (implementación en .cpp).

**Método estático**:

- `static u_short stringToUShort(std::string jugada)`: Convierte una representación en string del movimiento a formato `u_short`. El string debe tener formato "ABCD" donde:
  - "AB": casillero de origen (ej: "e2")
  - "CD": casillero de destino (ej: "e4")
  - El quinto carácter indica el tipo de jugadatipoDeJugada como dígito numérico.

**Operadores**:

- `operator==`: Compara si dos jugadas son iguales comparando directamente el campo `movimiento`.

### Implementación: `src/Jugada.cpp`

Contiene la implementación de los métodos definidos fuera de línea:

1. **`getCasillaDestino()`** (línea 18-21): Retorna `movimiento >> 6`, equivalente a `getLlegada()`.

2. **`stringToUShort(std::string)`** (líneas 22-32):
   - Utiliza el mapa `casillaANumero` (definido en `constantes.h`) para convertir notación algebraica (ej: "e2") a números.
   - Construye el `u_short` resultado usando operaciones de bits:
     - `salida` en bits 0-5
     - `llegada << 6` en bits 6-11
     - `tipoDeJugada << 12` en bits 12-17

## Dependencias externas

El módulo depende de:

- **`constantes.h`**: Proporciona el mapa `casallaANumero` y el namespace `constantes`.
- **`Tablero.h`**: Incluido pero no utilizado directamente en el código de Jugada.cpp.

## Consideraciones para refactoring

1. **Duplicación lógica**: `getCasillaDestino()` y `getLlegada()` realizan la misma operación. Consolidar en un solo método.

2. **Validación ausente**: `stringToUShort()` no valida que el string tenga el formato correcto ni que las casillas existan en el mapa.

3. **Código muerto**: El enum `tipoDeJugada` usa valores que podrían no ser necesarios (ej: CPDC, CPIC).

4. **Acoplamiento con constantes**: El método `stringToUShort()` depende fuertemente de que `casillaANumero` exista y contenga las casillas esperadas.

5. **Comentar código**: Hay bloques comentados en ambas líneas (constructor alternativo en .cpp, operadores en .h) que deberían eliminarse o mantenerse explícitamente.

6. **Notación confusa**: Nombres como PROMOTIONDER/PROMOTIONIZQ parecen hacer referencia a direcciones (derecha/izquierda) pero el significado no es claro sin contexto adicional.
