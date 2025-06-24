#include <stdio.h>
#include <stdlib.h>
#include "sym_tables.h"

int yyparse(void);
void print_tours(void);     

int main(void) {
    if (yyparse() == 0) {
        print_tours();
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