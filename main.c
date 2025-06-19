#include <stdio.h>
#include <stdlib.h>
#include "sym_tables.h"

int yyparse(void);
void dump(void);     // funzione di dump delle HT

int main(void) {
    if (yyparse() == 0) {
        dump();
    } else {
        fprintf(stderr, "Parsing fallito.\n");
        free_city_ht();
        free_tour_ht();
        return EXIT_FAILURE;
    }

    free_city_ht();
    free_tour_ht();

    return EXIT_SUCCESS;
}