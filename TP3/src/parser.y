/* Tp3 - Sintaxis y semantica de los lenguajes - utn frba*/

/* Inicio de la seccion de prólogo (declaraciones y definiciones de C y directivas del preprocesador) */
%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "general.h"
#include "reporte.h"

	/* Declaración de la funcion yylex del analizador léxico, necesaria para que la funcion yyparse del analizador sintáctico pueda invocarla cada vez que solicite un nuevo token */
extern int yylex(void);
    /* Declaración de la función, necesaria para obtener linea leida, utilizada para mostrar en reporte errores sintácticos */
extern const char *obtener_linea_actual(void);
	/* Declaracion de la función yyerror para reportar errores, necesaria para que la función yyparse del analizador sintáctico pueda invocarla para reportar un error */
void yyerror(const char*);

/* Puntero al archivo de entrada que maneja Flex */
extern FILE *yyin;
/* Variable para depuración de Bison (si está activa) */
#if YYDEBUG
extern int yydebug;
#endif
// Seccion la cual guarda temporalmente el tipo de dato para propagar a todas las variables declarada en una misma linea 
#if YYDEBUG    
extern int yydebug; // consultamos si yydebug fue activada al compilar
#endif
/*variables y funciones auxiliares para asociar al tipo de dato a multiples variables declaradas en la misma linea*/
static char tipo_actual[64] = ""; // memoria temporal  la cual retiene el texto de tipo de dato mientras bison lee variables declaradas en una misma linea
static void asignar_tipo_actual (const char *texto){
        strncpy(tipo_actual, texto, sizeof(tipo_actual) -1); // al sizeof le restamos el barra cero
        tipo_actual[sizeof(tipo_actual) -1] = '\0';
}
static void registrar_variable(const char *nombre, int linea){
        agregar_variable_descartada (nombre, tipo_actual, linea);
}
%}
/* Fin de la sección de prólogo (declaraciones y definiciones de C y directivas del preprocesador) */

/* Inicio de la sección de declaraciones de Bison */
%error-verbose
%locations
/* Tipo de valores semanticos que pueden portar los tokens/No terminales*/
%union {
        char *cadena;
        int entero;
}
/* =================================================================== 
 * 1 Palabras reservadas (Tokens Individuales)
 * =================================================================== */ 
 %token CHAR INT FLOAT DOUBLE VOID SHORT LONG SIGNED UNSIGNED SIZEOF
%token IF ELSE SWITCH CASE DEFAULT WHILE DO FOR CONTINUE BREAK RETURN GOTO
%token CONST VOLATILE STRUCT UNION ENUM AUTO REGISTER STATIC EXTERN TYPEDEF

/* =================================================================== 
* 2 Identificadores y constantes 
*=================================================================== */ 
%token <cadena> IDENTIFICADOR
%token <cadena> CONSTANTE_OCTAL CONSTANTE_DECIMAL CONSTANTE_HEXA
%token <cadena> CONSTANTE_REAL CONSTANTE_CARACTER LITERAL_CADENA

/* =================================================================== 
* 3 Operadores compuestos 
* =================================================================== */ 
%token OP_INCREMENTO OP_DECREMENTO
%token OP_IGUAL OP_DISTINTO OP_MENOR_IGUAL OP_MAYOR_IGUAL
%token OP_AND OP_OR
%token OP_FLECHA OP_SHL OP_SHR
%token MAS_IGUAL MENOS_IGUAL POR_IGUAL DIV_IGUAL MOD_IGUAL
%token AND_IGUAL XOR_IGUAL OR_IGUAL SHL_IGUAL SHR_IGUAL
%token ELLIPSIS

