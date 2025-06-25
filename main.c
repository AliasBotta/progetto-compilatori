#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sym_tables.h"

int yyparse(void);
void print_tours(FILE *output_file);

void print_usage(const char *prog_name) {
    fprintf(stderr, "Uso: %s [-o output_file]\n", prog_name);
    fprintf(stderr, "  -o output_file    Specifica il file di output (default: output.txt)\n");
    fprintf(stderr, "  -h, --help        Mostra questo messaggio di aiuto\n");
    fprintf(stderr, "  L'input viene letto da stdin\n");
    fprintf(stderr, "\nEsempi:\n");
    fprintf(stderr, "  %s < input.txt                    # Output in output.txt\n", prog_name);
    fprintf(stderr, "  %s -o risultato.txt < input.txt   # Output in risultato.txt\n", prog_name);
}

int main(int argc, char *argv[]) {
    char *output_filename = "output.txt"; // file di output di default
    
    // Parse dei parametri da riga di comando
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_filename = argv[i + 1];
                i++; // salta l'argomento successivo
            } else {
                fprintf(stderr, "Errore: -o richiede un nome file\n");
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Errore: parametro sconosciuto '%s'\n", argv[i]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Esegui il parsing
    if (yyparse() == 0) {
        // Apri il file di output
        FILE *output_file = fopen(output_filename, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Errore: impossibile aprire il file di output '%s'\n", output_filename);
            free_city_ht();
            free_tour_ht();
            return EXIT_FAILURE;
        }
        
        // Scrivi i risultati nel file
        print_tours(output_file);
        fclose(output_file);
        
        // Messaggio di successo su stderr (non nel file di output)
        fprintf(stderr, "Parsing completato con successo. Output scritto in '%s'\n", output_filename);
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