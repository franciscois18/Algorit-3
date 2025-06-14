#include "shift_and.h"

// IMPLEMENTACIÓN OPTIMIZADA DEL ALGORITMO SHIFT-AND

ShiftAndTable* shift_and_preprocess(const char *pattern) {
    if (!pattern) return NULL;
    
    int pattern_len = strlen(pattern);
    if (pattern_len == 0 || pattern_len > MAX_PATTERN_LENGTH) {
        return NULL;
    }
    
    ShiftAndTable *table = malloc(sizeof(ShiftAndTable));
    if (!table) return NULL;
    
    table->pattern_length = pattern_len;
    
    // Inicialización eficiente usando memset
    memset(table->pattern_mask, 0, sizeof(table->pattern_mask));
    
    //  Generar máscara de una sola pasada
    for (int i = 0; i < pattern_len; i++) {
        unsigned char c = (unsigned char)pattern[i];
        table->pattern_mask[c] |= (1ULL << i);
    }
    
    return table;
}

SearchResults* shift_and_search(const char *text, const char *pattern) {
    if (!text || !pattern) return NULL;
    
    ShiftAndTable *table = shift_and_preprocess(pattern);
    if (!table) return NULL;
    
    SearchResults *results = create_search_results(DEFAULT_RESULTS_CAPACITY);
    if (!results) {
        shift_and_free_table(table);
        return NULL;
    }
    
    int text_len = strlen(text);
    uint64_t state = 0;
    uint64_t match_mask = 1ULL << (table->pattern_length - 1);
    int comparisons = 0;
    
    // Cachear variables frecuentemente usadas
    const uint64_t *pattern_mask = table->pattern_mask;
    const int pattern_length = table->pattern_length;
    
    for (int i = 0; i < text_len; i++) {
        unsigned char c = (unsigned char)text[i];
        
        state = ((state << 1) | 1) & pattern_mask[c];
        comparisons++;
        
        // Verificar coincidencia completa
        if (state & match_mask) {
            int match_pos = i - pattern_length + 1;
            add_search_result(results, match_pos, comparisons);
        }
    }
    
    shift_and_free_table(table);
    return results;
}


// Búsqueda con múltiples patrones (Shift-And optimizado)
SearchResults* multi_pattern_shift_and_search(const char *text, 
                                             const char **patterns, 
                                             int num_patterns) {
    if (!text || !patterns || num_patterns <= 0) return NULL;
    
    SearchResults *results = create_search_results(20);
    if (!results) return NULL;
    
    // Preparar tablas para todos los patrones
    ShiftAndTable **tables = malloc(num_patterns * sizeof(ShiftAndTable*));
    if (!tables) {
        free_search_results(results);
        return NULL;
    }
    
    // Preprocesar todos los patrones
    for (int p = 0; p < num_patterns; p++) {
        tables[p] = shift_and_preprocess(patterns[p]);
        if (!tables[p]) {
            // Limpiar en caso de error
            for (int j = 0; j < p; j++) {
                shift_and_free_table(tables[j]);
            }
            free(tables);
            free_search_results(results);
            return NULL;
        }
    }
    
    int text_len = strlen(text);
    int total_comparisons = 0;
    
    // Array estático para estados (máximo 64 patrones)
    uint64_t states[64] = {0};
    
    // Procesar texto una sola vez para todos los patrones
    for (int i = 0; i < text_len; i++) {
        unsigned char c = (unsigned char)text[i];
        
        // Verificar todos los patrones en paralelo
        for (int p = 0; p < num_patterns && p < 64; p++) {
            ShiftAndTable *table = tables[p];
            
            states[p] = ((states[p] << 1) | 1) & table->pattern_mask[c];
            total_comparisons++;
            
            uint64_t match_mask = 1ULL << (table->pattern_length - 1);
            if (states[p] & match_mask) {
                int match_pos = i - table->pattern_length + 1;
                add_search_result(results, match_pos, total_comparisons);
            }
        }
    }
    
    // Limpiar tablas
    for (int p = 0; p < num_patterns; p++) {
        shift_and_free_table(tables[p]);
    }
    free(tables);
    
    return results;
}

void shift_and_free_table(ShiftAndTable *table) {
    free(table);
}