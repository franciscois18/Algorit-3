#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "preprocessing_structs.h"


// Crear y destruir configuración de tokenización
TokenizerConfig* tokenizer_config_create_default(void);
void tokenizer_config_free(TokenizerConfig *config);

// Crear y manejar lista de tokens
TokenList* token_list_create(size_t initial_capacity);
void token_list_free(TokenList *list);
bool token_list_add(TokenList *list, const char *text, size_t length, 
                   size_t position, bool is_word);
void token_list_print(const TokenList *list);

// Principales de tokenización
TokenList* tokenize_text(const char *text, const TokenizerConfig *config);
TokenList* tokenize_simple(const char *text); 
TokenList* tokenize_advanced(const char *text, const TokenizerConfig *config);

// Auxiliares de tokenización
bool is_word_character(char c);
bool is_punctuation_character(char c);
bool should_split_on_char(char c, const TokenizerConfig *config);

// Utilidad para tokens
char* reconstruct_text_from_tokens(const TokenList *tokens, const char *separator);
bool is_valid_token(const char *token, size_t min_length, size_t max_length);

#endif 