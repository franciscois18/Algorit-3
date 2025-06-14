#include "kmp.h"


// ALGORITMO KMP OPTIMIZADO

KMPTable* kmp_preprocess(const char *pattern) {
    if (!pattern) return NULL;
    
    int pattern_len = strlen(pattern);
    if (pattern_len == 0) return NULL;
    
    KMPTable *table = malloc(sizeof(KMPTable));
    if (!table) return NULL;
    
    table->pattern_length = pattern_len;
    table->failure_function = calloc(pattern_len, sizeof(int)); // inicializa a 0
    
    if (!table->failure_function) {
        free(table);
        return NULL;
    }
    
    // Construcción de tabla de fallos mejorada
    int j = 0;
    for (int i = 1; i < pattern_len; i++) {
        // Reducir comparaciones innecesarias
        while (j > 0 && pattern[i] != pattern[j]) {
            j = table->failure_function[j - 1];
        }
        
        if (pattern[i] == pattern[j]) {
            j++;
        }
        
        table->failure_function[i] = j;
    }
    
    return table;
}

SearchResults* kmp_search(const char *text, const char *pattern) {
    if (!text || !pattern) return NULL;
    
    KMPTable *table = kmp_preprocess(pattern);
    if (!table) return NULL;
    
    SearchResults *results = create_search_results(DEFAULT_RESULTS_CAPACITY);
    if (!results) {
        kmp_free_table(table);
        return NULL;
    }
    
    int text_len = strlen(text);
    int pattern_len = table->pattern_length;
    int comparisons = 0;
    int j = 0;
    
    // Cachear accesos a la tabla de fallos
    const int *failure_function = table->failure_function;
    
    for (int i = 0; i < text_len; i++) {
        // Minimizar accesos a la función de fallo
        while (j > 0 && text[i] != pattern[j]) {
            j = failure_function[j - 1];
            comparisons++;
        }
        
        comparisons++;
        if (text[i] == pattern[j]) {
            j++;
        }
        
        // Patrón encontrado
        if (j == pattern_len) {
            int match_pos = i - pattern_len + 1;
            add_search_result(results, match_pos, comparisons);
            j = failure_function[j - 1]; // Continuar buscando coincidencias
        }
    }
    
    kmp_free_table(table);
    return results;
}

// Liberar memoria de la tabla KMP
void kmp_free_table(KMPTable *table) {
    if (table) {
        free(table->failure_function);
        free(table);
    }
}