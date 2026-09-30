# TP2
## Flex para reconocimiento de categorías léxicas de C

### Ficha técnica
- **Estándar de C utilizado**: GNU C11
- **Compilador de C utilizado**: gcc
- **Versión mínima del compilador de C utilizada**: 15.2.0
- **Versión mínima de *flex* necesaria**: 2.6.4
- **Secuencia de comandos utilizados para compilar y ejecutar el programa**:
```bash
make all
./bin/tp2.exe archivo.i
```

### Descripción
En este trabajo práctico se desarrolló un analizador léxico para el lenguaje C utilizando **Flex**. El programa recibe como entrada un archivo fuente y reconoce distintas categorías léxicas, entre ellas identificadores, palabras reservadas, constantes numéricas y de caracteres, literales cadena, operadores y caracteres de puntuación. Además, detecta cadenas no reconocidas y genera un reporte con la información obtenida durante el análisis, incluyendo estadísticas y la posición (línea y columna) de los elementos que lo requieren.
