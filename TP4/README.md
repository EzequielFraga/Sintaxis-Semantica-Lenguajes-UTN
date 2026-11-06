# TP4: Trabajo Práctico Integrador (TPI)
## Analizador léxico, sintáctico y semántico de C

### Descripción del trabajo

El programa recibe un archivo de código fuente en C (`.i`) y lo analiza en tres etapas:

- **Análisis léxico** (`flex`): reconoce los tokens del lenguaje e informa las cadenas no reconocidas.
- **Análisis sintáctico** (`bison`): reconoce declaraciones, definiciones, expresiones y sentencias, e informa las estructuras sintácticas no reconocidas.
- **Análisis semántico**: mediante una tabla de símbolos, valida declaraciones, tipos en expresiones, invocaciones a funciones y sentencias `return`.

Al finalizar, genera un reporte con las variables declaradas, las funciones declaradas o definidas, los errores semánticos, las sentencias encontradas, las estructuras sintácticas no reconocidas y las cadenas no reconocidas.

### Ficha técnica
- **Estándar de C utilizado**: C11 con extensiones GNU (`-std=gnu11`)
- **Compilador de C utilizado**: gcc
- **Versión mínima del compilador de C utilizada**: 6.3.0
- **Versión mínima de *flex* necesaria**: flex 2.5.35
- **Versión mínima de *bison* necesaria**: bison (GNU Bison) 2.4.2
- **Secuencia de comandos utilizados para compilar y ejecutar el programa**:
```bash
make clean
make all
./bin/tp4 archivo.i
```

### Compilación

Desde la carpeta `TP4`:

```bash
make clean
make all
```

El ejecutable se genera en `bin/tp4`.

### Ejecución

Desde la carpeta `TP4`:

```bash
./bin/tp4 archivo.i
```

Para guardar el reporte en un archivo:

```bash
./bin/tp4 archivo.i > salida.txt
```

`archivo.i` contiene un programa de ejemplo que ejercita las validaciones de todas las partes del TP. También se puede ejecutar con cualquiera de los archivos de prueba, por ejemplo:

```bash
./bin/tp4 tests/input/test_4.i
```

Para ejecutar los tests de la cátedra y compararlos con las salidas esperadas:

```bash
sh tests/run_testsuite.sh ./bin/tp4
```

### Estructura del proyecto

```
TP4/
├── GNUmakefile                # Makefile de la cátedra
├── mkframework/               # Framework de make utilizado por el GNUmakefile
├── README.md
├── archivo.i                  # Programa de ejemplo para ejecutar
├── src/
│   ├── scanner.l              # Analizador léxico
│   ├── parser.y               # Gramática, acciones semánticas y main
│   ├── general.c / .h         # Manejo de la ubicación (línea y columna) de los tokens
│   ├── reporte.c / .h         # Registro de datos y generación del reporte final
│   ├── tabla_simbolos.c / .h  # Tabla de símbolos, ámbitos e historial de declaraciones
│   └── funciones.c / .h       # Validación de invocaciones y sentencias return
└── tests/
    ├── input/                 # Archivos .i de prueba
    ├── output/expected/       # Salidas esperadas de los tests de la cátedra
    ├── run_testsuite.sh       # Script que ejecuta las pruebas y compara las salidas
    ├── settings.sh            # Configuración de la suite de pruebas
    └── colors.sh              # Colores para la salida del script
```

## Tabla de símbolos (TS)

La tabla de símbolos es un componente del análisis semántico encargado de almacenar y administrar la información de los identificadores presentes en el código fuente. Permite registrar símbolos, consultar sus tipos y detectar inconsistencias semánticas.

Su implementación se encuentra en `src/tabla_simbolos.h` y `src/tabla_simbolos.c`.

Se integra con `src/parser.y` para registrar y consultar símbolos
durante el análisis semántico, y con `src/reporte.c` para proporcionar
el historial de declaraciones utilizado en la generación de los
listados de variables y funciones.

### Estructuras de datos

#### `Simbolo`

Representa una variable o función registrada en la tabla de símbolos. Contiene:

