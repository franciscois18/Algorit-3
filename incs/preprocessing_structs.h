#ifndef PREPROCESSING_STRUCTS_H
#define PREPROCESSING_STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>


#define MAX_TOKEN_LENGTH 256
#define MAX_TOKENS_PER_TEXT 10000
#define DEFAULT_STOPWORDS_CAPACITY 1000

// Almacenar un token
typedef struct {
    char *text;
    size_t length;
    size_t position;     
    bool is_word;       
} Token;

// Almacenar una lista de tokens
typedef struct {
    Token *tokens;
    size_t count;
    size_t capacity;
    size_t total_length; 
} TokenList;

// Configuración de tokenización
typedef struct {
    bool preserve_case;          
    bool keep_punctuation;       
    bool split_on_whitespace;     
    bool split_on_punctuation;   
    bool merge_whitespace;        
    size_t min_token_length;      
    size_t max_token_length;      
} TokenizerConfig;

// Estructura para normalización de texto
typedef struct {
    char *normalized_text;
    size_t length;
    size_t capacity;
} NormalizedText;

// Estructura para configuración de normalización
typedef struct {
    bool to_lowercase;           
    bool remove_accents;          
    bool normalize_whitespace;   
    bool remove_special_chars;    
    bool preserve_numbers;        
    bool preserve_hyphens;       
} NormalizationConfig;

// Estructura para lista de stopwords
typedef struct {
    char **words;
    size_t count;
    size_t capacity;
} StopWordList;

// Estructura para estadísticas de preprocesamiento
typedef struct {
    size_t original_length;
    size_t processed_length;
    size_t tokens_count;
    size_t words_count;
    size_t punctuation_count;
    size_t stopwords_removed;
    size_t special_chars_removed;
    double compression_ratio;
} PreprocessingStats;

// Procesamiento completo de texto
typedef struct {
    TokenList *tokens;
    NormalizedText *normalized;
    PreprocessingStats *stats;
} ProcessedText;

#endif 