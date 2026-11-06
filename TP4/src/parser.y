/* Tp4 - Sintaxis y semantica de los lenguajes - utn frba*/

/* Inicio de la seccion de prólogo (declaraciones y definiciones de C y directivas del preprocesador) */
%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "general.h"
#include "reporte.h"
#include "tabla_simbolos.h"
#include "funciones.h"
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
static int constante_actual = 0; // 0: declaración de variable normal, 1: declaración de constante
static void asignar_tipo_actual (const char *texto){
        strncpy(tipo_actual, texto, sizeof(tipo_actual) -1); // al sizeof le restamos el barra cero
        tipo_actual[sizeof(tipo_actual) -1] = '\0';
}
%}
/* Fin de la sección de prólogo (declaraciones y definiciones de C y directivas del preprocesador) */

/* Estructura para almacenar informacion de los parametros */
%code requires {
    #include "tabla_simbolos.h"
    #include "funciones.h"

    typedef struct {
        char tipo[64];
        char nombre[64];
        int es_valor_l;
        int es_constante;
    } InfoExpresion;
}


/* Inicio de la sección de declaraciones de Bison */
%error-verbose
%locations
/* Tipo de valores semanticos que pueden portar los tokens/No terminales*/
%union {
    char *cadena;
    int entero;
    ListaParametros info_parametros;
    InfoExpresion info_expresion;
    ListaArgumentos *argumentos;
}
/* Funciones auxiliares para armar el valor semantico de las expresiones.
   Van en %code porque necesitan que InfoExpresion ya este definida */
%code {
    static InfoExpresion crear_expresion(const char *tipo, const char * nombre, int es_valor_l, int es_constante){
        InfoExpresion info;
        strncpy(info.tipo, tipo, sizeof(info.tipo)-1);
        info.tipo[sizeof(info.tipo)-1] = '\0';

        strncpy(info.nombre, nombre, sizeof(info.nombre)-1);
        info.nombre[sizeof(info.nombre) - 1] = '\0';

        info.es_valor_l = es_valor_l;
        info.es_constante = es_constante;

        return info;
    }
     /* verificar si el texto del tipo contiene el simbolo * */
       static int es_tipo_arimetico(const char *tipo){
        if (strchr(tipo, '*')!= NULL)
        return 0; // no, no es numerico
        return 1; // si, es numerico
       }

       static int son_tipos_compatibles(const char * destino, const char*origen){
        if(strcmp(destino,origen)==0){
            return 1;
        }
        if(es_tipo_arimetico(destino) && es_tipo_arimetico(origen))
        return 1;
        return 0;
       }
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
%nonassoc PREC_IDENTIFICADOR
%left '(' ')' '[' ']' '.' OP_FLECHA
/* No-Terminal inicio de axioma */
%start input

/*=================================================================== 
* 5 Tipos semanticos de no-terminales para declaraciones y funciones
* se indica que estos no-terminales sintetizan una cadena (Char*)
*para propagar (ints) y lista de parametros
* =================================================================== */ 
%type <cadena> especificador_tipo tipo_base
%type <cadena> cabecera_definicion_funcion
%type <info_parametros> lista_parametros lista_parametros_no_vacia parametro
%type <info_expresion> expresion expresion_asignacion expresion_condicional expresion_constante
%type <info_expresion> expresion_or expresion_and expresion_igualdad expresion_relacional
%type <info_expresion> expresion_aditiva expresion_multiplicativa expresion_unaria
%type <info_expresion> expresion_postfija expresion_primaria
%type <argumentos> lista_argumentos

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
        agregar_error_sintactico(obtener_linea_codigo(@1.first_line), @1.first_line);
    }
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
    : especificador_tipo lista_declaradores_variables {
        constante_actual = 0;
        free($1);
      }
    | CONST especificador_tipo {
        constante_actual = 1;
      }
      lista_declaradores_variables {
        constante_actual = 0;
        free($2);
      }
    ;

lista_declaradores_variables
 : declarador_variable
 | lista_declaradores_variables ',' declarador_variable
 ;

