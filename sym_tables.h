#ifndef SYM_TABLES_H
#define SYM_TABLES_H

#include <stdio.h>
#include <stdlib.h>

// ** COSTANTI **  
#define CITY_ID_SIZE 3              // numero di caratteri alfabetici maiuscoli che compongono l'id della città
#define CITY_HTABLE_SIZE 631        // grandezza definita dal valore massimo prodotto d'hashing dell'id + 1 
#define TOUR_ID_SIZE 3              // lettera maiuscola + 2 cifre
#define TOUR_HTABLE_SIZE 2600       // 26 lettere * 100 combinazioni cifre = 2600 tour possibili
#define MAX_STAGES 100              // numero massimo di tappe per tour

// ** DEFINIZIONI ** 
typedef struct city {               // struttura per memorizzare le città nella HT
    char id[CITY_ID_SIZE + 1];      // identificativo univoco composto da 3 lettere maiuscole + '\0'
    char* name;                     // puntatore al nome della città
    double xy_pos[2];               // array con le coordinate della città
    struct city* next;              // puntatore a struttura successiva per le collisioni
} city;

typedef struct tour {               // struttura per memorizzare i tour nella HT
    char id[TOUR_ID_SIZE + 1];      // identificativo univoco lettera + 2 cifre + '\0'
    char* name;                     // puntatore al nome completo del tour
    char startingCity[CITY_ID_SIZE + 1];  // Id città di partenza
    char currentCity[CITY_ID_SIZE + 1];   // Id città corrente
    char stages[MAX_STAGES][CITY_ID_SIZE + 1]; // array delle tappe percorse
    int stage_count;                // numero di tappe percorse
    double total_distance;          // distanza percorsa totale
    double total_cost;              // costo totale del tour
} tour;

// Hash table per le città (con gestione collisioni)
extern city* CITY_HTABLE[CITY_HTABLE_SIZE];

// Hash table per i tour (accesso diretto)
extern tour* TOUR_HTABLE[TOUR_HTABLE_SIZE];

/* 
* Funzione che genera un hash per le città in base al nome tramite
* algoritmo che minimizza le collisioni. Prende input l'id di 3 lettere
* maiuscole della singola città e ne restituisce l'hash come intero.
*/
int get_city_hash(char city_id[]);

/*
* Funzione che genera un hash per i tour in base all'ID.
* Converte lettera maiuscola + 2 cifre in indice univoco.
* Esempio: X25 -> (X-A)*100 + 25 = 23*100 + 25 = 2325
*/
int get_tour_hash(char tour_id[]);

/*
* Funzione che aggiunge una città nella hashtable gestendo le collisioni.
* Prende a parametro i dati di una città.
*/
void add_city(char city_id[], char* city_name, double coords[]);

/*
* Funzione che aggiunge un tour alla hashtable.
* Accesso diretto senza collisioni grazie alla funzione di hash perfetta.
*/
void add_tour(char tour_id[], char* tour_name, char startingCity[]);

/*
* Funzione che gestisce l'aggiornamento del percorso di un tour.
* Aggiunge una nuova tappa e calcola la distanza e il costo.
*/
void update_tour(char tour_id[], char nextCity[]);

/*
* Funzione ausiliaria che restituisce le coordinate di una città.
* Prende a parametro un ID città e ritorna un puntatore ad un array
* di 2 dimensioni (x, y)
*/
double* get_city_coords(char city_id[]);

/*
* Funzione ausiliaria che restituisce un tour.
* Prende a parametro un id tour e ritorna il puntatore alla struttura 
* contenente il tour.
*/
tour* get_tour(char tour_id[]);

/*
* Funzione ausiliaria che restituisce il nome di una città.
* Prende a parametro un ID città e ritorna il nome della città.
*/
char* get_city_name(char city_id[]);

/*
* Funzione ausiliaria usata per la stampa degli errori semantici.
* Prende a parametro il messaggio ed eventuali parametri di formattazione 
* stringa (es. "errore %s", "tour_id").
*/
void semantic_error(const char* message, ...);

/*
* Funzione di dump di dati che genera un output nel formato richiesto:
* Nome tour: città1 -> città2 -> ... -> cittàN costoTotale€
*/
void dump();

/*
* Funzioni per la corretta gestione della memoria a 
* fine programma. Le funzioni gestiscono il deallocamento
* di tutte le strutture dinamiche.
*/
void free_city_ht();
void free_tour_ht();

#endif