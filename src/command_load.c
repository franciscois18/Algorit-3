#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "commands.h"
#include "document_main.h"
#include "preprocessing.h"

// Cargar y procesar documentos
int command_load(ProgramConfig *config) {
    printf("\n=== CARGANDO DOCUMENTOS ===\n");
    
    if (!config->corpus_path) {
        printf("Error: Debe especificar un directorio con -i/--input\n");
        return 1;
    }
    
    // Verificar que el directorio existe
    struct stat st;
    if (stat(config->corpus_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
        printf("Error: El directorio '%s' no existe o no es accesible\n", config->corpus_path);
        return 1;
    }
    
    // Crear colección de documentos
    DocumentCollection *collection = document_collection_create(1000);
    if (!collection) {
        printf("Error: No se pudo crear la colección de documentos\n");
        return 1;
    }
    
    // Cargar documentos
    clock_t start_time = clock();
    LoadingStats *stats;
    
    if (config->txt_only) {
        stats = load_documents_with_filter(collection, config->corpus_path, 
                                         filter_text_files_only, config->recursive);
    } else if (config->html_only) {
        stats = load_documents_with_filter(collection, config->corpus_path, 
                                         filter_html_files_only, config->recursive);
    } else {
        stats = load_documents_from_directory(collection, config->corpus_path, config->recursive);
    }
    
    if (!stats) {
        printf("Error: Fallo en la carga de documentos\n");
        document_collection_free(collection);
        return 1;
    }
    
    clock_t end_time = clock();
    double loading_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    // Aplicar límite si se especificó
    if (config->limit > 0 && collection->count > (size_t)config->limit) {
        printf("Aplicando límite de %d documentos...\n", config->limit);
        collection->count = config->limit;
    }
    
    printf("\nRESULTADOS DE CARGA:\n");
    printf("  - Documentos cargados: %zu\n", collection->count);
    printf("  - Tiempo de carga: %.2f segundos\n", loading_time);
    printf("  - Archivos procesados exitosamente: %zu\n", stats->files_loaded);
    printf("  - Archivos con errores: %zu\n", stats->files_failed);
    printf("  - Bytes totales cargados: %zu (%.2f MB)\n", 
           stats->total_bytes_loaded, stats->total_bytes_loaded / (1024.0 * 1024.0));
    
    if (config->show_stats) {
        printf("\nESTADÍSTICAS DETALLADAS:\n");
        print_loading_stats(stats);
        print_collection_summary(collection);
        
        // Mostrar muestra de documentos
        printf("\n=== MUESTRA DE DOCUMENTOS ===\n");
        int samples_to_show = collection->count < 5 ? collection->count : 5;
        for (int i = 0; i < samples_to_show; i++) {
            Document *doc = collection->documents[i];
            if (doc && doc->metadata) {
                printf("\nDocumento %d:\n", i + 1);
                printf("  Archivo: %s\n", doc->metadata->filename);
                printf("  Tamaño: %zu bytes\n", doc->metadata->file_size);
                printf("  Tipo: ");
                switch (doc->metadata->type) {
                    case DOC_TYPE_TXT: printf("TXT\n"); break;
                    case DOC_TYPE_HTML: printf("HTML\n"); break;
                    case DOC_TYPE_CSV: printf("CSV\n"); break;
                    default: printf("Desconocido\n"); break;
                }
                
                // Mostrar primeras líneas
                if (doc->clean_content && strlen(doc->clean_content) > 0) {
                    printf("  Vista previa: \"");
                    char preview[200];
                    strncpy(preview, doc->clean_content, 199);
                    preview[199] = '\0';
                    // Reemplazar saltos de línea con espacios para vista previa
                    for (int j = 0; preview[j]; j++) {
                        if (preview[j] == '\n' || preview[j] == '\r') preview[j] = ' ';
                    }
                    printf("%.150s", preview);
                    if (strlen(doc->clean_content) > 150) printf("...");
                    printf("\"\n");
                }
            }
        }
    }
    
    // Procesar documentos si se solicitó
    if (config->process_docs) {
        printf("\n=== PROCESANDO DOCUMENTOS ===\n");
        
        TokenizerConfig *tok_config = tokenizer_config_create_default();
        NormalizationConfig *norm_config = normalization_config_create_default();
        StopWordList *stopwords = load_spanish_stopwords();
        
        if (!tok_config || !norm_config || !stopwords) {
            printf("Error: No se pudieron crear las configuraciones de preprocesamiento\n");
        } else {
            size_t processed_count = 0;
            size_t total_tokens = 0;
            clock_t proc_start = clock();
            
            size_t docs_to_process = collection->count < 10 ? collection->count : 10;
            printf("Procesando primeros %zu documentos...\n", docs_to_process);
            
            for (size_t i = 0; i < docs_to_process; i++) {
                Document *doc = collection->documents[i];
                if (!doc || !doc->clean_content) continue;
                
                printf("  [%zu/%zu] %s... ", i+1, docs_to_process, doc->metadata->filename);
                
                ProcessedText *processed = process_text_complete(doc->clean_content, 
                                                               tok_config, norm_config, stopwords);
                
                if (processed && processed->tokens) {
                    printf("OK (%zu tokens)\n", processed->tokens->count);
                    total_tokens += processed->tokens->count;
                    processed_count++;
                    processed_text_free(processed);
                } else {
                    printf("FALLO\n");
                }
            }
            
            clock_t proc_end = clock();
            double proc_time = ((double)(proc_end - proc_start)) / CLOCKS_PER_SEC;
            
            printf("\nRESULTADOS DE PROCESAMIENTO:\n");
            printf("  - Documentos procesados: %zu\n", processed_count);
            printf("  - Tokens totales extraídos: %zu\n", total_tokens);
            printf("  - Tiempo de procesamiento: %.2f segundos\n", proc_time);
            printf("  - Promedio por documento: %.2f segundos\n", 
                   processed_count > 0 ? proc_time / processed_count : 0);
        }
        
        tokenizer_config_free(tok_config);
        normalization_config_free(norm_config);
        stopword_list_free(stopwords);
    }
    
    // Exportar resultados si se solicitó
    if (config->output_file) {
        printf("\nExportando resultados a %s...\n", config->output_file);
        FILE *out = fopen(config->output_file, "w");
        if (out) {
            if (config->export_csv) {
                fprintf(out, "filename,type,size,words_estimated\n");
                for (size_t i = 0; i < collection->count; i++) {
                    Document *doc = collection->documents[i];
                    if (doc && doc->metadata) {
                        fprintf(out, "\"%s\",%s,%zu,%zu\n", 
                               doc->metadata->filename,
                               doc->metadata->type == DOC_TYPE_TXT ? "TXT" : 
                               doc->metadata->type == DOC_TYPE_HTML ? "HTML" : "OTHER",
                               doc->metadata->file_size,
                               doc->clean_content ? strlen(doc->clean_content) / 5 : 0); // tenemos que revisar esto
                    }
                }
            } else if (config->export_json) {
                fprintf(out, "{\n  \"collection\": {\n");
                fprintf(out, "    \"total_documents\": %zu,\n", collection->count);
                fprintf(out, "    \"loading_time\": %.2f,\n", loading_time);
                fprintf(out, "    \"total_bytes\": %zu,\n", stats->total_bytes_loaded);
                fprintf(out, "    \"documents\": [\n");
                for (size_t i = 0; i < collection->count; i++) {
                    Document *doc = collection->documents[i];
                    if (doc && doc->metadata) {
                        fprintf(out, "      {\n");
                        fprintf(out, "        \"filename\": \"%s\",\n", doc->metadata->filename);
                        fprintf(out, "        \"type\": \"%s\",\n", 
                               doc->metadata->type == DOC_TYPE_TXT ? "TXT" : 
                               doc->metadata->type == DOC_TYPE_HTML ? "HTML" : "OTHER");
                        fprintf(out, "        \"size\": %zu\n", doc->metadata->file_size);
                        fprintf(out, "      }%s\n", i < collection->count - 1 ? "," : "");
                    }
                }
                fprintf(out, "    ]\n  }\n}\n");
            } else {
                fprintf(out, "REPORTE DE CARGA DE DOCUMENTOS\n");
                fprintf(out, "==============================\n\n");
                fprintf(out, "Directorio: %s\n", config->corpus_path);
                fprintf(out, "Documentos cargados: %zu\n", collection->count);
                fprintf(out, "Tiempo de carga: %.2f segundos\n", loading_time);
                fprintf(out, "Bytes totales: %zu\n\n", stats->total_bytes_loaded);
                
                for (size_t i = 0; i < collection->count; i++) {
                    Document *doc = collection->documents[i];
                    if (doc && doc->metadata) {
                        fprintf(out, "%s\t%zu\t%s\n", 
                               doc->metadata->filename, 
                               doc->metadata->file_size,
                               doc->metadata->type == DOC_TYPE_TXT ? "TXT" : 
                               doc->metadata->type == DOC_TYPE_HTML ? "HTML" : "OTHER");
                    }
                }
            }
            fclose(out);
            printf("Resultados exportados exitosamente.\n");
        } else {
            printf("Error: No se pudo crear el archivo de salida.\n");
        }
    }
    
    loading_stats_free(stats);
    document_collection_free(collection);
    
    return 0;
}