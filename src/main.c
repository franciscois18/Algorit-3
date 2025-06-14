#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "document_main.h"
#include "preprocessing.h"
#include "common.h"
#include "config.h"
#include "commands.h"


int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf(" SISTEMA AVANZADO DE ANÁLISIS DE DOCUMENTOS  \n");
        printf("Desarrollado para análisis de texto y búsqueda de patrones\n");
        printf("Versión 1.0 - Algoritmos de Texto\n\n");
        printf("Use '%s help' para ver los comandos disponibles.\n", argv[0]);
        printf("Ejemplo: %s help\n", argv[0]);
        return 0; 
    }
    // Configuración
    ProgramConfig *config = config_create();
    if (!config) {
        printf("Error: No se pudo inicializar la configuración\n");
        return 1;
    }
    
    // Determinar comando
    char *command = argv[1];
    argc--;
    argv++;
    
    // Parsear argumentos
    int parse_result = parse_arguments(argc, argv, config);
    if (parse_result == -1) {
        print_usage(argv[0]);
        config_free(config);
        return 0;
    } else if (parse_result == -2) {
        printf("Error en los argumentos. Use --help para ver la ayuda.\n");
        config_free(config);
        return 1;
    }
    
    // Ejecutar comando correspondiente
    int result = 0;
    
    if (strcmp(command, "load") == 0) {
        result = command_load(config);
    } else if (strcmp(command, "index") == 0) {
        result = command_index(config);
    } else if (strcmp(command, "search") == 0) {
        result = command_search(config);
    } else if (strcmp(command, "benchmark") == 0) {
        // TENGO QUE REVISAR ESTO LO QUITO POR AHORA result = command_benchmark(config);
    } else if (strcmp(command, "analyze") == 0) {
        result = command_analyze(config);
    } else if (strcmp(command, "help") == 0) {
        print_usage(argv[0]);
    } else {
        printf("Comando desconocido: %s\n", command);
        printf("Use '%s help' para ver los comandos disponibles.\n", argv[0]);
        result = 1;
    }
    
    config_free(config);
    
    if (result == 0) {
        printf("\n¡Operación completada exitosamente!\n");
    } else {
        printf("\nLa operación terminó con errores.\n");
    }
    
    return result;
}