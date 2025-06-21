%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sym_tables.h"

// Dichiarazione delle funzioni del lexer (generato da Flex)
int yylex(void);
void yyerror(const char *s);
%}

// Il parser genera errori in modalità verbosi
%define parse.error verbose

// Token union per gestire la tipizzazione
%union {
    char* string_type;
    float float_type;
}

%token <string_type> CITY_ID CITY_NAME TOUR_NAME TOUR_ID
%token <float_type> FLOAT_VALUE
%token OPEN_ROUNDED CLOSED_ROUNDED COMMA TOUR_SEPARATOR END_SECTION
%start start

%%

// GRAMMATICHE VARIE
start: p1 END_SECTION p2 END_SECTION p3
;

// Prima sezione: definizione delle città
p1: p1 city_definition
| city_definition
;

city_definition: CITY_ID CITY_NAME OPEN_ROUNDED FLOAT_VALUE COMMA FLOAT_VALUE CLOSED_ROUNDED { 
    double coordinates[2];
    coordinates[0] = $4;
    coordinates[1] = $6;
    add_city($1, $2, coordinates);
    free($1);  // libera memoria allocata da my_strdup
    free($2);
}

// Seconda sezione: definizione dei tour
p2: p2 TOUR_SEPARATOR tour_definition 
| tour_definition
;

tour_definition: TOUR_ID TOUR_NAME CITY_ID {
    add_tour($1, $2, $3);
    free($1);  // libera memoria allocata da my_strdup
    free($2);
    free($3);
}

// Terza sezione: percorsi dei tour
p3: p3 stage_definition
| stage_definition
;

stage_definition: TOUR_ID CITY_ID {
    update_tour($1, $2);
    free($1);  // libera memoria allocata da my_strdup
    free($2);
}

%%

// GESTIONE ERRORE SINTATTICO
void yyerror(const char *s) {
    fprintf(stderr, "ERRORE SINTATTICO: %s\n", s);
    free_city_ht();
    free_tour_ht();
    exit(EXIT_FAILURE);
}