#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Función Hash DJB2 
static unsigned long djb2_hash(const char *str, size_t capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % capacity;
}


// Crear una tabla hash
HashTable* ht_create(size_t capacity) {
    if (capacity == 0) {
        fprintf(stderr, "Error: La capacidad de la tabla hash no puede ser 0.\n");
        return NULL;
    }
    HashTable *table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) {
        perror("Error al asignar memoria para HashTable");
        return NULL;
    }

    table->capacity = capacity;
    table->size = 0;
    table->buckets = (HashNode**)calloc(table->capacity, sizeof(HashNode*));
    if (!table->buckets) {
        perror("Error al asignar memoria para los buckets de la HashTable");
        free(table);
        return NULL;
    }
    return table;
}

// Insertar una clave en la tabla hash
void ht_insert(HashTable *table, const char *key, int value) {
    if (!table || !key) return;

    size_t index = djb2_hash(key, table->capacity);
    HashNode *current_node = table->buckets[index];
    HashNode *prev_node = NULL;

    while (current_node != NULL) {
        if (strcmp(current_node->key, key) == 0) {
            current_node->value += value; 
            return;
        }
        prev_node = current_node;
        current_node = current_node->next;
    }

    // La clave no existe, creamos un nuevo nodo
    HashNode *new_node = (HashNode*)malloc(sizeof(HashNode));
    if (!new_node) {
        perror("Error al asignar memoria para HashNode");
        return;
    }

    new_node->key = strdup(key); 
    if (!new_node->key) {
        perror("Error al duplicar la clave");
        free(new_node);
        return;
    }
    new_node->value = value;
    new_node->next = NULL;

    // Insertar el nuevo nodo en la lista enlazada del bucket
    if (prev_node == NULL) {
        table->buckets[index] = new_node;
    } else {
        prev_node->next = new_node;
    }
    table->size++;

}

// Buscar una clave en la tabla hash
int* ht_search(HashTable *table, const char *key) {
    if (!table || !key) return NULL;

    size_t index = djb2_hash(key, table->capacity);
    HashNode *current_node = table->buckets[index];

    while (current_node != NULL) {
        if (strcmp(current_node->key, key) == 0) {
            return &(current_node->value);
        }
        current_node = current_node->next;
    }
    return NULL;
}

// Liberar la memoria ocupada por la tabla hash
void ht_free(HashTable *table) {
    if (!table) return;

    for (size_t i = 0; i < table->capacity; ++i) {
        HashNode *current_node = table->buckets[i];
        while (current_node != NULL) {
            HashNode *next_node = current_node->next;
            free(current_node->key);
            free(current_node);   
            current_node = next_node;
        }
    }
    free(table->buckets);
    free(table);      
}
