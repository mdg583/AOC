# Day 2 Part 2 C Implementation

This was from a crazy idea for a closed-form version of the algorithm. Somehow it worked.

The main ideas included:

- Assume we have a fast way to sum all of the n-repeated numbers between a and b.
- We face the issue that some numbers that are 2-repeated are also 3-repeated, etc
- While looking for solutions to this, a main question was: _can we quickly tell which numbers are both 2-repeated and 3-repeated?_
- After a lot of thinking, I came the the following conclusions:
  - Rule 1: any 4-repeated number is also a 2-repeated number. _If k | j, then any j-repeated number is also a k-repeated number_.
  - Rule 2: any number that is both 2-repeated and 3-repeated must be 6-repeated. _The set of numbers that are both k and j repeated are the same set of numbers that are (k * j)-repeated_.

Since all the numbers in the input are 10 digits or less, the only possibilities were $2 ... 10$ repeated digits. To start with, we only look at prime numbers since the others are included in previous numbers, by Rule 1.

So we look at 2,3,5,7

Which numbers may be in more than one of these groups?
- for 2 and 3, we need to look at 6-repeat digits
- for 2 and 5, we need to look at 10-repeat digits
- all the others (3 and 5, 3 and 7, 5 and 7) only result in numbers that are more than 10 digits.

So the method is: sum(a,b,2) + sum(a,b,3) + sum(a,b,5) - sum(a,b,6) + sum(a,b,7) - sum(a,b,10)

Once I saw this, it seemed possible to make a close-formed version.

The slow version does this:
``` C
uint64_t sum_range_1(uint64_t a, uint64_t b, int n){
    uint64_t r = 0;
    uint64_t i = nextn(a,n);
    uint64_t j = prevn(b,n);
    for(;i <= j; i++) r += repn(i,n);
    return r;
}
```

This finds the lower and upper 'base' number to be repeated n times, and generates and sums these.

The faster verion does this:
```C
// sum of n-repeat digits between a and b
uint64_t sum_range(uint64_t a, uint64_t b, int n){
    uint64_t r = 0;
    uint64_t i = nextn(a,n);
    uint64_t j = prevn(b,n);
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
```

This is similiar but it breaks the numbers into sets based on # digits of the base number, and uses some math to generate the sum, ie:

$$
1111+1212+1313+1414 = (11+12+13+14) + 100*(11+12+13+14)
$$
$$
101010+111111+121212+131313+141414 = (1 + 100 + 10000) * (10+11+12+13+14)
$$

The sum of the base numbers is computed using:

``` C
uint64_t quick_sum(uint64_t a, uint64_t b){
    return a + (b*(b+1) - a*(a+1))/2;
}
```

I'm not sure if this is optimized, but need to get some sleep...

