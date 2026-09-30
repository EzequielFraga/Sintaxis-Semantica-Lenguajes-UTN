#ifndef REPORTE_H
#define REPORTE_H

#include <stddef.h>

/*Estructura para variables declaradas*/
typedef struct VariableDeclarada{
    char *nombre;
    char *tipo;
    int linea;
    struct VariableDeclarada *siguiente;
} VariableDeclarada;

/*Estrutura para funciones declaradas o definidas*/
typedef struct FuncionReporte{
    char *nombre;
    char *tipo_retorno;
    char *parametros;
    int es_definicion; 
    int linea;
    struct FuncionReporte *siguiente;
} FuncionReporte;

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
typedef struct CadenaNoReconocida{
char *cadena;
int linea;
int columna;
struct CadenaNoReconocida *siguiente;
} CadenaNoReconocida;

/*Prototipos de inicializacion y reporte*/
void init_reporte(void);
void generar_reporte_final(void);
void liberar_memoria(void);

/*Funciones para registrar datos desde Flex y Bison*/
void agregar_variable_descartada(const char *nombre, const char *tipo, int linea);
void agregar_funcion(const char *nombre, const char *retorno, const char *params, int es_def, int linea);
void agregar_sentencia(const char *tipo, int linea, int columna);
void agregar_error_sintactico(const char *texto, int linea);
void agregar_no_reconocido(const char *cadena, int linea, int columna);

#endif /*REPORTE_H*/