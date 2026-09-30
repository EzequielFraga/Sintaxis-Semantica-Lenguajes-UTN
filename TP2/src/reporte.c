#include "reporte.h"
static Identificador* lista_id = NULL;
static size_t cant_id = 0;

static LiteralCadena* lista_lit = NULL;
static size_t cant_lit = 0;

static PalabraReservada* palabras = NULL;
static size_t cant_palabras = 0;

static Constante* constantes = NULL;
static size_t cant_constantes = 0;

static Operador* operadores = NULL;
static size_t cant_operadores = 0;

static CadenaNoReconocida* no_reconocidos = NULL;
static size_t cant_no_rec = 0;

static char* mi_strdup(const char* s) {
  size_t len = strlen(s) + 1;
  char* copia = malloc(len);
  if (copia) memcpy(copia, s, len);
  return copia;
}

void init_reporte(void) {}

void agregar_identificador(const char* nombre) {
    for (size_t i = 0; i < cant_id; i++) {
        if (strcmp(lista_id[i].nombre, nombre) == 0) {
            lista_id[i].cantidad++;
            return;
        }
    }

    // Creo una copia por si falla. Corrección notada por el profe
    char* copia = mi_strdup(nombre);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para el identificador.\n");
        return;
    }

    size_t nueva_cantidad = cant_id + 1;

    Identificador* aux = realloc(lista_id, nueva_cantidad * sizeof(Identificador));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de identificadores.\n");
        return;
    } // Uso un auxiliar para verificar si la lista falla y apunta a NULL, si es así, retorna, si no la lista original apunta
      // a lo mismo que apunta aux, es decir a la nueva lista. Calculo la dimensión de la nueva lista una vez que veo que no falló


    lista_id = aux;

    lista_id[cant_id].nombre = copia;
    lista_id[cant_id].cantidad = 1;

    cant_id = nueva_cantidad;
}

  static int calcular_longitud_cadena(const char* s) {
  return (int)strlen(s) - 2;  /* resta las 2 comillas */
}

void agregar_literal_cadena(const char* texto) {
    char* copia = mi_strdup(texto);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para el literal de cadena.\n");
        return;
    }

    size_t nueva_cantidad = cant_lit + 1;

    LiteralCadena* aux = realloc(lista_lit, nueva_cantidad * sizeof(LiteralCadena));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de literales.\n");
        return;
    }

    lista_lit = aux;

    lista_lit[cant_lit].texto = copia;
    lista_lit[cant_lit].longitud = calcular_longitud_cadena(texto);
    lista_lit[cant_lit].orden_aparicion = nueva_cantidad;

    cant_lit = nueva_cantidad;
}

void agregar_palabra_reservada(const char* palabra, CategoriaPalabra cat,
                               int linea, int columna) {
    char* copia = mi_strdup(palabra);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para la palabra reservada.\n");
        return;
    }

    size_t nueva_cantidad = cant_palabras + 1;

    PalabraReservada* aux = realloc(palabras, nueva_cantidad * sizeof(PalabraReservada));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de palabras reservadas.\n");
        return;
    }

    palabras = aux;

    palabras[cant_palabras].palabra = copia;
    palabras[cant_palabras].categoria = cat;
    palabras[cant_palabras].pos.linea = linea;
    palabras[cant_palabras].pos.columna = columna;

    cant_palabras = nueva_cantidad;
}

void agregar_constante_decimal(const char* texto) {
    char* copia = mi_strdup(texto);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para la constante decimal.\n");
        return;
    }

    size_t nueva_cantidad = cant_constantes + 1;

    Constante* aux = realloc(constantes, nueva_cantidad * sizeof(Constante));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de constantes.\n");
        return;
    }

    constantes = aux;

    constantes[cant_constantes].tipo = CONST_DECIMAL;
    constantes[cant_constantes].texto_original = copia;
    constantes[cant_constantes].valor_entero = strtol(texto, NULL, 10);
    constantes[cant_constantes].orden = nueva_cantidad;

    cant_constantes = nueva_cantidad;
}


void agregar_constante_hexa(const char* texto) {
    char* copia = mi_strdup(texto);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para la constante hexadecimal.\n");
        return;
    }

    size_t nueva_cantidad = cant_constantes + 1;

    Constante* aux = realloc(constantes, nueva_cantidad * sizeof(Constante));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de constantes.\n");
        return;
    }

    constantes = aux;

    constantes[cant_constantes].tipo = CONST_HEXA;
    constantes[cant_constantes].texto_original = copia;
    constantes[cant_constantes].valor_entero = strtol(texto, NULL, 16);
    constantes[cant_constantes].orden = nueva_cantidad;

    cant_constantes = nueva_cantidad;
}


