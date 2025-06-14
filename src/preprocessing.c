#include "preprocessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Preprocesamiento completo
ProcessedText* process_text_complete(const char *text, 
                                   const TokenizerConfig *tok_config,
                                   const NormalizationConfig *norm_config,
                                   const StopWordList *stopwords) {
    if (!text) {
        fprintf(stderr, "Error: Texto de entrada es NULL\n");
        return NULL;
    }

    // Crear estructura de texto procesado
    ProcessedText *processed = malloc(sizeof(ProcessedText));
    if (!processed) {
        fprintf(stderr, "Error: No se pudo asignar memoria para ProcessedText\n");
        return NULL;
    }

    // Inicializar campos
    processed->tokens = NULL;
    processed->normalized = NULL;
    processed->stats = NULL;

    // Normalización 
    NormalizedText *normalized_text = NULL;
    const char *text_to_tokenize = text;

    if (norm_config) {
        normalized_text = normalize_text(text, norm_config);
        if (normalized_text && normalized_text->normalized_text) {
            text_to_tokenize = normalized_text->normalized_text;
        } else {
            fprintf(stderr, "Advertencia: Error en normalización, usando texto original\n");
        }
    }

    // Tokenización
    TokenList *tokens = NULL;
    if (tok_config) {
        tokens = tokenize_text(text_to_tokenize, tok_config);
    } else {
        tokens = tokenize_simple(text_to_tokenize);
    }

    if (!tokens) {
        fprintf(stderr, "Error: No se pudieron generar tokens\n");
        if (normalized_text) normalized_text_free(normalized_text);
        free(processed);
        return NULL;
    }

    // Filtrado de stopwords 
    TokenList *filtered_tokens = tokens;
    if (stopwords) {
        filtered_tokens = filter_stopwords(tokens, stopwords);
        if (filtered_tokens && filtered_tokens != tokens) {
            // Solo liberar tokens originales si se creó una nueva lista
            token_list_free(tokens);
            tokens = filtered_tokens;
        }
    }

    // Asignar resultados
    processed->tokens = tokens;
    processed->normalized = normalized_text;

    // Calcular estadísticas
    processed->stats = calculate_preprocessing_stats(text, tokens, normalized_text);

    return processed;
}

// Liberar memoria de texto procesado
void processed_text_free(ProcessedText *processed) {
    if (!processed) return;

    if (processed->tokens) {
        token_list_free(processed->tokens);
    }
    
    if (processed->normalized) {
        normalized_text_free(processed->normalized);
    }
    
    if (processed->stats) {
        preprocessing_stats_free(processed->stats);
    }
    
    free(processed);
}

// FUNCIONES DE PRUEBA Y AUX
  

void test_tokenization(void) {
    printf("\n=== PRUEBA DE TOKENIZACIÓN ===\n");
    
    const char *test_text = "Hola mundo! ¿Cómo estás? Esto es una prueba de tokenización.";
    
    printf("Texto original: %s\n", test_text);
    
    // Prueba tokenización simple
    printf("\n--- Tokenización Simple ---\n");
    TokenList *simple_tokens = tokenize_simple(test_text);
    if (simple_tokens) {
        debug_print_tokens(simple_tokens);
        token_list_free(simple_tokens);
    }
    
    // Prueba tokenización avanzada
    printf("\n--- Tokenización Avanzada ---\n");
    TokenizerConfig *config = tokenizer_config_create_default();
    if (config) {
        TokenList *advanced_tokens = tokenize_advanced(test_text, config);
        if (advanced_tokens) {
            debug_print_tokens(advanced_tokens);
            token_list_free(advanced_tokens);
        }
        tokenizer_config_free(config);
    }
}

void test_normalization(void) {
    printf("\n=== PRUEBA DE NORMALIZACIÓN ===\n");
    
    const char *test_text = "HOLA Múndo! ¿Cómo estás?   Esto    es UNA prueba.";
    
    printf("Texto original: '%s'\n", test_text);
    
    // Prueba normalización simple
    printf("\n--- Normalización Simple ---\n");
    char *simple_normalized = normalize_simple(test_text);
    if (simple_normalized) {
        printf("Normalizado: '%s'\n", simple_normalized);
        free(simple_normalized);
    }
    
    // Prueba normalización avanzada
    printf("\n--- Normalización Avanzada ---\n");
    NormalizationConfig *config = normalization_config_create_default();
    if (config) {
        NormalizedText *normalized = normalize_text(test_text, config);
        if (normalized) {
            debug_print_normalization(test_text, normalized);
            normalized_text_free(normalized);
        }
        normalization_config_free(config);
    }
}

