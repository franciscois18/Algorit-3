#include "document_main.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>

// Detectar tipo de documento por extensión
DocumentType detect_document_type(const char *filepath) {
    if (!filepath) return DOC_TYPE_UNKNOWN;
    
    const char *ext = strrchr(filepath, '.');
    if (!ext) return DOC_TYPE_UNKNOWN;
    
    ext++; // Saltar el punto
    
    if (strcasecmp(ext, "txt") == 0) return DOC_TYPE_TXT;
    if (strcasecmp(ext, "html") == 0 || strcasecmp(ext, "htm") == 0) return DOC_TYPE_HTML;
    if (strcasecmp(ext, "csv") == 0) return DOC_TYPE_CSV;
    
    return DOC_TYPE_UNKNOWN;
}

// Extraer texto de contenido HTML (remover tags)
char* extract_text_from_html(const char *html_content) {
    if (!html_content) return NULL;
    
    size_t len = strlen(html_content);
    char *clean_text = malloc(len + 1);
    if (!clean_text) return NULL;
    
    size_t write_pos = 0;
    bool inside_tag = false;
    bool inside_script = false;
    bool inside_style = false;
    
    for (size_t i = 0; i < len; i++) {
        char c = html_content[i];
        
        // Detectar inicio de script o style
        if (!inside_tag && i + 7 < len && strncasecmp(&html_content[i], "<script", 7) == 0) {
            inside_script = true;
            inside_tag = true;
            continue;
        }
        
        if (!inside_tag && i + 6 < len && strncasecmp(&html_content[i], "<style", 6) == 0) {
            inside_style = true;
            inside_tag = true;
            continue;
        }
        
        // Detectar fin de script o style
        if (inside_script && i + 9 < len && strncasecmp(&html_content[i], "</script>", 9) == 0) {
            inside_script = false;
            i += 8; // Saltar el tag completo
            continue;
        }
        
        if (inside_style && i + 8 < len && strncasecmp(&html_content[i], "</style>", 8) == 0) {
            inside_style = false;
            i += 7; // Saltar el tag completo
            continue;
        }
        
        // Manejo de tags HTML
        if (c == '<') {
            inside_tag = true;
        } else if (c == '>') {
            inside_tag = false;
        } else if (!inside_tag && !inside_script && !inside_style) {
            // Solo agregar texto que no esté dentro de tags, scripts o styles
            if (c == '&') {
                // Manejo básico de entidades HTML
                if (i + 4 < len && strncmp(&html_content[i], "&amp;", 5) == 0) {
                    clean_text[write_pos++] = '&';
                    i += 4;
                } else if (i + 3 < len && strncmp(&html_content[i], "&lt;", 4) == 0) {
                    clean_text[write_pos++] = '<';
                    i += 3;
                } else if (i + 3 < len && strncmp(&html_content[i], "&gt;", 4) == 0) {
                    clean_text[write_pos++] = '>';
                    i += 3;
                } else if (i + 5 < len && strncmp(&html_content[i], "&quot;", 6) == 0) {
                    clean_text[write_pos++] = '"';
                    i += 5;
                } else if (i + 5 < len && strncmp(&html_content[i], "&nbsp;", 6) == 0) {
                    clean_text[write_pos++] = ' ';
                    i += 5;
                } else {
                    clean_text[write_pos++] = c;
                }
            } else {
                clean_text[write_pos++] = c;
            }
        }
    }
    
    clean_text[write_pos] = '\0';
    
    // Redimensionar si es necesario
    char *result = realloc(clean_text, write_pos + 1);
    return result ? result : clean_text;
}

// Limpiar contenido según el tipo de documento
char* clean_text_content(const char *raw_content, DocumentType type) {
    if (!raw_content) return NULL;
    
    switch (type) {
        case DOC_TYPE_HTML:
            return extract_text_from_html(raw_content);
        
        case DOC_TYPE_TXT:
        case DOC_TYPE_CSV:
        default:
            return strdup(raw_content);
    }
}

// Detección de idioma básica (se puede mejorar la verdad)
char* detect_language_simple(const char *content) {
    if (!content) return strdup("unknown");
    
    // Implementación se puede mejorar

    if (strstr(content, "ñ") || strstr(content, "Ñ") || 
        strstr(content, "á") || strstr(content, "é") || 
        strstr(content, "í") || strstr(content, "ó") || 
        strstr(content, "ú") || strstr(content, "ü")) {
        return strdup("es");
    }
    
    // Por defecto asumir inglés
    return strdup("en");
}