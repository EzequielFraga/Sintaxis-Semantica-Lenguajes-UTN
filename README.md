# Analizadores Léxico y Sintáctico en C
Proyecto académico desarrollado en la materia **Sintaxis y Semántica de los Lenguajes** de Ingeniería en Sistemas de Información — UTN FRBA.

El proyecto recorre distintas etapas del procesamiento de un lenguaje, desde el reconocimiento y clasificación de componentes léxicos hasta el análisis sintáctico de código C mediante una gramática formal.

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

## Sobre el proyecto
Los trabajos fueron realizados en equipo como parte de la cursada. Mi participación incluyó desarrollo, integración, debugging y testing del proyecto, con especial foco en el scanner, la gramática de expresiones, sentencias de selección y salto, manejo de precedencias y generación de reportes.

Además del desarrollo de las partes asignadas, trabajé sobre el funcionamiento integral de ambos analizadores y su integración.

## Estructura

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
└── README.md

## Objetivo
Aplicar conceptos de teoría de lenguajes, análisis léxico y análisis sintáctico mediante herramientas utilizadas para la construcción de compiladores e intérpretes, combinando los fundamentos teóricos con su implementación en C.