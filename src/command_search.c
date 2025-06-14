#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "commands.h"
#include "document_main.h"
#include "preprocessing.h"
#include "kmp.h"
#include "benchmark.h"
#include "common.h"
    

int command_search(ProgramConfig *config) {
    printf("\n=== BÚSQUEDA DE PATRONES ===\n");
    
    if (!config->corpus_path || !config->search_pattern) {
        printf("Error: Debe especificar directorio (-i) y patrón (-p)\n");
        return 1;
    }
    
    printf("Buscando patrón: \"%s\"\n", config->search_pattern);
    printf("Directorio: %s\n", config->corpus_path);
    
    // Cargar documentos
    DocumentCollection *collection = document_collection_create(1000);
    LoadingStats *stats = load_documents_from_directory(collection, config->corpus_path, config->recursive);
    
    if (!stats || collection->count == 0) {
        printf("Error: No se pudieron cargar documentos\n");
        document_collection_free(collection);
        return 1;
    }
    
    printf("Documentos cargados: %zu\n", collection->count);
    
    // Realizar búsqueda en cada documento
    clock_t start_time = clock();
    size_t total_matches = 0;
    size_t documents_with_matches = 0;
    
    printf("\nBuscando en documentos...\n");
    
    FILE *output = stdout;
    if (config->output_file) {
        output = fopen(config->output_file, "w");
        if (!output) {
            printf("Error: No se pudo abrir archivo de salida\n");
            output = stdout;
        }
    }
    
    // Escribir cabecera según formato
    if (config->export_csv && output != stdout) {
        fprintf(output, "document,matches,positions\n");
    } else if (config->export_json && output != stdout) {
        fprintf(output, "{\n  \"search_results\": {\n");
        fprintf(output, "    \"pattern\": \"%s\",\n", config->search_pattern);
        fprintf(output, "    \"documents\": [\n");
    }
    
    for (size_t i = 0; i < collection->count; i++) {
        Document *doc = collection->documents[i];
        if (!doc || !doc->clean_content) continue;
        
        if (i % 50 == 0) {
            printf("  Progreso: %zu/%zu documentos\r", i + 1, collection->count);
            fflush(stdout);
        }
        
        // Usar algoritmo KMP por defecto
        SearchResults *results = kmp_search(doc->clean_content, config->search_pattern);
        
        if (results && results->count > 0) {
            documents_with_matches++;
            total_matches += results->count;
            
            if (config->export_csv && output != stdout) {
                fprintf(output, "\"%s\",%d,\"", doc->metadata->filename, results->count);
                for (int j = 0; j < results->count; j++) {
                    fprintf(output, "%d", results->results[j].position);
                    if (j < results->count - 1) fprintf(output, ",");
                }
                fprintf(output, "\"\n");
            } else if (config->export_json && output != stdout) {
                fprintf(output, "      {\n");
                fprintf(output, "        \"document\": \"%s\",\n", doc->metadata->filename);
                fprintf(output, "        \"matches\": %d,\n", results->count);
                fprintf(output, "        \"positions\": [");
                for (int j = 0; j < results->count; j++) {
                    fprintf(output, "%d", results->results[j].position);
                    if (j < results->count - 1) fprintf(output, ", ");
                }
                fprintf(output, "]\n");
                fprintf(output, "      }%s\n", i < collection->count - 1 ? "," : "");
            } else {
                fprintf(output, "\n=== %s ===\n", doc->metadata->filename);
                fprintf(output, "Coincidencias encontradas: %d\n", results->count);
                for (int j = 0; j < results->count && j < 10; j++) {
                    int pos = results->results[j].position;
                    fprintf(output, "  Posición %d: ", pos);
                    
                    // Mostrar contexto alrededor de la coincidencia
                    int start = pos - 30 > 0 ? pos - 30 : 0;
                    int end = pos + strlen(config->search_pattern) + 30;
                    if (end > (int)strlen(doc->clean_content)) 
                        end = strlen(doc->clean_content);
                    
                    char context[200];
                    strncpy(context, doc->clean_content + start, end - start);
                    context[end - start] = '\0';
                    
                    // Limpiar saltos de línea para mejor visualización
                    for (int k = 0; context[k]; k++) {
                        if (context[k] == '\n' || context[k] == '\r') context[k] = ' ';
                    }
                    
                    fprintf(output, "...%s...\n", context);
                }
                if (results->count > 10) {
                    fprintf(output, "  ... y %d coincidencias más\n", results->count - 10);
                }
            }
        }
        
        free_search_results(results);
    }
    
    // Cerrar formato JSON
    if (config->export_json && output != stdout) {
        fprintf(output, "    ],\n");
        fprintf(output, "    \"summary\": {\n");
        fprintf(output, "      \"total_matches\": %zu,\n", total_matches);
        fprintf(output, "      \"documents_with_matches\": %zu,\n", documents_with_matches);
        fprintf(output, "      \"total_documents\": %zu\n", collection->count);
        fprintf(output, "    }\n");
        fprintf(output, "  }\n}\n");
    }
    
    clock_t end_time = clock();
    double search_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("\n\nRESULTADOS DE BÚSQUEDA:\n");
    printf("  - Patrón buscado: \"%s\"\n", config->search_pattern);
    printf("  - Documentos analizados: %zu\n", collection->count);
    printf("  - Documentos con coincidencias: %zu\n", documents_with_matches);
    printf("  - Total de coincidencias: %zu\n", total_matches);
    printf("  - Tiempo de búsqueda: %.3f segundos\n", search_time);
    printf("  - Velocidad: %.2f docs/segundo\n", collection->count / search_time);
    
    if (output != stdout) {
        fclose(output);
        printf("  - Resultados guardados en: %s\n", config->output_file);
    }
    
    loading_stats_free(stats);
    document_collection_free(collection);
    
    return 0;
}