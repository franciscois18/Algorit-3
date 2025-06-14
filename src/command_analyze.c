#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commands.h"
#include "document_main.h"
#include "preprocessing.h"
#include "tries.h"
#include "hash.h"


// Análisis de documentos
int command_analyze(ProgramConfig *config) {
    printf("\n=== ANÁLISIS DE DOCUMENTOS ===\n");
    
    if (!config->corpus_path) {
        printf("Error: Debe especificar un directorio con -i/--input\n");
        return 1;
    }
    
    // Cargar documentos
    DocumentCollection *collection = document_collection_create(1000);
    LoadingStats *stats = load_documents_from_directory(collection, config->corpus_path, config->recursive);
    
    if (!stats || collection->count == 0) {
        printf("Error: No se pudieron cargar documentos\n");
        document_collection_free(collection);
        return 1;
    }
    
    printf("Analizando %zu documentos...\n", collection->count);
    
    // Crear estructuras para análisis
    Trie *word_trie = trie_create();
    HashTable *word_hash = ht_create(50000);
    TokenizerConfig *tok_config = tokenizer_config_create_default();
    NormalizationConfig *norm_config = normalization_config_create_default();
    StopWordList *stopwords = load_spanish_stopwords();
    
    // Variables para estadísticas
    size_t total_words = 0;
    size_t total_unique_words = 0;
    size_t total_chars = 0;
    size_t total_sentences = 0;
    size_t docs_processed = 0;
    
    clock_t start_time = clock();
    
    printf("Procesando documentos para análisis...\n");
    
    for (size_t i = 0; i < collection->count; i++) {
        Document *doc = collection->documents[i];
        if (!doc || !doc->clean_content) continue;
        
        if (i % 100 == 0) {
            printf("  Progreso: %zu/%zu documentos\r", i + 1, collection->count);
            fflush(stdout);
        }
        
        total_chars += strlen(doc->clean_content);
        
        // Contar oraciones aproximadamente
        char *text = doc->clean_content;
        for (size_t j = 0; text[j]; j++) {
            if (text[j] == '.' || text[j] == '!' || text[j] == '?') {
                total_sentences++;
            }
        }
        
        // Procesar texto
        ProcessedText *processed = process_text_complete(doc->clean_content, 
                                                       tok_config, norm_config, stopwords);
        
        if (processed && processed->tokens) {
            total_words += processed->tokens->count;
            
            // Indexar palabras únicas
            for (size_t j = 0; j < processed->tokens->count; j++) {
                Token *token = &processed->tokens->tokens[j];
                if (token->is_word && token->length >= 2) {
                    trie_insert(word_trie, token->text);
                    
                    int *freq = ht_search(word_hash, token->text);
                    if (freq) {
                        (*freq)++;
                    } else {
                        ht_insert(word_hash, token->text, 1);
                    }
                }
            }
            processed_text_free(processed);
        }
        
        docs_processed++;
    }
    
    clock_t end_time = clock();
    double analysis_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    total_unique_words = word_trie->word_count;
    
    printf("\n\nRESULTADOS DEL ANÁLISIS:\n");
    printf("=====================================\n");
    printf("Documentos procesados:     %zu\n", docs_processed);
    printf("Tiempo de análisis:        %.2f segundos\n", analysis_time);
    printf("Caracteres totales:        %zu\n", total_chars);
    printf("Palabras totales:          %zu\n", total_words);
    printf("Palabras únicas:           %zu\n", total_unique_words);
    printf("Oraciones estimadas:       %zu\n", total_sentences);
    printf("Promedio palabras/doc:     %.1f\n", docs_processed > 0 ? (double)total_words / docs_processed : 0);
    printf("Promedio caracteres/doc:   %.1f\n", docs_processed > 0 ? (double)total_chars / docs_processed : 0);
    printf("Diversidad lexical:        %.3f\n", total_words > 0 ? (double)total_unique_words / total_words : 0);
    printf("Velocidad de procesamiento: %.1f docs/seg\n", analysis_time > 0 ? docs_processed / analysis_time : 0);
    
    // Análisis adicional de estadísticas
    if (config->show_stats) {
        printf("\nESTADÍSTICAS DETALLADAS:\n");
        printf("=====================================\n");
        
        // Distribución por tamaño de documento
        size_t small_docs = 0, medium_docs = 0, large_docs = 0;
        for (size_t i = 0; i < collection->count; i++) {
            Document *doc = collection->documents[i];
            if (!doc) continue;
            
            if (doc->metadata->file_size < 1024) small_docs++;
            else if (doc->metadata->file_size < 10240) medium_docs++;
            else large_docs++;
        }
        
        printf("Distribución por tamaño:\n");
        printf("  - Pequeños (< 1KB):     %zu (%.1f%%)\n", small_docs, 100.0 * small_docs / collection->count);
        printf("  - Medianos (1-10KB):    %zu (%.1f%%)\n", medium_docs, 100.0 * medium_docs / collection->count);
        printf("  - Grandes (> 10KB):     %zu (%.1f%%)\n", large_docs, 100.0 * large_docs / collection->count);
        
        // Estadísticas del Trie
        printf("\nEstructura del índice:\n");
        printf("  - Nodos en Trie:        %zu\n", word_trie->node_count);
        printf("  - Profundidad promedio: %d\n", trie_get_depth(word_trie));
        printf("  - Factor de carga hash: %.2f\n", (double)word_hash->size / word_hash->capacity);
        
        // Mostrar palabras más frecuentes
        printf("\nPalabras más frecuentes encontradas:\n");
        PrefixResult *common_prefixes[] = {
            trie_get_words_with_prefix(word_trie, "de"),
            trie_get_words_with_prefix(word_trie, "la"),
            trie_get_words_with_prefix(word_trie, "el"),
            trie_get_words_with_prefix(word_trie, "en"),
            trie_get_words_with_prefix(word_trie, "un")
        };
        
        const char *prefixes[] = {"de", "la", "el", "en", "un"};
        
        for (int i = 0; i < 5; i++) {
            if (common_prefixes[i] && common_prefixes[i]->count > 0) {
                printf("  - Palabras con '%s': %d palabras\n", prefixes[i], common_prefixes[i]->count);
            }
            prefix_result_free(common_prefixes[i]);
        }
    }
    
    // Exportar resultados si se especificó
    if (config->output_file) {
        FILE *output = fopen(config->output_file, "w");
        if (output) {
            if (config->export_json) {
                fprintf(output, "{\n");
                fprintf(output, "  \"analysis_results\": {\n");
                fprintf(output, "    \"documents_processed\": %zu,\n", docs_processed);
                fprintf(output, "    \"analysis_time\": %.2f,\n", analysis_time);
                fprintf(output, "    \"total_characters\": %zu,\n", total_chars);
                fprintf(output, "    \"total_words\": %zu,\n", total_words);
                fprintf(output, "    \"unique_words\": %zu,\n", total_unique_words);
                fprintf(output, "    \"estimated_sentences\": %zu,\n", total_sentences);
                fprintf(output, "    \"lexical_diversity\": %.3f,\n", total_words > 0 ? (double)total_unique_words / total_words : 0);
                fprintf(output, "    \"avg_words_per_doc\": %.1f,\n", docs_processed > 0 ? (double)total_words / docs_processed : 0);
                fprintf(output, "    \"processing_speed\": %.1f\n", analysis_time > 0 ? docs_processed / analysis_time : 0);
                fprintf(output, "  }\n}\n");
            } else if (config->export_csv) {
                fprintf(output, "metric,value\n");
                fprintf(output, "documents_processed,%zu\n", docs_processed);
                fprintf(output, "analysis_time,%.2f\n", analysis_time);
                fprintf(output, "total_characters,%zu\n", total_chars);
                fprintf(output, "total_words,%zu\n", total_words);
                fprintf(output, "unique_words,%zu\n", total_unique_words);
                fprintf(output, "estimated_sentences,%zu\n", total_sentences);
                fprintf(output, "lexical_diversity,%.3f\n", total_words > 0 ? (double)total_unique_words / total_words : 0);
            } else {
                fprintf(output, "REPORTE DE ANÁLISIS DE DOCUMENTOS\n");
                fprintf(output, "==================================\n\n");
                fprintf(output, "Directorio analizado: %s\n", config->corpus_path);
                fprintf(output, "Fecha del análisis: %s", ctime(&start_time));
                fprintf(output, "Documentos procesados: %zu\n", docs_processed);
                fprintf(output, "Tiempo total: %.2f segundos\n\n", analysis_time);
                fprintf(output, "MÉTRICAS PRINCIPALES:\n");
                fprintf(output, "- Caracteres totales: %zu\n", total_chars);
                fprintf(output, "- Palabras totales: %zu\n", total_words);
                fprintf(output, "- Palabras únicas: %zu\n", total_unique_words);
                fprintf(output, "- Diversidad lexical: %.3f\n", total_words > 0 ? (double)total_unique_words / total_words : 0);
            }
            fclose(output);
            printf("\nResultados del análisis guardados en: %s\n", config->output_file);
        }
    }
    
    tokenizer_config_free(tok_config);
    normalization_config_free(norm_config);
    stopword_list_free(stopwords);
    trie_free(word_trie);
    ht_free(word_hash);
    loading_stats_free(stats);
    document_collection_free(collection);
    
    return 0;
}