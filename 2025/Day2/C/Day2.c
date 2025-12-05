#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

// Idea from Simon Goater on Stack Overflow
// digits(5313) = 4
int digits(uint64_t x){
    return            1 +
              (x >= 10) +
             (x >= 100) +
            (x >= 1000) +
           (x >= 10000) +
          (x >= 100000) +
         (x >= 1000000) +
        (x >= 10000000) +
       (x >= 100000000) +
      (x >= 1000000000) +
     (x >= 10000000000ll);
}

// pad10(3) = 1000
uint64_t pad10(int n) {
    uint64_t r = 1;
    for(; n > 0; n--) r *= 10;
    return r;
}

// repn(123,3) = 123123123
uint64_t repn(uint64_t x, int n){
    uint64_t p = pad10(digits(x));
    uint64_t r = x;
    for(int i = 1; i < n; i++) r = r * p + x;
    return r;
}

// cutn(123658234,3) = 123
uint64_t cutn(uint64_t x, int n){
    return x / pad10(digits(x)-n);
}

// nextn(123658234,3) = 124
// nextn(12365823,3) = 100
int nextn(uint64_t x,int n){
    int d = digits(x);
    if( d % n == 0 ){
        uint64_t base = cutn(x,d/n);
        if(repn(base,n) < x) base++;
        return base;
    } else {
        return pad10(d/n);
    }
}

// prevn(123113234,3) = 122
// prevn(12343153,3) = 99
int prevn(uint64_t x,int n){
    int d = digits(x);
    if( d % n == 0 ){
        uint64_t base = cutn(x,d/n);
        if(repn(base,n) > x) base--;
        return base;
    } else {
        return pad10(d/n)-1;
    }
}

uint64_t quick_sum(uint64_t a, uint64_t b){
    return a + (b*(b+1) - a*(a+1))/2;
}

#define SUM_THRESH 99

// sum of n-repeat digits between a and b
uint64_t sum_range(uint64_t a, uint64_t b, int n){
    uint64_t r = 0;
    uint64_t i = nextn(a,n);
    uint64_t j = prevn(b,n);
    // if(j<i) return 0;
    // if(i <= SUM_THRESH){
    //     int tgt = min(SUM_THRESH,j);
    //     for(;i <= tgt; i++) r += repn(i,n);
    //     i++;
    // }
    while(i <= j){
        int d = digits(i);
        uint64_t p = pad10(d);
        int tgt = min((int)p-1,j);
        // sum the range from i to tgt, inclusive
        int s1 = quick_sum(i,tgt);
        r = s1;
        uint64_t pad_mul = p;
        for(int m = 2; m <= n; m++){
            r += s1 * pad_mul;
            pad_mul = pad_mul * p;
        }
        i = tgt + 1;
    }
    return r;
}

// sum of n-repeat digits between a and b
uint64_t sum_range_1(uint64_t a, uint64_t b, int n){
    uint64_t r = 0;
    uint64_t i = nextn(a,n);
    uint64_t j = prevn(b,n);
    for(;i <= j; i++) r += repn(i,n);
    return r;
}

uint64_t sum_all(uint64_t a, uint64_t b){
    uint64_t r2,r3,r5,r6,r7,r10;
    r2 = sum_range(a,b,2);
    r3 = sum_range(a,b,3);
    r5 = sum_range(a,b,5);
    r6 = sum_range(a,b,6);
    r7 = sum_range(a,b,7);
    r10 = sum_range(a,b,10);
    return r2 + r3 + r5 - r6 + r7 - r10;
}

uint64_t sum_all_1(uint64_t a, uint64_t b){
    // What's all this about?
    uint64_t  r2 = sum_range_1(a,b,2);
    uint64_t  r3 = sum_range_1(a,b,3);
    uint64_t  r5 = sum_range_1(a,b,5);
    uint64_t  r6 = sum_range_1(a,b,6);
    uint64_t  r7 = sum_range_1(a,b,7);
    uint64_t r10 = sum_range_1(a,b,10);
    return r2 + r3 + r5 - r6 + r7 - r10;
}

int main() {
    const char *fname = "input.txt";
    char line[500];
    FILE *file = fopen(fname,"r");
    char *read_line = fgets(line, 500, file);
    fclose(file);
    if(read_line == NULL) {
        perror("Error reading file");
        exit(1);
    }

    uint64_t r = 0;
    char *token = strtok(line, ",");
    while (token != NULL) {
        char *end;
        uint64_t a = strtoull(token, &end, 10);
        uint64_t b = strtoull(end+1, &end, 10);
        r += sum_all(a,b);
        token = strtok(NULL, ",");
    }

    printf("r=%llu\n", (long long unsigned int)r);
}
