%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reporte.h"
#include "parser.tab.h"

int num_linea = 1;
int num_columna = 1;

// Espacio máximo reservado
#define MAX_LINEA 1024

// Buffer que almacena la línea que FLEX está leyendo, acumulando los lexemas
// del yytext. Se va construyendo
static char linea_actual[MAX_LINEA] = "";

// Recibe cada lexema que FLEX reconoce y lo mete en el buffer
static void guardar_lexema_en_linea(const char *texto)
{
    for (int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == '\n')
            linea_actual[0] = '\0'; // Vacio el buffer
        else { // Se agregan caracteres
            size_t longitud = strlen(linea_actual);

            if (longitud < MAX_LINEA - 1) {
                linea_actual[longitud] = texto[i];
                linea_actual[longitud + 1] = '\0';
            }
        }
    }
}

// Para que Bison pueda acceder al buffer y asi mostrarlo en el reporte.
// No necesito ni quiero que Bison pueda modificar su contenido, por eso
// uso static, const y además le paso solamente un puntero al buffer
const char *obtener_linea_actual(void)
{
    return linea_actual;
}

// Antes de cualquier acción, se guarda el lexema en nuestro buffer
// Lo ponemos acá porque Bison después no reconoce los saltos ni los espacios
// como tokens, pero YY_USER_ACTION actúa cuando hace match con una regla
#define YY_USER_ACTION \
  guardar_lexema_en_linea(yytext); \
  yylloc.first_line = num_linea; \
  yylloc.first_column = num_columna; \
  for (int i = 0; yytext[i] != '\0'; i++) { \
      if (yytext[i] == '\n') { num_linea++; num_columna = 1; } \
      else { num_columna++; } \
  } \
  yylloc.last_line = num_linea; \
  yylloc.last_column = num_columna;
%}

%pointer
%option noyywrap

digito       [0-9]
exponente    ([eE][+-]?{digito}+)
sufijo_real  [fFlL]

%%

    /* --- CONSTANTES NUMÉRICAS --- */

({digito}+\.{digito}*|\.{digito}+){exponente}?{sufijo_real}? |
{digito}+{exponente}{sufijo_real}? {
    yylval.cadena = strdup(yytext);
    return CONSTANTE_REAL;
}

0[xX][0-9a-fA-F]+([uU](l|L|ll|LL)?|(l|L|ll|LL)[uU]?)? {
    yylval.cadena = strdup(yytext);
    return CONSTANTE_HEXA;
}

0[0-7]*([uU](l|L|ll|LL)?|(l|L|ll|LL)[uU]?)? {
    yylval.cadena = strdup(yytext);
    return CONSTANTE_OCTAL;
}

[1-9][0-9]*([uU](l|L|ll|LL)?|(l|L|ll|LL)[uU]?)? {
    yylval.cadena = strdup(yytext);
    return CONSTANTE_DECIMAL;
}

    /* --- CONSTANTES CARÁCTER Y LITERALES CADENA --- */

'([^'\\\n]|\\.)*' {
    yylval.cadena = strdup(yytext);
    return CONSTANTE_CARACTER;
}

\"([^"\\\n]|\\.)*\" {
    yylval.cadena = strdup(yytext);
    return LITERAL_CADENA;
}

    /* --- PALABRAS RESERVADAS (TIPOS Y ESPECIFICADORES) --- */

"void"     { return VOID; }
"char"     { return CHAR; }
"short"    { return SHORT; }
"int"      { return INT; }
"long"     { return LONG; }
"float"    { return FLOAT; }
"double"   { return DOUBLE; }
"signed"   { return SIGNED; }
"unsigned" { return UNSIGNED; }

"auto"     { return AUTO; }
"register" { return REGISTER; }
"static"   { return STATIC; }
"extern"   { return EXTERN; }
"typedef"  { return TYPEDEF; }

"const"    { return CONST; }
"volatile" { return VOLATILE; }
"struct"   { return STRUCT; }
"union"    { return UNION; }
"enum"     { return ENUM; }

    /* --- PALABRAS RESERVADAS (SENTENCIAS Y CONTROL) --- */

"case"     { return CASE; }
"default"  { return DEFAULT; }
"if"       { return IF; }
"else"     { return ELSE; }
"switch"   { return SWITCH; }
"while"    { return WHILE; }
"do"       { return DO; }
"for"      { return FOR; }
"goto"     { return GOTO; }
"continue" { return CONTINUE; }
"break"    { return BREAK; }
"return"   { return RETURN; }
"sizeof"   { return SIZEOF; }

    /* --- IDENTIFICADORES --- */

[_a-zA-Z]([_a-zA-Z]|[0-9])* { 
    yylval.cadena = strdup(yytext);
    return IDENTIFICADOR; 
}

    /* --- OPERADORES COMPUESTOS --- */

"++"  { return OP_INCREMENTO; }
"--"  { return OP_DECREMENTO; }
"=="  { return OP_IGUAL; }
"!="  { return OP_DISTINTO; }
"<="  { return OP_MENOR_IGUAL; }
">="  { return OP_MAYOR_IGUAL; }
"&&"  { return OP_AND; }
"||"  { return OP_OR; }
"->"  { return OP_FLECHA; }
"<<"  { return OP_SHL; }
">>"  { return OP_SHR; }
"+="  { return MAS_IGUAL; }
"-="  { return MENOS_IGUAL; }
"*="  { return POR_IGUAL; }
"/="  { return DIV_IGUAL; }
"%="  { return MOD_IGUAL; }
"&="  { return AND_IGUAL; }
"^="  { return XOR_IGUAL; }
"|="  { return OR_IGUAL; }
"<<=" { return SHL_IGUAL; }
">>=" { return SHR_IGUAL; }

    /* --- OPERADORES SIMPLES --- */

";"   { return ';'; }
"{"   { return '{'; }
"}"   { return '}'; }
"("   { return '('; }
")"   { return ')'; }
"["   { return '['; }
"]"   { return ']'; }
","   { return ','; }
":"   { return ':'; }
"..." { return ELLIPSIS; }
"."   { return '.'; }
"?"   { return '?'; }
"~"   { return '~'; }
"+"   { return '+'; }
"-"   { return '-'; }
"*"   { return '*'; }
"/"   { return '/'; }
"%"   { return '%'; }
"<"   { return '<'; }
">"   { return '>'; }
"&"   { return '&'; }
"|"   { return '|'; }
"!"   { return '!'; }
"="   { return '='; }
"^"   { return '^'; }

    /* --- ESPACIOS EN BLANCO --- */

[ \t\r\n]+ ;

    /* --- CADENAS NO RECONOCIDAS (ERROR LÉXICO) --- */

[^ \t\r\n] {
    int c;
    int lin = num_linea;
    int col = num_columna - yyleng;
    size_t len = yyleng;
    char* buf = malloc(len + 1);
    memcpy(buf, yytext, len);
    buf[len] = '\0';

    while ((c = input()) > 0 && c != ' ' && c != '\t' &&
           c != '\r' && c != '\n') {
        buf = realloc(buf, len + 2);
        buf[len++] = (char)c;
        buf[len] = '\0';
        num_columna++; //mantiene sincronizada la col por cada caracter extra 
    }
    if (c > 0) {
        unput((char)c);
    }
    num_columna += (int)(len - yyleng);

    agregar_no_reconocido(buf, lin, col);
    free(buf);
}

%%