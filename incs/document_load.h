#ifndef DOCUMENT_LOAD_H
#define DOCUMENT_LOAD_H

#include "document_structs.h"

// Gestión de colección de documentos
DocumentCollection* document_collection_create(size_t initial_capacity);
void document_collection_free(DocumentCollection *collection);
bool add_document_to_collection(DocumentCollection *collection, Document *doc);

// Carga de documentos individuales
Document* load_document_from_file(const char *filepath);
Document* load_document_from_string(const char *content, const char *filename);
void document_free(Document *doc);

// Carga masiva de documentos
LoadingStats* load_documents_from_directory(DocumentCollection *collection, 
                                          const char *directory_path, 
                                          bool recursive);
LoadingStats* load_documents_from_file_list(DocumentCollection *collection,
                                           const char *file_list_path);

// Carga con filtros personalizados
LoadingStats* load_documents_with_filter(DocumentCollection *collection,
                                        const char *directory_path,
                                        DocumentFilter filter,
                                        bool recursive);

// Gestión de estadísticas
LoadingStats* loading_stats_create(void);
void loading_stats_free(LoadingStats *stats);

#endif