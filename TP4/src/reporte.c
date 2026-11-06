#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reporte.h"
#include "tabla_simbolos.h"

static SentenciaReporte *sentencias_inicio = NULL;
static SentenciaReporte *sentencias_fin = NULL;

static ErrorSintactico *errores_inicio = NULL;
static ErrorSintactico *errores_fin = NULL;

static ErrorSemantico *errores_semanticos_inicio = NULL;
static ErrorSemantico *errores_semanticos_fin = NULL;

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
    sentencias_inicio = NULL;
    sentencias_fin = NULL;

    errores_inicio = NULL;
    errores_fin = NULL;

    errores_semanticos_inicio = NULL;
    errores_semanticos_fin = NULL;

    no_reconocidas_inicio = NULL;
    no_reconocidas_fin = NULL;
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

    while (*texto == ' ' || *texto == '\t') // Avanza el puntero hasta el primer caracter != espacio o tab.
        texto++;

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

void agregar_error_semantico(const char *mensaje, int linea, int columna) {
    ErrorSemantico *nuevo = malloc(sizeof(ErrorSemantico));
    if (nuevo == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }

    nuevo->mensaje = duplicar_cadena(mensaje);
    nuevo->linea = linea;
    nuevo->columna = columna;
    nuevo->siguiente = NULL;

    if (errores_semanticos_inicio == NULL)
        errores_semanticos_inicio = nuevo;
    else
        errores_semanticos_fin->siguiente = nuevo;

    errores_semanticos_fin = nuevo;
}

void generar_reporte_final(void)
{
    ErrorSintactico *error;
    CadenaNoReconocida *cadena;
    ErrorSemantico *error_semantico;
    
    const RegistroDeclaracion *registro;
    int hay_variables = 0;
    int hay_funciones = 0;

    printf("* Listado de variables declaradas (tipo de dato y numero de linea):\n");

    registro = obtener_declaraciones_ts();

    while (registro != NULL) {
        if (registro->clase == SIMBOLO_VARIABLE) {
            printf("%s: %s%s, linea %d, columna %d\n", registro->nombre, registro->es_constante ? "const " : "",
               registro->tipo, registro->linea, registro->columna);
            hay_variables = 1;
        }

        registro = registro->siguiente;
    }

    if (!hay_variables) 
        printf("-\n");

    printf("\n* Listado de funciones declaradas y definidas:\n");
    
    registro = obtener_declaraciones_ts();

    while (registro != NULL) {
        if (registro->clase == SIMBOLO_FUNCION) {
            printf("%s: %s, input: %s, retorna: %s, linea %d\n", registro->nombre,
               registro->es_definicion ? "definicion" : "declaracion",
               registro->parametros ? registro->parametros : "void", registro->tipo, registro->linea);
            hay_funciones = 1;
        }

        registro = registro->siguiente;
    }

    if (!hay_funciones) 
        printf("-\n");
    

    printf("\n* Listado de errores semanticos:\n");

    if (errores_semanticos_inicio == NULL)
        printf("-\n");
    else {
        error_semantico = errores_semanticos_inicio;

        while (error_semantico != NULL) {
            if (error_semantico->columna > 0) {
                printf("%d:%d: %s\n", error_semantico->linea, error_semantico->columna,error_semantico->mensaje);
            } else {
                printf("%s: linea %d\n", error_semantico->mensaje, error_semantico->linea);
            }

            error_semantico = error_semantico->siguiente;
        }
    }


    printf("\n* Listado de errores sintacticos:\n");

    if (errores_inicio == NULL)
        printf("-\n");
    else {
        error = errores_inicio;

        while (error != NULL) {
            printf("\"%s\": linea %d\n",
                   error->texto,
                   error->linea);

            error = error->siguiente;
        }
    }

    printf("\n* Listado de errores lexicos:\n");
    
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
    SentenciaReporte *sentencia;
    ErrorSintactico *error;
    CadenaNoReconocida *cadena;
    ErrorSemantico *error_semantico;

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

    while (errores_semanticos_inicio != NULL) {
        error_semantico = errores_semanticos_inicio;
        errores_semanticos_inicio = errores_semanticos_inicio->siguiente;

        free(error_semantico->mensaje);
        free(error_semantico);
    }
}