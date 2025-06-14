#include "invert_index.h"
#include "hash.h" 


// Hash Auxiliar (djb2) (Esto que tenemos aqui no es lo mismo de hash.c (?))
static unsigned long djb2_hash_internal(const char *str, size_t capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++)) { // Cast a unsigned char
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % capacity;
}

// Crear un PostingList
PostingList* posting_list_create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = DEFAULT_POSTING_LIST_CAPACITY;
    }
    PostingList *pl = malloc(sizeof(PostingList));
    if (!pl) {
        perror("Error al asignar memoria para PostingList");
        return NULL;
    }
    pl->postings = malloc(initial_capacity * sizeof(Posting));
    if (!pl->postings) {
        perror("Error al asignar memoria para postings array");
        free(pl);
        return NULL;
    }
    pl->count = 0;
    pl->capacity = initial_capacity;
    return pl;
}

//Función para liberar la memoria de un PostingList
void posting_list_free(PostingList *pl) {
    if (!pl) return;
    free(pl->postings);
    free(pl);
}

// AÑadir una nueva ocurrencia de un término en un documento
bool posting_list_add_posting(PostingList *pl, int doc_id, int frequency_in_doc) {
    if (!pl) return false;

    if (pl->count >= pl->capacity) {
        size_t new_capacity = pl->capacity * 2;
        if (new_capacity == 0) new_capacity = DEFAULT_POSTING_LIST_CAPACITY; // Caso de capacidad inicial 0
        Posting *new_postings = realloc(pl->postings, new_capacity * sizeof(Posting));
        if (!new_postings) {
            perror("Error al redimensionar postings array");
            return false; // No se pudo añadir
        }
        pl->postings = new_postings;
        pl->capacity = new_capacity;
    }

    pl->postings[pl->count].doc_id = doc_id;
    pl->postings[pl->count].frequency_in_doc = frequency_in_doc;
    pl->count++;
    return true;
}

// Implementación del índice invertido

InvertedIndex* inverted_index_create(size_t initial_dict_capacity) {
    if (initial_dict_capacity == 0) {
        initial_dict_capacity = DEFAULT_INVERTED_INDEX_DICT_CAPACITY;
    }
    InvertedIndex *index = malloc(sizeof(InvertedIndex));
    if (!index) {
        perror("Error al asignar memoria para InvertedIndex");
        return NULL;
    }

    index->dictionary = calloc(initial_dict_capacity, sizeof(IndexTermEntry*));
    if (!index->dictionary) {
        perror("Error al asignar memoria para el diccionario del InvertedIndex");
        free(index);
        return NULL;
    }

    index->dict_capacity = initial_dict_capacity;
    index->dict_size = 0;
    index->total_docs_indexed = 0;

    return index;
}

void inverted_index_free(InvertedIndex *index) {
    if (!index) return;

    for (size_t i = 0; i < index->dict_capacity; ++i) {
        IndexTermEntry *entry = index->dictionary[i];
        while (entry != NULL) {
            IndexTermEntry *next_entry = entry->next;
            free(entry->term);
            posting_list_free(entry->posting_list);
            free(entry);
            entry = next_entry;
        }
    }
    free(index->dictionary);
  
    free(index);
}

// Añadir o actualizar una ocurrencia de término
static bool inverted_index_add_term_occurrence(InvertedIndex *index, const char *term, int doc_id, int term_frequency_in_doc) {
    if (!index || !term || term_frequency_in_doc <= 0) return false;

    size_t bucket_index = djb2_hash_internal(term, index->dict_capacity);
    IndexTermEntry *entry = index->dictionary[bucket_index];
    IndexTermEntry *prev_entry = NULL;

    // Buscar el término en el bucket
    while (entry != NULL) {
        if (strcmp(entry->term, term) == 0) {
            return posting_list_add_posting(entry->posting_list, doc_id, term_frequency_in_doc);
        }
        prev_entry = entry;
        entry = entry->next;
    }

    // Término no encontrado en el diccionario, crear nueva entrada
    IndexTermEntry *new_entry = malloc(sizeof(IndexTermEntry));
    if (!new_entry) {
        perror("Error al asignar memoria para IndexTermEntry");
        return false;
    }
    new_entry->term = strdup(term);
    if (!new_entry->term) {
        perror("Error al duplicar el término");
        free(new_entry);
        return false;
    }
    new_entry->posting_list = posting_list_create(DEFAULT_POSTING_LIST_CAPACITY);
    if (!new_entry->posting_list) {
        free(new_entry->term);
        free(new_entry);
        return false;
    }
    new_entry->next = NULL;

    if (prev_entry == NULL) { 
        index->dictionary[bucket_index] = new_entry;
    } else {
        prev_entry->next = new_entry;
    }
    index->dict_size++;

    return posting_list_add_posting(new_entry->posting_list, doc_id, term_frequency_in_doc);
}


bool inverted_index_add_document(InvertedIndex *index, int doc_id, const TokenList *tokens) {
    if (!index || !tokens || tokens->count == 0) {
        return false;
    }

    HashTable *term_frequencies_in_doc = ht_create(tokens->count / 2 + 1);
    if (!term_frequencies_in_doc) {
        fprintf(stderr, "Error: No se pudo crear tabla hash temporal para frecuencias del documento %d\n", doc_id);
        return false;
    }

    for (size_t i = 0; i < tokens->count; ++i) {
        Token *token = &tokens->tokens[i];
        if (token->is_word && token->text && token->length > 0) {
            ht_insert(term_frequencies_in_doc, token->text, 1);
        }
    }

    bool success = true;
    for (size_t i = 0; i < term_frequencies_in_doc->capacity; ++i) {
        HashNode *node = term_frequencies_in_doc->buckets[i];
        while (node != NULL) {
            if (node->key) { 
                if (!inverted_index_add_term_occurrence(index, node->key, doc_id, node->value)) {
                    fprintf(stderr, "Error al añadir ocurrencia del término '%s' para doc %d\n", node->key, doc_id);
                    success = false;
                }
            }
            node = node->next;
        }
    }

    ht_free(term_frequencies_in_doc); // Liberar la tabla hash temporal

    if (success) {
        index->total_docs_indexed++;
    }

    return success;
}

// Consultar un término en el índice invertido
const PostingList* inverted_index_get_posting_list(const InvertedIndex *index, const char *term) {
    if (!index || !term) {
        return NULL;
    }

    size_t bucket_index = djb2_hash_internal(term, index->dict_capacity);
    IndexTermEntry *entry = index->dictionary[bucket_index];

    while (entry != NULL) {
        if (strcmp(entry->term, term) == 0) {
            return entry->posting_list;
        }
        entry = entry->next;
    }

    return NULL; //No se encontró el término
}
