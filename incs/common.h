#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


#define MAX_PATTERN_LENGTH 63
#define DEFAULT_RESULTS_CAPACITY 10
#define MIN_TEXT_LENGTH_FOR_OPTIMIZATION 1000

typedef struct {
    int position;      
    int comparisons;   
} SearchResult;

typedef struct {
    SearchResult *results;
    int count;
    int capacity;
} SearchResults;



// Manejo de resultados
SearchResults* create_search_results(int initial_capacity);
void add_search_result(SearchResults *results, int position, int comparisons);
void free_search_results(SearchResults *results);
void print_search_results(const SearchResults *results, const char *algorithm_name);

// Utilidad y testing
void run_tests(void);
void print_usage(const char *program_name);
void compare_algorithms(const char *text, const char *pattern);

#endif