#include "document_main.h"

#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>


// Procesar directorio de forma recursiva
static void process_directory(const char *dir_path, ProcessContext *ctx) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("Advertencia: No se puede abrir directorio %s: %s\n", dir_path, strerror(errno));
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Saltar . y ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Construir ruta completa
        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        struct stat file_stat;
        if (stat(full_path, &file_stat) != 0) {
            printf("Advertencia: No se puede obtener información de %s: %s\n", full_path, strerror(errno));
            continue;
        }
        
        if (S_ISDIR(file_stat.st_mode)) {
            // Es un directorio
            if (ctx->recursive) {
                process_directory(full_path, ctx);
            }
        } else if (S_ISREG(file_stat.st_mode)) {
            // Es un archivo regular
            ctx->stats->total_files_found++;
            
            // Aplicar filtro
            if (ctx->filter(full_path, &file_stat)) {
                // El archivo pasa el filtro, intentar cargarlo
                Document *doc = load_document_from_file(full_path);
                if (doc) {
                    if (add_document_to_collection(ctx->collection, doc)) {
                        ctx->stats->files_loaded++;
                        ctx->stats->total_bytes_loaded += file_stat.st_size;
                        
                        // Actualizar contadores por tipo
                        switch (doc->metadata->type) {
                            case DOC_TYPE_TXT:
                                ctx->stats->txt_files++;
                                break;
                            case DOC_TYPE_HTML:
                                ctx->stats->html_files++;
                                break;
                            default:
                                ctx->stats->other_files++;
                                break;
                        }
                    } else {
                        document_free(doc);
                        ctx->stats->files_failed++;
                    }
                } else {
                    ctx->stats->files_failed++;
                }
            }
        }
    }
    
    closedir(dir);
}

// Cargar documentos con filtro personalizado
LoadingStats* load_documents_with_filter(DocumentCollection *collection,
                                        const char *directory_path,
                                        DocumentFilter filter,
                                        bool recursive) {
    if (!collection || !directory_path || !filter) {
        return NULL;
    }
    
    LoadingStats *stats = loading_stats_create();
    if (!stats) {
        return NULL;
    }
    
    clock_t start_time = clock();
    
    // Crear contexto para la función recursiva
    ProcessContext ctx = {
        .collection = collection,
        .filter = filter,
        .stats = stats,
        .recursive = recursive
    };
    
    // Iniciar procesamiento desde el directorio raíz
    process_directory(directory_path, &ctx);
    
    clock_t end_time = clock();
    stats->loading_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    return stats;
}

// Filtro que acepta todos los archivos soportados
static bool filter_all_supported(const char *filepath, const struct stat *file_stat) {
    (void)file_stat; // Suprimir advertencia de parámetro no usado
    return is_supported_file_type(filepath);
}

// Cargar documentos desde directorio (versión simplificada)
LoadingStats* load_documents_from_directory(DocumentCollection *collection, 
                                          const char *directory_path, 
                                          bool recursive) {
    return load_documents_with_filter(collection, directory_path, 
                                     filter_all_supported, recursive);
}

// Cargar documentos desde lista de archivos
LoadingStats* load_documents_from_file_list(DocumentCollection *collection,
                                           const char *file_list_path) {
    if (!collection || !file_list_path) {
        return NULL;
    }
    
    LoadingStats *stats = loading_stats_create();
    if (!stats) {
        return NULL;
    }
    
    clock_t start_time = clock();
    
    FILE *file_list = fopen(file_list_path, "r");
    if (!file_list) {
        loading_stats_free(stats);
        return NULL;
    }
    
    char filepath[MAX_PATH_LENGTH];
    while (fgets(filepath, sizeof(filepath), file_list)) {
        // Remover salto de línea
        filepath[strcspn(filepath, "\n")] = 0;
        
        if (strlen(filepath) == 0) continue; // Saltar líneas vacías
        
        stats->total_files_found++;
        
        // Verificar si el archivo es soportado
        if (is_supported_file_type(filepath)) {
            Document *doc = load_document_from_file(filepath);
            if (doc) {
                if (add_document_to_collection(collection, doc)) {
                    stats->files_loaded++;
                    stats->total_bytes_loaded += doc->metadata->file_size;
                    
                    // Actualizar contadores por tipo
                    switch (doc->metadata->type) {
                        case DOC_TYPE_TXT:
                            stats->txt_files++;
                            break;
                        case DOC_TYPE_HTML:
                            stats->html_files++;
                            break;
                        default:
                            stats->other_files++;
                            break;
                    }
                } else {
                    document_free(doc);
                    stats->files_failed++;
                }
            } else {
                stats->files_failed++;
            }
        } else {
            stats->files_failed++; // Contar archivos no soportados como fallidos
        }
    }
    
    fclose(file_list);
    
    clock_t end_time = clock();
    stats->loading_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    return stats;
}