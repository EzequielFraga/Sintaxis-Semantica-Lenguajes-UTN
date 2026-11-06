#ifndef TABLA_SIMBOLOS_H
#define TABLA_SIMBOLOS_H

typedef enum {
    SIMBOLO_VARIABLE,
    SIMBOLO_FUNCION
} ClaseSimbolo;

typedef struct Simbolo {
    char *nombre;
    char *tipo;
    int linea;
    int columna;
    int linea_definicion;
    int columna_definicion;
    int es_constante;
    int esta_definida;
    char *tipos_parametros;
    ClaseSimbolo clase;
    struct Simbolo *siguiente;
} Simbolo;

// Estructura para almacenar informacion de los parametros
typedef struct Parametro { // Un parámetro solo de una función
    char *nombre;
    char *tipo;
    int linea;
    int columna;
    struct Parametro *siguiente;
} Parametro;

typedef struct { // Una lista de esa función
    char *descripcion;
    char *tipos;
    Parametro *parametros;
} ListaParametros;

typedef struct RegistroDeclaracion {
    ClaseSimbolo clase;
    char *nombre;
    char *tipo;
    int linea;
    int columna;
    int es_constante;
    
    // Solo para funciones
    char *parametros;
    int es_definicion;
    struct RegistroDeclaracion *siguiente;
} RegistroDeclaracion;

// Prototipos 
Simbolo *buscar_simbolo(const char *nombre);
int agregar_simbolo(const char *nombre, const char *tipo, ClaseSimbolo clase, int linea, int columna, 
    int es_constante);
int asignar_tipos_parametros(const char *nombre, const char *tipos);
void liberar_tabla_simbolos(void);
void registrar_parametros(Parametro *parametros);
void liberar_parametros(Parametro *parametros);
void iniciar_ambito_local(void);
void finalizar_ambito_local(void);
void registrar_variable(const char *nombre, const char *tipo, int linea, int columna, int es_constante);
void registrar_declaracion_variable_ts(const char *nombre, const char *tipo, int linea, int columna, 
    int es_constante); // Registro de declaraciones para el reporte
void registrar_declaracion_funcion_ts(const char *nombre, const char *tipo_retorno, const char *parametros, 
    int es_definicion, int linea);

const RegistroDeclaracion *obtener_declaraciones_ts(void);

#endif