#ifndef SYM_TABLES_H
#define SYM_TABLES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ** COSTANTI **  
#define CITY_ID_SIZE 3              
#define CITY_HTABLE_SIZE 1000       
#define TOUR_ID_SIZE 3              
#define TOUR_HTABLE_SIZE 2600       
#define MAX_STAGES 100              

// ** STRUTTURE PER GESTIONE ERRORI **
typedef struct {
    bool success;
    char* error_message;
} validation_result;

// ** DEFINIZIONI ORIGINALI ** 
typedef struct city {               
    char id[CITY_ID_SIZE + 1];      
    char* name;                     
    double xy_pos[2];               
    struct city* next;              
} city;

typedef struct tour {               
    char id[TOUR_ID_SIZE + 1];      
    char* name;                     
    char startingCity[CITY_ID_SIZE + 1];  
    char currentCity[CITY_ID_SIZE + 1];   
    char stages[MAX_STAGES][CITY_ID_SIZE + 1]; 
    int stage_count;                
    double total_distance;          
    double total_cost;              
} tour;

// Hash table globali
extern city* CITY_HTABLE[CITY_HTABLE_SIZE];
extern tour* TOUR_HTABLE[TOUR_HTABLE_SIZE];

// ** FUNZIONI DI HASHING **
int get_city_hash(char city_id[]);
int get_tour_hash(char tour_id[]);

// ** FUNZIONI DI VALIDAZIONE (PURE) **
validation_result validate_city_addition(char city_id[], char* city_name);
validation_result validate_tour_addition(char tour_id[], char* tour_name, char startingCity[]);
validation_result validate_tour_update(char tour_id[], char nextCity[]);

// ** FUNZIONI DI INSERIMENTO (UNSAFE - ASSUMONO VALIDAZIONE GIÀ FATTA) **
bool add_city_unsafe(char city_id[], char* city_name, double coords[]);
bool add_tour_unsafe(char tour_id[], char* tour_name, char startingCity[]);
bool update_tour_unsafe(char tour_id[], char nextCity[]);

// ** FUNZIONI DI LOOKUP **
double* get_city_coords(char city_id[]);
tour* get_tour(char tour_id[]);
char* get_city_name(char city_id[]);
bool city_exists(char city_id[]);
bool tour_exists(char tour_id[]);

// ** FUNZIONI DI UTILITÀ **
void free_validation_result(validation_result* result);
void semantic_error(const char* message, ...);  // Solo per errori fatali
void print_tours();
void free_city_ht();
void free_tour_ht();

#endif