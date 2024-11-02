#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ALPHABET_SIZE 256

typedef struct {
    int line;
    int index;
} Occurrence;

typedef struct SuffixTreeNode {
    struct SuffixTreeNode* children[ALPHABET_SIZE];
    Occurrence occurrences[100];
    int occurrenceCount;
} SuffixTreeNode;

typedef struct {
    SuffixTreeNode* root;
} SuffixTree;

// Create a new suffix tree node and initialize children and occurrence count
SuffixTreeNode* createNode() {
    SuffixTreeNode* newNode = (SuffixTreeNode*)malloc(sizeof(SuffixTreeNode));
    for (int i = 0; i < ALPHABET_SIZE; i++) newNode->children[i] = NULL;
    newNode->occurrenceCount = 0;
    return newNode;
}

// Initialize the suffix tree with a root node
SuffixTree* createSuffixTree() {
    SuffixTree* suffixTree = (SuffixTree*)malloc(sizeof(SuffixTree));
    suffixTree->root = createNode();
    return suffixTree;
}

// Insert a suffix into the suffix tree, recording occurrences at each node
void insertSuffix(SuffixTree* suffixTree, const char* suffix, int lineNum, int startIndex) {
    SuffixTreeNode* currentNode = suffixTree->root;
    for (int i = startIndex; suffix[i] != '\0'; i++) {
        char ch = suffix[i];
        if (currentNode->children[ch] == NULL) currentNode->children[ch] = createNode();
        currentNode = currentNode->children[ch];
        // Store the occurrence if under max count limit
        if (currentNode->occurrenceCount < 100) {
            currentNode->occurrences[currentNode->occurrenceCount].line = lineNum + 1;
            currentNode->occurrences[currentNode->occurrenceCount].index = startIndex;
            currentNode->occurrenceCount++;
        }
    }
}

// Build the suffix tree by inserting all suffixes of each line
void buildSuffixTree(SuffixTree* suffixTree, char** lines, int lineCount) {
    for (int lineNum = 0; lineNum < lineCount; lineNum++) {
        const char* line = lines[lineNum];
        int length = strlen(line);
        for (int i = 0; i < length; i++) insertSuffix(suffixTree, line, lineNum, i);
    }
}

// Recursive search for a pattern in the suffix tree, printing occurrences found
void searchPattern(SuffixTreeNode* node, const char* pattern, int index) {
    if (node == NULL) return;
    int counter = 0;

    if (pattern[index] == '\0') { // Pattern match reached
        if (node->occurrenceCount > 0) {
            for (int i = 0; i < node->occurrenceCount; i++) {
                printf("Found at Line: %d position: %d\n", node->occurrences[i].line, node->occurrences[i].index);
                counter++;
            }
        } 
        printf("Number of Occurrences: %d\n", counter);
        return;
    }
    // Continue searching for next character in pattern
    char ch = pattern[index];
    searchPattern(node->children[ch], pattern, index + 1);
}

void freeSuffixTree(SuffixTreeNode* node) {
    for (int i = 0; i < ALPHABET_SIZE; i++) if (node->children[i] != NULL) freeSuffixTree(node->children[i]);
    free(node);
}

char** readFile(const char* filename, int* lineCount) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        return NULL;
    }

    char** lines = malloc(100 * sizeof(char*));
    *lineCount = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        lines[*lineCount] = strdup(line);
        (*lineCount)++;
    }
    fclose(file);
    return lines;
}

int main() {
    clock_t start,end;
    start = clock();

    const char* filename = "sherlock2.txt";
    int lineCount = 0;
    char** lines = readFile(filename, &lineCount);
    if (lines == NULL) {
        return EXIT_FAILURE;
    }
    SuffixTree* suffixTree = createSuffixTree();
    buildSuffixTree(suffixTree, lines, lineCount);
    char pattern[256];
    printf("Enter the pattern to search: ");
    scanf("%255s", pattern);
    searchPattern(suffixTree->root, pattern, 0);
    
    end = clock();
    double time_taken = ((double)end-start)/CLOCKS_PER_SEC;
    printf("Time taken: %f\n", time_taken);

    for (int i = 0; i < lineCount; i++) free(lines[i]);
    free(lines);
    freeSuffixTree(suffixTree->root);
    free(suffixTree);
    return EXIT_SUCCESS;
}