- `nombre`: identificador del símbolo.
- `tipo`: tipo de dato asociado.
- `linea` y `columna`: ubicación de la declaración.
- `linea_definicion` y `columna_definicion`: ubicación de la definición, cuando corresponde.
- `es_constante`: indica si fue declarado como constante.
- `esta_definida`: indica si una función ya fue definida.
- `tipos_parametros`: tipos de los parámetros de una función.
- `clase`: categoría del símbolo (`SIMBOLO_VARIABLE` o `SIMBOLO_FUNCION`).
- `siguiente`: referencia al siguiente símbolo de la lista enlazada.

#### `Parametro`

Representa un parámetro individual de una función. Almacena su nombre, tipo, línea, columna y una referencia al siguiente parámetro.

#### `ListaParametros`

Agrupa la información de los parámetros de una función mediante:

- `descripcion`: descripción de los parámetros.
- `tipos`: representación de sus tipos.
- `parametros`: referencia a la lista enlazada de parámetros.

#### `RegistroDeclaracion`

Almacena información de las declaraciones que deben conservarse para el reporte final.

Contiene la clase del símbolo, nombre, tipo, ubicación y constancia. Para las funciones, también conserva sus parámetros y un indicador que distingue declaraciones de definiciones.

Los registros se organizan en una lista enlazada que conserva el orden de aparición.

### Gestión de ámbitos (scopes)

La implementación distingue dos ámbitos:

- **Global:** contiene los símbolos declarados fuera de las funciones.
- **Local:** contiene los símbolos registrados durante el análisis de una función, incluidos sus parámetros.

La función `buscar_simbolo()` prioriza la búsqueda en el ámbito local, cuando está activo, y luego continúa en el global.

Las funciones `iniciar_ambito_local()` y `finalizar_ambito_local()` administran la entrada y salida del ámbito local.

Al finalizar una función, sus símbolos locales se liberan y dejan de estar disponibles para búsquedas posteriores.

La implementación contempla ámbitos globales y locales de función, sin implementar un sistema general de ámbitos anidados, como una pila.

### Historial de declaraciones

Además de las listas de símbolos utilizadas para las validaciones semánticas, la TS mantiene un historial mediante la estructura `RegistroDeclaracion`.

Esta separación permite cumplir dos responsabilidades:

- **Validación semántica:** trabajar con los símbolos disponibles en el ámbito correspondiente.
- **Generación del reporte:** conservar las declaraciones en orden de aparición, incluso después de liberar los símbolos locales.

El historial se administra mediante:

- `registrar_declaracion_variable_ts()`: registra una variable declarada.
- `registrar_declaracion_funcion_ts()`: registra una declaración o definición de función.
- `obtener_declaraciones_ts()`: proporciona acceso al historial para generar los listados.

De esta manera, los listados de variables y funciones se generan utilizando información administrada por la TS y respetando el orden de aparición requerido por el enunciado.

### Operaciones principales

La tabla de símbolos proporciona las siguientes funciones:

- `buscar_simbolo()`: busca un identificador en los ámbitos disponibles.
- `agregar_simbolo()`: incorpora un símbolo, verificando posibles duplicaciones en el ámbito correspondiente.
- `registrar_variable()`: registra una variable y gestiona situaciones de redeclaración.
- `registrar_parametros()`: incorpora los parámetros de una función como símbolos.
- `asignar_tipos_parametros()`: asocia información de los tipos de parámetros a una función.
- `iniciar_ambito_local()` y `finalizar_ambito_local()`: administran el ámbito local.
- `liberar_parametros()`: libera una lista de parámetros.
- `liberar_tabla_simbolos()`: libera la memoria administrada por la TS.

Estas operaciones se utilizan desde el parser y las rutinas semánticas para realizar las validaciones correspondientes.

### Gestión de memoria

La implementación utiliza memoria dinámica y listas enlazadas para almacenar símbolos, parámetros y registros de declaraciones.

Los símbolos locales se liberan mediante `finalizar_ambito_local()` al terminar el análisis de la función correspondiente.

Al finalizar el procesamiento, `liberar_tabla_simbolos()` libera los símbolos globales y el historial de declaraciones. Por su parte, `liberar_parametros()` permite liberar las listas de parámetros utilizadas durante el análisis.

