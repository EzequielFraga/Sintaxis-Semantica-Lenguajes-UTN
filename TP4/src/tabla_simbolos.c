#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabla_simbolos.h"
#include "reporte.h"

// Dos tablas: una global y una local (la de la función que se está analizando)
static Simbolo *tabla = NULL; // Variables globales y funciones
static Simbolo *tabla_local = NULL; // Símbolos locales de la función actual
static int dentro_funcion = 0; // Si se está analizando el cuerpo de una función o no

// Historial de declaraciones válidas, en orden de aparición
// Pertenece al módulo de la tabla de símbolos
static RegistroDeclaracion *declaraciones_inicio = NULL;
static RegistroDeclaracion *declaraciones_fin = NULL;

static void agregar_registro_ts(ClaseSimbolo clase, const char *nombre, const char *tipo, int linea,
    int columna, int es_constante, const char *parametros, int es_definicion) 
{
    RegistroDeclaracion *nuevo = calloc(1, sizeof(RegistroDeclaracion));
    if (nuevo == NULL) {
        fprintf(stderr, "Error de memoria al registrar declaracion\n");
        exit(EXIT_FAILURE);
    }

    nuevo->clase = clase;
    nuevo->nombre = strdup(nombre);
    nuevo->tipo = strdup(tipo);
    nuevo->linea = linea;
    nuevo->columna = columna;
    nuevo->es_constante = es_constante;
    nuevo->parametros = parametros ? strdup(parametros) : NULL;
    nuevo->es_definicion = es_definicion;

    if (nuevo->nombre == NULL || nuevo->tipo == NULL || (parametros != NULL && nuevo->parametros == NULL)) {

        fprintf(stderr, "Error de memoria al registrar declaracion\n");
        free(nuevo->nombre);
        free(nuevo->tipo);
        free(nuevo->parametros);
        free(nuevo);
        exit(EXIT_FAILURE);
    }

    if (declaraciones_fin == NULL)
        declaraciones_inicio = nuevo;
    else 
        declaraciones_fin->siguiente = nuevo;

    declaraciones_fin = nuevo;
}

void registrar_declaracion_variable_ts(const char *nombre, const char *tipo, int linea, int columna,
    int es_constante) 
{
    agregar_registro_ts(SIMBOLO_VARIABLE, nombre, tipo, linea, columna, es_constante, NULL, 0);
}

void registrar_declaracion_funcion_ts(const char *nombre, const char *tipo_retorno, const char *parametros,
    int es_definicion, int linea) 
{
    agregar_registro_ts(SIMBOLO_FUNCION, nombre, tipo_retorno, linea, 0, 0, parametros, es_definicion);
}

const RegistroDeclaracion *obtener_declaraciones_ts(void) {
    return declaraciones_inicio;
}
// Si estoy dentro de una función, busco el ID en sus parametros y variables locales
// Si no lo encuentro, busco en la tabla global
Simbolo *buscar_simbolo(const char *nombre) {
    Simbolo *actual;

    if (dentro_funcion) {
        actual = tabla_local;

        while (actual != NULL) {
            if (strcmp(actual->nombre, nombre) == 0)
                return actual;

            actual = actual->siguiente;
        }
    }

    actual = tabla;

    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0)
            return actual;

        actual = actual->siguiente;
    }

    return NULL;
}

int agregar_simbolo(const char *nombre, const char *tipo, ClaseSimbolo clase, int linea, int columna,
                    int es_constante) {

    Simbolo *actual = dentro_funcion ? tabla_local : tabla;

    // Busco duplicados en el ambito correspondiente
    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0)
            return 0;

        actual = actual->siguiente;
    }

    Simbolo *nuevo = malloc(sizeof(Simbolo));
    if (nuevo == NULL)
        return -1;

    nuevo->nombre = strdup(nombre);
    nuevo->tipo = strdup(tipo);
    if (nuevo->nombre == NULL || nuevo->tipo == NULL) {
        free(nuevo->nombre);
        free(nuevo->tipo);
        free(nuevo);
        return -1;
    }
    
    nuevo->clase = clase;
    nuevo->linea = linea;
    nuevo->columna = columna;
    nuevo->linea_definicion = 0;
    nuevo->columna_definicion = 0;
    nuevo->es_constante = es_constante;
    nuevo->esta_definida = 0;
    nuevo->tipos_parametros = NULL;

    // Insertar al principio de la tabla correspondiente
    if (dentro_funcion) {
        nuevo->siguiente = tabla_local;
        tabla_local = nuevo;
    } else {
        nuevo->siguiente = tabla;
        tabla = nuevo;
    }

    return 1;
}

int asignar_tipos_parametros(const char *nombre, const char *tipos) {
    Simbolo *simbolo = buscar_simbolo(nombre);
    if (simbolo == NULL || simbolo->clase != SIMBOLO_FUNCION)
        return 0;

    char *copia = strdup(tipos);
    if (copia == NULL)
        return -1;

    free(simbolo->tipos_parametros);
    simbolo->tipos_parametros = copia;

    return 1;
}

