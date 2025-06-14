#ifndef DOCUMENT_STRUCTS_H
#define DOCUMENT_STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_PATH_LENGTH 4096
#define MAX_FILENAME_LENGTH 256
#define INITIAL_BUFFER_SIZE 8192
#define MAX_DOCUMENTS 10000

// Tipos de documentos soportados
typedef enum {
    DOC_TYPE_UNKNOWN,
    DOC_TYPE_TXT,
    DOC_TYPE_HTML,
    DOC_TYPE_CSV
} DocumentType;

// Metadatos del documento
typedef struct {
    char *filename;
    char *full_path;
    size_t file_size;
    DocumentType type;
    time_t last_modified;
    char *title;       
    char *language;     
} DocumentMetadata;

// Estructura para un documento cargado
typedef struct {
    DocumentMetadata *metadata;
    char *raw_content;     
    char *clean_content;    
    size_t content_length;
    bool is_loaded;
} Document;

// Colección de documentos
typedef struct {
    Document **documents;
    size_t count;
    size_t capacity;
    char *collection_path; 
} DocumentCollection;

// Estadísticas de carga
typedef struct {
    size_t total_files_found;
    size_t files_loaded;
    size_t files_failed;
    size_t total_bytes_loaded;
    double loading_time;
    size_t txt_files;
    size_t html_files;
    size_t other_files;
} LoadingStats;


// Tipo de función para filtros
typedef bool (*DocumentFilter)(const char *filepath, const struct stat *file_stat);

#endif