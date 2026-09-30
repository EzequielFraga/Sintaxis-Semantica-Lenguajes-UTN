#ifndef REPORTE_H 
#define REPORTE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int linea;
  int columna;
} Posicion;

typedef struct {
  char* nombre;
  int cantidad;
} Identificador;

typedef struct {
  char* texto;
  int longitud;
  int orden_aparicion;
} LiteralCadena;

typedef enum {
  CAT_ALMACENAMIENTO,
  CAT_TIPO,
  CAT_CALIFICADOR,
  CAT_STRUCT_UNION,
  CAT_ENUM,
  CAT_ETIQUETA,
  CAT_SELECCION,
  CAT_ITERACION,
  CAT_SALTO,
  CAT_UNARIO
} CategoriaPalabra;

typedef struct {
  char* palabra;
  Posicion pos;
  CategoriaPalabra categoria;
} PalabraReservada;

typedef enum {
  CONST_DECIMAL,
  CONST_HEXA,
  CONST_OCTAL,
  CONST_REAL,
  CONST_CARACTER
} TipoConstante;

typedef struct {
  TipoConstante tipo;
  char* texto_original;
  long valor_entero;
  double valor_real;
  int orden;
} Constante;

typedef struct {
  char* op;
  int cantidad;
  int orden_aparicion;
} Operador;

typedef struct {
  char* cadena;
  Posicion pos;
} CadenaNoReconocida;

void init_reporte(void);
void agregar_identificador(const char* nombre);
void agregar_literal_cadena(const char* texto);
void agregar_palabra_reservada(const char* palabra, CategoriaPalabra cat,
                               int linea, int columna);
void agregar_constante_decimal(const char* texto);
void agregar_constante_hexa(const char* texto);
void agregar_constante_octal(const char* texto);
void agregar_constante_real(const char* texto);
void agregar_constante_caracter(const char* texto);
void agregar_operador(const char* op);
void agregar_no_reconocido(const char* cadena, int linea, int columna);
void generar_reporte_final(void);
void liberar_memoria(void);

#endif