#include "stopwords.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Función para obtener stopwords en español
static const char** get_spanish_stopwords(int *count) {
    static const char* spanish_stopwords[] = {
        "el", "la", "de", "que", "y", "a", "en", "un", "es", "se", "no", "te", "lo", "le", "da", "su", "por", "son",
        "con", "para", "al", "las", "del", "los", "una", "por", "como", "pero", "sus", "le", "ya", "o", "porque",
        "cuando", "muy", "sin", "sobre", "también", "me", "hasta", "hay", "donde", "quien", "desde", "todo", "nos",
        "durante", "todos", "uno", "les", "ni", "contra", "otros", "ese", "eso", "ante", "ellos", "e", "esto", "mí",
        "antes", "algunos", "qué", "unos", "yo", "otro", "otras", "otra", "él", "tanto", "esa", "estos", "mucho",
        "quienes", "nada", "muchos", "cual", "poco", "ella", "estar", "estas", "algunas", "algo", "nosotros", "mi",
        "mis", "tú", "te", "ti", "tu", "tus", "ellas", "nosotras", "vosotros", "vosotras", "os", "mío", "mía",
        "míos", "mías", "tuyo", "tuya", "tuyos", "tuyas", "suyo", "suya", "suyos", "suyas", "nuestro", "nuestra",
        "nuestros", "nuestras", "vuestro", "vuestra", "vuestros", "vuestras", "esos", "esas", "estoy", "estás",
        "está", "estamos", "estáis", "están", "esté", "estés", "estemos", "estéis", "estén", "estaré", "estarás",
        "estará", "estaremos", "estaréis", "estarán", "estaría", "estarías", "estaríamos", "estaríais", "estarían",
        "estaba", "estabas", "estábamos", "estabais", "estaban", "estuve", "estuviste", "estuvo", "estuvimos",
        "estuvisteis", "estuvieron", "sea", "seas", "seamos", "seáis", "sean", "soy", "eres", "somos", "sois",
        "fui", "fuiste", "fue", "fuimos", "fuisteis", "fueron", "seré", "serás", "será", "seremos", "seréis",
        "serán", "sería", "serías", "seríamos", "seríais", "serían", "tengo", "tienes", "tiene", "tenemos",
        "tenéis", "tienen", "tenga", "tengas", "tengamos", "tengáis", "tengan", "tendré", "tendrás", "tendrá",
        "tendremos", "tendréis", "tendrán", "tendría", "tendrías", "tendríamos", "tendríais", "tendrían",
        "tenía", "tenías", "teníamos", "teníais", "tenían", "tuve", "tuviste", "tuvo", "tuvimos", "tuvisteis",
        "tuvieron", "hago", "haces", "hace", "hacemos", "hacéis", "hacen", "haga", "hagas", "hagamos", "hagáis",
        "hagan", "haré", "harás", "hará", "haremos", "haréis", "harán", "haría", "harías", "haríamos", "haríais",
        "harían", "hacía", "hacías", "hacíamos", "hacíais", "hacían", "hice", "hiciste", "hizo", "hicimos",
        "hicisteis", "hicieron", "doy", "das", "damos", "dais", "dan", "dé", "des", "demos", "deis", "den",
        "daré", "darás", "dará", "daremos", "daréis", "darán", "daría", "darías", "daríamos", "daríais",
        "darían", "daba", "dabas", "dábamos", "dabais", "daban", "di", "diste", "dio", "dimos", "disteis",
        "dieron"
    };
    
    *count = sizeof(spanish_stopwords) / sizeof(spanish_stopwords[0]);
    return spanish_stopwords;
}

// Función para obtener stopwords en inglés
static const char** get_english_stopwords(int *count) {
    static const char* english_stopwords[] = {
        "a", "an", "and", "are", "as", "at", "be", "by", "for", "from", "has", "he", "in", "is", "it",
        "its", "of", "on", "that", "the", "to", "was", "were", "will", "with", "the", "i", "me", "my",
        "myself", "we", "our", "ours", "ourselves", "you", "your", "yours", "yourself", "yourselves",
        "him", "his", "himself", "she", "her", "hers", "herself", "they", "them", "their", "theirs",
        "themselves", "what", "which", "who", "whom", "this", "these", "am", "been", "being", "have",
        "had", "having", "do", "does", "did", "doing", "would", "should", "could", "ought", "i'm",
        "you're", "he's", "she's", "it's", "we're", "they're", "i've", "you've", "we've", "they've",
        "i'd", "you'd", "he'd", "she'd", "we'd", "they'd", "i'll", "you'll", "he'll", "she'll",
        "we'll", "they'll", "isn't", "aren't", "wasn't", "weren't", "hasn't", "haven't", "hadn't",
        "doesn't", "don't", "didn't", "won't", "wouldn't", "shan't", "shouldn't", "can't", "cannot",
        "couldn't", "mustn't", "let's", "that's", "who's", "what's", "here's", "there's", "when's",
        "where's", "why's", "how's", "daren't", "needn't", "oughtn't", "mightn't", "above", "below",
        "up", "down", "out", "off", "over", "under", "again", "further", "then", "once", "here",
        "there", "when", "where", "why", "how", "all", "any", "both", "each", "few", "more", "most",
        "other", "some", "such", "no", "nor", "not", "only", "own", "same", "so", "than", "too",
        "very", "can", "just", "now", "until", "while", "because", "if", "or", "but", "about",
        "against", "between", "into", "through", "during", "before", "after", "through", "above",
        "below", "up", "down", "in", "out", "on", "off", "over", "under", "again", "further", "then", "once"
    };
    
    *count = sizeof(english_stopwords) / sizeof(english_stopwords[0]);
    return english_stopwords;
}

