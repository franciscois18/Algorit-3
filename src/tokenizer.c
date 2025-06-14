#include "tokenizer.h"
#include <ctype.h>

// Crear configuración por defecto
TokenizerConfig* tokenizer_config_create_default(void) {
    TokenizerConfig *config = malloc(sizeof(TokenizerConfig));
    if (!config) return NULL;
    
    config->preserve_case = false;
    config->keep_punctuation = false;
    config->split_on_whitespace = true;
    config->split_on_punctuation = true;
    config->merge_whitespace = true;
    config->min_token_length = 1;
    config->max_token_length = MAX_TOKEN_LENGTH;
    
    return config;
}


// Crear lista de tokens
TokenList* token_list_create(size_t initial_capacity) {
    TokenList *list = malloc(sizeof(TokenList));
    if (!list) return NULL;
    
    list->tokens = malloc(initial_capacity * sizeof(Token));
    if (!list->tokens) {
        free(list);
        return NULL;
    }
    
    list->count = 0;
    list->capacity = initial_capacity;
    list->total_length = 0;
    
    return list;
}

// Liberar lista de tokens
void token_list_free(TokenList *list) {
    if (!list) return;
    
    for (size_t i = 0; i < list->count; i++) {
        free(list->tokens[i].text);
    }
    free(list->tokens);
    free(list);
}

// Agregar token a la lista
bool token_list_add(TokenList *list, const char *text, size_t length, 
                   size_t position, bool is_word) {
    if (!list || !text) return false;
    
    // Redimensionar si es necesario
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        Token *new_tokens = realloc(list->tokens, new_capacity * sizeof(Token));
        if (!new_tokens) return false;
        
        list->tokens = new_tokens;
        list->capacity = new_capacity;
    }
    
    // Crear copia del texto
    char *token_text = malloc(length + 1);
    if (!token_text) return false;
    
    strncpy(token_text, text, length);
    token_text[length] = '\0';
    
    // Agregar token
    Token *token = &list->tokens[list->count];
    token->text = token_text;
    token->length = length;
    token->position = position;
    token->is_word = is_word;
    
    list->count++;
    list->total_length += length;
    
    return true;
}

// Imprimir lista de tokens
void token_list_print(const TokenList *list) {
    if (!list) return;
    
    printf("TokenList: %zu tokens, %zu total length\n", list->count, list->total_length);
    for (size_t i = 0; i < list->count; i++) {
        Token *token = &list->tokens[i];
        printf("  [%zu] '%s' (len=%zu, pos=%zu, word=%s)\n", 
               i, token->text, token->length, token->position, 
               token->is_word ? "yes" : "no");
    }
}


// Tokenización simple
TokenList* tokenize_simple(const char *text) {
    TokenizerConfig *config = tokenizer_config_create_default();
    TokenList *result = tokenize_text(text, config);
    tokenizer_config_free(config);
    return result;
}

// Tokenización principal
TokenList* tokenize_text(const char *text, const TokenizerConfig *config) {
    if (!text || !config) return NULL;
    
    TokenList *list = token_list_create(100);
    if (!list) return NULL;
    
    size_t text_len = strlen(text);
    size_t start = 0;
    
    for (size_t i = 0; i <= text_len; i++) {
        char c = text[i];
        bool should_split = (c == '\0') || should_split_on_char(c, config);
        
        if (should_split) {
            // Procesar token acumulado
            if (i > start) {
                size_t token_len = i - start;
                
                // Verificar longitud
                if (token_len >= config->min_token_length && 
                    token_len <= config->max_token_length) {
                    
                    // Determinar si es palabra
                    bool is_word = true;
                    for (size_t j = start; j < i; j++) {
                        if (!is_word_character(text[j])) {
                            is_word = false;
                            break;
                        }
                    }
                    
                    // Crear token
                    char *token_text = malloc(token_len + 1);
                    if (token_text) {
                        strncpy(token_text, text + start, token_len);
                        token_text[token_len] = '\0';
                        
                        // Convertir a minúsculas si es necesario
                        if (!config->preserve_case) {
                            for (size_t j = 0; j < token_len; j++) {
                                token_text[j] = tolower(token_text[j]);
                            }
                        }
                        
                        token_list_add(list, token_text, token_len, start, is_word);
                        free(token_text);
                    }
                }
            }
            
            // Agregar puntuación si se debe mantener
            if (c != '\0' && config->keep_punctuation && is_punctuation_character(c)) {
                char punct[2] = {c, '\0'};
                token_list_add(list, punct, 1, i, false);
            }
            
            // Saltar espacios en blanco consecutivos si merge_whitespace está activo
            if (config->merge_whitespace && isspace(c)) {
                while (i + 1 < text_len && isspace(text[i + 1])) {
                    i++;
                }
            }
            
            start = i + 1;
        }
    }
    
    return list;
}

// Tokenización avanzada (alias para compatibilidad)
TokenList* tokenize_advanced(const char *text, const TokenizerConfig *config) {
    return tokenize_text(text, config);
}

// Reconstruir texto desde tokens
char* reconstruct_text_from_tokens(const TokenList *tokens, const char *separator) {
    if (!tokens || tokens->count == 0) return NULL;
    
    size_t sep_len = separator ? strlen(separator) : 0;
    size_t total_len = tokens->total_length + (tokens->count - 1) * sep_len + 1;
    
    char *result = malloc(total_len);
    if (!result) return NULL;
    
    result[0] = '\0';
    
    for (size_t i = 0; i < tokens->count; i++) {
        if (i > 0 && separator) {
            strcat(result, separator);
        }
        strcat(result, tokens->tokens[i].text);
    }
    
    return result;
}


// Funciones auxiliares
bool is_word_character(char c) {
    return isalnum(c) || c == '_';
}

bool is_punctuation_character(char c) {
    return ispunct(c) && c != '_';
}

bool should_split_on_char(char c, const TokenizerConfig *config) {
    if (config->split_on_whitespace && isspace(c)) {
        return true;
    }
    if (config->split_on_punctuation && is_punctuation_character(c)) {
        return true;
    }
    return false;
}


bool is_valid_token(const char *token, size_t min_length, size_t max_length) {
    if (!token) return false;
    size_t len = strlen(token);
    return len >= min_length && len <= max_length;
}

void tokenizer_config_free(TokenizerConfig *config) {
    if (config) {
        free(config);
    }
}
