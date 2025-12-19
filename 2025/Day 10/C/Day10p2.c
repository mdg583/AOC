#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEFT(i) (2*i + 1)
#define RIGHT(i) (2*i + 2)
#define PARENT(i) ((i-1)/2)

typedef struct {
    int i; int v;
} Node;

// Heap functions
void hpush(Node *heap, int n, Node x){
    while(n > 0 && heap[PARENT(n)].v < x.v){
        heap[n] = heap[PARENT(n)];
        n = PARENT(n);
    }
    heap[n] = x;
}

Node hpop(Node *heap, int n){
    Node r = heap[0];
    Node x = heap[n-1]; n--;
    int i = 0;
    while(1){
        if(LEFT(i) >= n) break;
        int child = (RIGHT(i) >= n) || heap[LEFT(i)].v >= heap[RIGHT(i)].v ? LEFT(i) : RIGHT(i);
        if(x.v >= heap[child].v) break;
        heap[i] = heap[child];
        i = child;
    }
    heap[i] = x;
    return r;
}

int num_bits(int x){
    int r = 0;
    for(; x > 0; x /= 2) r += x & 1;
    return r;
}

// sort the 12-bit numbers by the number of bits
int *sorted_ints(int max_bits){
    int n = 1 << max_bits;
    Node *heap = (Node *)malloc(sizeof(Node)*n);
    int *sorted = (int *)malloc(sizeof(int)*n);
    // count bits
    for(int i = 0; i < n; i++){
        heap[i].i = i;
        heap[i].v = num_bits(i);
    }
    // heapify
    for(int i = 0; i < n; i++){
        hpush(heap, i, heap[i]);
    }
    // sort
    for(int i = 0; i < n; i++){
        Node x = hpop(heap, n-i);
        sorted[n-i-1] = x.i;
    }
    free(heap);
    return sorted;
}

int parse_buttons(int *buttons, int max, char *s){
    int n = 0;
    char *token = strtok(s, "(){} ");
    char inner_string[100];
    while(n < max && token != NULL){
        buttons[n] = 0;
        int i = 0;
        int p2 = 1;
        int t_len = strlen(token);
        int j1 = 0;
        int j2;
        while(j1 < t_len){
            int j2 = j1;
            while(token[j2] != ',' && token[j2] != '\0') j2++;
            token[j2] = '\0';
            buttons[n] += 1<<atoi(token+j1);
            j1 = j2+1;
        }
        n++;
        token = strtok(NULL, "(){} ");
    }
    return n;
}

int parse_joltage(int *buttons, int max, char *s){
    int n = 0;
    char *token = strtok(s, ",");
    while(n < max && token != NULL){
        buttons[n] = atoi(token);
        token = strtok(NULL, ",");
        n++;
    }
    return n;
}

int check_joltage(int *buttons, int n, int *joltage_tgt, int m, int x){
    int joltage[100] = {0};
    int i = 0;
    for(; x > 0; x /= 2){
        if(x & 1){
            // add button i
            printf("Add button %d (%d)\n", i, buttons[i]);
            int j = 0;
            for(int y = buttons[i]; y > 0; y /= 2){
                if(y & 1){
                    joltage[j] += 1;
                    if(joltage[j] > joltage_tgt[j]) return 0;
                }
                j++;
            }
            for(j = 0; j < m; j++) printf("%d ", joltage[j]);
            printf("\n");
        }
        i++;
    }
    // check if joltages equal
    for(int j = 0; j < m; j++){
        if(joltage[j] != joltage_tgt[j]) return 0;
    }
    return 1;
}

int fewest_presses(char *line){
    int lights;
    int buttons[200];
    int joltage_tgt[200];
    int n,m;
    char *token;
    printf("%s", line);
    token = strtok(line, "[]{}");
    char *bstring = strtok(NULL, "[]{}");
    char *jstring = strtok(NULL, "[]{}");
    n = parse_buttons(buttons, 200, bstring);
    m = parse_joltage(joltage_tgt, 200, jstring);

    for(int i = 0; i < n; i++){
        printf("%d ", buttons[i]);
    }
    printf("\n");

    for(int i = 0; i < m; i++){
        printf("%d ", joltage_tgt[i]);
    }
    printf("\n");

    int *bits_sorted = sorted_ints(n);
    for(int i = 0; i < (1<<n); i++){
        int x = bits_sorted[i];
        printf("Checking %d\n", x);
        int matched = check_joltage(buttons, n, joltage_tgt, m, x);
        if(matched){
            int r = num_bits(bits_sorted[i]);
            free(bits_sorted);
            return r;
        }
    }
    free(bits_sorted);
    exit(1);
    return -1;
}

int main() {
    int n = 0;
    char line[1024];
    int total = 0;
    while(fgets(line, 1024, stdin) != NULL){
        int x1 = fewest_presses(line);
        printf("fewest presses: %d\n", x1);
        total += x1;
        n++;
        return(0);
    }
    printf("Parsed %d lines\n", n);
    printf("Fewest presses: %d\n", total);
}
