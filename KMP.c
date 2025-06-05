#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

long long *v1 = NULL; 
int v1_size = 0;       
int count_kmp = 0;
void append(long long value) {
    v1 = (long long *)realloc(v1, (v1_size + 1) * sizeof(long long));
    if (v1 == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    v1[v1_size++] = value;
}

// Compute the longest prefix suffix (LPS) array for KMP pattern matching
void computeLPSArray(char *pat, int M, int *lps) {
    int len = 0;
    lps[0] = 0; // LPS of the first character is always 0

    int i = 1;
    while (i < M) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) len = lps[len - 1];
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

// KMP algorithm to search for a pattern in the given text
void KMPSearch(char *pat, char *txt) {
    int M = strlen(pat);
    int N = strlen(txt);
    int *lps = (int *)malloc(M * sizeof(int));

    computeLPSArray(pat, M, lps); // Build LPS array
    int temp=0;
    int i = 0, j = 0; // i is index for txt, j is index for pat
    while (i < N) {
        if (pat[j] == txt[i]) {
            j++;
            i++;
        }

        if (j == M) { // Pattern found
            long long it = 0;
            while (it < v1_size && v1[it] <= i - j) it++;

            int c = (it == 0) ? i - j + 1 : i - j - v1[it - 1];

            count_kmp++;
            if(temp==0){
                printf("Found at line: %lld position: %d\n", it + 1, c-1);
                temp++;
            }
            else printf("Found at line: %lld position: %d\n", it + 1, c);

            j = lps[j - 1]; // Reset j using LPS array
        } else if (i < N && pat[j] != txt[i]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
    free(lps);
}

int main() {
    clock_t start, end;
    start = clock();

    FILE *newfile = fopen("sherlock.txt", "r");
    if (newfile == NULL) {
        printf("Error: Could not open the file.\n");
        return 1;
    }

    char *s1 = NULL;
    size_t s1_len = 0;
    char line[1024]; 

    long long total_length = 0;
    while (fgets(line, sizeof(line), newfile)) {
        size_t line_len = strlen(line);

        s1 = (char *)realloc(s1, s1_len + line_len + 1);
        if (s1 == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        strcpy(s1 + s1_len, line);
        s1_len += line_len;

        total_length += line_len;
        append(total_length);
    }
    fclose(newfile);

    char s2[1024];
    printf("Enter pattern to search: ");
    fgets(s2, sizeof(s2), stdin);

    s2[strcspn(s2, "\n")] = '\0';

    KMPSearch(s2, s1);

    printf("Number of Occurrences: %d\n", count_kmp);

    end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n", time_taken);
    
    free(s1);
    free(v1);

    return 0;
}
