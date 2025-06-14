#include "tries.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>



static TrieNode* trie_node_create(void) {
    TrieNode *node = calloc(1, sizeof(TrieNode));
    if (!node) {
        perror("Error al crear nodo del trie");
        return NULL;
    }
    return node;
}

Trie* trie_create(void) {
    Trie *trie = malloc(sizeof(Trie));
    if (!trie) {
        perror("Error al crear trie");
        return NULL;
    }
    
    trie->root = trie_node_create();
    if (!trie->root) {
        free(trie);
        return NULL;
    }
    
    trie->word_count = 0;
    trie->node_count = 1;
    
    return trie;
}

void trie_insert(Trie *trie, const char *word) {
    if (!trie || !word || strlen(word) == 0) return;
    
    TrieNode *current = trie->root;
    int word_len = strlen(word);
    
    for (int i = 0; i < word_len; i++) {
        unsigned char index = (unsigned char)word[i];
        
        if (!current->children[index]) {
            current->children[index] = trie_node_create();
            if (!current->children[index]) return;
            trie->node_count++;
        }
        
        current = current->children[index];
        current->prefix_count++;
    }
    
    if (!current->is_end_of_word) {
        current->is_end_of_word = true;
        trie->word_count++;
    }
    current->frequency++;
}

bool trie_search(Trie *trie, const char *word) {
    if (!trie || !word) return false;
    
    TrieNode *current = trie->root;
    int word_len = strlen(word);
    
    for (int i = 0; i < word_len; i++) {
        unsigned char index = (unsigned char)word[i];
        
        if (!current->children[index]) {
            return false;
        }
        
        current = current->children[index];
    }
    
    return current->is_end_of_word;
}

int trie_get_frequency(Trie *trie, const char *word) {
    if (!trie || !word) return 0;
    
    TrieNode *current = trie->root;
    int word_len = strlen(word);
    
    for (int i = 0; i < word_len; i++) {
        unsigned char index = (unsigned char)word[i];
        
        if (!current->children[index]) {
            return 0;
        }
        
        current = current->children[index];
    }
    
    return current->is_end_of_word ? current->frequency : 0;
}


bool trie_starts_with(Trie *trie, const char *prefix) {
    if (!trie || !prefix) return false;
    
    TrieNode *current = trie->root;
    int prefix_len = strlen(prefix);
    
    for (int i = 0; i < prefix_len; i++) {
        unsigned char index = (unsigned char)prefix[i];
        
        if (!current->children[index]) {
            return false;
        }
        
        current = current->children[index];
    }
    
    return current->prefix_count > 0;
}

// RESULTADO DE BÚSQUEDA POR PREFIJO

PrefixResult* prefix_result_create(int initial_capacity) {
    PrefixResult *result = malloc(sizeof(PrefixResult));
    if (!result) return NULL;
    
    result->words = malloc(initial_capacity * sizeof(char*));
    result->frequencies = malloc(initial_capacity * sizeof(int));
    
    if (!result->words || !result->frequencies) {
        free(result->words);
        free(result->frequencies);
        free(result);
        return NULL;
    }
    
    result->count = 0;
    result->capacity = initial_capacity;
    
    return result;
}

void prefix_result_add(PrefixResult *result, const char *word, int frequency) {
    if (!result || !word) return;
    
    if (result->count >= result->capacity) {
        int new_capacity = result->capacity * 2;
        char **new_words = realloc(result->words, new_capacity * sizeof(char*));
        int *new_frequencies = realloc(result->frequencies, new_capacity * sizeof(int));
        
        if (!new_words || !new_frequencies) {
            free(new_words);
            free(new_frequencies);
            return;
        }
        
        result->words = new_words;
        result->frequencies = new_frequencies;
        result->capacity = new_capacity;
    }
    
    result->words[result->count] = strdup(word);
    result->frequencies[result->count] = frequency;
    result->count++;
}

void prefix_result_free(PrefixResult *result) {
    if (!result) return;
    
    for (int i = 0; i < result->count; i++) {
        free(result->words[i]);
    }
    free(result->words);
    free(result->frequencies);
    free(result);
}

