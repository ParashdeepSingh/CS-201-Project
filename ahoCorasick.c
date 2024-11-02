#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CHAR 256
#define MAX_PATTERN_LENGTH 100

// Structure of a Trie Node
typedef struct TrieNode {
    struct TrieNode* children[MAX_CHAR]; // Array of pointers to child nodes 
    struct TrieNode* failureLink; // Pointer to the node which is the longest proper suffix of the current node
    int output;  //// Index of the pattern if this node marks the end of a pattern, otherwise -1 
} TrieNode;

// Function to create a new Trie Node
TrieNode* createNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    for (int i = 0; i < MAX_CHAR; i++) node->children[i] = NULL; //Initially there are no children
    node->failureLink = NULL; // Initially there is no failure link
    node->output = -1; // Initialize output to -1 (indicates no pattern here)
    return node;
}

// Function to insert a pattern into the Trie
void insertPattern(TrieNode* root, const char* pattern, int patternIndex) {
    TrieNode* current = root;  // Start from the root
    while (*pattern) {
        int index = *pattern; // Get the ASCII index of the character
        if (!current->children[index]) current->children[index] = createNode(); // Create a new node if it does not exist
        current = current->children[index]; // move to current node's child
        pattern++;
    }
    current->output = patternIndex;
}

// Function to build failure links in the Trie using BFS
void buildFailureLinks(TrieNode* root) {
    TrieNode* queue[MAX_CHAR * MAX_CHAR];
    int front = 0, rear = 0;
    root->failureLink = root; // Root's failure link points to itself
    for (int i = 0; i < MAX_CHAR; i++) {
        if (root->children[i]) {
            root->children[i]->failureLink = root;// Set failure link for direct children
            queue[rear++] = root->children[i];
        } else root->children[i] = root; // If no child, link it back to root
    }
    while (front < rear) {
        TrieNode* current = queue[front++];
        for (int i = 0; i < MAX_CHAR; i++) {
            if (current->children[i]) {
                TrieNode* fail = current->failureLink;
                while (!fail->children[i]) fail = fail->failureLink; // Follow failure links until we find a child for the current character
                current->children[i]->failureLink = fail->children[i];
                if (fail->children[i]->output != -1) current->children[i]->output = fail->children[i]->output; // If the failure link points to an output node, inherit the output

                queue[rear++] = current->children[i];
            }
        }
    }
}

void searchPatternsInText(TrieNode* root, FILE* file, const char* pattern) {
    char line[MAX_PATTERN_LENGTH];
    int lineNumber = 0;
    int totalOccurrences = 0; 
    TrieNode* current;

    while (fgets(line, sizeof(line), file)) {
        lineNumber++;
        current = root;
        for (int i = 0; line[i] != '\0'; i++) {
            int index = line[i];
            while (current->children[index] == NULL) current = current->failureLink;
            current = current->children[index];
            if (current->output != -1) {
                printf("Found at line: %d position: %lu\n", lineNumber, i - strlen(pattern) + 1);
                totalOccurrences++; 
            }
        }
    }
    
    printf("Number of Occurrences: %d\n", totalOccurrences);
}

int main() {
    clock_t start, end;
    start = clock();

    char pattern[MAX_PATTERN_LENGTH];
    printf("Enter the pattern to search: ");
    scanf("%s", pattern); 

    FILE* file = fopen("sherlock.txt", "r"); 
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    TrieNode* root = createNode();
    insertPattern(root, pattern, 0);
    buildFailureLinks(root);
    searchPatternsInText(root, file, pattern);

    end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n", time_taken);

    fclose(file);
    return 0;
}
