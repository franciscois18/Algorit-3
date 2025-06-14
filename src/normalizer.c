#include "normalizer.h"
#include <ctype.h>


// Obtener el mapeo de acentos
static const AccentMapping* get_accent_map(int *map_size) {
    static const AccentMapping accent_map[] = {
        {"á", "a"}, {"à", "a"}, {"ä", "a"}, {"â", "a"},
        {"é", "e"}, {"è", "e"}, {"ë", "e"}, {"ê", "e"},
        {"í", "i"}, {"ì", "i"}, {"ï", "i"}, {"î", "i"},
        {"ó", "o"}, {"ò", "o"}, {"ö", "o"}, {"ô", "o"},
        {"ú", "u"}, {"ù", "u"}, {"ü", "u"}, {"û", "u"},
        {"ñ", "n"}, {"ç", "c"},
        {"Á", "A"}, {"À", "A"}, {"Ä", "A"}, {"Â", "A"},
        {"É", "E"}, {"È", "E"}, {"Ë", "E"}, {"Ê", "E"},
        {"Í", "I"}, {"Ì", "I"}, {"Ï", "I"}, {"Î", "I"},
        {"Ó", "O"}, {"Ò", "O"}, {"Ö", "O"}, {"Ô", "O"},
        {"Ú", "U"}, {"Ù", "U"}, {"Ü", "U"}, {"Û", "U"},
        {"Ñ", "N"}, {"Ç", "C"}
    };
    
    *map_size = sizeof(accent_map) / sizeof(accent_map[0]);
    return accent_map;
}

// Crear configuración por defecto
NormalizationConfig* normalization_config_create_default(void) {
    NormalizationConfig *config = malloc(sizeof(NormalizationConfig));
    if (!config) return NULL;
    
    config->to_lowercase = true;
    config->remove_accents = true;
    config->normalize_whitespace = true;
    config->remove_special_chars = false;
    config->preserve_numbers = true;
    config->preserve_hyphens = true;
    
    return config;
}

void normalization_config_free(NormalizationConfig *config) {
    if (config) {
        free(config);
    }
}

// Crear texto normalizado
NormalizedText* normalized_text_create(size_t initial_capacity) {
    NormalizedText *text = malloc(sizeof(NormalizedText));
    if (!text) return NULL;
    
    text->normalized_text = malloc(initial_capacity);
    if (!text->normalized_text) {
        free(text);
        return NULL;
    }
    
    text->normalized_text[0] = '\0';
    text->length = 0;
    text->capacity = initial_capacity;
    
    return text;
}

void normalized_text_free(NormalizedText *text) {
    if (text) {
        free(text->normalized_text);
        free(text);
    }
}

// Funciones auxiliares
char remove_accent_char(char c) {
    int map_size;
    const AccentMapping *accent_map = get_accent_map(&map_size);
    
    for (int i = 0; i < map_size; i++) {
        if (accent_map[i].accented[0] == c) {
            return accent_map[i].base[0];
        }
    }
    return c;
}

bool is_accent_char(char c) {
    int map_size;
    const AccentMapping *accent_map = get_accent_map(&map_size);
    
    for (int i = 0; i < map_size; i++) {
        if (accent_map[i].accented[0] == c) {
            return true;
        }
    }
    return false;
}

bool is_whitespace_char(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

// Normalización específica - minúsculas
char* to_lowercase_string(const char *text) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    for (size_t i = 0; i < len; i++) {
        result[i] = tolower(text[i]);
    }
    result[len] = '\0';
    
    return result;
}

// Normalización específica - acentos
char* remove_accents_string(const char *text) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    for (size_t i = 0; i < len; i++) {
        result[i] = remove_accent_char(text[i]);
    }
    result[len] = '\0';
    
    return result;
}

// Normalización específica - espacios en blanco
char* normalize_whitespace_string(const char *text) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    size_t j = 0;
    bool in_whitespace = false;
    
    for (size_t i = 0; i < len; i++) {
        if (is_whitespace_char(text[i])) {
            if (!in_whitespace) {
                result[j++] = ' ';
                in_whitespace = true;
            }
        } else {
            result[j++] = text[i];
            in_whitespace = false;
        }
    }
    
    // Remover espacio final si existe
    if (j > 0 && result[j-1] == ' ') {
        j--;
    }
    
    result[j] = '\0';
    return result;
}

// Normalización específica - puntuación
char* normalize_punctuation_string(const char *text) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    size_t j = 0;
    
    for (size_t i = 0; i < len; i++) {
        char c = text[i];
        
        // Mantener letras, números y algunos caracteres especiales
        if (isalnum(c) || c == ' ' || c == '-' || c == '_') {
            result[j++] = c;
        } else if (ispunct(c)) {
            // Reemplazar puntuación con espacio
            if (j > 0 && result[j-1] != ' ') {
                result[j++] = ' ';
            }
        }
    }
    
    result[j] = '\0';
    return result;
}

// Normalización simple
char* normalize_simple(const char *text) {
    if (!text) return NULL;
    
    NormalizationConfig *config = normalization_config_create_default();
    NormalizedText *normalized = normalize_text(text, config);
    
    char *result = NULL;
    if (normalized) {
        result = malloc(normalized->length + 1);
        if (result) {
            strcpy(result, normalized->normalized_text);
        }
    }
    
    normalized_text_free(normalized);
    normalization_config_free(config);
    
    return result;
}

// Normalización principal
NormalizedText* normalize_text(const char *text, const NormalizationConfig *config) {
    if (!text || !config) return NULL;

    size_t input_len = strlen(text);
    NormalizedText *result = normalized_text_create(input_len + 1);
    if (!result) return NULL;

    int map_size;
    const AccentMapping *accent_map = get_accent_map(&map_size);

    int i = 0;
    int j = 0;
    bool last_char_was_whitespace = false;

    while (text[i] != '\0') {
        unsigned char current_char = text[i];
        bool replaced = false;

        // Intentar reemplazo por acento
        if (config->remove_accents) {
            for (int k = 0; k < map_size; k++) {
                size_t accented_len = strlen(accent_map[k].accented);
                if (strncmp(&text[i], accent_map[k].accented, accented_len) == 0) {
                    current_char = accent_map[k].base[0];
                    i += accented_len;
                    replaced = true;
                    break;
                }
            }
        }

        if (!replaced) {
            current_char = text[i++];
        }

        // Convertir a minúsculas si aplica
        if (config->to_lowercase) {
            current_char = tolower(current_char);
        }

        // Normalizar espacio en blanco
        if (is_whitespace_char(current_char)) {
            if (config->normalize_whitespace && !last_char_was_whitespace) {
                result->normalized_text[j++] = ' ';
                last_char_was_whitespace = true;
            }
            continue;
        }

        // Filtrar si remove_special_chars está activo
        bool keep = false;
        if (config->remove_special_chars) {
            if (isalnum(current_char)) keep = true;
            if (config->preserve_numbers && isdigit(current_char)) keep = true;
            if (config->preserve_hyphens && (current_char == '-' || current_char == '_')) keep = true;
        } else {
            keep = true;
        }

        if (keep) {
            result->normalized_text[j++] = current_char;
            last_char_was_whitespace = false;
        }
    }

    // Quitar espacio final si hay
    if (config->normalize_whitespace && j > 0 && result->normalized_text[j - 1] == ' ') {
        j--;
    }

    result->normalized_text[j] = '\0';
    result->length = j;
    return result;
}