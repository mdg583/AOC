#include <stdio.h>
#include <stdlib.h>

// cat input.txt | ./Day7.out

int main(int argc, char *argv[]) {
    long long total;
    FILE *f = fopen(argv[1], "r");
    for(int k = 0; k < 10000; k++){
        rewind(f);
        // First row: find S and count cols
        int s_loc = -1; int cols = 0;
        char c = fgetc(f);
        while(c != EOF && c != '\n' && c != '\r'){
            if(c == 'S') s_loc = cols;
            c = fgetc(f); cols++;
        }

        // initialize beams
        long long *beams = (long long  *)malloc(sizeof(long long)*cols);
        for(int i = 0; i < cols; i++) {
            beams[i] = (i==s_loc);
        }

        // check flow
        int i = 0;
        while((c = fgetc(f)) != EOF){
            if(c == '\n') { i = 0; continue; }
            if(c == '^') {
                if(i > 0) beams[i-1] += beams[i];
                if(i < cols-1) beams[i+1] += beams[i];
                beams[i] = 0;
            }
            i++;
        }
        // total = 0;
        for(i = 0; i < cols; i++) total += beams[i];
    }
    fclose(f);
    printf("Beams: %llu\n",total);
}
