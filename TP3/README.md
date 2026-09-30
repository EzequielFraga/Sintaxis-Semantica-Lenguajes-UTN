# TP3
## Bison para reconocimiento de estructuras sintácticas

### Ficha técnica
- **Estándar de C utilizado**: gnu11
- **Compilador de C utilizado**: gcc
- **Versión mínima del compilador de C utilizada**: 6.3.0
- **Versión mínima de *flex* necesaria**: 2.5.4
- **Versión mínima de *bison* necesaria**: 2.4.1
- **Secuencia de comandos para compilar y ejecutar el programa**:
```bash
make
./bin/tp3.exe tests/input/archivo.i
```
- **Secuencia de comandos para ejecutar la suite de pruebas**:
```bash
./tests/run_testsuite.sh ./bin/tp3.exe
```

### Descripción

Se implementó en lenguaje C un analizador sintáctico (*parser*) para ANSI C (C89/C90) a partir de un archivo fuente preprocesado (`.i`).

El desarrollo integra **Flex** para el análisis léxico y **Bison** para el análisis sintáctico. El seguimiento de las posiciones en el código se realiza mediante la directiva `%locations` (`yylloc` y `@$`), gestionando la información de manera dinámica para generar un reporte final por la salida estándar (`stdout`) con el siguiente formato:

1. **Variables declaradas:** Orden de aparición, identificador, tipo de dato y número de línea (respetando orden de izquierda a derecha en declaraciones múltiples).
2. **Funciones declaradas o definidas:** Orden de aparición, nombre, clasificación (`declaracion` / `definicion`), parámetros de entrada (`void` o tipos/nombres) y tipo de dato del valor de retorno.
3. **Sentencias:** Tipo de sentencia (`for`, `while`, `do/while`, `if`, `if/else`, `switch`, `case`, `default`, `break`, `continue`, `return`), número de línea y número de columna.
4. **Estructuras sintácticas no reconocidas:** Texto de la estructura y número de línea de las sentencias que presentaron errores sintácticos, recuperando el flujo mediante el token `error`.
5. **Cadenas no reconocidas:** Secuencias léxicas no válidas indicando número de línea y de columna.

*(En caso de que un listado no registre elementos, se indica con `-`)*.

### Estructura del proyecto

- `src/`: Especificación léxica (`scanner.l`), gramática y parser (`parser.y`), módulos de reporte (`reporte.c`, `reporte.h`) y utilitarios (`general.c`, `general.h`).
- `tests/input/`: Archivos fuente preprocesados de entrada (`.i`).
- `tests/output/`: Archivos de salida generados durante la ejecución de los tests.
- `tests/output/expected/`: Salidas de referencia esperadas por la suite de pruebas.
- `tests/run_testsuite.sh`: Script bash para la verificación automática de diferencias y cálculo del porcentaje de cobertura.
- `bin/`: Directorio donde se aloja el ejecutable binario compilado.
- `GNUmakefile`: Archivo de configuración para la compilación, generación de dependencias automáticas y limpieza del proyecto.