## Expresiones

Esta parte del análisis semántico comprueba los tipos y la validez de las operaciones dentro de las expresiones. Se implementa en las reglas de expresiones de `src/parser.y`, y consulta la tabla de símbolos para obtener el tipo y las características de cada identificador.

### Validaciones

- **Identificador no declarado:** `L:C: 'X' sin declarar`
- **Inicialización con un tipo incompatible:** `L:C: Incompatibilidad de tipos al inicializar el tipo 'T1' usando el tipo 'T2'`
- **Asignación a algo que no es un valor-L** (por ejemplo, `1 = 1`): `L:C: Se requiere un valor-L modificable como operando izquierdo de la asignacion`
- **Asignación a una variable `const`:** `L:C: Asignacion de la variable de solo lectura 'X'`
- **Multiplicación con operandos no aritméticos:** `L:C: Operandos invalidos del operador binario * (tienen 'T1' y 'T2')`

### Estructuras de datos

#### `InfoExpresion`

Es el valor semántico que acompaña a cada expresión durante el análisis. Contiene:

- `tipo`: tipo de dato de la expresión (por ejemplo, `int` o `char *`). Queda vacío cuando el tipo es desconocido por un error anterior.
- `nombre`: nombre del identificador, si la expresión es un identificador. Queda vacío en otro caso.
- `es_valor_l`: indica si la expresión es un valor-L, es decir, si puede aparecer a la izquierda de una asignación.
- `es_constante`: indica si el identificador fue declarado `const`.

### Propagación de tipos

Cada regla de expresión arma su `InfoExpresion` mediante `crear_expresion()`, a partir de la información de sus operandos:

- **Constantes:** las enteras, octales, hexadecimales y de carácter son `int`; las reales son `double`; los literales cadena son `char *`.
- **Identificadores:** se busca su tipo en la tabla de símbolos. Las variables son valores-L y conservan su condición de `const`. Las funciones se representan como puntero a función (por ejemplo, `void (*)(void)`).
- **Operadores relacionales, de igualdad y lógicos:** el resultado es `int`.
- **Operadores aditivos, multiplicativos, unarios y de incremento o decremento:** el resultado toma el tipo del operando.
- **`sizeof`:** el resultado es `unsigned int`.
- **Asignación:** el resultado toma el tipo del operando izquierdo y deja de ser un valor-L.

### Compatibilidad de tipos

- `es_tipo_arimetico()`: considera aritmético a todo tipo que no sea un puntero.
- `son_tipos_compatibles()`: dos tipos son compatibles si son iguales o si ambos son aritméticos.

Cuando un operando tiene tipo desconocido por un error anterior, no se informan errores adicionales, para evitar errores en cascada.

### Limitaciones

- Solo se valida el operador `*`. La validación de los operadores aditivos y unarios es opcional según el enunciado y no se implementa.
- La validación de `*` se aplica cuando alguno de los operandos es un identificador.

## Funciones: invocaciones y sentencias `return`

Este módulo valida el uso de funciones y las sentencias de retorno. Su implementación se encuentra en `src/funciones.h` y `src/funciones.c`, y se invoca desde las reglas de `src/parser.y` (los puntos de integración están marcados con el comentario `INTEGRANTE 3`).

El módulo no modifica la tabla de símbolos: la consulta para saber si un identificador es una variable o una función, y guarda por su cuenta la información adicional que necesitan los mensajes de error.

### Validaciones

- **Invocación a una función no declarada:** `L:C: Funcion 'X' sin declarar`
- **Invocación a algo que no es una función:** `L:C: El objeto invocado 'X' no es una funcion o un puntero a una funcion`
- **Menos argumentos que parámetros:** `L:C: Insuficientes argumentos para la funcion 'X'`
- **Más argumentos que parámetros:** `L:C: Demasiados argumentos para la funcion 'X'`
- **Argumento de tipo incompatible:** `L:C: Incompatibilidad de tipos para el argumento N de 'X'`
- **Uso del valor de una función `void`:** `L:C: No se ignora el valor de retorno void como deberia ser`
- **`return;` en una función que no es `void`:** `L:C: 'return' sin valor en una funcion que no retorna void`
- **`return` con un valor de tipo incompatible:** `L:C: Incompatibilidad de tipos al retornar el tipo 'T1' pero se esperaba 'T2'`