declarador_variable
    : IDENTIFICADOR {
        registrar_variable($1, tipo_actual, @1.first_line, @1.first_column, constante_actual);
        fn_registrar_ubicacion($1, @1.first_line, @1.first_column);
        free($1);
    }
    | IDENTIFICADOR '=' expresion {
        fn_verificar_valor_void($3.tipo, @3.first_line, @3.first_column);
        registrar_variable($1, tipo_actual, @1.first_line, @1.first_column, constante_actual);
        fn_registrar_ubicacion($1, @1.first_line, @1.first_column);
        if($3.tipo[0]!= '\0' && strcmp($3.tipo, "void") != 0 && !son_tipos_compatibles(tipo_actual, $3.tipo)){
            char mensaje[256];
            snprintf(mensaje, sizeof(mensaje),"Incompatibilidad de tipos al inicializar el tipo '%s' usando el tipo '%s'", tipo_actual, $3.tipo);
            agregar_error_semantico(mensaje, @3.first_line, @3.first_column);
        }
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
        fn_registrar_funcion($2, $1, @2.first_line, @2.first_column);

        int declaracion_valida = 1;
        int resultado = agregar_simbolo($2, $1, SIMBOLO_FUNCION, @2.first_line, @2.first_column, 0);
        if (resultado == 1) {
            if (asignar_tipos_parametros($2, $4.tipos) != 1) {
                fprintf(stderr, "Error al guardar los parametros de %s\n", $2);
                exit(EXIT_FAILURE);
            }
        }
        else if (resultado == 0) {
            Simbolo *existente = buscar_simbolo($2);
            if (existente->clase == SIMBOLO_VARIABLE) {
                char mensaje[512];

                snprintf(mensaje, sizeof(mensaje), "'%s' redeclarado como un tipo diferente de simbolo\n"
                "Nota: la declaracion previa de '%s' es de tipo '%s': %d:%d",
                $2, $2, existente->tipo,
                existente->linea, existente->columna);

                agregar_error_semantico(mensaje, @2.first_line, @2.first_column);
                declaracion_valida = 0;
            }
            else if (strcmp(existente->tipo, $1) != 0 || existente->tipos_parametros == NULL ||
            strcmp(existente->tipos_parametros, $4.tipos) != 0) {
                char mensaje[512];

                snprintf(mensaje, sizeof(mensaje),
                "conflicto de tipos para '%s'; la ultima es de tipo '%s(%s)'\n"
                "Nota: la declaracion previa de '%s' es de tipo '%s(%s)': %d:%d",
                $2, $1, $4.tipos,
                $2, existente->tipo,
                existente->tipos_parametros ? existente->tipos_parametros : "",
                existente->linea, existente->columna);

                agregar_error_semantico(mensaje, @2.first_line, @2.first_column);
                declaracion_valida = 0;
            }
        }
        else {
            fprintf(stderr, "Error al registrar la funcion: %s\n", $2);
            exit(EXIT_FAILURE);
        }

        if (declaracion_valida) {
            Simbolo *simbolo = buscar_simbolo($2);
            if (simbolo != NULL && !simbolo->esta_definida)
                registrar_declaracion_funcion_ts($2, $1, $4.descripcion, 0, @2.first_line);
        }

        free($1);
        free($2);
        free($4.descripcion);
        free($4.tipos);
        liberar_parametros($4.parametros);
    }
    ;

definicion_funcion
    : cabecera_definicion_funcion bloque_compuesto {
        fn_finalizar_definicion();
        finalizar_ambito_local();
        free($1);
    }
    ;

