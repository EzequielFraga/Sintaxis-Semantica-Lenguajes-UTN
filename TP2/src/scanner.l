
        /* Inicio de la sección de definiciones. Aquí: */
        /* 1. Cualquier texto sangrado o encerrado entre '%{' y '%}' (estos últimos dos sin sangrar en líneas ocupadas únicamente por ellos) se copia íntegramente al archivo de salida del analizador léxico (scanner) generado (*.lex.yy.c) */
        /* De esas formas podemos poner declaraciones y definiciones de C y directivas del preprocesador */
        /* 2. Los comentarios (sólo hay de múltiples líneas) sin sangría también se copian tal cual al archivo de salida del analizador léxico (scanner) generado (*.lex.yy.c) */
%{

#include <stdio.h>
#include "reporte.h"
int num_linea = 1;
int num_columna = 1;

#define YY_USER_ACTION \
  { int i; for (i = 0; yytext[i] != '\0'; i++) { \
      if (yytext[i] == '\n') { num_linea++; num_columna = 1; } \
      else { num_columna++; } \
    } \
  }
%}
%option noyywrap
        /* La siguiente línea define explícitamente a yytext como un puntero a un caracter ( char* yytext; ). Ésta es la opción por defecto si no se pone nada. */
%pointer
        /* Con la siguiente opción la función de análisis int yylex(void); se comporta como si en la sección de usuario existiera la función int yywrap(void) return 1; la cual devuelve verdadero (no-cero), haciendo que una vez que la función de análisis int yylex(void) reciba una indicación de EOF desde YY_INPUT, asuma que no hay nada más para analizar de yyin y finalice (haciendo un return 0;) */
%option noyywrap

    /* Condiciones de arranque inclusivas */
    /* %s */
    /* Condiciones de arranque exclusivas */
    /* %x */
    /* Declaraciones con cada nombre y su definición; se utilizan para simplificar las reglas */
digito       [0-9]
exponente    ([eE][+-]?{digito}+)
sufijo_real  [fFlL]
    /* Fin de la sección de definiciones */
%%


        /* Esto podría utilizarse para declarar variables... */
        /* 2. NO pueden hacerse comentarios sin sangría... */
        /* constantes numericas */
({digito}+\.{digito}*|\.{digito}+){exponente}?{sufijo_real}? |
{digito}+{exponente}{sufijo_real}? {
    agregar_constante_real(yytext);
}

0[xX][0-9a-fA-F]+([uU](l|L|ll|LL)?|(l|L|ll|LL)[uU]?)? {
    agregar_constante_hexa(yytext);
}

0[0-7]*([uU](l|L|ll|LL)?|(l|L|ll|LL)[uU]?)? {
    agregar_constante_octal(yytext);
}

[1-9][0-9]*([uU](l|L|ll|LL)?|(l|L|ll|LL)[uU]?)? {
    agregar_constante_decimal(yytext);
}
            /* Palabras reservadas */
"auto" {
    agregar_palabra_reservada(yytext, CAT_ALMACENAMIENTO,num_linea,num_columna - yyleng);
}

"register" {
    agregar_palabra_reservada(yytext, CAT_ALMACENAMIENTO,num_linea,num_columna - yyleng);
}

"static" {
    agregar_palabra_reservada(yytext, CAT_ALMACENAMIENTO,num_linea,num_columna - yyleng);
}

"extern" {
    agregar_palabra_reservada(yytext, CAT_ALMACENAMIENTO,num_linea,num_columna - yyleng);
}

"typedef" {
    agregar_palabra_reservada(yytext, CAT_ALMACENAMIENTO,num_linea,num_columna - yyleng);
}

"void" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}

"char" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}
"short" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}

"int" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}

"long" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}

"float" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}

"double" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}

"signed" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}

"unsigned" {
    agregar_palabra_reservada(yytext, CAT_TIPO,num_linea,num_columna - yyleng);
}

"const" {
    agregar_palabra_reservada(yytext, CAT_CALIFICADOR,num_linea,num_columna - yyleng);
}

"volatile" {
    agregar_palabra_reservada(yytext, CAT_CALIFICADOR,num_linea,num_columna - yyleng);
}

"struct" {
    agregar_palabra_reservada(yytext, CAT_STRUCT_UNION,num_linea,num_columna - yyleng);
}

"union" {
    agregar_palabra_reservada(yytext, CAT_STRUCT_UNION,num_linea,num_columna - yyleng);
}

"enum" {
    agregar_palabra_reservada(yytext, CAT_ENUM,num_linea,num_columna - yyleng);
}

"case" {
    agregar_palabra_reservada(yytext, CAT_ETIQUETA,num_linea,num_columna - yyleng);
}

"default" {
    agregar_palabra_reservada(yytext, CAT_ETIQUETA,num_linea,num_columna - yyleng);
}

"if" {
    agregar_palabra_reservada(yytext, CAT_SELECCION,num_linea,num_columna - yyleng);
}

"else" {
    agregar_palabra_reservada(yytext, CAT_SELECCION,num_linea,num_columna - yyleng);
}

"switch" {
    agregar_palabra_reservada(yytext, CAT_SELECCION,num_linea,num_columna - yyleng);
}

