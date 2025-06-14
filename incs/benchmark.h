#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "common.h"
#include <stdbool.h>

typedef struct {
    double execution_time;
    int total_comparisons;
    int matches_found;
    size_t memory_used;
    double throughput_mbps;
} AlgorithmStats;

typedef struct {
    AlgorithmStats kmp_stats;
    AlgorithmStats shift_and_stats;
    AlgorithmStats shift_or_stats;
    char *test_description;
    size_t text_length;
    size_t pattern_length;
    char *document_name;  
    char *pattern;         
    time_t timestamp;      
} ComparisonReport;

double measure_algorithm_time(SearchResults* (*algorithm_func)(const char*, const char*),
                             const char *text, const char *pattern);
AlgorithmStats calculate_algorithm_stats(SearchResults* (*algorithm_func)(const char*, const char*),
                                        const char *text, const char *pattern);
ComparisonReport* generate_comparison_report(const char *text, const char *pattern, 
                                           const char *description);
void print_comparison_report(const ComparisonReport *report);
void free_comparison_report(ComparisonReport *report);

#endif

