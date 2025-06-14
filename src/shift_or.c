#include "shift_or.h"


// IMPLEMENTACIÓN OPTIMIZADA DEL ALGORITMO SHIFT-OR

ShiftOrTable* shift_or_preprocess(const char *pattern) {
    if (!pattern) return NULL;
    
    int pattern_len = strlen(pattern);
    if (pattern_len == 0 || pattern_len > MAX_PATTERN_LENGTH) {
        return NULL;
    }
    
    ShiftOrTable *table = malloc(sizeof(ShiftOrTable));
    if (!table) return NULL;
    
    table->pattern_length = pattern_len;
    
    // Inicialización más eficiente
    for (int i = 0; i < 256; i++) {
        table->pattern_mask[i] = ~0ULL;
    }
    
    // Crear máscara específica para cada carácter del patrón
    for (int i = 0; i < pattern_len; i++) {
        unsigned char c = (unsigned char)pattern[i];
        table->pattern_mask[c] &= ~(1ULL << i);
    }
    
    return table;
}

SearchResults* shift_or_search(const char *text, const char *pattern) {
    if (!text || !pattern) return NULL;
    
    ShiftOrTable *table = shift_or_preprocess(pattern);
    if (!table) return NULL;
    
    SearchResults *results = create_search_results(DEFAULT_RESULTS_CAPACITY);
    if (!results) {
        shift_or_free_table(table);
        return NULL;
    }
    
    int text_len = strlen(text);
    uint64_t state = ~0ULL;
    uint64_t match_mask = ~(1ULL << (table->pattern_length - 1));
    int comparisons = 0;
    
    //  Cachear variables frecuentemente usadas
    const uint64_t *pattern_mask = table->pattern_mask;
    
    for (int i = 0; i < text_len; i++) {
        unsigned char c = (unsigned char)text[i];
        
        state = (state << 1) | pattern_mask[c];
        comparisons++;
        
        // Verificar si encontramos una coincidencia completa
        if (state <= match_mask) {
            int match_pos = i - table->pattern_length + 1;
            add_search_result(results, match_pos, comparisons);
        }
    }
    
    shift_or_free_table(table);
    return results;
}

void shift_or_free_table(ShiftOrTable *table) {
    free(table); 
}