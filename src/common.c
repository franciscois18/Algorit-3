#include "common.h"


SearchResults* create_search_results(int initial_capacity) {
    //  Capacidad por defecto si no se especifica
    if (initial_capacity <= 0) {
        initial_capacity = DEFAULT_RESULTS_CAPACITY; 
    }
    
    SearchResults *results = malloc(sizeof(SearchResults));
    if (!results) return NULL;
    
    results->results = malloc(initial_capacity * sizeof(SearchResult));
    if (!results->results) {
        free(results);
        return NULL;
    }
    
    results->count = 0;
    results->capacity = initial_capacity;
    return results;
}

// Crecimiento más eficiente del array dinámico
void add_search_result(SearchResults *results, int position, int comparisons) {
    if (!results) return;
    
    // Expandir si es necesario con crecimiento menos agresivo
    if (results->count >= results->capacity) {
        // Se usa 1.5x en lugar de 2x para reducir desperdicio de memoria
        int new_capacity = results->capacity + (results->capacity >> 1);
        if (new_capacity <= results->capacity) new_capacity = results->capacity + 1;
        
        SearchResult *new_results = realloc(results->results, 
                                          new_capacity * sizeof(SearchResult));
        if (!new_results) return; 
        
        results->results = new_results;
        results->capacity = new_capacity;
    }
    
    // Acceso directo sin indexación repetida
    SearchResult *current = &results->results[results->count];
    current->position = position;
    current->comparisons = comparisons;
    results->count++;
}

void free_search_results(SearchResults *results) {
    if (results) {
        free(results->results);
        free(results);
    }
}

//  Imprimir resultados de maneraoptimizada
void print_search_results(const SearchResults *results, const char *algorithm_name) {
    if (!results || !algorithm_name) return;
    
    printf("\nResultados de %s:\n", algorithm_name);
    printf("Coincidencias encontradas: %d\n", results->count);
    
    if (results->count > 0) {
        printf("Posiciones: ");
        
    
        for (int i = 0; i < results->count; i++) {
            if (i > 0) printf(", ");
            printf("%d", results->results[i].position);
        }
        
        printf("\nComparaciones totales: %d\n", 
               results->results[results->count - 1].comparisons);
    }
    printf("\n");
}