# Day 2

## Part 1

Idea: If we have a double number like 11, how do we figure out the next one?

The next such numbers are 22, 33, ... 99. No 3-digit numbers work, and the first 4-digit number is 1010. The number being doubled in each case is 2,3,...,9,10. These are just being iterated!

I get an upper and lower bound for i and j, the first and last double number in the range. Then read off the ones in between.

``` R
library(dplyr)
x = readr::read_delim("input.txt",",",col_types = "c",col_names = FALSE) %>%
  stringr::str_split("-")

comp.gt = function(x,y) nchar(x) > nchar(y) || (nchar(x) == nchar(y) && x > y)
options(scipen = 999)
# i: 10000 -> 5 -> 2.5 -> 3 -> 100 -> 100100
# j: 10000 -> 5 -> 2.5 -> 2 -> 99+1 -> 100100
f = function(range){
  r = 0;
  i = 10^(max(ceiling(nchar(range[1]) / 2)-1,0))
  while(comp.gt(range[1], paste0(i,i,""))) i = i + 1
  j = 10^(max(floor(nchar(range[2]) / 2),0))
  while(comp.gt(paste0(j,j,""),range[2])) j = j - 1
  if(j < i) return(0)
  sum(as.numeric(paste0(i:j,i:j,"")))
}
y = vapply(x,f,0)
y
```

## Part 2

The function f above can be modified to divide the number length by n and repeat each part n times. But it is also possible to just figure out what repeated number is the next/previous compared to a given full number, which makes it faster.

``` R
library(dplyr)
options(scipen = 999)

x = readr::read_delim("input.txt",",",col_types = "c",col_names = FALSE) %>%
  stringr::str_split("-")

pasten = function(x,n) paste0(rep(x,n),collapse="")
nextn = function(x,n,larger=TRUE){
  xlen = nchar(x);
  if(xlen %% n==0){
    x1 = substring(x,1,xlen/n)
    if(larger){
      if(pasten(x1,n) < x) x1 = as.numeric(x1)+1
    }else{
      if(pasten(x1,n) > x) x1 = as.numeric(x1)-1
    }
    x1
  }else{
    r = 10^(max(ceiling(xlen / n)-1,0))
    if(larger) r else r-1
  }
}
nextn(258375372,3) # first possible 3 times repeated integer >= x
nextn(25837537,3,FALSE) # first possible 3-times repeated integer <= x

f = function(range,n){
  i = nextn(range[1],n,TRUE)
  j = nextn(range[2],n,FALSE)
  if(j < i) return(0)
  as.numeric(vapply(i:j,function(x) pasten(x,n),""))
}
sum(vapply(x,function(xi){
  print(xi)
  r = c()
  for(n in 2:nchar(xi[2])) r = union(r, f(xi,n))
  sum(r)
},0))
```

I guess that the sum of 11+22+...+99 has a closed form solution. Maybe that is a faster way to solve this?

$$
1 + 2 + ... + 9 = 45
$$

$$
11+22+...+99 = 45 + 10 * 45 = 495
$$

$$
10+11+...+99 = 4905
$$

$$
1010+1111+...+9999 = 4905 * 100 + 4905 = 495405
$$


