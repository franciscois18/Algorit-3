#include "document_main.h"
#include <string.h>
#include <strings.h>

// Filtro para archivos HTML únicamente
bool filter_html_files_only(const char *filepath, const struct stat *file_stat) {
    (void)file_stat; 
    
    if (!filepath) return false;
    
    const char *ext = strrchr(filepath, '.');
    if (!ext) return false;
    
    return (strcasecmp(ext, ".html") == 0 || 
            strcasecmp(ext, ".htm") == 0);
}

// Filtro para archivos de texto únicamente
bool filter_text_files_only(const char *filepath, const struct stat *file_stat) {
    (void)file_stat; 
    
    if (!filepath) return false;
    
    const char *ext = strrchr(filepath, '.');
    if (!ext) return false;
    
    return strcasecmp(ext, ".txt") == 0;
}

// Filtro por rango de tamaño (implementación de ejemplo)
bool filter_by_size_range(const char *filepath, const struct stat *file_stat) {
    (void)filepath; 
    
    if (!file_stat) return false;
    
    // Filtrar archivos entre 1KB y 10MB
    return (file_stat->st_size >= 1024 && file_stat->st_size <= 10 * 1024 * 1024);
}