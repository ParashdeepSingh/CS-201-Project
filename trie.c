#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAX_CHAR 256

long long *v1 = NULL;
int v1_size = 0;
int count_trie = 0;

// Append function similar to vector's push_back in C++
void append(long long value) {
    v1 = (long long *)realloc(v1, (v1_size + 1) * sizeof(long long));
    if (v1 == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    v1[v1_size++] = value;
}

// Trie node structure with children and indexes array
typedef struct PrefixTrieNode {
    struct PrefixTrieNode *children[MAX_CHAR];
    int *indexes;
    int index_count;
} PrefixTrieNode;

// Create a new trie node and initialize fields
PrefixTrieNode* createNode() {
    PrefixTrieNode *node = (PrefixTrieNode *)malloc(sizeof(PrefixTrieNode));
    node->indexes = NULL;
    node->index_count = 0;
    for (int i = 0; i < MAX_CHAR; i++) node->children[i] = NULL;
    return node;
}

// Insert a prefix into the trie and track its index
void insertPrefix(PrefixTrieNode *node, const char *s, int index) {
    node->indexes = (int *)realloc(node->indexes, (node->index_count + 1) * sizeof(int));
    if (node->indexes == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    node->indexes[node->index_count++] = index;

    if (*s != '\0') {
        char cIndex = *s;
        if (node->children[cIndex] == NULL) node->children[cIndex] = createNode();
        insertPrefix(node->children[cIndex], s + 1, index+1);
    }
}

// Search for a pattern in the trie and return matching indexes
int* search(PrefixTrieNode *node, const char *s, int *result_count) {
    if (*s == '\0') {
        *result_count = node->index_count;
        return node->indexes;
    }
    if (node->children[(unsigned char)*s] != NULL) return search(node->children[(unsigned char)*s], s + 1, result_count);
    return NULL;
}

typedef struct PrefixTrie {
    PrefixTrieNode *root;
} PrefixTrie;

// Building a prefix trie from the input text
PrefixTrie* createPrefixTrie(const char *txt) {
    PrefixTrie *trie = (PrefixTrie *)malloc(sizeof(PrefixTrie));
    trie->root = createNode();
    for (int i = 0; txt[i] != '\0'; i++) insertPrefix(trie->root, txt + i, i);
    return trie;
}

void searchInTrie(PrefixTrie *trie, const char *pat) {
    int result_count = 0;
    int *result = search(trie->root, pat, &result_count);
    int temp = 0;
    if (result != NULL) {
        int patLen = strlen(pat);
        for (int i = 0; i < result_count; i++) {
            long long it = 0;
            while (it < v1_size && v1[it] <= result[i] - patLen) it++;

            int c = (it == 0) ? result[i] - patLen + 1 : result[i] - patLen - v1[it - 1];

            count_trie++;
            if (temp == 0) {
                printf("Found at line: %lld position: %d\n", it + 1, c - 1);
                temp++;
            } else {
                printf("Found at line: %lld position: %d\n", it + 1, c);
            }
        }
    }
}

char* readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open the file.\n");
        exit(1);
    }

    char *text = NULL;
    size_t text_len = 0;
    char line[1024];
    long long total_length = 0;

    while (fgets(line, sizeof(line), file)) {
        size_t line_len = strlen(line);

        text = (char *)realloc(text, text_len + line_len + 1);
        if (text == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        strcpy(text + text_len, line);
        text_len += line_len;

        total_length += line_len;
        append(total_length);
    }
    fclose(file);
    return text;
}

int main() {
    clock_t start, end;
    start = clock();

    char *text = readFile("sherlock2.txt");

    PrefixTrie *trie = createPrefixTrie(text);

    char pattern[1024];
    printf("Enter pattern to search: ");
    fgets(pattern, sizeof(pattern), stdin);
    pattern[strcspn(pattern, "\n")] = '\0';

    searchInTrie(trie, pattern);

    printf("Number of occurrences: %d\n", count_trie);

    end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n", time_taken);

    free(text);
    free(v1);

    return 0;
}