void test_stopwords(void) {
    printf("\n=== PRUEBA DE STOPWORDS ===\n");
    
    const char *test_text = "Esta es una prueba de eliminación de palabras vacías en español";
    
    printf("Texto original: %s\n", test_text);
    
    // Tokenizar primero
    TokenList *tokens = tokenize_simple(test_text);
    if (!tokens) {
        printf("Error en tokenización\n");
        return;
    }
    
    printf("\n--- Tokens Originales ---\n");
    debug_print_tokens(tokens);
    
    // Cargar stopwords en español
    StopWordList *stopwords = load_spanish_stopwords();
    if (stopwords) {
        TokenList *filtered = filter_stopwords(tokens, stopwords);
        if (filtered) {
            printf("\n--- Tokens Después de Filtrar Stopwords ---\n");
            debug_print_tokens(filtered);
            token_list_free(filtered);
        }
        stopword_list_free(stopwords);
    }
    
    token_list_free(tokens);
}

void test_complete_preprocessing(void) {
    printf("\n=== PRUEBA DE PREPROCESAMIENTO COMPLETO ===\n");
    
    const char *test_text = "HOLA Múndo! Esta es una PRUEBA completa de preprocesamiento. "
                           "¿Funciona correctamente el sistema? ¡Esperamos que sí!";
    
    printf("Texto original: %s\n", test_text);
    
    // Crear configuraciones
    TokenizerConfig *tok_config = tokenizer_config_create_default();
    NormalizationConfig *norm_config = normalization_config_create_default();
    StopWordList *stopwords = load_spanish_stopwords();
    
    if (!tok_config || !norm_config || !stopwords) {
        printf("Error creando configuraciones\n");
        goto cleanup;
    }
    
    // Procesar texto completo
    ProcessedText *processed = process_text_complete(test_text, tok_config, norm_config, stopwords);
    
    if (processed) {
        printf("\n--- RESULTADOS DEL PREPROCESAMIENTO ---\n");
        
        if (processed->normalized) {
            printf("Texto normalizado: '%s'\n", 
                   processed->normalized->normalized_text ? processed->normalized->normalized_text : "NULL");
        }
        
        if (processed->tokens) {
            printf("\nTokens procesados:\n");
            debug_print_tokens(processed->tokens);
        }
        
        if (processed->stats) {
            printf("\nEstadísticas:\n");
            print_preprocessing_stats(processed->stats);
        }
        
        processed_text_free(processed);
    }
    
cleanup:
    if (tok_config) tokenizer_config_free(tok_config);
    if (norm_config) normalization_config_free(norm_config);
    if (stopwords) stopword_list_free(stopwords);
}

// FUNCIONES DE DEPURACION

void debug_print_tokens(const TokenList *list) {
    if (!list) {
        printf("Lista de tokens es NULL\n");
        return;
    }
    
    printf("Total de tokens: %zu\n", list->count);
    printf("Longitud total: %zu\n", list->total_length);
    
    for (size_t i = 0; i < list->count && i < 20; i++) {
        Token *token = &list->tokens[i];
        printf("[%zu] '%s' (pos: %zu, len: %zu, es_palabra: %s)\n", 
               i, 
               token->text ? token->text : "NULL", 
               token->position, 
               token->length,
               token->is_word ? "sí" : "no");
    }
    
    if (list->count > 20) {
        printf("... y %zu tokens más\n", list->count - 20);
    }
}

void debug_print_normalization(const char *original, const NormalizedText *normalized) {
    if (!original || !normalized) {
        printf("Parámetros inválidos para debug_print_normalization\n");
        return;
    }
    
    printf("Texto original (%zu caracteres): '%s'\n", strlen(original), original);
    
    if (normalized->normalized_text) {
        printf("Texto normalizado (%zu caracteres): '%s'\n", 
               normalized->length, normalized->normalized_text);
        
        size_t orig_len = strlen(original);
        double reduction = orig_len > 0 ? 
            (double)(orig_len - normalized->length) / orig_len * 100.0 : 0.0;
        
        printf("Reducción: %.2f%%\n", reduction);
    } else {
        printf("Texto normalizado: NULL\n");
    }
}