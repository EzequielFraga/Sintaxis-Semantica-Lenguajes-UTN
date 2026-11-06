#ifndef FUNCIONES_H
#define FUNCIONES_H

/*
 *
 * Validaciones semanticas de invocaciones a funciones y sentencias return.
 *
 * No modifica la tabla de simbolos: la usa para saber si un nombre es
 * variable o funcion, y guarda aparte (en este modulo) lo que la TS no tiene:
 *   - cantidad, tipo y ubicacion de cada parametro de cada funcion
 *   - linea:columna donde se declaro cada identificador (para las "Nota:")
 *
 * Los tipos se manejan como cadenas, igual que InfoExpresion.tipo
 * ("int", "char *", ...). La cadena vacia "" significa tipo desconocido.
 */

/* ---------- Declaraciones ---------- */

/* Llamar en cada regla 'parametro' (antes de reducir la lista). */
void fn_agregar_parametro(const char *tipo, int tiene_nombre, int linea, int columna);

/* Prototipo: llamar en declaracion_funcion. Usa los parametros acumulados. */
void fn_registrar_funcion(const char *nombre, const char *tipo_retorno, int linea, int columna);

/* Definicion: llamar en una accion entre ')' y bloque_compuesto, y
 * fn_finalizar_definicion() al terminar el bloque. */
void fn_iniciar_definicion(const char *nombre, const char *tipo_retorno, int linea, int columna);
void fn_finalizar_definicion(void);

/* Llamar en declarador_variable: guarda la ubicacion de la variable. */
void fn_registrar_ubicacion(const char *nombre, int linea, int columna);

/* ---------- Expresiones ---------- */

/* Tipo "real" de una expresion: si tipo esta vacio pero nombre es una
 * funcion, devuelve su tipo puntero ("void (*)(void)"). Si no, devuelve tipo. */
const char *fn_tipo_expresion(const char *tipo, const char *nombre);

typedef struct ListaArgumentos ListaArgumentos;

ListaArgumentos *la_crear(void);
void la_agregar(ListaArgumentos *la, const char *tipo, int linea, int columna);
void la_liberar(ListaArgumentos *la);

/* Valida nombre(args) y devuelve el tipo de retorno ("" si no se sabe).
 * args puede ser NULL. linea/columna: del identificador invocado. */
const char *fn_validar_llamada(const char *nombre, int linea, int columna,
                               const ListaArgumentos *args);

/* Si tipo es "void" informa "No se ignora el valor de retorno void..." y devuelve 1 */
int fn_verificar_valor_void(const char *tipo, int linea, int columna);

/* ---------- Return ---------- */

void fn_validar_return_sin_valor(int linea, int columna);
void fn_validar_return_con_valor(const char *tipo, int linea, int columna);

void fn_liberar(void);

#endif /* FUNCIONES_H */