void registrar_parametros(Parametro *parametros) {
    for (Parametro *p = parametros; p != NULL; p = p->siguiente) {
        int resultado = agregar_simbolo(p->nombre, p->tipo, SIMBOLO_VARIABLE, p->linea, p->columna, 0);

        if (resultado == 0) { // El nombre ya existe en la tabla global 
            char mensaje[256];
            snprintf(mensaje, sizeof(mensaje), "Parametro redeclarado: %s", p->nombre);
            agregar_error_semantico(mensaje, p->linea, p->columna);
        }
        else if (resultado == -1) {
            fprintf(stderr, "Error al registrar el parametro: %s\n", p->nombre);
            exit(EXIT_FAILURE);
        }
    }
}

void registrar_variable(const char *nombre, const char *tipo, int linea, int columna, int es_constante) {
    int resultado = agregar_simbolo(nombre, tipo, SIMBOLO_VARIABLE, linea, columna, es_constante);

    if (resultado == 1)
        registrar_declaracion_variable_ts(nombre, tipo, linea, columna, es_constante);
    else if (resultado == 0) {
        char mensaje[512];
        Simbolo *existente = buscar_simbolo(nombre);

        if (existente != NULL && existente->clase == SIMBOLO_FUNCION) {
            snprintf(mensaje, sizeof(mensaje),
            "'%s' redeclarado como un tipo diferente de simbolo\n"
            "Nota: la declaracion previa de '%s' es de tipo '%s(%s)': %d:%d",
            nombre, nombre, existente->tipo,
            existente->tipos_parametros ? existente->tipos_parametros : "",
            existente->linea, existente->columna);
        }
        else if (existente != NULL && (strcmp(existente->tipo, tipo) != 0 ||
            existente->es_constante != es_constante)) {
            char tipo_nuevo[128];
            char tipo_anterior[128];

            snprintf(tipo_nuevo, sizeof(tipo_nuevo), "%s%s", es_constante ? "const " : "", tipo);

            snprintf(tipo_anterior, sizeof(tipo_anterior), "%s%s", existente->es_constante ? "const " : "",
             existente->tipo);

            snprintf(mensaje, sizeof(mensaje),
            "conflicto de tipos para '%s'; la ultima es de tipo '%s'\n"
            "Nota: la declaracion previa de '%s' es de tipo '%s': %d:%d",
            nombre, tipo_nuevo, nombre, tipo_anterior,
            existente->linea, existente->columna);
        }
        else if (existente != NULL) {
            snprintf(mensaje, sizeof(mensaje), "Redeclaracion de '%s'\n"
            "Nota: la declaracion previa de '%s' es de tipo '%s': %d:%d", nombre, nombre, existente->tipo,
            existente->linea, existente->columna);
        }
        else {
            fprintf(stderr, "Error: no se encontro el simbolo previo: %s\n", nombre);
            exit(EXIT_FAILURE);
        }

        agregar_error_semantico(mensaje, linea, columna);
    }
    else {
        fprintf(stderr, "Error al registrar la variable: %s\n", nombre);
        exit(EXIT_FAILURE);
    }
}

// Tabla local
void iniciar_ambito_local(void) {
    tabla_local = NULL;
    dentro_funcion = 1;
}

void finalizar_ambito_local(void) {
    if (!dentro_funcion)
        return;

    Simbolo *actual = tabla_local;

    while (actual != NULL) {
        Simbolo *siguiente = actual->siguiente;

        free(actual->nombre);
        free(actual->tipo);
        free(actual->tipos_parametros);
        free(actual);

        actual = siguiente;
    }

    tabla_local = NULL;
    dentro_funcion = 0;
}

// Liberar info.
void liberar_tabla_simbolos(void) {
    Simbolo *actual = tabla;

    while (actual != NULL) {
        Simbolo *siguiente = actual->siguiente;

        free(actual->nombre);
        free(actual->tipo);
        free(actual->tipos_parametros);
        free(actual);

        actual = siguiente;
    }

    tabla = NULL;
    RegistroDeclaracion *registro = declaraciones_inicio;

    while (registro != NULL) {
        RegistroDeclaracion *siguiente = registro->siguiente;

        free(registro->nombre);
        free(registro->tipo);
        free(registro->parametros);
        free(registro);

        registro = siguiente;
    }

    declaraciones_inicio = NULL;
    declaraciones_fin = NULL;
}

void liberar_parametros(Parametro *parametros) {
    while (parametros != NULL) {
        Parametro *siguiente = parametros->siguiente;
        free(parametros->nombre);
        free(parametros->tipo);
        free(parametros);
        parametros = siguiente;
    }
}