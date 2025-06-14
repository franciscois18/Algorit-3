#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "config.h"







// Parsear argumentos de línea de comandos
int parse_arguments(int argc, char *argv[], ProgramConfig *config) {
    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"format", required_argument, 0, 'f'},
        {"pattern", required_argument, 0, 'p'},
        {"algorithm", required_argument, 0, 'a'},
        {"limit", required_argument, 0, 'l'},
        {"max-distance", required_argument, 0, 1001},
        {"index-file", required_argument, 0, 1002},
        {"top-words", required_argument, 0, 1003},
        {"min-size", required_argument, 0, 1004},
        {"max-size", required_argument, 0, 1005},
        {"recursive", no_argument, 0, 'r'},
        {"txt-only", no_argument, 0, 't'},
        {"html-only", no_argument, 0, 'h'},
        {"stats", no_argument, 0, 's'},
        {"verbose", no_argument, 0, 'v'},
        {"approximate", no_argument, 0, 1010},
        {"case-sensitive", no_argument, 0, 1011},
        {"rebuild-index", no_argument, 0, 1012},
        {"trie", no_argument, 0, 1013},
        {"hash", no_argument, 0, 1014},
        {"word-freq", no_argument, 0, 1015},
        {"similarity", no_argument, 0, 1016},
        {"help", no_argument, 0, 1020},
        {0, 0, 0, 0}
    };
    
    int c;
    int option_index = 0;
    
    while ((c = getopt_long(argc, argv, "i:o:f:p:a:l:rthsv", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                config->corpus_path = strdup(optarg);
                break;
            case 'o':
                config->output_file = strdup(optarg);
                break;
            case 'f':
                free(config->output_format);
                config->output_format = strdup(optarg);
                if (strcmp(optarg, "csv") == 0) config->export_csv = true;
                else if (strcmp(optarg, "json") == 0) config->export_json = true;
                break;
            case 'p':
                config->search_pattern = strdup(optarg);
                break;
            case 'l':
                config->limit = atoi(optarg);
                break;
            case 'r':
                config->recursive = true;
                break;
            case 't':
                config->txt_only = true;
                break;
            case 'h':
                config->html_only = true;
                break;
            case 's':
                config->show_stats = true;
                break;
            case 1001: // max-distance
                config->max_distance = atoi(optarg);
                break;
            case 1002: // index archivo
                config->index_file = strdup(optarg);
                break;
            case 1010: // busqueda aproximada
                config->approximate_search = true;
                break;
            case 1020: // help
                return -1;
            case '?':
                return -2;
            default:
                break;
        }
    }
    
    return 0;
}



void print_usage(const char *program_name) {
    printf("\n=== SISTEMA AVANZADO DE ANÁLISIS DE DOCUMENTOS ===\n\n");
    printf("Uso: %s [COMANDO] [OPCIONES]\n\n", program_name);
    
    printf("COMANDOS:\n");
    printf("  load        Cargar y procesar documentos desde un directorio\n");
    printf("  index       Construir índice de documentos para búsqueda rápida\n");
    printf("  search      Buscar patrones en documentos indexados\n");
    printf("  benchmark   Ejecutar pruebas de rendimiento de algoritmos\n");
    printf("  analyze     Analizar estadísticas de documentos\n");
    printf("  help        Mostrar esta ayuda\n\n");
    
    printf("OPCIONES GENERALES:\n");
    printf("  -i, --input DIR         Directorio de documentos (obligatorio)\n");
    printf("  -o, --output FILE       Archivo de salida para resultados\n");
    printf("  -f, --format FORMAT     Formato de salida: text, csv, json (default: text)\n");
    printf("  -r, --recursive         Buscar archivos recursivamente\n");
    printf("  -l, --limit N           Limitar a N documentos\n");
    printf("  -v, --verbose           Mostrar información detallada\n");
    printf("  --help                  Mostrar esta ayuda\n\n");
    
    printf("OPCIONES DE FILTRADO:\n");
    printf("  -t, --txt-only          Cargar solo archivos de texto\n");
    printf("  -h, --html-only         Cargar solo archivos HTML\n");
    printf("  --min-size SIZE         Tamaño mínimo de archivo en bytes\n");
    printf("  --max-size SIZE         Tamaño máximo de archivo en bytes\n\n");
    
    printf("OPCIONES DE BÚSQUEDA:\n");
    printf("  -p, --pattern PATTERN   Patrón a buscar (obligatorio para search)\n");
    printf("  -a, --algorithm ALG     Algoritmo: kmp, shift-and, shift-or (default: kmp)\n");
    printf("  --approximate           Búsqueda aproximada con tolerancia a errores\n");
    printf("  --max-distance N        Distancia máxima para búsqueda aproximada (default: 2)\n");
    printf("  --case-sensitive        Búsqueda sensible a mayúsculas/minúsculas\n\n");
    
    printf("OPCIONES DE INDEXACIÓN:\n");
    printf("  --index-file FILE       Archivo donde guardar/cargar el índice\n");
    printf("  --rebuild-index         Forzar reconstrucción del índice\n");
    printf("  --trie                  Usar estructura Trie para indexación\n");
    printf("  --hash                  Usar tabla hash para indexación\n\n");
    
    printf("OPCIONES DE ANÁLISIS:\n");
    printf("  -s, --stats             Mostrar estadísticas detalladas\n");
    printf("  --top-words N           Mostrar las N palabras más frecuentes\n");
    printf("  --word-freq             Generar análisis de frecuencia de palabras\n");
    printf("  --similarity            Calcular similitud entre documentos\n\n");
    
    printf("EJEMPLOS DE USO:\n");
    printf("  # Cargar documentos y mostrar estadísticas\n");
    printf("  %s load -i corpus/ -r -s\n\n", program_name);
    printf("  # Construir índice de documentos\n");
    printf("  %s index -i corpus/ -r --index-file corpus.idx --trie\n\n", program_name);
    printf("  # Buscar patrón usando KMP\n");
    printf("  %s search -i corpus/ -p \"algoritmo\" -a kmp --index-file corpus.idx\n\n", program_name);
    printf("  # Búsqueda aproximada con máximo 2 errores\n");
    printf("  %s search -i corpus/ -p \"algortimo\" --approximate --max-distance 2\n\n", program_name);
    printf("  # Benchmark de algoritmos\n");
    printf("  %s benchmark -i corpus/ -p \"patrón\" -o results.csv -f csv\n\n", program_name);
    printf("  # Análisis de texto con palabras frecuentes\n");
    printf("  %s analyze -i corpus/ -s --top-words 20 --word-freq\n\n", program_name);
    
    printf("FORMATOS DE SALIDA:\n");
    printf("  text        Formato de texto legible (default)\n");
    printf("  csv         Valores separados por comas\n");
    printf("  json        Formato JSON estructurado\n\n");
    

}



ProgramConfig* config_create() {
    ProgramConfig *config = calloc(1, sizeof(ProgramConfig));
    if (!config) return NULL;
    
    // Valores por defecto
    config->recursive = false;
    config->txt_only = false;
    config->html_only = false;
    config->show_stats = false;
    config->process_docs = true;
    config->build_index = false;
    config->search_mode = false;
    config->benchmark_mode = false;
    config->approximate_search = false;
    config->export_csv = false;
    config->export_json = false;
    config->limit = -1;
    config->max_distance = 2;
    config->output_format = strdup("text");
    
    return config;
}

void config_free(ProgramConfig *config) {
    if (!config) return;
    
    free(config->corpus_path);
    free(config->output_file);
    free(config->search_pattern);
    free(config->index_file);
    free(config->output_format);
    free(config);
}

