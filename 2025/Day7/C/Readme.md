# Day 7 in C

Decided to try to make a fast version of this. It seemed like the data could be processed about as fast as the stream could be read in.

First find 'S' on the first row, and count rows. Then update beams line by line.

``` C
#include <stdio.h>
#include <stdlib.h>

// cat input.txt | ./Day7.out

int main() {
    // First row: find S and count cols
    int s_loc = -1; int cols = 0;
    char c = getchar();
    while(c != EOF && c != '\n' && c != '\r'){
        if(c == 'S') s_loc = cols;
        c = getchar(); cols++;
    }

    // initialize beams
    long long *beams = (long long  *)malloc(sizeof(long long)*cols);
    for(int i = 0; i < cols; i++) {
        beams[i] = (i==s_loc);
    }

    // check flow
    int i = 0;
    while((c = getchar()) != EOF){
        if(c == '\n') { i = 0; continue; }
        if(c == '^') {
            if(i > 0) beams[i-1] += beams[i];
            if(i < cols-1) beams[i+1] += beams[i];
            beams[i] = 0;
        }
        i++;
    }
    long long total = 0;
    for(i = 0; i < cols; i++) total += beams[i];
    printf("Beams: %llu\n",total);
}
```
