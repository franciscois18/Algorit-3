#ifndef SHIFT_OR_H
#define SHIFT_OR_H

#include "common.h"

typedef struct {
    uint64_t pattern_mask[256];  
    int pattern_length;
} ShiftOrTable;


ShiftOrTable* shift_or_preprocess(const char *pattern);

SearchResults* shift_or_search(const char *text, const char *pattern);

// Funcion para limpiar la tabla de Shift-Or
void shift_or_free_table(ShiftOrTable *table);

#endif