void agregar_constante_octal(const char* texto) {
    char* copia = mi_strdup(texto);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para la constante octal.\n");
        return;
    }

    size_t nueva_cantidad = cant_constantes + 1;

    Constante* aux = realloc(constantes, nueva_cantidad * sizeof(Constante));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de constantes.\n");
        return;
    }

    constantes = aux;

    constantes[cant_constantes].tipo = CONST_OCTAL;
    constantes[cant_constantes].texto_original = copia;
    constantes[cant_constantes].valor_entero = strtol(texto, NULL, 8);
    constantes[cant_constantes].orden = nueva_cantidad;

    cant_constantes = nueva_cantidad;
}


void agregar_constante_real(const char* texto) {
    char* copia = mi_strdup(texto);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para la constante real.\n");
        return;
    }

    size_t nueva_cantidad = cant_constantes + 1;

    Constante* aux = realloc(constantes, nueva_cantidad * sizeof(Constante));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de constantes.\n");
        return;
    }

    constantes = aux;

    constantes[cant_constantes].tipo = CONST_REAL;
    constantes[cant_constantes].texto_original = copia;
    constantes[cant_constantes].valor_real = strtod(texto, NULL);
    constantes[cant_constantes].orden = nueva_cantidad;

    cant_constantes = nueva_cantidad;
}


void agregar_constante_caracter(const char* texto) {
    char* copia = mi_strdup(texto);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para la constante de caracter.\n");
        return;
    }

    size_t nueva_cantidad = cant_constantes + 1;

    Constante* aux = realloc(constantes, nueva_cantidad * sizeof(Constante));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de constantes.\n");
        return;
    }

    constantes = aux;

    constantes[cant_constantes].tipo = CONST_CARACTER;
    constantes[cant_constantes].texto_original = copia;
    constantes[cant_constantes].orden = nueva_cantidad;

    cant_constantes = nueva_cantidad;
}

void agregar_operador(const char* op) {
    for (size_t i = 0; i < cant_operadores; i++) {
        if (strcmp(operadores[i].op, op) == 0) {
            operadores[i].cantidad++;
            return;
        }
    }

    char* copia = mi_strdup(op);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para el operador.\n");
        return;
    }

    size_t nueva_cantidad = cant_operadores + 1;

    Operador* aux = realloc(operadores, nueva_cantidad * sizeof(Operador));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de operadores.\n");
        return;
    }

    operadores = aux;

    operadores[cant_operadores].op = copia;
    operadores[cant_operadores].cantidad = 1;
    operadores[cant_operadores].orden_aparicion = nueva_cantidad;

    cant_operadores = nueva_cantidad;
}

void agregar_no_reconocido(const char* cadena, int linea, int columna) {
    char* copia = mi_strdup(cadena);

    if (copia == NULL) {
        fprintf(stderr, "Error al reservar memoria para la cadena no reconocida.\n");
        return;
    }

    size_t nueva_cantidad = cant_no_rec + 1;

    CadenaNoReconocida* aux = realloc(no_reconocidos, nueva_cantidad * sizeof(CadenaNoReconocida));

    if (aux == NULL) {
        free(copia);
        fprintf(stderr, "Error al redimensionar la lista de cadenas no reconocidas.\n");
        return;
    }

    no_reconocidos = aux;

    no_reconocidos[cant_no_rec].cadena = copia;
    no_reconocidos[cant_no_rec].pos.linea = linea;
    no_reconocidos[cant_no_rec].pos.columna = columna;

    cant_no_rec = nueva_cantidad;
}

static int comp_lista_id(const void* a, const void* b) {
  return strcmp(((Identificador*)a)->nombre, ((Identificador*)b)->nombre);
}

static int comp_lista_lit(const void* a, const void* b) {
  LiteralCadena* la = (LiteralCadena*)a;
  LiteralCadena* lb = (LiteralCadena*)b;
  if (la->longitud != lb->longitud) return la->longitud - lb->longitud;
  return la->orden_aparicion - lb->orden_aparicion;
}

