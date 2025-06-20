%{
// #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

#define MAX_LINE_LENGTH 1024

/* Implementazione custom di strdup per compatibilità */
char* my_strdup(const char* s) {
    if (s == NULL) return NULL;
    char* dup = malloc(strlen(s) + 1);
    if (dup != NULL) {
        strcpy(dup, s);
    }
    return dup;
}

int line_number = 1;
char unknown_sequence[MAX_LINE_LENGTH] = "";
int sequence = 0;

void lexical_error(const char *msg);
%}

%option noyywrap

%x ERROR_STATE

/* Regex migliorate per float e interi */
CITY_ID_REGEX       [A-Z]{3}
CITY_NAME_REGEX     \"[A-Za-z]+\" 
FLOAT_REGEX         [+-]?[0-9]+\.[0-9]+
INTEGER_VALUE_REGEX [0-9]+
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

{INTEGER_VALUE_REGEX} {
    yylval.integer_type = atoi(yytext);
    return INTEGER_VALUE;
}

[ \t\r]+                 { /* ignora spazi, tabulazioni, carriage return */ }

\n {
    line_number++;
    /* ignora newline */
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
        snprintf(msg, sizeof(msg), "ERRORE LESSICALE - Riga %d\nSequenza non riconosciuta: %s", line_number, unknown_sequence);
        lexical_error(msg);
    }
    . {
        if (sequence < MAX_LINE_LENGTH - 1) {
            unknown_sequence[sequence++] = yytext[0];
        }
    }
}

%%

void lexical_error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}