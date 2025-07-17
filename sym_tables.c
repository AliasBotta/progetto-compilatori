#include "sym_tables.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

// ** INIZIALIZZAZIONE DELLE HASHTABLE **
city* CITY_HTABLE[CITY_HTABLE_SIZE] = {NULL};    // hashtable per le città, inizializzata a NULL
tour* TOUR_HTABLE[TOUR_HTABLE_SIZE] = {NULL};    // hashtable per i tour, inizializzata a NULL

// ** FUNZIONI DI HASHING **
int get_city_hash(char city_id[]) {
    // genera hash a partire da 3 lettere maiuscole dell'id città
    int hash = 0;
    for (int i = 0; i < CITY_ID_SIZE; ++i) {
        hash = (hash * 31 + city_id[i]) % CITY_HTABLE_SIZE;
    }
    return hash;
}

int get_tour_hash(char tour_id[]) {
    // funzione di hash perfetta per tour ID (lettera + 2 cifre)
    // Esempio: X25 -> (X-A)*100 + 25 = 23*100 + 25 = 2325
    if (tour_id[0] < 'A' || tour_id[0] > 'Z') {
        return -1; // ID non valido
    }
    
    int letter_value = tour_id[0] - 'A';  // 0-25
    int number_value = (tour_id[1] - '0') * 10 + (tour_id[2] - '0'); // 0-99
    
    return letter_value * 100 + number_value;
}

