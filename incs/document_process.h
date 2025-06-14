#ifndef DOCUMENT_PROCESS_H
#define DOCUMENT_PROCESS_H

#include "document_structs.h"

// Procesamiento de contenido
char* extract_text_from_html(const char *html_content);
char* clean_text_content(const char *raw_content, DocumentType type);

// Detección de tipo de documento
DocumentType detect_document_type(const char *filepath);

// Detección de idioma básica
char* detect_language_simple(const char *content);

#endif