#ifndef KMP_H
#define KMP_H

#include "common.h"

typedef struct {
    int *failure_function;  
    int pattern_length;
} KMPTable;

KMPTable* kmp_preprocess(const char *pattern);
SearchResults* kmp_search(const char *text, const char *pattern);
void kmp_free_table(KMPTable *table);

#endif