void generar_reporte_final(void) {
  printf("* Listado de identificadores encontrados:\n");
  if (cant_id == 0) {
    printf("-\n");
  } else {
    qsort(lista_id, cant_id, sizeof(Identificador),
          comp_lista_id);
    for (size_t i = 0; i < cant_id; i++) {
      printf("%s: aparece %d %s\n", lista_id[i].nombre,
             lista_id[i].cantidad,
             lista_id[i].cantidad == 1 ? "vez" : "veces");
    }
  }

  printf("* Listado de literales cadena encontrados:\n");
  if (cant_lit == 0) {
    printf("-\n");
  } else {
    qsort(lista_lit, cant_lit, sizeof(LiteralCadena), comp_lista_lit);
    for (size_t i = 0; i < cant_lit; i++) {
      printf("%s: longitud %d\n", lista_lit[i].texto, lista_lit[i].longitud);
    }
  }

  const char* nombres_cat[] = {"clase de almacenamiento",
                               "especificadores de tipo",
                               "calificadores de tipo",
                               "struct o union",
                               "enumeracion",
                               "etiqueta",
                               "seleccion",
                               "iteracion",
                               "salto",
                               "unario"};

  for (int cat = 0; cat <= CAT_UNARIO; cat++) {
    printf("* Listado de palabras reservadas (%s):\n", nombres_cat[cat]);
    int imp = 0;
    for (size_t i = 0; i < cant_palabras; i++) {
      if (palabras[i].categoria == (CategoriaPalabra)cat) {
        printf("%s: linea %d, columna %d\n", palabras[i].palabra,
               palabras[i].pos.linea, palabras[i].pos.columna);
        imp = 1;
      }
    }
    if (!imp) printf("-\n");
  }

  printf("* Listado de constantes enteras decimales:\n");
  long acumulado_dec = 0;
  int imp_dec = 0;
  for (size_t i = 0; i < cant_constantes; i++) {
    if (constantes[i].tipo == CONST_DECIMAL) {
      printf("%s: valor %ld\n", constantes[i].texto_original,
             constantes[i].valor_entero);
      acumulado_dec += constantes[i].valor_entero;
      imp_dec = 1;
    }
  }
  if (!imp_dec)
    printf("-\n");
  else
    printf("Total acumulado de sumar todas las constantes decimales: %ld\n",
           acumulado_dec);

  printf("* Listado de constantes enteras hexadecimales:\n");
  int imp_hex = 0;
  for (size_t i = 0; i < cant_constantes; i++) {
    if (constantes[i].tipo == CONST_HEXA) {
      printf("%s: valor entero decimal %ld\n", constantes[i].texto_original,
             constantes[i].valor_entero);
      imp_hex = 1;
    }
  }
  if (!imp_hex) printf("-\n");

  printf("* Listado de constantes enteras octales:\n");
  int imp_oct = 0;
  for (size_t i = 0; i < cant_constantes; i++) {
    if (constantes[i].tipo == CONST_OCTAL) {
      printf("%s: valor entero decimal %ld\n", constantes[i].texto_original,
             constantes[i].valor_entero);
      imp_oct = 1;
    }
  }
  if (!imp_oct) printf("-\n");

  printf("* Listado de constantes reales:\n");
  int imp_real = 0;
  for (size_t i = 0; i < cant_constantes; i++) {
    if (constantes[i].tipo == CONST_REAL) {
      double enteras, mantisa;
      mantisa = modf(constantes[i].valor_real, &enteras);
      printf("%s: parte entera %f, mantisa %f\n", constantes[i].texto_original,
             enteras, fabs(mantisa));
      imp_real = 1;
    }
  }
  if (!imp_real) printf("-\n");

  printf("* Listado de constantes caracter enumerados:\n");
  int imp_car = 0, num_car = 1;
  for (size_t i = 0; i < cant_constantes; i++) {
    if (constantes[i].tipo == CONST_CARACTER) {
      printf("%d) %s\n", num_car++, constantes[i].texto_original);
      imp_car = 1;
    }
  }
  if (!imp_car) printf("-\n");

  printf("* Listado de operadores/caracteres de puntuación:\n");
  if (cant_operadores == 0) {
    printf("-\n");
  } else {
    for (size_t i = 0; i < cant_operadores; i++) {
      printf("%s: aparece %d %s\n", operadores[i].op, operadores[i].cantidad,
             operadores[i].cantidad == 1 ? "vez" : "veces");
    }
  }

  printf("* Listado de cadenas no reconocidas:\n");
  if (cant_no_rec == 0) {
    printf("-\n");
  } else {
    for (size_t i = 0; i < cant_no_rec; i++) {
      printf("%s: linea %d, columna %d\n", no_reconocidos[i].cadena,
             no_reconocidos[i].pos.linea, no_reconocidos[i].pos.columna);
    }
  }
}

void liberar_memoria(void) {
  for (size_t i = 0; i < cant_id; i++) free(lista_id[i].nombre);
  free(lista_id);

  for (size_t i = 0; i < cant_lit; i++) free(lista_lit[i].texto);
  free(lista_lit);

  for (size_t i = 0; i < cant_palabras; i++) free(palabras[i].palabra);
  free(palabras);

  for (size_t i = 0; i < cant_constantes; i++)
    free(constantes[i].texto_original);
  free(constantes);

  for (size_t i = 0; i < cant_operadores; i++) free(operadores[i].op);
  free(operadores);

  for (size_t i = 0; i < cant_no_rec; i++) free(no_reconocidos[i].cadena);
  free(no_reconocidos);
}
