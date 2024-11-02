#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define NO_OF_CHARS 256

long long *v1;
int v1_size = 0;  
int count_fa = 0; 

// Function to find the lower bound of a value in a sorted array
long long lower_bound(long long *arr, int size, long long value) {
    int low = 0, high = size;
    while (low < high) {
        int mid = (low + high) / 2;
        if (arr[mid] < value) low = mid + 1;
        else high = mid;
    }
    return low;
}

// Dynamically add a new value to the array v1 similar to vector.push_back() in C++
void add_to_v1(long long value) {
    v1_size++;
    v1 = (long long *)realloc(v1, v1_size * sizeof(long long));
    v1[v1_size - 1] = value;
}

// Get the next state for the finite automata based on pattern match and character x
int getNextState(char *pat, int M, int state, int x) {
    if (state < M && x == pat[state]) return state + 1;
    int ns, i;
    for (ns = state; ns > 0; ns--) {
        if (pat[ns - 1] == x) {
            for (i = 0; i < ns - 1; i++) if (pat[i] != pat[state - ns + 1 + i]) break;
            if (i == ns - 1) return ns;
        }
    }
    return 0;
}

// Build the transition function table for finite automata
void computeTF(char *pat, int M, int TF[][NO_OF_CHARS]) {
    int state, x;
    for (state = 0; state <= M; ++state) for (x = 0; x < NO_OF_CHARS; ++x) TF[state][x] = getNextState(pat, M, state, x);
}

// Search for pattern in the text using finite automata
void search(char *pat, char *txt) {
    int M = strlen(pat);
    int N = strlen(txt);

    int TF[M + 1][NO_OF_CHARS];
    computeTF(pat, M, TF);// Build transition table

    int i, state = 0;
    for (i = 0; i < N; i++) {
        state = TF[state][(unsigned char)txt[i]];
        if (state == M) {// Pattern found
            long long it = lower_bound(v1, v1_size, i - M + 1);
            int c;
            if (it == 0) c = i - M + 1;
            else c = i - M + 1 - v1[it - 1];
            if (it == 0) c++;
            count_fa++;
            printf("Found at line: %lld position: %d\n", it + 1, c-1);
        }
    }
}


int main() {
    clock_t start, end;
    start = clock();

    FILE *newfile;
    newfile = fopen("sherlock.txt", "r");

    if (newfile == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    char *s1 = (char *)malloc(1);
    s1[0] = '\0'; 
    char line[1024];

    while (fgets(line, sizeof(line), newfile)) {
        size_t len = strlen(line);
        s1 = (char *)realloc(s1, strlen(s1) + len + 2); 
        strcat(s1, line);
        strcat(s1, " ");

        if (v1_size == 0) add_to_v1(len);
        else add_to_v1(v1[v1_size - 1] + len + 1);
    }
    fclose(newfile);

    char s2[256];
    printf("Enter the pattern to search: ");
    fgets(s2, sizeof(s2), stdin);
    s2[strcspn(s2, "\n")] = '\0'; 

    search(s2, s1);
    printf("Number of Occurrences: %d\n", count_fa);

    end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n", time_taken);
    
    free(s1);
    free(v1);
    return 0;
}
