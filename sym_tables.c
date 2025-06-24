#include "sym_tables.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

// ** INIZIALIZZAZIONE DELLE HASHTABLE **
city* CITY_HTABLE[CITY_HTABLE_SIZE] = {NULL};    
tour* TOUR_HTABLE[TOUR_HTABLE_SIZE] = {NULL};    

// ** FUNZIONI DI HASHING **
int get_city_hash(char city_id[]) {
    int hash = 0;
    for (int i = 0; i < CITY_ID_SIZE; ++i) {
        hash = (hash * 31 + city_id[i]) % CITY_HTABLE_SIZE;
    }
    return hash;
}

int get_tour_hash(char tour_id[]) {
    if (tour_id[0] < 'A' || tour_id[0] > 'Z') {
        return -1;
    }
    int letter_value = tour_id[0] - 'A';  
    int number_value = (tour_id[1] - '0') * 10 + (tour_id[2] - '0'); 
    return letter_value * 100 + number_value;
}

// ** FUNZIONI DI LOOKUP **
bool city_exists(char city_id[]) {
    city* current = CITY_HTABLE[get_city_hash(city_id)];
    while (current != NULL) {
        if (strncmp(current->id, city_id, CITY_ID_SIZE) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool tour_exists(char tour_id[]) {
    int hash = get_tour_hash(tour_id);
    if (hash < 0 || hash >= TOUR_HTABLE_SIZE) {
        return false;
    }
    return TOUR_HTABLE[hash] != NULL;
}

double* get_city_coords(char city_id[]) {
    city* current = CITY_HTABLE[get_city_hash(city_id)];
    while (current != NULL) {
        if (strncmp(current->id, city_id, CITY_ID_SIZE) == 0) {
            return current->xy_pos;
        }
        current = current->next;
    }
    return NULL;
}

tour* get_tour(char tour_id[]) {
    int hash = get_tour_hash(tour_id);
    if (hash < 0 || hash >= TOUR_HTABLE_SIZE) {
        return NULL;
    }
    return TOUR_HTABLE[hash];
}

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

// ** FUNZIONI DI VALIDAZIONE (PURE) **
validation_result validate_city_addition(char city_id[], char* city_name) {
    validation_result result = {true, NULL};
    
    if (city_exists(city_id)) {
        result.success = false;
        asprintf(&result.error_message, 
            "Città con ID '%s' già presente. Tentativo di inserimento: (%s, %s)", 
            city_id, city_id, city_name);
    }
    
    return result;
}

validation_result validate_tour_addition(char tour_id[], char* tour_name, char startingCity[]) {
    validation_result result = {true, NULL};
    
    int hash = get_tour_hash(tour_id);
    if (hash < 0 || hash >= TOUR_HTABLE_SIZE) {
        result.success = false;
        asprintf(&result.error_message, "ID tour '%s' non valido", tour_id);
        return result;
    }
    
    if (tour_exists(tour_id)) {
        result.success = false;
        asprintf(&result.error_message, 
            "Tour con ID '%s' già presente. Conflitto con '%s'", 
            tour_id, tour_name);
        return result;
    }
    
    if (!city_exists(startingCity)) {
        result.success = false;
        asprintf(&result.error_message, 
            "Città di partenza '%s' non valida per il tour '%s'", 
            startingCity, tour_id);
        return result;
    }
    
    return result;
}

validation_result validate_tour_update(char tour_id[], char nextCity[]) {
    validation_result result = {true, NULL};
    
    if (!tour_exists(tour_id)) {
        result.success = false;
        asprintf(&result.error_message, "Tour '%s' non trovato", tour_id);
        return result;
    }
    
    if (!city_exists(nextCity)) {
        result.success = false;
        asprintf(&result.error_message, "Città '%s' non valida", nextCity);
        return result;
    }
    
    tour* t = get_tour(tour_id);
    if (t && t->stage_count >= MAX_STAGES) {
        result.success = false;
        asprintf(&result.error_message, 
            "Troppo tappe per il tour '%s'. Massimo %d consentite", 
            tour_id, MAX_STAGES);
        return result;
    }
    
    return result;
}

// ** FUNZIONI DI INSERIMENTO (UNSAFE) **
bool add_city_unsafe(char city_id[], char* city_name, double coords[]) {
    int hash = get_city_hash(city_id);
    
    city* new_city = malloc(sizeof(city));
    if (new_city == NULL) {
        semantic_error("Errore di allocazione memoria per città");
        return false;
    }
    
    strncpy(new_city->id, city_id, CITY_ID_SIZE);
    new_city->id[CITY_ID_SIZE] = '\0';
    
    new_city->name = malloc(strlen(city_name) + 1);
    if (new_city->name == NULL) {
        free(new_city);
        semantic_error("Errore di allocazione memoria per nome città");
        return false;
    }
    strcpy(new_city->name, city_name);
    
    new_city->xy_pos[0] = coords[0];
    new_city->xy_pos[1] = coords[1];
    new_city->next = CITY_HTABLE[hash];
    CITY_HTABLE[hash] = new_city;
    
    return true;
}

bool add_tour_unsafe(char tour_id[], char* tour_name, char startingCity[]) {
    int hash = get_tour_hash(tour_id);
    
    tour* new_tour = malloc(sizeof(tour));
    if (new_tour == NULL) {
        semantic_error("Errore di allocazione memoria per tour");
        return false;
    }
    
    strncpy(new_tour->id, tour_id, TOUR_ID_SIZE);
    new_tour->id[TOUR_ID_SIZE] = '\0';
    
    new_tour->name = malloc(strlen(tour_name) + 1);
    if (new_tour->name == NULL) {
        free(new_tour);
        semantic_error("Errore di allocazione memoria per nome tour");
        return false;
    }
    strcpy(new_tour->name, tour_name);
    
    strncpy(new_tour->startingCity, startingCity, CITY_ID_SIZE);
    new_tour->startingCity[CITY_ID_SIZE] = '\0';
    strncpy(new_tour->currentCity, startingCity, CITY_ID_SIZE);
    new_tour->currentCity[CITY_ID_SIZE] = '\0';
    
    strncpy(new_tour->stages[0], startingCity, CITY_ID_SIZE);
    new_tour->stages[0][CITY_ID_SIZE] = '\0';
    new_tour->stage_count = 1;
    
    new_tour->total_distance = 0.0;
    new_tour->total_cost = 250.0;
    
    TOUR_HTABLE[hash] = new_tour;
    return true;
}

bool update_tour_unsafe(char tour_id[], char nextCity[]) {
    tour* t = get_tour(tour_id);
    
    double* current_coords = get_city_coords(t->currentCity);
    double* next_coords = get_city_coords(nextCity);
    
    double dx = current_coords[0] - next_coords[0];
    double dy = current_coords[1] - next_coords[1];
    double distance = sqrt(dx * dx + dy * dy);
    
    t->total_distance += distance;
    t->total_cost += 250.0 + (10.0 * distance);
    
    strncpy(t->currentCity, nextCity, CITY_ID_SIZE);
    t->currentCity[CITY_ID_SIZE] = '\0';
    
    strncpy(t->stages[t->stage_count], nextCity, CITY_ID_SIZE);
    t->stages[t->stage_count][CITY_ID_SIZE] = '\0';
    t->stage_count++;
    
    return true;
}

// ** FUNZIONI DI UTILITÀ **
void free_validation_result(validation_result* result) {
    if (result && result->error_message) {
        free(result->error_message);
        result->error_message = NULL;
    }
}

void semantic_error(const char* message, ...) {
    fprintf(stderr, "ERRORE FATALE: ");
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);
    fprintf(stderr, "\n");

    free_city_ht();
    free_tour_ht();
    exit(EXIT_FAILURE);
}

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

void free_tour_ht() {
    for (int i = 0; i < TOUR_HTABLE_SIZE; ++i) {
        if (TOUR_HTABLE[i] != NULL) {
            free(TOUR_HTABLE[i]->name);
            free(TOUR_HTABLE[i]);
            TOUR_HTABLE[i] = NULL;
        }
    }
}

void print_tours() {
    for (int idx = 0; idx < TOUR_HTABLE_SIZE; ++idx) {
        tour* current_tour = TOUR_HTABLE[idx];
        if (current_tour != NULL) {
            printf("%s: ", current_tour->name);
            
            for (int stop = 0; stop < current_tour->stage_count; stop++) {
                char* location_name = get_city_name(current_tour->stages[stop]);
                if (location_name == NULL) {
                    semantic_error("Nome città non trovato per la tappa '%s' del tour '%s'", 
                                 current_tour->stages[stop], current_tour->id);
                }
                
                if (location_name[0] == '"' && location_name[strlen(location_name)-1] == '"') {
                    int name_length = strlen(location_name);
                    for (int pos = 1; pos < name_length-1; pos++) {
                        printf("%c", location_name[pos]);
                    }
                } else {
                    printf("%s", location_name);
                }
                
                if (stop < current_tour->stage_count - 1) {
                    printf(" -> ");
                }
            }
            
            printf(" %.2f€\n", current_tour->total_cost);
        }
    }
}