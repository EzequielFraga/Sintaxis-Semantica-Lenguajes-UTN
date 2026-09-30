#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reporte.h"


static VariableDeclarada *variables_inicio = NULL;
static VariableDeclarada *variables_fin = NULL;

static FuncionReporte *funciones_inicio = NULL;
static FuncionReporte *funciones_fin = NULL;

static SentenciaReporte *sentencias_inicio = NULL;
static SentenciaReporte *sentencias_fin = NULL;

static ErrorSintactico *errores_inicio = NULL;
static ErrorSintactico *errores_fin = NULL;

static CadenaNoReconocida *no_reconocidas_inicio = NULL;
static CadenaNoReconocida *no_reconocidas_fin = NULL;


static char *duplicar_cadena(const char *cadena)
{
    char *copia;

    if (cadena == NULL)
        return NULL;

    copia = malloc(strlen(cadena) + 1);

    if (copia == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(copia, cadena);

    return copia;
}


void init_reporte(void)
{
    variables_inicio = NULL;
    variables_fin = NULL;

    funciones_inicio = NULL;
    funciones_fin = NULL;

    sentencias_inicio = NULL;
    sentencias_fin = NULL;

    errores_inicio = NULL;
    errores_fin = NULL;

    no_reconocidas_inicio = NULL;
    no_reconocidas_fin = NULL;
}

void agregar_variable_descartada(const char *nombre, const char *tipo, int linea)
{
    VariableDeclarada *nueva;

    nueva = malloc(sizeof(VariableDeclarada));

    if (nueva == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }

    nueva->nombre = duplicar_cadena(nombre);
    nueva->tipo = duplicar_cadena(tipo);
    nueva->linea = linea;
    nueva->siguiente = NULL;

    if (variables_inicio == NULL)
        variables_inicio = nueva;
    else
        variables_fin->siguiente = nueva;

    variables_fin = nueva;
}


void agregar_funcion(const char *nombre, const char *retorno,
                     const char *params, int es_def, int linea)
{
    FuncionReporte *nueva;

    nueva = malloc(sizeof(FuncionReporte));

    if (nueva == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }

    nueva->nombre = duplicar_cadena(nombre);
    nueva->tipo_retorno = duplicar_cadena(retorno);
    nueva->parametros = duplicar_cadena(params);
    nueva->es_definicion = es_def;
    nueva->linea = linea;
    nueva->siguiente = NULL;

    if (funciones_inicio == NULL)
        funciones_inicio = nueva;
    else
        funciones_fin->siguiente = nueva;

    funciones_fin = nueva;
}


void agregar_sentencia(const char *tipo, int linea, int columna)
{
    SentenciaReporte *nueva; 
    SentenciaReporte *actual; // Para recorrer

    nueva = malloc(sizeof(SentenciaReporte));

    if (nueva == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }

    nueva->tipo = duplicar_cadena(tipo);
    nueva->linea = linea;
    nueva->columna = columna;
    nueva->siguiente = NULL;

    // Lista vacía 
    if (sentencias_inicio == NULL) {
        sentencias_inicio = nueva;
        sentencias_fin = nueva;
        return;
    }

    // Insertar al principio
    if (linea < sentencias_inicio->linea ||
        (linea == sentencias_inicio->linea &&
         columna < sentencias_inicio->columna)) {

        nueva->siguiente = sentencias_inicio;
        sentencias_inicio = nueva;
        return;
    }

    // Buscar con actual donde insertar (fijandose en el siguiente)
    actual = sentencias_inicio;

    while (actual->siguiente != NULL &&
           (actual->siguiente->linea < linea ||
            (actual->siguiente->linea == linea &&
             actual->siguiente->columna <= columna))) {
        actual = actual->siguiente;
    }

    nueva->siguiente = actual->siguiente;
    actual->siguiente = nueva;

    // Si quedó al final, actualizar sentencias_fin
    if (nueva->siguiente == NULL)
        sentencias_fin = nueva;

    // La lista se va a imprimir en el orden en el que aparecen las sentencias en el archivo fuente
    // Aunque Bison reduzca bottom-up, primero desde las estructuras más chicas a más grandes
}


void agregar_error_sintactico(const char *texto, int linea)
{
    ErrorSintactico *nuevo;

    nuevo = malloc(sizeof(ErrorSintactico));

    if (nuevo == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }

    nuevo->texto = duplicar_cadena(texto);
    nuevo->linea = linea;
    nuevo->siguiente = NULL;

    if (errores_inicio == NULL)
        errores_inicio = nuevo;
    else
        errores_fin->siguiente = nuevo;

    errores_fin = nuevo;
}


void agregar_no_reconocido(const char *cadena, int linea, int columna)
{
    CadenaNoReconocida *nueva;

    nueva = malloc(sizeof(CadenaNoReconocida));

    if (nueva == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }

    nueva->cadena = duplicar_cadena(cadena);
    nueva->linea = linea;
    nueva->columna = columna;
    nueva->siguiente = NULL;

    if (no_reconocidas_inicio == NULL)
        no_reconocidas_inicio = nueva;
    else
        no_reconocidas_fin->siguiente = nueva;

    no_reconocidas_fin = nueva;
}

void generar_reporte_final(void)
{
    VariableDeclarada *variable;
    FuncionReporte *funcion;
    SentenciaReporte *sentencia;
    ErrorSintactico *error;
    CadenaNoReconocida *cadena;

  printf("* Listado de variables declaradas (tipo de dato y numero de linea):\n");

    if (variables_inicio == NULL) {
        printf("-\n");
    } else {
        variable = variables_inicio;

        while (variable != NULL) {
            printf("%s: %s, linea %d\n",
                   variable->nombre,
                   variable->tipo,
                 variable  -> linea);

            variable = variable->siguiente;
        }
    }

    printf("\n* Listado de funciones declaradas o definidas:\n");

    if (funciones_inicio == NULL) {
        printf("-\n");
    } else {
        funcion = funciones_inicio;

        while (funcion != NULL) {
            printf("%s: %s, input: %s, retorna: %s, linea %d\n",
                   funcion->nombre,
                   funcion->es_definicion ? "definicion" : "declaracion",
                   funcion->parametros ? funcion->parametros : "void",
                   funcion->tipo_retorno,
                   funcion->linea);

            funcion = funcion->siguiente;
        }
    }

    printf("\n* Listado de sentencias indicando tipo, numero de linea y de columna:\n");

    if (sentencias_inicio == NULL) {
        printf("-\n");
    } else {
        sentencia = sentencias_inicio;

        while (sentencia != NULL) {
            printf("%s:  linea %d, columna %d\n",
                   sentencia->tipo,
                   sentencia->linea,
                   sentencia->columna);

            sentencia = sentencia->siguiente;
        }
    }

    printf("\n* Listado de estructuras sintacticas no reconocidas:\n");

    if (errores_inicio == NULL) {
        printf("-\n");
    } else {
        error = errores_inicio;

        while (error != NULL) {
            printf("\"%s\": linea %d\n",
                   error->texto,
                   error->linea);

            error = error->siguiente;
        }
    }

    printf("\n* Listado de cadenas no reconocidas:\n");

    if (no_reconocidas_inicio == NULL) {
        printf("-\n");
    } else {
        cadena = no_reconocidas_inicio;

        while (cadena != NULL) {
            printf("%s: linea %d, columna %d\n",
                   cadena->cadena,
                   cadena->linea,
                   cadena->columna);

            cadena = cadena->siguiente;
        }
    }
}


void liberar_memoria(void)
{
    VariableDeclarada *variable;
    FuncionReporte *funcion;
    SentenciaReporte *sentencia;
    ErrorSintactico *error;
    CadenaNoReconocida *cadena;

    while (variables_inicio != NULL) {
        variable = variables_inicio;
        variables_inicio = variables_inicio->siguiente;

        free(variable->nombre);
        free(variable->tipo);
        free(variable);
    }

    while (funciones_inicio != NULL) {
        funcion = funciones_inicio;
        funciones_inicio = funciones_inicio->siguiente;

        free(funcion->nombre);
        free(funcion->tipo_retorno);
        free(funcion->parametros);
        free(funcion);
    }

    while (sentencias_inicio != NULL) {
        sentencia = sentencias_inicio;
        sentencias_inicio = sentencias_inicio->siguiente;

        free(sentencia->tipo);
        free(sentencia);
    }

    while (errores_inicio != NULL) {
        error = errores_inicio;
        errores_inicio = errores_inicio->siguiente;

        free(error->texto);
        free(error);
    }

    while (no_reconocidas_inicio != NULL) {
        cadena = no_reconocidas_inicio;
        no_reconocidas_inicio = no_reconocidas_inicio->siguiente;

        free(cadena->cadena);
        free(cadena);
    }
}