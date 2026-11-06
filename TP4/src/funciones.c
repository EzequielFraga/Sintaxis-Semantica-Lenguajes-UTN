#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "funciones.h"
#include "tabla_simbolos.h"
#include "reporte.h"

/* ================================================================
 * Auxiliares de memoria y cadenas
 * ================================================================ */

static void *reservar(size_t tam)
{
    void *p = calloc(1, tam);
    if (p == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static void *redimensionar(void *p, size_t tam)
{
    void *q = realloc(p, tam);
    if (q == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        exit(EXIT_FAILURE);
    }
    return q;
}

static char *copiar(const char *s)
{
    char *c;
    if (s == NULL)
        s = "";
    c = reservar(strlen(s) + 1);
    strcpy(c, s);
    return c;
}

/* Pool de cadenas: los tipos que devuelve este modulo viven hasta fn_liberar() */
typedef struct NodoCadena {
    char *texto;
    struct NodoCadena *siguiente;
} NodoCadena;

static NodoCadena *pool = NULL;

static const char *en_pool(const char *s)
{
    NodoCadena *n;
    if (s == NULL)
        s = "";
    for (n = pool; n != NULL; n = n->siguiente)
        if (strcmp(n->texto, s) == 0)
            return n->texto;
    n = reservar(sizeof(NodoCadena));
    n->texto = copiar(s);
    n->siguiente = pool;
    pool = n;
    return n->texto;
}

static int vacio(const char *s)
{
    return s == NULL || s[0] == '\0';
}

/* Arma el mensaje con formato printf y lo agrega al reporte */
static void error_semantico(int linea, int columna, const char *formato, ...)
{
    char buffer[1024];
    va_list args;

    va_start(args, formato);
    vsnprintf(buffer, sizeof(buffer), formato, args);
    va_end(args);

    agregar_error_semantico(buffer, linea, columna);
}

/* ================================================================
 * Compatibilidad de tipos
 * ================================================================ */

static int es_aritmetico(const char *tipo)
{
    static const char *palabras[] = {
        "char", "short", "int", "long", "float", "double", "signed", "unsigned", NULL
    };
    char palabra[32];
    const char *p = tipo;
    int i, n, ok;

    if (vacio(tipo) || strchr(tipo, '*') != NULL || strchr(tipo, '(') != NULL)
        return 0;

    while (*p != '\0') {
        while (*p == ' ')
            p++;
        if (*p == '\0')
            break;
        n = 0;
        while (*p != '\0' && *p != ' ' && n < (int)sizeof(palabra) - 1)
            palabra[n++] = *p++;
        palabra[n] = '\0';

        ok = 0;
        for (i = 0; palabras[i] != NULL; i++)
            if (strcmp(palabra, palabras[i]) == 0)
                ok = 1;
        if (!ok)
            return 0;
    }
    return 1;
}

static int es_puntero(const char *tipo)
{
    return !vacio(tipo) && strchr(tipo, '*') != NULL;
}

/* 1 si un valor de tipo 'origen' se puede usar donde se espera 'destino'.
 * Con tipo desconocido devuelve 1 para no generar errores en cascada. */
static int compatibles(const char *destino, const char *origen)
{
    if (vacio(destino) || vacio(origen))
        return 1;
    if (strcmp(destino, "void") == 0 || strcmp(origen, "void") == 0)
        return strcmp(destino, origen) == 0;
    if (es_aritmetico(destino) && es_aritmetico(origen))
        return 1;
    if (strcmp(destino, origen) == 0)
        return 1;
    if (es_puntero(destino) && es_puntero(origen) &&
        (strcmp(destino, "void *") == 0 || strcmp(origen, "void *") == 0))
        return 1;
    return 0;
}

/* ================================================================
 * Ubicaciones de identificadores (la TS no guarda la columna)
 * ================================================================ */

typedef struct Ubicacion {
    char *nombre;
    int linea;
    int columna;
    struct Ubicacion *siguiente;
} Ubicacion;

static Ubicacion *ubicaciones = NULL;

static Ubicacion *buscar_ubicacion(const char *nombre)
{
    Ubicacion *u;
    for (u = ubicaciones; u != NULL; u = u->siguiente)
        if (strcmp(u->nombre, nombre) == 0)
            return u;
    return NULL;
}

void fn_registrar_ubicacion(const char *nombre, int linea, int columna)
{
    Ubicacion *u;

    if (buscar_ubicacion(nombre) != NULL)   /* se queda con la primera, como la TS */
        return;

    u = reservar(sizeof(Ubicacion));
    u->nombre = copiar(nombre);
    u->linea = linea;
    u->columna = columna;
    u->siguiente = ubicaciones;
    ubicaciones = u;
}

/* ================================================================
 * Firmas de funciones
 * ================================================================ */

typedef struct {
    char *tipo;
    int tiene_nombre;
    int linea;
    int columna;
} Param;

typedef struct Firma {
    char *nombre;
    char *tipo_retorno;
    int cantidad;          /* cantidad real (0 para "(void)") */
    int sin_prototipo;     /* "()" -> no se controla la cantidad */
    Param *params;
    struct Firma *siguiente;
} Firma;

static Firma *firmas = NULL;

/* Parametros de la lista que se esta leyendo */
static Param *pendientes = NULL;
static int cant_pendientes = 0;

static Firma *buscar_firma(const char *nombre)
{
    Firma *f;
    for (f = firmas; f != NULL; f = f->siguiente)
        if (strcmp(f->nombre, nombre) == 0)
            return f;
    return NULL;
}

void fn_agregar_parametro(const char *tipo, int tiene_nombre, int linea, int columna)
{
    pendientes = redimensionar(pendientes, sizeof(Param) * (cant_pendientes + 1));
    pendientes[cant_pendientes].tipo = copiar(tipo);
    pendientes[cant_pendientes].tiene_nombre = tiene_nombre;
    pendientes[cant_pendientes].linea = linea;
    pendientes[cant_pendientes].columna = columna;
    cant_pendientes++;
}

static void descartar_pendientes(void)
{
    int i;
    for (i = 0; i < cant_pendientes; i++)
        free(pendientes[i].tipo);
    free(pendientes);
    pendientes = NULL;
    cant_pendientes = 0;
}

void fn_registrar_funcion(const char *nombre, const char *tipo_retorno, int linea, int columna)
{
    Firma *f;
    int es_void;

    fn_registrar_ubicacion(nombre, linea, columna);

    /* Se usa la primera declaracion */
    if (buscar_firma(nombre) != NULL) {
        descartar_pendientes();
        return;
    }

    f = reservar(sizeof(Firma));
    f->nombre = copiar(nombre);
    f->tipo_retorno = copiar(tipo_retorno);

    es_void = cant_pendientes == 1 && !pendientes[0].tiene_nombre &&
              strcmp(pendientes[0].tipo, "void") == 0;

    if (cant_pendientes == 0) {
        f->sin_prototipo = 1;
        descartar_pendientes();
    } else if (es_void) {
        descartar_pendientes();
    } else {
        f->cantidad = cant_pendientes;
        f->params = pendientes;      /* la firma se queda con el arreglo */
        pendientes = NULL;
        cant_pendientes = 0;
    }

    f->siguiente = firmas;
    firmas = f;
}

/* "float, unsigned long" o "void" */
static void texto_params(const Firma *f, char *buffer, size_t tam)
{
    int i;
    buffer[0] = '\0';
    if (f->cantidad == 0) {
        snprintf(buffer, tam, "%s", f->sin_prototipo ? "" : "void");
        return;
    }
    for (i = 0; i < f->cantidad; i++) {
        if (i > 0)
            strncat(buffer, ", ", tam - strlen(buffer) - 1);
        strncat(buffer, f->params[i].tipo, tam - strlen(buffer) - 1);
    }
}

/* ================================================================
 * Funcion que se esta definiendo (para los return)
 * ================================================================ */

static Firma *funcion_actual = NULL;

void fn_iniciar_definicion(const char *nombre, const char *tipo_retorno, int linea, int columna)
{
    fn_registrar_funcion(nombre, tipo_retorno, linea, columna);
    funcion_actual = buscar_firma(nombre);
}

void fn_finalizar_definicion(void)
{
    funcion_actual = NULL;
}

/* ================================================================
 * Expresiones e invocaciones
 * ================================================================ */

/* 1 si el nombre se refiere a una funcion. Manda la TS; si no esta en la
 * TS pero tiene firma, es una funcion cuya definicion se esta leyendo
 * (la TS la registra al terminar el cuerpo), por ejemplo en una recursion. */
static int es_funcion(const char *nombre, Simbolo **simbolo)
{
    Simbolo *s = buscar_simbolo(nombre);
    if (simbolo != NULL)
        *simbolo = s;
    if (s != NULL)
        return s->clase == SIMBOLO_FUNCION;
    return buscar_firma(nombre) != NULL;
}

const char *fn_tipo_expresion(const char *tipo, const char *nombre)
{
    Firma *f;
    char params[512];
    char buffer[640];

    if (!vacio(tipo))
        return en_pool(tipo);

    if (!vacio(nombre) && es_funcion(nombre, NULL) && (f = buscar_firma(nombre)) != NULL) {
        texto_params(f, params, sizeof(params));
        snprintf(buffer, sizeof(buffer), "%s (*)(%s)", f->tipo_retorno, params);
        return en_pool(buffer);
    }
    return "";
}

struct ListaArgumentos {
    int cantidad;
    const char **tipos;
    int *lineas;
    int *columnas;
};

ListaArgumentos *la_crear(void)
{
    return reservar(sizeof(ListaArgumentos));
}

void la_agregar(ListaArgumentos *la, const char *tipo, int linea, int columna)
{
    int n = la->cantidad + 1;
    la->tipos = redimensionar(la->tipos, sizeof(char *) * n);
    la->lineas = redimensionar(la->lineas, sizeof(int) * n);
    la->columnas = redimensionar(la->columnas, sizeof(int) * n);
    la->tipos[la->cantidad] = en_pool(tipo);
    la->lineas[la->cantidad] = linea;
    la->columnas[la->cantidad] = columna;
    la->cantidad = n;
}

void la_liberar(ListaArgumentos *la)
{
    if (la == NULL)
        return;
    free(la->tipos);
    free(la->lineas);
    free(la->columnas);
    free(la);
}

const char *fn_validar_llamada(const char *nombre, int linea, int columna,
                               const ListaArgumentos *args)
{
    Simbolo *s;
    Firma *f;
    Ubicacion *u;
    int cant_args = args != NULL ? args->cantidad : 0;
    int i;

    if (!es_funcion(nombre, &s)) {
        if (s == NULL) {
            error_semantico(linea, columna, "Funcion '%s' sin declarar", nombre);
            return "int";   /* C89: declaracion implicita como int */
        }
        u = buscar_ubicacion(nombre);
        error_semantico(linea, columna, "El objeto invocado '%s' no es una funcion o un puntero a una funcion\n"
                "Nota: declarado aqui: %d:%d",
                nombre,
                u != NULL ? u->linea : s->linea, u != NULL ? u->columna : 0);
        return "";
    }

    f = buscar_firma(nombre);
    if (f == NULL)
        return s != NULL ? en_pool(s->tipo) : "";

    u = buscar_ubicacion(nombre);

    if (!f->sin_prototipo) {
        if (cant_args < f->cantidad) {
            error_semantico(linea, columna, "Insuficientes argumentos para la funcion '%s'\n"
                "Nota: declarado aqui: %d:%d",
                nombre, u->linea, u->columna);
        } else if (cant_args > f->cantidad) {
            error_semantico(linea, columna, "Demasiados argumentos para la funcion '%s'\n"
                "Nota: declarado aqui: %d:%d",
                nombre, u->linea, u->columna);
        } else {
            for (i = 0; i < cant_args; i++) {
                if (!compatibles(f->params[i].tipo, args->tipos[i])) {
                    error_semantico(args->lineas[i], args->columnas[i],
                        "Incompatibilidad de tipos para el argumento %d de '%s'\n"
                        "Nota: se esperaba '%s' pero el argumento es de tipo '%s': %d:%d",
                        i + 1, nombre,
                        f->params[i].tipo, args->tipos[i],
                        f->params[i].linea, f->params[i].columna);
                }
            }
        }
    }

    return en_pool(f->tipo_retorno);
}

int fn_verificar_valor_void(const char *tipo, int linea, int columna)
{
    if (!vacio(tipo) && strcmp(tipo, "void") == 0) {
        error_semantico(linea, columna, "No se ignora el valor de retorno void como deberia ser");
        return 1;
    }
    return 0;
}

/* ================================================================
 * Return
 * ================================================================ */

void fn_validar_return_sin_valor(int linea, int columna)
{
    Ubicacion *u;

    if (funcion_actual == NULL || strcmp(funcion_actual->tipo_retorno, "void") == 0)
        return;

    u = buscar_ubicacion(funcion_actual->nombre);
    error_semantico(linea, columna, "'return' sin valor en una funcion que no retorna void\n"
                "Nota: declarado aqui: %d:%d",
                u->linea, u->columna);
}

void fn_validar_return_con_valor(const char *tipo, int linea, int columna)
{
    const char *esperado;

    if (funcion_actual == NULL || vacio(tipo))
        return;

    esperado = funcion_actual->tipo_retorno;

    /* return con valor en funcion void: el enunciado no lo pide (gcc da warning) */
    if (strcmp(esperado, "void") == 0)
        return;

    if (fn_verificar_valor_void(tipo, linea, columna))
        return;

    if (!compatibles(esperado, tipo))
        error_semantico(linea, columna, "Incompatibilidad de tipos al retornar el tipo '%s' pero se esperaba '%s'",
                tipo, esperado);
}

/* ================================================================
 * Liberacion
 * ================================================================ */

void fn_liberar(void)
{
    int i;

    while (firmas != NULL) {
        Firma *f = firmas;
        firmas = firmas->siguiente;
        for (i = 0; i < f->cantidad; i++)
            free(f->params[i].tipo);
        free(f->params);
        free(f->nombre);
        free(f->tipo_retorno);
        free(f);
    }
    while (ubicaciones != NULL) {
        Ubicacion *u = ubicaciones;
        ubicaciones = ubicaciones->siguiente;
        free(u->nombre);
        free(u);
    }
    while (pool != NULL) {
        NodoCadena *n = pool;
        pool = pool->siguiente;
        free(n->texto);
        free(n);
    }
    descartar_pendientes();
}