// Crear lista de stopwords
StopWordList* stopword_list_create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = DEFAULT_STOPWORDS_CAPACITY;
    }
    
    StopWordList *list = malloc(sizeof(StopWordList));
    if (!list) {
        return NULL;
    }
    
    list->words = malloc(initial_capacity * sizeof(char*));
    if (!list->words) {
        free(list);
        return NULL;
    }
    
    list->count = 0;
    list->capacity = initial_capacity;
    
    return list;
}

// Liberar memoria de la lista de stopwords
void stopword_list_free(StopWordList *list) {
    if (!list) return;
    
    for (size_t i = 0; i < list->count; i++) {
        free(list->words[i]);
    }
    
    free(list->words);
    free(list);
}

// Función auxiliar para convertir a minúsculas
static char* to_lowercase(const char *word) {
    if (!word) return NULL;
    
    size_t len = strlen(word);
    char *lower = malloc(len + 1);
    if (!lower) return NULL;
    
    for (size_t i = 0; i < len; i++) {
        lower[i] = tolower((unsigned char)word[i]);
    }
    lower[len] = '\0';
    
    return lower;
}

// Redimensionar la lista si es necesario
static bool resize_stopword_list(StopWordList *list) {
    if (list->count < list->capacity) {
        return true;
    }
    
    size_t new_capacity = list->capacity * 2;
    char **new_words = realloc(list->words, new_capacity * sizeof(char*));
    if (!new_words) {
        return false;
    }
    
    list->words = new_words;
    list->capacity = new_capacity;
    return true;
}

// Añadir palabra a la lista de stopwords
bool stopword_list_add(StopWordList *list, const char *word) {
    if (!list || !word || strlen(word) == 0) {
        return false;
    }
    
    // Verificar si la palabra ya existe
    if (stopword_list_contains(list, word)) {
        return true; // Ya existe, no es error
    }
    
    // Redimensionar si es necesario
    if (!resize_stopword_list(list)) {
        return false;
    }
    
    // Convertir a minúsculas y copiar
    char *lower_word = to_lowercase(word);
    if (!lower_word) {
        return false;
    }
    
    list->words[list->count] = lower_word;
    list->count++;
    
    return true;
}

// Verificar si una palabra está en la lista de stopwords
bool stopword_list_contains(const StopWordList *list, const char *word) {
    if (!list || !word) {
        return false;
    }
    
    char *lower_word = to_lowercase(word);
    if (!lower_word) {
        return false;
    }
    
    bool found = false;
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->words[i], lower_word) == 0) {
            found = true;
            break;
        }
    }
    
    free(lower_word);
    return found;
}

// Cargar stopwords en español
StopWordList* load_spanish_stopwords(void) {
    int count;
    const char** stopwords = get_spanish_stopwords(&count);
    
    StopWordList *list = stopword_list_create(count + 50); // Un poco más de espacio
    if (!list) {
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        if (!stopword_list_add(list, stopwords[i])) {
            stopword_list_free(list);
            return NULL;
        }
    }
    
    return list;
}

// Cargar stopwords en inglés
StopWordList* load_english_stopwords(void) {
    int count;
    const char** stopwords = get_english_stopwords(&count);
    
    StopWordList *list = stopword_list_create(count + 50); // Un poco más de espacio
    if (!list) {
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        if (!stopword_list_add(list, stopwords[i])) {
            stopword_list_free(list);
            return NULL;
        }
    }
    
    return list;
}

// Cargar stopwords desde archivo
StopWordList* load_stopwords_from_file(const char *filename) {
    if (!filename) {
        return NULL;
    }
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }
    
    StopWordList *list = stopword_list_create(DEFAULT_STOPWORDS_CAPACITY);
    if (!list) {
        fclose(file);
        return NULL;
    }
    
    char line[MAX_TOKEN_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Remover salto de línea
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Saltar líneas vacías y comentarios
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }
        
        // Añadir palabra
        if (!stopword_list_add(list, line)) {
            fprintf(stderr, "Warning: No se pudo añadir la palabra '%s'\n", line);
        }
    }
    
    fclose(file);
    return list;
}

// Filtrar stopwords de una lista de tokens
TokenList* filter_stopwords(const TokenList *tokens, const StopWordList *stopwords) {
    if (!tokens || !stopwords) {
        return NULL;
    }
    
    // Crear nueva lista de tokens
    TokenList *filtered = malloc(sizeof(TokenList));
    if (!filtered) {
        return NULL;
    }
    
    filtered->tokens = malloc(tokens->capacity * sizeof(Token));
    if (!filtered->tokens) {
        free(filtered);
        return NULL;
    }
    
    filtered->count = 0;
    filtered->capacity = tokens->capacity;
    filtered->total_length = 0;
    
    // Filtrar tokens
    for (size_t i = 0; i < tokens->count; i++) {
        const Token *token = &tokens->tokens[i];
        
        // Solo filtrar palabras (no puntuación)
        if (!token->is_word || !stopword_list_contains(stopwords, token->text)) {
            // Copiar token
            Token *new_token = &filtered->tokens[filtered->count];
            
            // Copiar texto
            new_token->text = malloc(token->length + 1);
            if (!new_token->text) {
                // Error: liberar memoria parcial
                for (size_t j = 0; j < filtered->count; j++) {
                    free(filtered->tokens[j].text);
                }
                free(filtered->tokens);
                free(filtered);
                return NULL;
            }
            
            strcpy(new_token->text, token->text);
            new_token->length = token->length;
            new_token->position = token->position;
            new_token->is_word = token->is_word;
            
            filtered->count++;
            filtered->total_length += token->length;
        }
    }
    
    return filtered;
}

