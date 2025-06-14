#ifndef DOCUMENT_FILTERS_H
#define DOCUMENT_FILTERS_H

#include "document_structs.h"

// Filtros predefinidos para carga selectiva
bool filter_by_size_range(const char *filepath, const struct stat *file_stat);
bool filter_text_files_only(const char *filepath, const struct stat *file_stat);
bool filter_html_files_only(const char *filepath, const struct stat *file_stat);

#endif