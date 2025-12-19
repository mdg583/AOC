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

int parse_lights(char *s){
    int ret = 0;
    int p2 = 1;
    for(int i = 0; i < strlen(s); i++){
        if(s[i]=='#') ret += p2;
        p2 = p2 * 2;
    }
    return ret;
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

int fewest_presses(char *line){
    int lights;
    int buttons[200];
    int n;
    char *token;
    // printf("%s", line);
    token = strtok(line, "[]{}");
    lights = parse_lights(token);
    token = strtok(NULL, "[]{}");
    n = parse_buttons(buttons, 200, token);
    token = strtok(NULL, "[]{}");

    // for(int i = 0; i < n; i++){
    //     printf("%d ", buttons[i]);
    // }
    // printf("\n");
    // printf("Looking for: %d among %d digits\n", lights, n);

    // Check the 2^n possible combinations of button presses
    if(lights == 0) return 0;
    int *bits_sorted = sorted_ints(n);
    for(int i = 0; i < (1<<n); i++){
        int check = 0;
        int x = bits_sorted[i];
        int j = 0;
        for(; x > 0; x /= 2){
            if(x & 1) check ^= buttons[j];
            j++;
        }
        if(check == lights){
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
    }
    printf("Parsed %d lines\n", n);
    printf("Fewest presses: %d\n", total);
}
