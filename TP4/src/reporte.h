#ifndef REPORTE_H
#define REPORTE_H

#include <stddef.h>

/*Estructura para sentencias*/
typedef struct SentenciaReporte {
    char *tipo;
    int linea;
    int columna;
    struct SentenciaReporte *siguiente;
} SentenciaReporte;

/*Estructura para errores sintacticos*/
typedef struct ErrorSintactico {
    char *texto;
    int linea;
    struct ErrorSintactico *siguiente;
} ErrorSintactico;

/*Estructura para cadenas no reconocidas (errores lexicos)*/
typedef struct CadenaNoReconocida {
    char *cadena;
    int linea;
    int columna;
    struct CadenaNoReconocida *siguiente;
} CadenaNoReconocida;

/*Estructura para errores semánticos*/
typedef struct ErrorSemantico {
    char *mensaje;
    int linea;
    int columna;
    struct ErrorSemantico *siguiente;        
} ErrorSemantico;

/*Prototipos de inicializacion y reporte*/
void init_reporte(void);
void generar_reporte_final(void);
void liberar_memoria(void);

/*Funciones para registrar datos desde Flex y Bison*/
void agregar_sentencia(const char *tipo, int linea, int columna);
void agregar_error_sintactico(const char *texto, int linea);
void agregar_no_reconocido(const char *cadena, int linea, int columna);
void agregar_error_semantico(const char *mensaje, int linea, int columna);

#endif /*REPORTE_H*/