// ** INSERIMENTO DI UNA CITTÀ NELLA HASHTABLE ** --> remind, stiamo utilizzando la tecnica con chaining
void add_city(char city_id[], char* city_name, double coords[]) {
    int hash = get_city_hash(city_id);
    
    // controllo se la città esiste già
    city* current = CITY_HTABLE[hash];
    while (current != NULL) {
        if (strncmp(current->id, city_id, CITY_ID_SIZE) == 0) {
            semantic_error("Città con ID '%s' duplicato. Tentativo di inserimento fallito di: (%s, %s)", 
                          city_id, city_id, city_name);
        }
        current = current->next;
    }

    city* new_city = malloc(sizeof(city));
    if (new_city == NULL) {
        fprintf(stderr, "Errore: malloc fallita per city\n");
        exit(EXIT_FAILURE);
    }
    
    strncpy(new_city->id, city_id, CITY_ID_SIZE); // non necessitiamo di malloc in quanto l'ID è istanziato staticamente (ha dmensione fissa 3)
    new_city->id[CITY_ID_SIZE] = '\0';
    
    new_city->name = malloc(strlen(city_name) + 1); // qui invece necessitiamo di malloc in quanto il nome è variabile
    if (new_city->name == NULL) {
        fprintf(stderr, "Errore: malloc fallita per city_name\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_city->name, city_name);
    
    new_city->xy_pos[0] = coords[0];
    new_city->xy_pos[1] = coords[1];
    new_city->next = CITY_HTABLE[hash]; // inserimento in testa alla chain
    CITY_HTABLE[hash] = new_city;
}

// ** INSERIMENTO DI UN TOUR NELLA HASHTABLE **
void add_tour(char tour_id[], char* tour_name, char startingCity[]) {
    int hash = get_tour_hash(tour_id);
    
    // validazione hash
    if (hash < 0 || hash >= TOUR_HTABLE_SIZE) {
        semantic_error("ID tour '%s' non valido.", tour_id);
        return;
    }
    
    // controllo se il tour esiste già
    if (TOUR_HTABLE[hash] != NULL) {
        semantic_error("Tour con ID '%s' (%s) già presente.\nConflitto con %s tentato in inserimento.", 
                      tour_id, TOUR_HTABLE[hash]->name, tour_name);
        return;
    }
    
    // verifica che la città di partenza esista
    double* city_coords = get_city_coords(startingCity);
    if (city_coords == NULL) {
        semantic_error("Città di partenza con ID '%s' non valida per il tour '%s'.", startingCity, tour_id);
        return;
    }
    
    tour* new_tour = malloc(sizeof(tour));
    if (new_tour == NULL) {
        fprintf(stderr, "Errore: malloc fallita per tour\n");
        exit(EXIT_FAILURE);
    }
    
    strncpy(new_tour->id, tour_id, TOUR_ID_SIZE);
    new_tour->id[TOUR_ID_SIZE] = '\0';
    
    new_tour->name = malloc(strlen(tour_name) + 1);
    if (new_tour->name == NULL) {
        fprintf(stderr, "Errore: malloc fallita per tour_name\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_tour->name, tour_name);
    
    strncpy(new_tour->startingCity, startingCity, CITY_ID_SIZE);
    new_tour->startingCity[CITY_ID_SIZE] = '\0';
    strncpy(new_tour->currentCity, startingCity, CITY_ID_SIZE);
    new_tour->currentCity[CITY_ID_SIZE] = '\0';
    
    // inizializza la prima tappa
    strncpy(new_tour->stages[0], startingCity, CITY_ID_SIZE);
    new_tour->stages[0][CITY_ID_SIZE] = '\0';
    new_tour->stage_count = 1;
    
    new_tour->total_distance = 0.0;
    new_tour->total_cost = 250.0; // costo base per la città di partenza
    
    TOUR_HTABLE[hash] = new_tour;
}

// ** AGGIORNAMENTO PERCORSO DI UN TOUR **
void update_tour(char tour_id[], char nextCity[]) {
    tour* t = get_tour(tour_id);
    if (t == NULL) {
        semantic_error("Tour con ID '%s' non trovato.", tour_id);
        return;
    }
    
    // verifica che la città di destinazione esista
    double* next_coords = get_city_coords(nextCity);
    if (next_coords == NULL) {
        semantic_error("Città con ID '%s' non valida.", nextCity);
        return;
    }
    
    // calcola distanza dalla città corrente alla prossima
    double* current_coords = get_city_coords(t->currentCity);
    if (current_coords == NULL) {
        semantic_error("Città corrente con ID '%s' non valida.", t->currentCity);
        return;
    }
    
    double dx = current_coords[0] - next_coords[0];
    double dy = current_coords[1] - next_coords[1];
    double distance = sqrt(dx * dx + dy * dy);
    
    // aggiorna tour
    t->total_distance += distance;
    t->total_cost += 250.0 + (10.0 * distance); // 250€ per tappa + 10€ per unità di distanza
    
    // aggiorna città corrente
    strncpy(t->currentCity, nextCity, CITY_ID_SIZE);
    t->currentCity[CITY_ID_SIZE] = '\0';
    
    // aggiungi tappa all'array delle tappe
    if (t->stage_count < MAX_STAGES) {
        strncpy(t->stages[t->stage_count], nextCity, CITY_ID_SIZE);
        t->stages[t->stage_count][CITY_ID_SIZE] = '\0';
        t->stage_count++;
    } else {
        semantic_error("Troppo tappe per il tour '%s'. Massimo %d tappe consentite.", tour_id, MAX_STAGES);
    }
}

// ** RECUPERO COORDINATE DI UNA CITTÀ **
double* get_city_coords(char city_id[]) {
    city* current = CITY_HTABLE[get_city_hash(city_id)];
    
    while (current != NULL) {
        if (strncmp(current->id, city_id, CITY_ID_SIZE) == 0) {
            return current->xy_pos;
        }
        current = current->next;
    }
    
    return NULL; // città non trovata
}

// ** RECUPERO TOUR PER ID **
tour* get_tour(char tour_id[]) {
    int hash = get_tour_hash(tour_id);
    
    if (hash < 0 || hash >= TOUR_HTABLE_SIZE) {
        return NULL;
    }
    
    return TOUR_HTABLE[hash];
}

// ** RECUPERO NOME CITTÀ PER ID **
char* get_city_name(char city_id[]) {
    city* current = CITY_HTABLE[get_city_hash(city_id)];
    
    while (current != NULL) {
        if (strncmp(current->id, city_id, CITY_ID_SIZE) == 0) {
            return current->name;
        }
        current = current->next;
    }
    
    return NULL;
}

// ** PRINTF ERRORE SEMANTICO **
void semantic_error(const char* message, ...) { // ... sono in parametri variabili
    fprintf(stderr, "ERRORE SEMANTICO: "); // printf specifico per scrivere su uno stream specifico
    va_list args; // lista argomenti variabili
    va_start(args, message); // inizializza la lista args
    vfprintf(stderr, message, args); // fprintf con la lsita args
    va_end(args); // pulisce la lista
    fprintf(stderr, "\n");

    free_city_ht();
    free_tour_ht();
    exit(EXIT_FAILURE);
}

// ** DEALLOCAZIONE MEMORIA HASHTABLE CITTÀ **
void free_city_ht() {
    for (int i = 0; i < CITY_HTABLE_SIZE; ++i) {
        city* current = CITY_HTABLE[i];
        while (current != NULL) {
            city* to_free = current;
            current = current->next;
            free(to_free->name);
            free(to_free);
        }
        CITY_HTABLE[i] = NULL;
    }
}

// ** DEALLOCAZIONE MEMORIA HASHTABLE TOUR **
void free_tour_ht() {
    for (int i = 0; i < TOUR_HTABLE_SIZE; ++i) {
        if (TOUR_HTABLE[i] != NULL) {
            free(TOUR_HTABLE[i]->name);
            free(TOUR_HTABLE[i]);
            TOUR_HTABLE[i] = NULL;
        }
    }
}

// Visualizzazione dei percorsi turistici registrati
void print_tours(FILE *output_file) {
    for (int idx = 0; idx < TOUR_HTABLE_SIZE; ++idx) {
        tour* current_tour = TOUR_HTABLE[idx];
        if (current_tour != NULL) {
            // Visualizza identificativo percorso
            fprintf(output_file, "%s: ", current_tour->name);
            
            // Elabora e mostra l'itinerario completo
            for (int stop = 0; stop < current_tour->stage_count; stop++) {
                char* location_name = get_city_name(current_tour->stages[stop]);
                if (location_name == NULL) {
                    semantic_error("Nome città non trovato per la tappa '%s' del tour '%s'", 
                                 current_tour->stages[stop], current_tour->id);
                }
                
                // Gestione delle virgolette nel nome località
                if (location_name[0] == '"' && location_name[strlen(location_name)-1] == '"') {
                    // Rimuove delimitatori di stringa
                    int name_length = strlen(location_name);
                    for (int pos = 1; pos < name_length-1; pos++) {
                        fprintf(output_file, "%c", location_name[pos]);
                    }
                } else {
                    fprintf(output_file, "%s", location_name);
                }
                
                // Separatore tra destinazioni
                if (stop < current_tour->stage_count - 1) {
                    fprintf(output_file, " -> ");
                }
            }
            
            // Mostra prezzo complessivo
            fprintf(output_file, " %.2f€\n", current_tour->total_cost);
        }
    }
}