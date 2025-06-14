#include "document_main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>


// Leer contenido completo de un archivo
char* read_file_content(const char *filepath, size_t *content_length) {
    FILE *file = fopen(filepath, "rb");
    if (!file) return NULL;
    
    // Obtener tamaño del archivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size < 0) {
        fclose(file);
        return NULL;
    }
    
    // Reservar memoria para el contenido + terminador nulo
    char *content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    // Leer contenido
    size_t bytes_read = fread(content, 1, file_size, file);
    content[bytes_read] = '\0';
    
    fclose(file);
    
    if (content_length) {
        *content_length = bytes_read;
    }
    
    return content;
}

// Imprimir estadísticas de carga
void print_loading_stats(const LoadingStats *stats) {
    if (!stats) return;
    
    printf("\n=== ESTADÍSTICAS DE CARGA ===\n");
    printf("Archivos encontrados: %zu\n", stats->total_files_found);
    printf("Archivos cargados: %zu\n", stats->files_loaded);
    printf("Archivos fallidos: %zu\n", stats->files_failed);
    printf("Bytes totales cargados: %zu (%.2f MB)\n", 
           stats->total_bytes_loaded, 
           stats->total_bytes_loaded / (1024.0 * 1024.0));
    printf("Tiempo de carga: %.2f segundos\n", stats->loading_time);
    printf("\nDesglose por tipo:\n");
    printf("  Archivos TXT: %zu\n", stats->txt_files);
    printf("  Archivos HTML: %zu\n", stats->html_files);
    printf("  Otros archivos: %zu\n", stats->other_files);
    
    if (stats->loading_time > 0) {
        printf("Velocidad: %.2f MB/s\n", 
               (stats->total_bytes_loaded / (1024.0 * 1024.0)) / stats->loading_time);
    }
}

// Imprimir información de documento
void print_document_info(const Document *doc) {
    if (!doc || !doc->metadata) return;
    
    printf("\n=== INFORMACIÓN DEL DOCUMENTO ===\n");
    printf("Archivo: %s\n", doc->metadata->filename);
    printf("Ruta: %s\n", doc->metadata->full_path);
    printf("Tipo: ");
    switch (doc->metadata->type) {
        case DOC_TYPE_TXT: printf("Texto plano"); break;
        case DOC_TYPE_HTML: printf("HTML"); break;
        case DOC_TYPE_CSV: printf("CSV"); break;
        default: printf("Desconocido"); break;
    }
    printf("\n");
    printf("Tamaño: %zu bytes\n", doc->metadata->file_size);
    printf("Contenido limpio: %zu caracteres\n", strlen(doc->clean_content));
    printf("Idioma: %s\n", doc->metadata->language);
    printf("Título: %s\n", doc->metadata->title);
}

// Imprimir resumen de colección
void print_collection_summary(const DocumentCollection *collection) {
    if (!collection) return;
    
    printf("\n=== RESUMEN DE LA COLECCIÓN ===\n");
    printf("Documentos cargados: %zu\n", collection->count);
    printf("Capacidad: %zu\n", collection->capacity);
    
    if (collection->collection_path) {
        printf("Ruta base: %s\n", collection->collection_path);
    }
    
    // Estadísticas por tipo
    size_t txt_count = 0, html_count = 0, other_count = 0;
    size_t total_size = 0;
    
    for (size_t i = 0; i < collection->count; i++) {
        Document *doc = collection->documents[i];
        if (doc && doc->metadata) {
            total_size += doc->metadata->file_size;
            switch (doc->metadata->type) {
                case DOC_TYPE_TXT: txt_count++; break;
                case DOC_TYPE_HTML: html_count++; break;
                default: other_count++; break;
            }
        }
    }
    
    printf("\nDesglose por tipo:\n");
    printf("  TXT: %zu\n", txt_count);
    printf("  HTML: %zu\n", html_count);
    printf("  Otros: %zu\n", other_count);
    printf("\nTamaño total: %zu bytes (%.2f MB)\n", 
           total_size, total_size / (1024.0 * 1024.0));
}



// Verificar si un tipo de archivo es soportado
bool is_supported_file_type(const char *filepath) {
    DocumentType type = detect_document_type(filepath);
    return type != DOC_TYPE_UNKNOWN;
}

// Obtener tamaño de archivo
size_t get_file_size(const char *filepath) {
    struct stat file_stat;
    if (stat(filepath, &file_stat) != 0) {
        return 0;
    }
    return file_stat.st_size;
}
