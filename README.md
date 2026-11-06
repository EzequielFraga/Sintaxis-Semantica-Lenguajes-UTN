# Analizadores Léxico, Sintáctico y Semántico en C
Proyecto académico desarrollado en la materia **Sintaxis y Semántica de los Lenguajes** de Ingeniería en Sistemas de Información — UTN FRBA.

El proyecto recorre distintas etapas del procesamiento de un lenguaje, desde el reconocimiento y clasificación de componentes léxicos hasta el análisis sintáctico y semántico de código ANSI C (C89/C90).

## Tecnologías
- C
- Flex
- Bison
- Git / GitHub
- GNUMake

## TP2 — Analizador Léxico
Analizador léxico desarrollado con **Flex y C**, encargado de procesar código fuente y reconocer diferentes categorías de lexemas.

Entre sus funcionalidades se incluyen:
- Reconocimiento de palabras reservadas e identificadores.
- Reconocimiento de constantes enteras y reales.
- Reconocimiento de operadores y signos de puntuación.
- Procesamiento de literales.
- Detección de elementos no reconocidos.
- Seguimiento de línea y columna.
- Generación de reportes a partir del código analizado.
- Manejo dinámico de memoria en C.

El scanner utiliza expresiones regulares y las reglas de reconocimiento de Flex para transformar la entrada en unidades léxicas clasificadas.

➡️ [Ver TP2](./TP2)

## TP3 — Analizador Sintáctico
Extensión del analizador léxico mediante la integración de **Flex y Bison** para analizar estructuras sintácticas de código **ANSI C (C89/C90)**.

El parser reconoce, entre otras construcciones:
- Expresiones y operadores con sus correspondientes precedencias y asociatividades.
- Declaraciones.
- Definiciones de funciones.
- Sentencias de selección (`if`, `if/else`, `switch`).
- Sentencias iterativas.
- Sentencias de salto (`return`, `break`, `continue`).
- Bloques y sentencias de expresión.
- Errores sintácticos y generación de reportes.

Durante el desarrollo se trabajó con gramáticas independientes del contexto, valores semánticos, precedencia de operadores y resolución de conflictos `shift/reduce`, incluyendo el problema clásico del **dangling else**.

➡️ [Ver TP3](./TP3)

## TP4 — Analizador Semántico

Extensión del analizador léxico y sintáctico mediante la incorporación de análisis semántico, utilizando una tabla de símbolos implementada en C con memoria dinámica.

Entre sus funcionalidades se incluyen:

- Registro de declaraciones y definiciones de variables y funciones.

- Gestión de ámbitos globales y locales de función.

- Resolución de identificadores según su ámbito de visibilidad.

- Detección de identificadores no declarados y redeclaraciones.

- Detección de redefiniciones de funciones y declaraciones incompatibles.

- Validaciones de tipos en expresiones, asignaciones y llamadas a funciones.

- Validación de argumentos y sentencias return.

- Generación de reportes de errores semánticos integrados con el análisis léxico y sintáctico.

La tabla de símbolos utiliza estructuras dinámicas y permite administrar la información de los identificadores durante el procesamiento del código fuente.

Uno de los desafíos principales fue gestionar correctamente los ámbitos, distinguir entre declaraciones y definiciones de funciones, y conservar el historial de declaraciones para los reportes incluso después de liberar los símbolos locales.

➡️ [Ver TP4](./TP4)

## Sobre el proyecto

Los trabajos fueron desarrollados en equipo, junto con otros dos compañeros, como parte de la cursada de Sintaxis y Semántica de los Lenguajes en UTN FRBA.

Mi participación incluyó desarrollo, integración, depuración y pruebas, con especial foco en el scanner, la gramática de expresiones, las sentencias de selección y salto, el manejo de precedencias y la generación de reportes durante las primeras etapas.

En el TP4, mi principal responsabilidad fue el diseño e implementación de la tabla de símbolos, incluyendo la gestión de ámbitos, el registro y validación de declaraciones y definiciones, y su integración con el parser y el sistema de reportes.

El proyecto permitió trabajar de manera colaborativa sobre los distintos componentes de un analizador de código fuente, integrando progresivamente las etapas léxica, sintáctica y semántica.

## Estructura
```text
.
├── TP2/
│   ├── src/
│   ├── tests/
│   ├── GNUmakefile
│   └── README.md
│
├── TP3/
│   ├── src/
│   ├── tests/
│   ├── GNUmakefile
│   └── README.md
│
├── TP4/
│   ├── src/
│   ├── tests/
│   ├── mkframework/
│   ├── GNUmakefile
│   └── README.md
│
└── README.md
```
## Objetivo

Aplicar conceptos de teoría de lenguajes, análisis léxico, sintáctico y semántico mediante herramientas utilizadas para la construcción de compiladores e intérpretes, combinando los fundamentos teóricos con su implementación en C.
