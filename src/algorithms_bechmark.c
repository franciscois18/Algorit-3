#include "benchmark.h"
#include "kmp.h"
#include "shift_and.h"
#include "shift_or.h"


// Medir tiempo de ejecución
double measure_algorithm_time(SearchResults* (*algorithm_func)(const char*, const char*),
                             const char *text, const char *pattern) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    SearchResults *results = algorithm_func(text, pattern);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    if (results) {
        free_search_results(results);
    }
    
    double elapsed = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

AlgorithmStats calculate_algorithm_stats(SearchResults* (*algorithm_func)(const char*, const char*),
                                        const char *text, const char *pattern) {
    AlgorithmStats stats = {0};
    
    // Medir tiempo de ejecución
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    SearchResults *results = algorithm_func(text, pattern);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calcular tiempo transcurrido
    stats.execution_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    
    if (results) {
        stats.matches_found = results->count;
        if (results->count > 0) {
            stats.total_comparisons = results->results[results->count - 1].comparisons;
        }
        
        // Calcular throughput aproximado
        size_t text_length = strlen(text);
        if (stats.execution_time > 0) {
            stats.throughput_mbps = (text_length / (1024.0 * 1024.0)) / stats.execution_time;
        }
        
        // Memoria usada (aproximación)
        stats.memory_used = sizeof(SearchResults) + 
                           (results->capacity * sizeof(SearchResult));
        
        free_search_results(results);
    }
    
    return stats;
}

ComparisonReport* generate_comparison_report(const char *text, const char *pattern, 
                                           const char *description) {
    if (!text || !pattern) return NULL;
    
    ComparisonReport *report = malloc(sizeof(ComparisonReport));
    if (!report) return NULL;
    
    // Copiar descripción
    if (description) {
        report->test_description = malloc(strlen(description) + 1);
        if (report->test_description) {
            strcpy(report->test_description, description);
        }
    } else {
        report->test_description = NULL;
    }
    
    report->text_length = strlen(text);
    report->pattern_length = strlen(pattern);
    
    // Calcular estadísticas para cada algoritmo
    report->kmp_stats = calculate_algorithm_stats(kmp_search, text, pattern);
    report->shift_and_stats = calculate_algorithm_stats(shift_and_search, text, pattern);
    report->shift_or_stats = calculate_algorithm_stats(shift_or_search, text, pattern);
    
    return report;
}

void print_comparison_report(const ComparisonReport *report) {
    if (!report) return;
    
    printf("\n");
    for (int i = 0; i < 80; i++) printf("=");
    printf("\n");
    printf("REPORTE DE COMPARACIÓN DE ALGORITMOS\n");
    for (int i = 0; i < 80; i++) printf("=");
    printf("\n");
    
    if (report->test_description) {
        printf("Descripción: %s\n", report->test_description);
    }
    
    printf("Longitud del texto: %zu caracteres\n", report->text_length);
    printf("Longitud del patrón: %zu caracteres\n", report->pattern_length);
    printf("\n");
    
    // Tabla de resultados
    printf("%-12s %-12s %-12s %-12s %-12s %-12s\n",
           "Algoritmo", "Tiempo(s)", "Coincid.", "Comparac.", "Memoria(B)", "Thrput(MB/s)");
    for (int i = 0; i < 80; i++) printf("-");
    printf("\n");
    
    printf("%-12s %-12.6f %-12d %-12d %-12zu %-12.2f\n",
           "KMP", 
           report->kmp_stats.execution_time,
           report->kmp_stats.matches_found,
           report->kmp_stats.total_comparisons,
           report->kmp_stats.memory_used,
           report->kmp_stats.throughput_mbps);
    
    printf("%-12s %-12.6f %-12d %-12d %-12zu %-12.2f\n",
           "Shift-And",
           report->shift_and_stats.execution_time,
           report->shift_and_stats.matches_found,
           report->shift_and_stats.total_comparisons,
           report->shift_and_stats.memory_used,
           report->shift_and_stats.throughput_mbps);
    
    printf("%-12s %-12.6f %-12d %-12d %-12zu %-12.2f\n",
           "Shift-Or",
           report->shift_or_stats.execution_time,
           report->shift_or_stats.matches_found,
           report->shift_or_stats.total_comparisons,
           report->shift_or_stats.memory_used,
           report->shift_or_stats.throughput_mbps);
    
    printf("\n");
    
    // Análisis de rendimiento
    printf("ANÁLISIS DE RENDIMIENTO:\n");
    for (int i = 0; i < 40; i++) printf("-");
    printf("\n");
    
    // Encontrar el algoritmo más rápido
    double min_time = report->kmp_stats.execution_time;
    const char *fastest = "KMP";
    
    if (report->shift_and_stats.execution_time < min_time) {
        min_time = report->shift_and_stats.execution_time;
        fastest = "Shift-And";
    }
    
    if (report->shift_or_stats.execution_time < min_time) {
        min_time = report->shift_or_stats.execution_time;
        fastest = "Shift-Or";
    }
    
    printf("Algoritmo más rápido: %s (%.6f segundos)\n", fastest, min_time);
    
    // Encontrar el que hace menos comparaciones
    int min_comparisons = report->kmp_stats.total_comparisons;
    const char *most_efficient = "KMP";
    
    if (report->shift_and_stats.total_comparisons < min_comparisons) {
        min_comparisons = report->shift_and_stats.total_comparisons;
        most_efficient = "Shift-And";
    }
    
    if (report->shift_or_stats.total_comparisons < min_comparisons) {
        min_comparisons = report->shift_or_stats.total_comparisons;
        most_efficient = "Shift-Or";
    }
    
    printf("Menos comparaciones: %s (%d comparaciones)\n", most_efficient, min_comparisons);
    
    printf("\n");
}

void free_comparison_report(ComparisonReport *report) {
    if (report) {
        free(report->test_description);
        free(report);
    }
}