cabecera_definicion_funcion
    : especificador_tipo IDENTIFICADOR '(' lista_parametros ')' {
        fn_iniciar_definicion($2, $1, @2.first_line, @2.first_column);

        int definicion_valida = 1;
        Simbolo *existente = buscar_simbolo($2);
        if (existente == NULL) {
            int resultado = agregar_simbolo($2, $1, SIMBOLO_FUNCION, @2.first_line, @2.first_column, 0);
            if (resultado != 1 || asignar_tipos_parametros($2, $4.tipos) != 1) {
                fprintf(stderr, "Error al registrar la funcion: %s\n", $2);
                exit(EXIT_FAILURE);
            }

            existente = buscar_simbolo($2);
            existente->esta_definida = 1;
            existente->linea_definicion = @2.first_line;
            existente->columna_definicion = @2.first_column;
        }
        else if (existente->clase == SIMBOLO_VARIABLE) {
            definicion_valida = 0;
            char mensaje[512];

            snprintf(mensaje, sizeof(mensaje),
            "'%s' redeclarado como un tipo diferente de simbolo\n"
            "Nota: la declaracion previa de '%s' es de tipo '%s': %d:%d",
            $2, $2, existente->tipo,
            existente->linea, existente->columna);

            agregar_error_semantico(mensaje, @2.first_line, @2.first_column);
        }
        else if (existente->esta_definida) {
            char mensaje[512];

            snprintf(mensaje, sizeof(mensaje), "Redefinicion de '%s'\n"
            "Nota: la definicion previa de '%s' es de tipo '%s(%s)': %d:%d",
            $2, $2, existente->tipo,
            existente->tipos_parametros ? existente->tipos_parametros : "",
            existente->linea_definicion, existente->columna_definicion);

            agregar_error_semantico(mensaje, @2.first_line, @2.first_column);
            definicion_valida = 0;
        }
        else if (strcmp(existente->tipo, $1) != 0 || existente->tipos_parametros == NULL ||
         strcmp(existente->tipos_parametros, $4.tipos) != 0) {
            definicion_valida = 0;
            char mensaje[512];

            snprintf(mensaje, sizeof(mensaje),
            "conflicto de tipos para '%s'; la ultima es de tipo '%s(%s)'\n"
            "Nota: la declaracion previa de '%s' es de tipo '%s(%s)': %d:%d",
            $2, $1, $4.tipos,
            $2, existente->tipo,
            existente->tipos_parametros ? existente->tipos_parametros : "",
            existente->linea, existente->columna);

            agregar_error_semantico(mensaje, @2.first_line, @2.first_column);
        }
        else {
            existente->esta_definida = 1;
            existente->linea_definicion = @2.first_line;
            existente->columna_definicion = @2.first_column;
        }

        iniciar_ambito_local();
        registrar_parametros($4.parametros);
        if (definicion_valida)
            registrar_declaracion_funcion_ts($2, $1, $4.descripcion, 1, @2.first_line);
        

        $$ = strdup($2);
        if ($$ == NULL) {
            fprintf(stderr, "Error al reservar memoria.\n");
            exit(EXIT_FAILURE);
        }

        free($1);
        free($2);
        free($4.descripcion);
        free($4.tipos);
        liberar_parametros($4.parametros);
    }
    ;

lista_parametros
    : /* vacio */ {
        $$.descripcion = strdup("");
        $$.tipos = strdup("");
        $$.parametros = NULL; /* Inicializar lista parametros */
    }
    | lista_parametros_no_vacia {
        $$ = $1;
    }
    ;

lista_parametros_no_vacia
    : parametro {
        $$ = $1;
    }
    | lista_parametros_no_vacia ',' parametro {
        char descripcion[256];
        char tipos[256];

        snprintf(descripcion, sizeof(descripcion), "%s, %s", $1.descripcion, $3.descripcion);

        snprintf(tipos, sizeof(tipos), "%s, %s", $1.tipos, $3.tipos);

        $$.descripcion = strdup(descripcion);
        $$.tipos = strdup(tipos);
        $$.parametros = $1.parametros;

        if ($$.parametros == NULL)
            $$.parametros = $3.parametros; 
        else {
            Parametro *ultimo = $$.parametros;

            while (ultimo->siguiente != NULL)
                ultimo = ultimo->siguiente;

            ultimo->siguiente = $3.parametros;
        } /* Une los parametros */

        free($1.descripcion);
        free($1.tipos);
        free($3.descripcion);
        free($3.tipos);
    }
    ;

