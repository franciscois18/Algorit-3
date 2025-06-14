#ifndef STOPWORDS_H
#define STOPWORDS_H

#include "preprocessing_structs.h"

// Crear y manejar lista de stopwords
StopWordList* stopword_list_create(size_t initial_capacity);
void stopword_list_free(StopWordList *list);
bool stopword_list_add(StopWordList *list, const char *word);
bool stopword_list_contains(const StopWordList *list, const char *word);

// Cargar stopwords predefinidas
StopWordList* load_spanish_stopwords(void);
StopWordList* load_english_stopwords(void);
StopWordList* load_stopwords_from_file(const char *filename);

// Filtrar stopwords de tokens
TokenList* filter_stopwords(const TokenList *tokens, const StopWordList *stopwords);

#endif 