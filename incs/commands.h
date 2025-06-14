#ifndef COMMANDS_H
#define COMMANDS_H

#include "config.h"

// Función para cargar documentos desde un directorio
int command_load(ProgramConfig *config);

// Función para crear índices de documentos cargados
int command_index(ProgramConfig *config);

// Función para buscar patrones en documentos usando diferentes algoritmos
int command_search(ProgramConfig *config);

// Función para ejecutar benchmarks de rendimiento de algoritmos de búsqueda
int command_benchmark(ProgramConfig *config);

// Función para analizar estadísticas de documentos y generar informes
int command_analyze(ProgramConfig *config);

#endif 