/*=================================================================== 
* 4 Precedencia y asociatividad (De menor a mayor precendecia) 
*=================================================================== */ 
%nonassoc PREC_IF
%nonassoc ELSE
%right '=' MAS_IGUAL MENOS_IGUAL POR_IGUAL DIV_IGUAL MOD_IGUAL AND_IGUAL XOR_IGUAL OR_IGUAL SHL_IGUAL SHR_IGUAL
%left  OP_OR
%left  OP_AND
%left  '|'
%left  '^'
%left  '&'
%left  OP_IGUAL OP_DISTINTO
%left  '<' '>' OP_MENOR_IGUAL OP_MAYOR_IGUAL
%left  OP_SHL OP_SHR
%left  '+' '-'
%left  '*' '/' '%'
%right OP_INCREMENTO OP_DECREMENTO '!' '~' SIZEOF
%left  '(' ')' '[' ']' '.' OP_FLECHA
/* No-Terminal inicio de axioma */
%start input 

/*=================================================================== 
* 5 Tipos semanticos de no-terminales para declaraciones y funciones
* se indica que estos no-terminales sintetizan una cadena (Char*)
*para propagar (ints) y lista de parametros
* =================================================================== */ 
%type <cadena> especificador_tipo tipo_base
%type <cadena> lista_parametros lista_parametros_no_vacia parametro

/* Fin de la sección de declaraciones de Bison */

/* Inicio de la sección de reglas gramaticales */
%%

input
        : /* archivo vacio */
        | lista_definiciones_externas
        ;
lista_definiciones_externas
       : definicion_externa
       | lista_definiciones_externas definicion_externa
       ;
       
definicion_externa
    : ';'
    | declaracion_variable ';'
    | declaracion_funcion ';'
    | definicion_funcion
    | error ';' {
        /* captura y sincronizacion basica ante errores sintacticos */
        agregar_error_sintactico(obtener_linea_actual(), @1.first_line); // @1.first_line indica numero de linea donde empezo el primer elemento de esa regla
      }
    ;
    /* =============================================================
 *           REGLAS PARA TIPOS DE DATOS
 * ============================================================= */


tipo_base
    :INT                            { $$ = strdup("int"); } // reservo memoria dinamica, copio el tipo de dato y $$ almacena puntero a dicha cadena
    | CHAR                          { $$ = strdup("char"); }
    | FLOAT                         { $$ = strdup("float"); }
    | DOUBLE                        { $$ = strdup("double"); }
    | VOID                          { $$ = strdup("void"); }
    | SHORT                         { $$ = strdup("short"); }
    | LONG                          { $$ = strdup("long"); }
    | UNSIGNED                      { $$ = strdup("unsigned int"); }
    ;

/* =============================================================
 *          REGLAS PARA DECLARACION DE VARIABLES
 * ============================================================= */

 /* al leer el tipo, lo guardara en tipo_actual para toda la linea*/
 declaracion_variable
   : especificador_tipo lista_declaradores_variables 
   ;
lista_declaradores_variables
 : declarador_variable
 | lista_declaradores_variables ',' declarador_variable
 ;

 declarador_variable 
  : IDENTIFICADOR {
        registrar_variable ($1, @1.first_line);
        free($1);
  }
  | IDENTIFICADOR '=' expresion {
        registrar_variable($1, @1.first_line);
        free($1);
    }
    ; 
especificador_tipo
   : tipo_base { 
        asignar_tipo_actual($1);
        $$ = $1; 
   }
   | UNSIGNED tipo_base {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "unsigned %s", $2);
        free($2);
        asignar_tipo_actual(buffer);
        $$ = strdup(buffer);
   }
   | SIGNED tipo_base {
        char buffer[64]; 
        snprintf(buffer, sizeof(buffer), "signed %s", $2);
        free($2);
        asignar_tipo_actual(buffer);
        $$ = strdup(buffer);        
   }
   ;

/* =============================================================
 * REGLAS PARA FUNCIONES (DECLARACIONES Y DEFINICIONES)
 * ============================================================= */
 declaracion_funcion
   : especificador_tipo IDENTIFICADOR '(' lista_parametros ')' {
        agregar_funcion($2, $1, $4, 0, @2.first_line);
        free($1);
        free($2);
        free($4);
 }
 ;
