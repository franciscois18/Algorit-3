
#ifndef HASH_H
#define HASH_H


#include <stddef.h>

// Nodo para la lista enlazada en cada bucket de la tabla hash (manejo de colisiones por encadenamiento)
typedef struct HashNode {
    char *key;              
    int value;              
    struct HashNode *next; 
} HashNode;

// Estructura principal de la Tabla Hash
typedef struct HashTable {
    HashNode **buckets;   
    size_t capacity;    
    size_t size;        
} HashTable;

HashTable* ht_create(size_t capacity);
void ht_insert(HashTable *table, const char *key, int value);
int* ht_search(HashTable *table, const char *key);
void ht_free(HashTable *table);





#endif