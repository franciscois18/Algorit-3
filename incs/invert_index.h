#ifndef INVERT_INDEX_H
#define INVERT_INDEX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessing_structs.h" 

#define DEFAULT_POSTING_LIST_CAPACITY 10
#define DEFAULT_INVERTED_INDEX_DICT_CAPACITY 1024 

// Entrada Lista de postings 
typedef struct {
    int doc_id; 
    int frequency_in_doc;  
} Posting;

// Lista de postings para un término específico
typedef struct {
    Posting *postings;       
    size_t count;          
    size_t capacity;     
} PostingList;

// Entrada en la tabla hash del diccionario del índice invertido
typedef struct IndexTermEntry {
    char *term;               
    PostingList *posting_list;  
    struct IndexTermEntry *next; 
} IndexTermEntry;

// Estructura principal del Índice Invertido
typedef struct {
    IndexTermEntry **dictionary; 
    size_t dict_capacity;          
    size_t dict_size;         
    size_t total_docs_indexed; 
} InvertedIndex;

// --- Funciones para PostingList ---
PostingList* posting_list_create(size_t initial_capacity);
void posting_list_free(PostingList *pl);
bool posting_list_add_posting(PostingList *pl, int doc_id, int frequency_in_doc);

// --- Funciones para el Índice Invertido ---
InvertedIndex* inverted_index_create(size_t initial_dict_capacity);
void inverted_index_free(InvertedIndex *index);

bool inverted_index_add_document(InvertedIndex *index, int doc_id, const TokenList *tokens);

const PostingList* inverted_index_get_posting_list(const InvertedIndex *index, const char *term);

#endif 
