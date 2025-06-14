#ifndef DOCUMENT_UTILS_H
#define DOCUMENT_UTILS_H

#include "document_structs.h"

typedef struct {
    DocumentCollection *collection;
    DocumentFilter filter;
    LoadingStats *stats;
    bool recursive;
} ProcessContext;

// Utilidades de archivos
bool is_supported_file_type(const char *filepath);
size_t get_file_size(const char *filepath);
char* read_file_content(const char *filepath, size_t *content_length);

// Funciones de estadísticas y debug
void print_loading_stats(const LoadingStats *stats);
void print_document_info(const Document *doc);
void print_collection_summary(const DocumentCollection *collection);

#endif