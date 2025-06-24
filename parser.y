%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sym_tables.h"

int yylex(void);
void yyerror(const char *s);

int semantic_errors = 0;
int syntax_errors = 0;

// Macro per recovery compatto
#define SYNTAX_ERROR() do { syntax_errors++; yyerrok; } while(0)
#define FREE_STRINGS(...) do { char* args[] = {__VA_ARGS__}; \
    for(int i = 0; i < sizeof(args)/sizeof(char*); i++) free(args[i]); } while(0)
%}

%define parse.error verbose
%define parse.lac full
%locations

%union {
    char* string_type;
    float float_type;
    struct { double coords[2]; } coords;
}

%token <string_type> CITY_ID CITY_NAME TOUR_NAME TOUR_ID
%token <float_type> FLOAT_VALUE
%token OPEN_ROUNDED CLOSED_ROUNDED COMMA TOUR_SEPARATOR END_SECTION

%type <coords> coordinates

%start start

%%

start: sections {
    if (semantic_errors > 0) {
        fprintf(stderr, "\n=== ERRORI: Sintattici:%d Semantici:%d ===\n", 
                syntax_errors, semantic_errors);
        YYABORT;
    }
}
;

sections: city_section END_SECTION tour_section END_SECTION stage_section
;

// ============ SEZIONE CITTÀ ============
city_section: city_list | city_list error { SYNTAX_ERROR(); };

city_list: city_list city_definition
         | city_definition  
         | city_list error city_definition { SYNTAX_ERROR(); }
         | error { SYNTAX_ERROR(); }
;

city_definition: CITY_ID CITY_NAME coordinates { 
    validation_result result = validate_city_addition($1, $2);
    if (!result.success) {
        fprintf(stderr, "ERRORE SEMANTICO (riga %d): %s\n", @1.first_line, result.error_message);
        semantic_errors++;
        free_validation_result(&result);
    } else {
        add_city_unsafe($1, $2, $3.coords);
    }
    FREE_STRINGS($1, $2);
}
| error CITY_NAME coordinates { SYNTAX_ERROR(); free($2); }
| CITY_ID error coordinates   { SYNTAX_ERROR(); free($1); }
| CITY_ID CITY_NAME error     { SYNTAX_ERROR(); FREE_STRINGS($1, $2); }
;

coordinates: OPEN_ROUNDED FLOAT_VALUE COMMA FLOAT_VALUE CLOSED_ROUNDED {
    $$.coords[0] = $2; $$.coords[1] = $4;
}
| error FLOAT_VALUE COMMA FLOAT_VALUE error {  // Parentesi mancanti
    SYNTAX_ERROR(); $$.coords[0] = $2; $$.coords[1] = $4;
}
| OPEN_ROUNDED error COMMA error CLOSED_ROUNDED {  // Coordinate malformate
    SYNTAX_ERROR(); $$.coords[0] = 0.0; $$.coords[1] = 0.0;
}
;

// ============ SEZIONE TOUR ============
tour_section: tour_list | tour_list error { SYNTAX_ERROR(); };

tour_list: tour_list TOUR_SEPARATOR tour_definition
         | tour_definition
         | tour_list error tour_definition { SYNTAX_ERROR(); }
         | error { SYNTAX_ERROR(); }
;

tour_definition: TOUR_ID TOUR_NAME CITY_ID {
    validation_result result = validate_tour_addition($1, $2, $3);
    if (!result.success) {
        fprintf(stderr, "ERRORE SEMANTICO (riga %d): %s\n", @1.first_line, result.error_message);
        semantic_errors++;
        free_validation_result(&result);
    } else {
        add_tour_unsafe($1, $2, $3);
    }
    FREE_STRINGS($1, $2, $3);
}
| error TOUR_NAME CITY_ID { SYNTAX_ERROR(); FREE_STRINGS($2, $3); }
| TOUR_ID error CITY_ID   { SYNTAX_ERROR(); FREE_STRINGS($1, $3); }
| TOUR_ID TOUR_NAME error { SYNTAX_ERROR(); FREE_STRINGS($1, $2); }
;

// ============ SEZIONE STAGE ============
stage_section: stage_list | stage_list error { SYNTAX_ERROR(); };

stage_list: stage_list stage_definition
          | stage_definition
          | stage_list error stage_definition { SYNTAX_ERROR(); }
          | error { SYNTAX_ERROR(); }
;

stage_definition: TOUR_ID CITY_ID {
    validation_result result = validate_tour_update($1, $2);
    if (!result.success) {
        fprintf(stderr, "ERRORE SEMANTICO (riga %d): %s\n", @1.first_line, result.error_message);
        semantic_errors++;
        free_validation_result(&result);
    } else {
        update_tour_unsafe($1, $2);
    }
    FREE_STRINGS($1, $2);
}
| error CITY_ID   { SYNTAX_ERROR(); free($2); }
| TOUR_ID error   { SYNTAX_ERROR(); free($1); }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERRORE SINTATTICO (riga %d): %s\n", yylloc.first_line, s);
    syntax_errors++;
}