#include <stdio.h>
#include <stdlib.h>
#include "sym_tables.h"

extern int semantic_errors;
extern int syntax_errors;

int yyparse(void);

int main(void) {
    printf("=== Tour Agency Analyzer ===\n");
    printf("Inizio parsing...\n\n");
    
    int parse_result = yyparse();
    
    if (parse_result == 0 && semantic_errors == 0 && syntax_errors == 0) {
        printf("=== RISULTATI ===\n");
        print_tours();
        printf("\nParsing completato con successo!\n");
    } else {
        fprintf(stderr, "\n=== PARSING FALLITO ===\n");
        fprintf(stderr, "Errori sintattici: %d\n", syntax_errors);
        fprintf(stderr, "Errori semantici: %d\n", semantic_errors);
        
        free_city_ht();
        free_tour_ht();
        return EXIT_FAILURE;
    }

    free_city_ht();
    free_tour_ht();
    return EXIT_SUCCESS;
}