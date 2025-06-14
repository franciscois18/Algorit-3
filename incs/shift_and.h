#ifndef SHIFT_AND_H
#define SHIFT_AND_H

#include "common.h"

typedef struct {
    uint64_t pattern_mask[256];  
    int pattern_length;
} ShiftAndTable;

ShiftAndTable* shift_and_preprocess(const char *pattern);
SearchResults* shift_and_search(const char *text, const char *pattern);
SearchResults* multi_pattern_shift_and_search(const char *text, 
                                             const char **patterns, 
                                             int num_patterns);
void shift_and_free_table(ShiftAndTable *table);

#endif