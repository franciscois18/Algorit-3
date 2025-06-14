#ifndef CONFIG_H
#define CONFIG_H

#include "tries.h"
#include "hash.h"
#include "document_main.h"
#include "common.h"
#include <stdbool.h>

// Configuración del programa
typedef struct {
    char *corpus_path;
    char *output_file;
    char *search_pattern;
    char *index_file;
    bool recursive;
    bool txt_only;
    bool html_only;
    bool show_stats;
    bool process_docs;
    bool build_index;
    bool search_mode;
    bool benchmark_mode;
    bool approximate_search;
    bool export_csv;
    bool export_json;
    int limit;
    int max_distance;
    char *output_format;
} ProgramConfig;

// Indice de documentos
typedef struct {
    Trie *word_trie;
    HashTable *word_hash;
    DocumentCollection *collection;
    char *index_path;
    time_t created_time;
} DocumentIndex;

// Configuración
ProgramConfig* config_create();
void config_free(ProgramConfig *config);
int parse_arguments(int argc, char *argv[], ProgramConfig *config);
void print_usage(const char *program_name);

#endif 