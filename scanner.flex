%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

#define MAX_LINE_LENGTH 1024

char* my_strdup(const char* s) {
    if (s == NULL) return NULL;
    char* dup = malloc(strlen(s) + 1);
    if (dup != NULL) {
        strcpy(dup, s);
    }
    return dup;
}

// Variabili per tracking delle posizioni
int line_number = 1;
int column_number = 1;
char unknown_sequence[MAX_LINE_LENGTH] = "";
int sequence = 0;

void lexical_error(const char *msg);
void update_location();

// Macro per aggiornare yylloc
#define YY_USER_ACTION update_location();
%}

%option noyywrap 
%x ERROR_STATE

CITY_ID_REGEX       [A-Z]{3}
CITY_NAME_REGEX     \"[A-Za-z]+\" 
FLOAT_REGEX         [+-]?(0|[1-9][0-9]*)\.[0-9]+
TOUR_NAME_REGEX     [A-Za-z]+[ ][A-Za-z]+|[A-Za-z]+
TOUR_ID_REGEX       [A-Z][0-9]{2}

%%

"%%%"                    return END_SECTION;
"&&&"                    return TOUR_SEPARATOR;
"("                      return OPEN_ROUNDED;
")"                      return CLOSED_ROUNDED;
","                      return COMMA;

{CITY_ID_REGEX} {
    yylval.string_type = my_strdup(yytext);
    return CITY_ID;
}

{CITY_NAME_REGEX} {
    yylval.string_type = my_strdup(yytext);
    return CITY_NAME;
}

{FLOAT_REGEX} {
    yylval.float_type = atof(yytext);
    return FLOAT_VALUE;
}

{TOUR_ID_REGEX} {
    yylval.string_type = my_strdup(yytext);
    return TOUR_ID;
}

{TOUR_NAME_REGEX} {
    yylval.string_type = my_strdup(yytext);
    return TOUR_NAME;
}

[ \t\r]+                 { /* ignora spazi, tabulazioni */ }

\n {
    line_number++;
    column_number = 1;
}

. {
    BEGIN(ERROR_STATE);
    sequence = 0;
    unknown_sequence[sequence++] = yytext[0];
}

<ERROR_STATE>{
    [ \n] { 
        unknown_sequence[sequence] = '\0'; 
        char msg[2000];
        snprintf(msg, sizeof(msg), "ERRORE LESSICALE - Riga %d, Colonna %d\nSequenza non riconosciuta: %s", 
                line_number, column_number, unknown_sequence);
        lexical_error(msg);
    }
    . {
        if (sequence < MAX_LINE_LENGTH - 1) {
            unknown_sequence[sequence++] = yytext[0];
        }
    }
}

%%

void update_location() {
    yylloc.first_line = yylloc.last_line = line_number;
    yylloc.first_column = column_number;
    column_number += yyleng;
    yylloc.last_column = column_number - 1;
}

void lexical_error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}