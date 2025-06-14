#ifndef NORMALIZER_H
#define NORMALIZER_H

#include "preprocessing_structs.h"



// Estructura para  el mapeo de acentos
typedef struct {
    const char* accented;
    const char* base;
} AccentMapping;


// Crear y destruir configuración de normalización
NormalizationConfig* normalization_config_create_default(void);
void normalization_config_free(NormalizationConfig *config);

// Crear y manejar texto normalizado
NormalizedText* normalized_text_create(size_t initial_capacity);
void normalized_text_free(NormalizedText *text);

// Principales de normalización
NormalizedText* normalize_text(const char *text, const NormalizationConfig *config);
char* normalize_simple(const char *text);  

// Específicas de normalización
char* to_lowercase_string(const char *text);
char* remove_accents_string(const char *text);
char* normalize_whitespace_string(const char *text);
char* normalize_punctuation_string(const char *text);

// Auxiliares de normalización
char remove_accent_char(char c);
bool is_accent_char(char c);
bool is_whitespace_char(char c);

#endif 