"do" {
    agregar_palabra_reservada(yytext, CAT_ITERACION,num_linea,num_columna - yyleng);
}

"while" {
    agregar_palabra_reservada(yytext, CAT_ITERACION,num_linea,num_columna - yyleng);
}

"for" {
    agregar_palabra_reservada(yytext, CAT_ITERACION,num_linea,num_columna - yyleng);
}

"goto" {
    agregar_palabra_reservada(yytext, CAT_SALTO,num_linea,num_columna - yyleng);
}

"continue" {
    agregar_palabra_reservada(yytext, CAT_SALTO,num_linea,num_columna - yyleng);
}

"break" {
    agregar_palabra_reservada(yytext, CAT_SALTO,num_linea,num_columna - yyleng);
}

"return" {
    agregar_palabra_reservada(yytext, CAT_SALTO,num_linea,num_columna - yyleng);
}

"sizeof" {
    agregar_palabra_reservada(yytext, CAT_UNARIO,num_linea,num_columna - yyleng);
}
        /*---- Identificadores----*/
[_a-zA-Z]([_a-zA-Z]|[0-9])* { agregar_identificador(yytext); }

        /*----Operadores / Caracteres de puntuación---- */  
";" {
    agregar_operador(yytext);
}
"{" {
    agregar_operador(yytext);
}
"}" {
    agregar_operador(yytext);
}
"(" {
    agregar_operador(yytext);
}
")" {
    agregar_operador(yytext);
}
"[" {
    agregar_operador(yytext);
}
"]" {
    agregar_operador(yytext);
}
"," {
    agregar_operador(yytext);
}
":" {
    agregar_operador(yytext);
}
"..." {
    agregar_operador(yytext);
}
"." {
    agregar_operador(yytext);
}
"?" {
    agregar_operador(yytext);
}
"~" {
    agregar_operador(yytext);
}
"++" {
    agregar_operador(yytext);
}
"+=" {
    agregar_operador(yytext);
}
"+" {
    agregar_operador(yytext);
}
"--" {
    agregar_operador(yytext);
}
"-=" {
    agregar_operador(yytext);
}
"->" {
    agregar_operador(yytext);
}
"-" {
    agregar_operador(yytext);
}
"*=" {
    agregar_operador(yytext);
}
"*" {
    agregar_operador(yytext);
}
"/=" {
    agregar_operador(yytext);
}
"/" {
    agregar_operador(yytext);
}
"%=" {
    agregar_operador(yytext);
}
"%" {
    agregar_operador(yytext);
}
"==" {
    agregar_operador(yytext);
}
"!=" {
    agregar_operador(yytext);
}
"<=" {
    agregar_operador(yytext);
}
"<<=" {
    agregar_operador(yytext);
}
"<<" {
    agregar_operador(yytext);
}
"<" {
    agregar_operador(yytext);
}
">>=" {
    agregar_operador(yytext);
}
">=" {
    agregar_operador(yytext);
}
">>" {
    agregar_operador(yytext);
}
">" {
    agregar_operador(yytext);
}
"&&" {
    agregar_operador(yytext);
}
"&=" {
    agregar_operador(yytext);
}
"&" {
    agregar_operador(yytext);
}
"||" {
    agregar_operador(yytext);
}
"|=" {
    agregar_operador(yytext);
}
"|" {
    agregar_operador(yytext);
}
"!" {
    agregar_operador(yytext);
}
"=" {
    agregar_operador(yytext);
}
"^=" {
    agregar_operador(yytext);
}
"^" {
    agregar_operador(yytext);
}

                 /*----Ignorar espacios, tabs y saltos de línea -----*/
[ \t\r\n]+   ;

                /* Constanes caracter */

'([^'\\\n]|\\.)*' {
    agregar_constante_caracter(yytext);
}
             /*----Literales cadena-----*/

\"([^"\\\n]|\\.)*\" {
    agregar_literal_cadena(yytext);
}

            /*---- Cadenas no reconocidas y reglas----*/
[^ \t\r\n;{}()\[\],:.?~+\-*/%=<>&|!^"'\\]+ {
    agregar_no_reconocido(yytext, num_linea, num_columna - yyleng);
}

        /* Reglas por defecto de Flex: */
        /* <*>.|\n { ECHO; } */
        /* (ECHO; escribe yytext en la salida del escáner: FILE* yyout (variable global a la cual por defecto se le asigna stdout)) */
        /* <<EOF>> { yyterminate(); } */
        /* (yyterminate(); hace que la función yylex finalice retornando un 0, indicando un fin-de-entrada (EOF)) */

%%
        /* Fin de la sección de reglas */

        /* Inicio de la sección de código de usuario, la cual: */
        /* 1. Se utiliza para rutinas de complemento que llaman al analizador léxico o son llamadas por este; la presencia de esta sección es opcional */
        /* 2. Toda esta sección simplemente se copia íntegramente al archivo de salida del analizador léxico (scanner) generado (*.lex.yy.c) */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo de entrada> [opciones]\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Error: no se pudo abrir el archivo '%s'\n", argv[1]);
        return 1;
    }

    init_reporte();
    yylex();
    generar_reporte_final();
    liberar_memoria();

    fclose(yyin);
    return 0;
}

            /* Fin de la sección de código de usuario */