parametro
    : especificador_tipo IDENTIFICADOR {
        fn_agregar_parametro($1, 1, @2.first_line, @2.first_column); 
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%s %s", $1, $2);

        $$.descripcion = strdup(buffer);
        $$.tipos = strdup($1);
        Parametro *nuevo = malloc(sizeof(Parametro));
        if (nuevo == NULL) {
            fprintf(stderr, "Error al reservar memoria para parametro.\n");
            exit(EXIT_FAILURE);
        }

        nuevo->nombre = strdup($2);
        nuevo->tipo = strdup($1);
        nuevo->linea = @2.first_line;
        nuevo->columna = @2.first_column;
        nuevo->siguiente = NULL;

        $$.parametros = nuevo;

        free($1);
        free($2);
    }
    | especificador_tipo {
        fn_agregar_parametro($1, 0, @1.first_line, @1.first_column);
        $$.descripcion = strdup($1);
        $$.tipos = strdup($1);
        $$.parametros = NULL;
        free($1);
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
    ;

/* =============================================================
 * REGLAS PARA EXPRESIONES
 * ============================================================= */

expresion
    : expresion_asignacion
    ;

expresion_asignacion
    : expresion_condicional
    | expresion_unaria operador_asignacion expresion_asignacion {
        fn_verificar_valor_void($3.tipo, @2.first_line, @2.first_column);
        if (!$1.es_valor_l){
                agregar_error_semantico("Se requiere un valor-L modificable como operando izquierdo de la asignacion", @2.first_line, @2.first_column);
        }
        else if($1.es_constante){
              /* Es una variable, pero declarada const por lo tanto no se puede modificar */
                 char mensaje[256];
                 snprintf(mensaje, sizeof(mensaje), "Asignacion de la variable de solo lectura '%s'", $1.nombre);
                 agregar_error_semantico(mensaje, @2.first_line, @2.first_column);}
        /* la asignacion devuelve el tipo del operando del lado izquierdo que no es valor-l*/
         $$ = crear_expresion($1.tipo, "", 0, 0);
        }
    
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
    | expresion_or '?' expresion ':' expresion_condicional { $$ = crear_expresion($3.tipo, "", 0, 0);}
      /* recursividad a derecha: a ? b : (c ? d : e) */
    ;

expresion_constante
    : expresion_condicional
    ;

expresion_or
    : expresion_and
    | expresion_or OP_OR expresion_and   { $$ = crear_expresion("int", "", 0, 0); }
      /* recursividad a izquerda: (a || b) || c */
    ;

expresion_and
    : expresion_igualdad
    | expresion_and OP_AND expresion_igualdad  { $$ = crear_expresion("int", "", 0, 0); }
    ;

expresion_igualdad
    : expresion_relacional
    | expresion_igualdad OP_IGUAL expresion_relacional { $$ = crear_expresion("int", "", 0, 0); }
    | expresion_igualdad OP_DISTINTO expresion_relacional { $$ = crear_expresion("int", "", 0, 0); }
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
    : expresion_multiplicativa /*el resultado toma el tipo del operando izquierdo.
       La consigna indica que validar + y - es opcional */
    | expresion_aditiva '+' expresion_multiplicativa  { $$ = crear_expresion($1.tipo, "", 0, 0); }
    | expresion_aditiva '-' expresion_multiplicativa { $$ = crear_expresion($1.tipo, "", 0, 0); }
    ;

expresion_multiplicativa
    : expresion_unaria
    | expresion_multiplicativa '*' expresion_unaria  {
        /* Se valida solo si algun operando es un identificador*/
        int hay_identificador = ($1.nombre[0]!='\0') || ($3.nombre[0]!='\0');
        /* Tipo vacio  si es desconocido por un error anterior, no se reporta de nuevo */
          int hay_desconocido = ($1.tipo[0] == '\0') || ($3.tipo[0] == '\0');
        if(hay_identificador && !hay_desconocido && (!es_tipo_arimetico($1.tipo) || !es_tipo_arimetico($3.tipo))){
            char mensaje[256];
            snprintf(mensaje, sizeof(mensaje),
                "Operandos invalidos del operador binario * (tienen '%s' y '%s')",
             $1.tipo, $3.tipo);

             agregar_error_semantico(mensaje, @2.first_line, @2.first_column);
            $$ = crear_expresion("", "", 0, 0);
        }
        else{
             $$ = crear_expresion($1.tipo, "", 0, 0);   
        }
    }
    ;

expresion_unaria
    : expresion_postfija{
        if($1.tipo[0] == '\0' && $1.nombre[0] != '\0' && $1.es_valor_l){
             char mensaje[256];
            snprintf(mensaje, sizeof(mensaje), "'%s' sin declarar", $1.nombre);
            agregar_error_semantico(mensaje, @1.first_line, @1.first_column);
            $$ = crear_expresion("", "", 1, 0);
        }
        else{
            $$ = $1;
        }
    }
    | OP_INCREMENTO expresion_unaria  { $$ = crear_expresion($2.tipo, "", 0, 0); }
    | OP_DECREMENTO expresion_unaria { $$ = crear_expresion($2.tipo, "", 0, 0); }
    | operador_unario expresion_unaria { $$ = crear_expresion($2.tipo, "", 0, 0); }    /* Simplificacion: &, *, - y ! devuelven el tipo del operando,
                                                                                        consigna indica que validar los operadores unarios es opcional */
    | SIZEOF '(' nombre_tipo ')'  { $$ = crear_expresion("unsigned int", "", 0, 0); } /*le dicen al parser que ficha le corresponde*/
    ;

operador_unario
    : '&'
    | '*'
    | '-'
    | '!'
    ;

expresion_postfija
    : expresion_primaria
    | expresion_postfija  '[' expresion ']'
    /* invocacion a funcion por su nombre */
    | IDENTIFICADOR '(' ')' {
        $$ = crear_expresion(fn_validar_llamada($1, @1.first_line, @1.first_column, NULL), "", 0, 0);
        free($1);
      }
    | IDENTIFICADOR '(' lista_argumentos ')' {
        /* los argumentos se reducen antes, por eso sus errores salen primero */
        $$ = crear_expresion(fn_validar_llamada($1, @1.first_line, @1.first_column, $3), "", 0, 0);
        la_liberar($3);
        free($1);
      }
    | expresion_postfija   '(' ')' { $$ = crear_expresion("", "", 0, 0); }
    | expresion_postfija '(' lista_argumentos ')' { la_liberar($3); $$ = crear_expresion("", "", 0, 0); }
    | expresion_postfija OP_INCREMENTO { $$ = crear_expresion($1.tipo, "", 0, 0); }
    | expresion_postfija OP_DECREMENTO { $$ = crear_expresion($1.tipo, "", 0, 0); }
    ;


/* Cada argumento guarda su tipo y su ubicacion */
lista_argumentos
    : expresion_asignacion {
        $$ = la_crear();
        la_agregar($$, fn_tipo_expresion($1.tipo, $1.nombre), @1.first_line, @1.first_column);
      }
    | lista_argumentos ',' expresion_asignacion {
        la_agregar($1, fn_tipo_expresion($3.tipo, $3.nombre), @3.first_line, @3.first_column);
        $$ = $1;
      }
    ;

expresion_primaria
        : IDENTIFICADOR %prec PREC_IDENTIFICADOR { 
         Simbolo *simbolo = buscar_simbolo($1); 
        if (simbolo == NULL){
            $$ = crear_expresion("", $1, 1,0);
        }
        else if(simbolo->clase == SIMBOLO_FUNCION){
             char tipo_funcion[64];
             const char *parametros = (simbolo-> tipos_parametros !=NULL) ? simbolo->tipos_parametros : "";
              snprintf(tipo_funcion, sizeof(tipo_funcion), "%s (*)(%s)", simbolo->tipo, parametros);
               $$ = crear_expresion(tipo_funcion, $1, 0, 0);
        }
        else{
              $$ = crear_expresion(simbolo->tipo, $1, 1, simbolo->es_constante);
        }
        free($1);
    }
    | CONSTANTE_OCTAL   { $$ = crear_expresion("int", "", 0, 0); free($1); }  /*armamos la ficha, con el tipo de dato, nombre vacio pq una cte no es un identificador, primer 0 es valor_l, el segundo es_cte*/
    | CONSTANTE_DECIMAL   { $$ = crear_expresion("int", "", 0, 0); free($1); }
    | CONSTANTE_HEXA        { $$ = crear_expresion("int", "", 0, 0); free($1); }
    | CONSTANTE_REAL         { $$ = crear_expresion("double", "", 0, 0); free($1); }
    | CONSTANTE_CARACTER    { $$ = crear_expresion("int", "", 0, 0); free($1); }
    | LITERAL_CADENA        { $$ = crear_expresion("char *", "", 0, 0); free($1); }
    | '(' expresion ')' { $$ = $2; }  /* $$= $2 lo que hace es copiar la ficha completa sobre info de la expresion ( tipo, si es lvalue, si es const)*/
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
        fn_validar_return_sin_valor(@1.first_line, @1.first_column); /* INTEGRANTE 3 */
    }
    | RETURN expresion ';' {
        agregar_sentencia("return", @1.first_line, @1.first_column);
        fn_validar_return_con_valor(fn_tipo_expresion($2.tipo, $2.nombre),
                                    @2.first_line, @2.first_column); /* INTEGRANTE 3 */
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
    | error ';' {
        agregar_error_sintactico(obtener_linea_codigo(@1.first_line), @1.first_line);
        yyerrok;
      }
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
        fn_liberar();
        finalizar_ambito_local();
        liberar_tabla_simbolos();
        return 0;
}

/* Definición de la funcion yyerror para reportar errores, necesaria para que la funcion yyparse del analizador sintáctico pueda invocarla para reportar un error */
void yyerror(const char *literalCadena)
{
    (void)literalCadena;
}

/* Fin de la sección de epílogo (código de usuario) */
int yywrap(void) {
    return 1;
}