definicion_funcion 
: especificador_tipo IDENTIFICADOR  '(' lista_parametros ')' bloque_compuesto {
        agregar_funcion($2, $1, $4, 1, @2.first_line);
        free($1);
        free($2);
        free($4);
} 
;

lista_parametros 
: /* aca iria el vacio*/                   {$$ = strdup("");}                               
|lista_parametros_no_vacia                   {$$ = $1;}
;

lista_parametros_no_vacia
    : parametro                     { $$ = $1; }
    | lista_parametros_no_vacia ',' parametro {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s, %s", $1, $3);
        free($1);
        free($3);
        $$ = strdup(buffer);
    }
    ;
parametro
    : especificador_tipo IDENTIFICADOR {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%s %s", $1, $2);
        free($1);
        free($2);
        $$ = strdup(buffer);
    }
    | especificador_tipo {
        $$ = $1;
    }
    ;

bloque_compuesto
    : '{' lista_elementos_bloque '}'
    | '{' '}'
    ;

lista_elementos_bloque
    : elemento_bloque
    | lista_elementos_bloque elemento_bloque
    ;

elemento_bloque
    : declaracion_variable ';'
    | sentencia  
    | error ';'
    ;

/* =============================================================
 * REGLAS PARA EXPRESIONES
 * ============================================================= */

expresion
    : expresion_asignacion
    ;

expresion_asignacion
    : expresion_condicional
    | expresion_unaria operador_asignacion expresion_asignacion
      /* recursividad a derecha: a = (b = 3) */
    ;

operador_asignacion
    : '='
    | MAS_IGUAL
    | MENOS_IGUAL
    | POR_IGUAL
    | DIV_IGUAL
    | MOD_IGUAL
    | AND_IGUAL
    | XOR_IGUAL
    | OR_IGUAL
    | SHL_IGUAL
    | SHR_IGUAL
    ;

expresion_condicional
    : expresion_or
    | expresion_or '?' expresion ':' expresion_condicional
      /* recursividad a derecha: a ? b : (c ? d : e) */
    ;

expresion_constante
    : expresion_condicional
    ;

expresion_or
    : expresion_and
    | expresion_or OP_OR expresion_and
      /* recursividad a izquerda: (a || b) || c */
    ;

expresion_and
    : expresion_igualdad
    | expresion_and OP_AND expresion_igualdad
    ;

expresion_igualdad
    : expresion_relacional
    | expresion_igualdad OP_IGUAL expresion_relacional
    | expresion_igualdad OP_DISTINTO expresion_relacional
    ;

expresion_relacional
    : expresion_aditiva
    | expresion_relacional operador_relacional expresion_aditiva
    ;

operador_relacional
    : OP_MAYOR_IGUAL
    | OP_MENOR_IGUAL
    | '<'
    | '>'
    ;

expresion_aditiva
    : expresion_multiplicativa
    | expresion_aditiva '+' expresion_multiplicativa
    | expresion_aditiva '-' expresion_multiplicativa
    ;

expresion_multiplicativa
    : expresion_unaria
    | expresion_multiplicativa '*' expresion_unaria
    ;

expresion_unaria
    : expresion_postfija
    | OP_INCREMENTO expresion_unaria
    | OP_DECREMENTO expresion_unaria
    | operador_unario expresion_unaria
    | SIZEOF '(' nombre_tipo ')'
    ;

operador_unario
    : '&'
    | '*'
    | '-'
    | '!'
    ;

expresion_postfija
    : expresion_primaria
    | expresion_postfija '[' expresion ']'
    | expresion_postfija '(' ')'
    | expresion_postfija '(' lista_argumentos ')'
    | expresion_postfija OP_INCREMENTO
    | expresion_postfija OP_DECREMENTO
    ;

lista_argumentos
    : expresion_asignacion
    | lista_argumentos ',' expresion_asignacion
    ;

