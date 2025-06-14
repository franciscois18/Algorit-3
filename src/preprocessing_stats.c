#include "preprocessing_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Crear estructura de estadísticas
PreprocessingStats* preprocessing_stats_create(void) {
    PreprocessingStats *stats = malloc(sizeof(PreprocessingStats));
    if (!stats) {
        fprintf(stderr, "Error: No se pudo asignar memoria para PreprocessingStats\n");
        return NULL;
    }
    
    // Inicializar todos los campos a cero
    stats->original_length = 0;
    stats->processed_length = 0;
    stats->tokens_count = 0;
    stats->words_count = 0;
    stats->punctuation_count = 0;
    stats->stopwords_removed = 0;
    stats->special_chars_removed = 0;
    stats->compression_ratio = 0.0;
    
    return stats;
}

// Liberar memoria de estadísticas
void preprocessing_stats_free(PreprocessingStats *stats) {
    if (stats) {
        free(stats);
    }
}

// Auxiliar para contar caracteres especiales
static size_t count_special_chars(const char *text) {
    if (!text) return 0;
    
    size_t count = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        // Considerar especiales
        if (!isalnum(c) && !isspace(c) && 
            c != '.' && c != ',' && c != '!' && c != '?' && 
            c != ':' && c != ';' && c != '-' && c != '_') {
            count++;
        }
    }
    return count;
}



// Función auxiliares

static size_t count_punctuation(const char *text) {
    if (!text) return 0;
    
    size_t count = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (ispunct(text[i])) {
            count++;
        }
    }
    return count;
}




// Contar palabras en tokens
static size_t count_words_in_tokens(const TokenList *tokens) {
    if (!tokens) return 0;
    
    size_t word_count = 0;
    for (size_t i = 0; i < tokens->count; i++) {
        if (tokens->tokens[i].is_word) {
            word_count++;
        }
    }
    return word_count;
}

// Estimar stopwords removidas
static size_t estimate_stopwords_removed(const char *original_text, const TokenList *tokens) {
    if (!original_text || !tokens) return 0;
    
    // Tokenización simple del texto original para comparar
    size_t original_word_count = 0;
    bool in_word = false;
    
    for (size_t i = 0; original_text[i] != '\0'; i++) {
        if (isalnum(original_text[i])) {
            if (!in_word) {
                original_word_count++;
                in_word = true;
            }
        } else {
            in_word = false;
        }
    }
    
    size_t processed_word_count = count_words_in_tokens(tokens);
    
    // La diferencia puede ser una aproximación de stopwords removidas
    return (original_word_count > processed_word_count) ? 
           (original_word_count - processed_word_count) : 0;
}

// Calcular estadísticas de preprocesamiento
PreprocessingStats* calculate_preprocessing_stats(const char *original_text, 
                                                 const TokenList *tokens,
                                                 const NormalizedText *normalized) {
    if (!original_text) {
        fprintf(stderr, "Error: Texto original es NULL\n");
        return NULL;
    }
    
    PreprocessingStats *stats = preprocessing_stats_create();
    if (!stats) {
        return NULL;
    }
    
    // Estadísticas del texto original
    stats->original_length = strlen(original_text);
    stats->punctuation_count = count_punctuation(original_text);
    stats->special_chars_removed = count_special_chars(original_text);
    
    // Estadísticas del texto normalizado
    if (normalized && normalized->normalized_text) {
        stats->processed_length = normalized->length;
        // Calcular caracteres especiales removidos
        size_t normalized_special = count_special_chars(normalized->normalized_text);
        stats->special_chars_removed = (stats->special_chars_removed > normalized_special) ?
                                      (stats->special_chars_removed - normalized_special) : 0;
    } else {
        stats->processed_length = stats->original_length;
        stats->special_chars_removed = 0;
    }
    
    // Estadísticas de tokens
    if (tokens) {
        stats->tokens_count = tokens->count;
        stats->words_count = count_words_in_tokens(tokens);
        stats->stopwords_removed = estimate_stopwords_removed(original_text, tokens);
    }
    
    // Calcular ratio de compresión
    if (stats->original_length > 0) {
        stats->compression_ratio = (double)stats->processed_length / (double)stats->original_length;
    } else {
        stats->compression_ratio = 1.0;
    }
    
    return stats;
}

// Imprimir estadísticas de forma detallada
void print_preprocessing_stats(const PreprocessingStats *stats) {
    if (!stats) {
        printf("Estadísticas no disponibles (NULL)\n");
        return;
    }
    
    printf("=== ESTADÍSTICAS DE PREPROCESAMIENTO ===\n\n");
    
    printf("LONGITUDES\n");
    printf("Texto original:      %8zu caracteres\n", stats->original_length);
    printf("Texto procesado:     %8zu caracteres\n", stats->processed_length);
    printf("Reducción:           %8ld caracteres\n", (long)stats->original_length - (long)stats->processed_length);
    printf("Ratio de compresión: %11.2f%%\n\n", stats->compression_ratio * 100.0);
    
    printf("TOKENIZACIÓN\n");
    printf("Total de tokens:     %8zu\n", stats->tokens_count);
    printf("Palabras:            %8zu\n", stats->words_count);
    printf("Puntuación:          %8zu\n\n", stats->punctuation_count);
    
    printf("FILTRADO\n");
    printf("Stopwords removidas: %8zu\n", stats->stopwords_removed);
    printf("Caract. especiales:  %8zu\n\n", stats->special_chars_removed);
    
    // Estadísticas adicionales calculadas
    if (stats->tokens_count > 0) {
        double avg_token_length = (double)stats->processed_length / (double)stats->tokens_count;
        printf("=== ESTADÍSTICAS ADICIONALES ===\n");
        printf("Longitud promedio por token: %.2f caracteres\n", avg_token_length);
        
        if (stats->words_count > 0) {
            double word_ratio = (double)stats->words_count / (double)stats->tokens_count * 100.0;
            printf("Porcentaje de tokens que son palabras: %.2f%%\n", word_ratio);
        }
        
        if (stats->original_length > 0) {
            double token_density = (double)stats->tokens_count / (double)stats->original_length * 100.0;
            printf("Densidad de tokens: %.2f tokens por 100 caracteres\n", token_density);
        }
    }
    
    // Análisis de eficiencia
    printf("\n=== ANÁLISIS DE EFICIENCIA ===\n");
    if (stats->compression_ratio < 0.8) {
        printf("Buena compresión de texto (reducción > 20%%)\n");
    } else if (stats->compression_ratio < 0.95) {
        printf("Compresión moderada de texto\n");
    } else {
        printf("Poca compresión de texto\n");
    }
    
    if (stats->stopwords_removed > 0) {
        printf("Se removieron %zu stopwords\n", stats->stopwords_removed);
    } else {
        printf("No se removieron stopwords\n");
    }
    
    if (stats->special_chars_removed > 0) {
        printf("Se removieron %zu caracteres especiales\n", stats->special_chars_removed);
    } else {
        printf("No se removieron caracteres especiales\n");
    }
    
    printf("\n");
}