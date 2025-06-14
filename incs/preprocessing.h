#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "preprocessing_structs.h"
#include "tokenizer.h"
#include "normalizer.h"
#include "stopwords.h"
#include "preprocessing_stats.h"


// Funciones principales de preprocesamiento
ProcessedText* process_text_complete(const char *text, 
                                   const TokenizerConfig *tok_config,
                                   const NormalizationConfig *norm_config,
                                   const StopWordList *stopwords);

void processed_text_free(ProcessedText *processed);


// Funciones para pruebas
void test_tokenization(void);
void test_normalization(void);
void test_stopwords(void);
void test_complete_preprocessing(void);

// Funciones de depuracion
void debug_print_tokens(const TokenList *list);
void debug_print_normalization(const char *original, const NormalizedText *normalized);

#endif 