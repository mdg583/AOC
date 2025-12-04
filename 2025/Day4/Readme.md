# Day 4

## Part 1

This seemed like it could be done with a kernel smoothing function, but I couldn't figure out a way to do it in stock R, and I didn't want to implement a 2d convolution. So I decided to use one from a package. gsignal had one which is exactly what was needed.

``` R
library(dplyr)
# install.packages("gsignal")
lines = readr::read_lines("input.txt")
rolls = lapply(lines,function(x){
  1*(stringr::str_split_1(x,"") == "@")
}) %>% do.call(rbind, .)

smooth = rbind(c(1,1,1),c(1,0,1),c(1,1,1))

grid_neighbours = gsignal::conv2(rolls,smooth,shape="same")
sum(rolls & grid_neighbours < 4)
```

## Part 2

Made a function to indicate which rolls can be removed, repeated finding the rolls to be removed and removing them

``` R
library(dplyr)
# install.packages("gsignal")
lines = readr::read_lines("input.txt")
rolls = lapply(lines,function(x){
  1*(stringr::str_split_1(x,"") == "@")
}) %>% do.call(rbind, .)

smooth = rbind(c(1,1,1),c(1,0,1),c(1,1,1))
can_remove = function(x){
  xn = gsignal::conv2(x,smooth,shape="same")
  x & (xn < 4)
}

removed = 0
to_remove = 0
while(TRUE){
  to_remove = can_remove(rolls)
  if(sum(to_remove) == 0) break;
  removed = removed + sum(to_remove)
  rolls = rolls - to_remove
}
removed
```

