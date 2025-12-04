# Day 1

## Part 1

Create a list of dial locations after each successive rotation, and check how many land on 0.

``` R
x = readr::read_lines("input.txt")
xdir = substring(x,1,1)
xlen = as.numeric(substring(x,2))
sum((50+cumsum(xlen*ifelse(xdir=="L",-1,1))) %% 100 == 0)
```

## Part 2

Treat each final location of the dial as a class in terms of how many times the dial has turned past 0. Sum the difference in classes between each step. For the landing on 0 case, we double count a bunch of times and miss-count other times depending on if the class includes the lower or upper bound. So do both and average.

``` R
x = readr::read_lines("input.txt")
xdir = substring(x,1,1)
xlen = as.numeric(substring(x,2))
xcum = cumsum(c(50,xlen*ifelse(xdir=="L",-1,1)))
0.5 * sum(
  abs(diff(floor(xcum / 100))) +
  abs(diff(floor((xcum-1) / 100)))
)
```