void prefix_result_print(const PrefixResult *result, const char *prefix) {
    if (!result || !prefix) return;
    
    printf("\nPalabras que comienzan con '%s' (%d encontradas):\n", prefix, result->count);
    printf("=================================================\n");
    
    if (result->count == 0) {
        printf("No se encontraron palabras con este prefijo.\n");
        return;
    }
    
    for (int i = 0; i < result->count; i++) {
        printf("  %-20s (frecuencia: %d)\n", result->words[i], result->frequencies[i]);
    }
    printf("\n");
}

// BÚSQUEDA POR PREFIJO

static void collect_words_with_prefix(TrieNode *node, char *current_word, int depth, 
                                     PrefixResult *result) {
    if (!node || !result) return;
    
    if (node->is_end_of_word) {
        current_word[depth] = '\0';
        prefix_result_add(result, current_word, node->frequency);
    }
    
    for (int i = 0; i < ALPHABET; i++) {
        if (node->children[i]) {
            current_word[depth] = (char)i;
            collect_words_with_prefix(node->children[i], current_word, depth + 1, result);
        }
    }
}

PrefixResult* trie_get_words_with_prefix(Trie *trie, const char *prefix) {
    if (!trie || !prefix) return NULL;
    
    TrieNode *current = trie->root;
    int prefix_len = strlen(prefix);
    
    for (int i = 0; i < prefix_len; i++) {
        unsigned char index = (unsigned char)prefix[i];
        
        if (!current->children[index]) {
            return prefix_result_create(1);
        }
        
        current = current->children[index];
    }
    
    PrefixResult *result = prefix_result_create(current->prefix_count > 0 ? current->prefix_count : 10);
    if (!result) return NULL;
    
    char word_buffer[1000];
    strcpy(word_buffer, prefix);
    
    collect_words_with_prefix(current, word_buffer, prefix_len, result);
    
    return result;
}

// ESTADÍSTICAS Y UTILIDADES

void trie_print_stats(const Trie *trie) {
    if (!trie) return;
    
    printf("\n=== ESTADÍSTICAS DEL TRIE ===\n");
    printf("Palabras únicas: %zu\n", trie->word_count);
    printf("Nodos totales: %zu\n", trie->node_count);
    printf("Factor de compresión: %.2f%%\n", 
           trie->word_count > 0 ? (100.0 * trie->node_count) / trie->word_count : 0.0);
    printf("============================\n\n");
}

static int calculate_max_depth(TrieNode *node, int current_depth) {
    if (!node) return current_depth - 1;
    
    int max_depth = current_depth;
    
    for (int i = 0; i < ALPHABET; i++) {
        if (node->children[i]) {
            int child_depth = calculate_max_depth(node->children[i], current_depth + 1);
            if (child_depth > max_depth) {
                max_depth = child_depth;
            }
        }
    }
    
    return max_depth;
}

int trie_get_depth(const Trie *trie) {
    if (!trie || !trie->root) return 0;
    return calculate_max_depth(trie->root, 0);
}

static void print_all_words_recursive(TrieNode *node, char *buffer, int depth) {
    if (!node) return;
    
    if (node->is_end_of_word) {
        buffer[depth] = '\0';
        printf("  %-20s (frecuencia: %d)\n", buffer, node->frequency);
    }
    
    for (int i = 0; i < ALPHABET; i++) {
        if (node->children[i]) {
            buffer[depth] = (char)i;
            print_all_words_recursive(node->children[i], buffer, depth + 1);
        }
    }
}

void trie_print_all_words(const Trie *trie) {
    if (!trie || !trie->root) return;
    
    printf("\n=== TODAS LAS PALABRAS EN EL TRIE ===\n");
    
    char buffer[1000];
    print_all_words_recursive(trie->root, buffer, 0);
    
    printf("====================================\n\n");
}

static void trie_node_free(TrieNode *node) {
    if (!node) return;
    
    for (int i = 0; i < ALPHABET; i++) {
        if (node->children[i]) {
            trie_node_free(node->children[i]);
        }
    }
    
    free(node);
}

void trie_free(Trie *trie) {
    if (!trie) return;
    
    trie_node_free(trie->root);
    free(trie);
}