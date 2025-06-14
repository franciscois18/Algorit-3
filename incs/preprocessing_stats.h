#ifndef PREPROCESSING_STATS_H
#define PREPROCESSING_STATS_H

#include "preprocessing_structs.h"

// Crear y calcular estadísticas
PreprocessingStats* preprocessing_stats_create(void);
void preprocessing_stats_free(PreprocessingStats *stats);
PreprocessingStats* calculate_preprocessing_stats(const char *original_text, 
                                                 const TokenList *tokens,
                                                 const NormalizedText *normalized);

// Imprimir estadísticas
void print_preprocessing_stats(const PreprocessingStats *stats);

#endif 