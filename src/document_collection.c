#include "document_main.h"

#include <stdlib.h>
#include <string.h>

// Crear colección de documentos
DocumentCollection* document_collection_create(size_t initial_capacity) {
    DocumentCollection *collection = malloc(sizeof(DocumentCollection));
    if (!collection) return NULL;
    
    collection->documents = calloc(initial_capacity, sizeof(Document*));
    if (!collection->documents) {
        free(collection);
        return NULL;
    }
    
    collection->count = 0;
    collection->capacity = initial_capacity;
    collection->collection_path = NULL;
    
    return collection;
}

// Liberar colección de documentos
void document_collection_free(DocumentCollection *collection) {
    if (!collection) return;
    
    for (size_t i = 0; i < collection->count; i++) {
        document_free(collection->documents[i]);
    }
    
    free(collection->documents);
    free(collection->collection_path);
    free(collection);
}

// Agregar documento a la colección
bool add_document_to_collection(DocumentCollection *collection, Document *doc) {
    if (!collection || !doc) return false;
    
    // Expandir capacidad si es necesario
    if (collection->count >= collection->capacity) {
        size_t new_capacity = collection->capacity * 2;
        Document **new_docs = realloc(collection->documents, 
                                     new_capacity * sizeof(Document*));
        if (!new_docs) return false;
        
        collection->documents = new_docs;
        collection->capacity = new_capacity;
    }
    
    collection->documents[collection->count++] = doc;
    return true;
}

// Cargar documento individual desde archivo
Document* load_document_from_file(const char *filepath) {
    if (!filepath) return NULL;
    
    // Verificar que el archivo existe
    struct stat file_stat;
    if (stat(filepath, &file_stat) != 0) {
        return NULL;
    }
    
    Document *doc = malloc(sizeof(Document));
    if (!doc) return NULL;
    
    // Inicializar estructura
    doc->metadata = malloc(sizeof(DocumentMetadata));
    if (!doc->metadata) {
        free(doc);
        return NULL;
    }
    
    // Llenar metadatos
    doc->metadata->full_path = strdup(filepath);
    const char *filename = strrchr(filepath, '/');
    doc->metadata->filename = strdup(filename ? filename + 1 : filepath);
    doc->metadata->file_size = file_stat.st_size;
    doc->metadata->type = detect_document_type(filepath);
    doc->metadata->last_modified = file_stat.st_mtime;
    doc->metadata->title = strdup(doc->metadata->filename); 
    doc->metadata->language = strdup("unknown");
    
    // Leer contenido
    size_t content_length;
    doc->raw_content = read_file_content(filepath, &content_length);
    if (!doc->raw_content) {
        document_free(doc);
        return NULL;
    }
    
    doc->content_length = content_length;
    
    // Limpiar contenido
    doc->clean_content = clean_text_content(doc->raw_content, doc->metadata->type);
    if (!doc->clean_content) {
        document_free(doc);
        return NULL;
    }
    
    // Detectar idioma del contenido limpio
    free(doc->metadata->language);
    doc->metadata->language = detect_language_simple(doc->clean_content);
    
    doc->is_loaded = true;
    
    return doc;
}

// Cargar documento desde string
Document* load_document_from_string(const char *content, const char *filename) {
    if (!content || !filename) return NULL;
    
    Document *doc = malloc(sizeof(Document));
    if (!doc) return NULL;
    
    // Inicializar estructura
    doc->metadata = malloc(sizeof(DocumentMetadata));
    if (!doc->metadata) {
        free(doc);
        return NULL;
    }
    
    // Llenar metadatos
    doc->metadata->full_path = strdup(filename);
    doc->metadata->filename = strdup(filename);
    doc->metadata->file_size = strlen(content);
    doc->metadata->type = detect_document_type(filename);
    doc->metadata->last_modified = time(NULL);
    doc->metadata->title = strdup(filename);
    
    // Copiar contenido
    doc->raw_content = strdup(content);
    if (!doc->raw_content) {
        document_free(doc);
        return NULL;
    }
    
    doc->content_length = strlen(content);
    
    // Limpiar contenido
    doc->clean_content = clean_text_content(doc->raw_content, doc->metadata->type);
    if (!doc->clean_content) {
        document_free(doc);
        return NULL;
    }
    
    // Detectar idioma
    doc->metadata->language = detect_language_simple(doc->clean_content);
    
    doc->is_loaded = true;
    
    return doc;
}

// Liberar documento
void document_free(Document *doc) {
    if (!doc) return;
    
    if (doc->metadata) {
        free(doc->metadata->filename);
        free(doc->metadata->full_path);
        free(doc->metadata->title);
        free(doc->metadata->language);
        free(doc->metadata);
    }
    
    free(doc->raw_content);
    free(doc->clean_content);
    free(doc);
}

// Crear estadísticas de carga
LoadingStats* loading_stats_create(void) {
    LoadingStats *stats = calloc(1, sizeof(LoadingStats));
    return stats;
}

// Liberar estadísticas
void loading_stats_free(LoadingStats *stats) {
    free(stats);
}