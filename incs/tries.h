#ifndef TRIES_H
#define TRIES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>     
#include <stdbool.h>
#include <stddef.h>

#define ALPHABET 256


typedef struct TrieNode {
    struct TrieNode *children[ALPHABET];  
    bool is_end_of_word;     
    int frequency;           
    int prefix_count;       
} TrieNode;

// Estructura principal del Trie
typedef struct Trie {
    TrieNode *root;         
    size_t word_count;       
    size_t node_count;      
} Trie;

// Para guardar resultados de búsqueda por prefijo
typedef struct PrefixResult {
    char **words;           
    int *frequencies;        
    int count;              
    int capacity;         
} PrefixResult;


// Básicas del Trie
Trie* trie_create(void);
void trie_insert(Trie *trie, const char *word);
bool trie_search(Trie *trie, const char *word);
int trie_get_frequency(Trie *trie, const char *word);
bool trie_starts_with(Trie *trie, const char *prefix);
PrefixResult* trie_get_words_with_prefix(Trie *trie, const char *prefix);
void trie_free(Trie *trie);

// Funciones para manejar los resultados de búsqueda
PrefixResult* prefix_result_create(int initial_capacity);
void prefix_result_add(PrefixResult *result, const char *word, int frequency);
void prefix_result_free(PrefixResult *result);
void prefix_result_print(const PrefixResult *result, const char *prefix);

// Funciones para estadísticas y debug
void trie_print_stats(const Trie *trie);
int trie_get_depth(const Trie *trie);
void trie_print_all_words(const Trie *trie);

#endif 