Cuando corresponde, el mensaje se completa con una nota que indica dónde fue declarado el símbolo o el parámetro involucrado (`Nota: declarado aqui: L:C`).

### Estructuras de datos

#### `Firma`

Representa la firma de una función. Guarda el tipo de retorno, la cantidad de parámetros y, para cada parámetro, su tipo y su ubicación.

Una función declarada con `()` se considera sin prototipo y no se controla la cantidad de argumentos. Una función declarada con `(void)` tiene cero parámetros.

#### `Ubicacion`

Guarda la línea y la columna donde se declaró cada identificador, para las notas de los mensajes de error.

#### `ListaArgumentos`

Se arma durante el análisis de una invocación y guarda el tipo y la ubicación de cada argumento.

### Funcionamiento

- **Registro de funciones:** al reducir cada parámetro se acumulan su tipo y su ubicación (`fn_agregar_parametro()`). Al reducir la declaración o el encabezado de la definición, se arma la firma (`fn_registrar_funcion()`).
- **Definiciones:** la firma se registra antes de analizar el cuerpo (`fn_iniciar_definicion()`), para que las sentencias `return` conozcan el tipo de retorno esperado y para admitir llamadas recursivas. Al terminar el cuerpo se llama a `fn_finalizar_definicion()`.
- **Invocaciones:** la gramática tiene una regla específica para `IDENTIFICADOR '(' argumentos ')'`. Como los argumentos se reducen antes que la invocación, sus errores se informan primero, en el mismo orden que la salida esperada. La validación la realiza `fn_validar_llamada()`, que devuelve el tipo de retorno de la función para que la invocación pueda usarse dentro de otras expresiones.
- **Uso de un nombre de función como valor:** su tipo se representa como puntero a función (por ejemplo, `int (*)(int)`), lo que permite detectar casos como `return imprimir;`.
- **Compatibilidad de tipos:** todos los tipos aritméticos son compatibles entre sí. Los punteros son compatibles si son del mismo tipo, o si uno de ellos es `void *`. Un tipo desconocido (por un error anterior) no genera errores adicionales, para evitar errores en cascada.

### Operaciones principales

- `fn_agregar_parametro()`: acumula el tipo y la ubicación de un parámetro.
- `fn_registrar_funcion()`: registra la firma de una función declarada.
- `fn_iniciar_definicion()` y `fn_finalizar_definicion()`: indican el inicio y el fin del cuerpo de una función.
- `fn_registrar_ubicacion()`: guarda la ubicación de una variable declarada.
- `fn_validar_llamada()`: valida una invocación y devuelve el tipo de retorno.
- `fn_verificar_valor_void()`: informa el uso del valor de una función `void`.
- `fn_validar_return_sin_valor()` y `fn_validar_return_con_valor()`: validan las sentencias `return`.
- `fn_liberar()`: libera la memoria administrada por el módulo.

### Gestión de memoria

Las firmas, las ubicaciones y las cadenas de tipos que usa el módulo se guardan en listas enlazadas con memoria dinámica, y se liberan al finalizar mediante `fn_liberar()`. Las listas de argumentos se liberan al terminar la validación de cada invocación, mediante `la_liberar()`.

### Limitaciones

- No se informa un `return` con valor dentro de una función `void`, porque el enunciado no define un mensaje para ese caso.
- Solo se validan las invocaciones hechas directamente por el nombre de la función. Las invocaciones sobre otras expresiones (por ejemplo, `(*p)(x)`) se reconocen sintácticamente pero no se validan.

### Pruebas

- `tests/input/test_return.i`: `return` sin valor en una función `int` y `return` con tipos incompatibles.
- `tests/input/test_void.i`: uso del valor de una función `void` en una inicialización y en una asignación.
- `tests/input/test_4.i` (de la cátedra): cantidad de argumentos en una invocación.