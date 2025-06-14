#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commands.h"
#include "document_main.h"
#include "preprocessing.h"
#include "tries.h"
#include "hash.h"

// Construir índice
int command_index(ProgramConfig *config) {
    printf("\n=== CONSTRUYENDO ÍNDICE ===\n");
    
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
    
    printf("Documentos cargados: %zu\n", collection->count);
    
    // Crear estructuras de indexación
    Trie *trie = trie_create();
    HashTable *hash_table = ht_create(10000);
    
    if (!trie || !hash_table) {
        printf("Error: No se pudieron crear las estructuras de indexación\n");
        document_collection_free(collection);
        return 1;
    }
    
    // Procesar documentos y construir índice
    TokenizerConfig *tok_config = tokenizer_config_create_default();
    NormalizationConfig *norm_config = normalization_config_create_default();
    StopWordList *stopwords = load_spanish_stopwords();
    
    clock_t start_time = clock();
    size_t total_words_indexed = 0;
    
    printf("Indexando documentos...\n");
    for (size_t i = 0; i < collection->count; i++) {
        Document *doc = collection->documents[i];
        if (!doc || !doc->clean_content) continue;
        
        if (i % 100 == 0 || i == collection->count - 1) {
            printf("  Progreso: %zu/%zu documentos\r", i + 1, collection->count);
            fflush(stdout);
        }
        
        ProcessedText *processed = process_text_complete(doc->clean_content, 
                                                       tok_config, norm_config, stopwords);
        
        if (processed && processed->tokens) {
            // Indexar en Trie
            for (size_t j = 0; j < processed->tokens->count; j++) {
                Token *token = &processed->tokens->tokens[j];
                if (token->is_word && token->length >= 3) {
                    trie_insert(trie, token->text);
                    
                    // También indexar en hash table
                    int *freq = ht_search(hash_table, token->text);
                    if (freq) {
                        (*freq)++;
                    } else {
                        ht_insert(hash_table, token->text, 1);
                    }
                    total_words_indexed++;
                }
            }
            processed_text_free(processed);
        }
    }
    
    clock_t end_time = clock();
    double indexing_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("\n\nÍNDICE CONSTRUIDO EXITOSAMENTE:\n");
    printf("  - Palabras únicas indexadas: %zu\n", trie->word_count);
    printf("  - Total de palabras procesadas: %zu\n", total_words_indexed);
    printf("  - Tiempo de indexación: %.2f segundos\n", indexing_time);
    printf("  - Nodos en Trie: %zu\n", trie->node_count);
    
    // Guardar índice si se especificó archivo
    if (config->index_file) {
        printf("Guardando índice en %s...\n", config->index_file);
        printf("Nota: Funcionalidad de persistencia pendiente de implementación\n");
    }
    
    // Mostrar estadísticas del Trie
    if (config->show_stats) {
        printf("\nESTADÍSTICAS DEL ÍNDICE:\n");
        trie_print_stats(trie);
        
        // Mostrar algunas palabras como ejemplo
        printf("\nEjemplos de palabras indexadas:\n");
        PrefixResult *result = trie_get_words_with_prefix(trie, "al");
        if (result && result->count > 0) {
            printf("Palabras que empiezan con 'al':\n");
            int show_count = result->count < 10 ? result->count : 10;
            for (int i = 0; i < show_count; i++) {
                printf("  - %s (frecuencia: %d)\n", result->words[i], result->frequencies[i]);
            }
        }
        prefix_result_free(result);
    }
    
    tokenizer_config_free(tok_config);
    normalization_config_free(norm_config);
    stopword_list_free(stopwords);
    trie_free(trie);
    ht_free(hash_table);
    loading_stats_free(stats);
    document_collection_free(collection);
    
    return 0;
}