expresion_primaria
    : IDENTIFICADOR
    | CONSTANTE_OCTAL
    | CONSTANTE_DECIMAL
    | CONSTANTE_HEXA
    | CONSTANTE_REAL
    | CONSTANTE_CARACTER
    | LITERAL_CADENA
    | '(' expresion ')'
    ;

nombre_tipo
    : CHAR
    | INT
    | FLOAT
    | DOUBLE
    ;

/* =============================================================
 *          REGLAS PARA SENTENCIAS
 * ============================================================= */

sentencia
    : sentencia_compuesta
    | sentencia_expresion
    | sentencia_seleccion
    | sentencia_iteracion 
    | sentencia_salto
    | sentencia_etiquetada
    ;

sentencia_salto
    : CONTINUE ';' {
        agregar_sentencia("continue", @1.first_line, @1.first_column);
    }
    | BREAK ';' {
        agregar_sentencia("break", @1.first_line, @1.first_column);
    }
    | RETURN ';' {
        agregar_sentencia("return", @1.first_line, @1.first_column);
    }
    | RETURN expresion ';' {
        agregar_sentencia("return", @1.first_line, @1.first_column);
    }
    ;

sentencia_seleccion
    : IF '(' expresion ')' sentencia %prec PREC_IF
      {
          agregar_sentencia("if", @1.first_line, @1.first_column);
      }
    | IF '(' expresion ')' sentencia ELSE sentencia
      {
          agregar_sentencia("if/else", @1.first_line, @1.first_column);
      }
    | SWITCH '(' expresion ')' sentencia 
      {
          agregar_sentencia("switch", @1.first_line, @1.first_column);
      }
    ;

sentencia_etiquetada
    : CASE expresion_constante ':' {agregar_sentencia ("case", @1.first_line, @1.first_column);} sentencia
    | DEFAULT ':' { agregar_sentencia("default", @1.first_line, @1.first_column); } sentencia
    ;

sentencia_expresion
    : ';'
    | expresion ';'
    ;

sentencia_compuesta
    : bloque_compuesto
    ;

/* =============================================================
 *          REGLAS PARA SENTENCIAS ITERACION
 * ============================================================= */

 sentencia_iteracion
    : WHILE { agregar_sentencia("while", @1.first_line, @1.first_column); } '(' expresion ')' sentencia
    | DO { agregar_sentencia("do/while", @1.first_line, @1.first_column); } sentencia WHILE '(' expresion ')' ';'
    | FOR { agregar_sentencia("for", @1.first_line, @1.first_column); } '(' expresion_opcional ';' expresion_opcional ';' expresion_opcional ')' sentencia
    ;

 expresion_opcional
 : /*Vacio*/
 | expresion
 ;


  %%
/* Fin de la sección de reglas gramaticales */



/* Inicio de la sección de epílogo (código de usuario) */

int main(int argc, char *argv[]) {
        if(argc <2){
                fprintf(stderr, "Uso: %s <ruta archivo entrada>\n" , argv[0]);
                return 1;
        }

        FILE * archivo= fopen(argv[1], "r");
        if(!archivo){
                perror("Error al abrir el archivo de entrada");
        return 1;
        }

        yyin = archivo;
        inicializarUbicacion();
        #if YYDEBUG
        yydebug = 0;
        #endif
        init_reporte();
        yyparse();
        fclose(archivo);
        generar_reporte_final();
        liberar_memoria();
        return 0;
}

	/* Definición de la funcion yyerror para reportar errores, necesaria para que la funcion yyparse del analizador sintáctico pueda invocarla para reportar un error */
void yyerror(const char* literalCadena) {
        fprintf(stderr, "Bison: %d:%d: %s\n", yylloc.first_line, yylloc.first_column, literalCadena);
}

/* Fin de la sección de epílogo (código de usuario) */
int yywrap(void) {
    return 1;
}