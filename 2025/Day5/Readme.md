# Day 5

For the first part I just check if each item comes after the start of each range, and before the end, and then check for which items this is true for a least one range.

The `outer` function creates a matrix of calls the function (`>=` and `<=`) for pairs of elements in the two inputs (items and range boundaries). `apply` then checks if the condition is true for any range for each item.

``` R
library(dplyr)
lines = readr::read_lines("input.txt")
i = which(lines=="")
ranges = lines[1:(i-1)] %>%
  stringr::str_split_fixed("-",2) %>%
  as.numeric() %>% matrix(ncol=2)
items = lines[(i+1):length(lines)] %>%
  as.numeric()
sum(apply(outer(items,ranges[,1],`>=`) & outer(items,ranges[,2],`<=`),1,any))
```

# Part 2

For part two, ranges need to be combined using some logic in order to remove overlap and count items by subtracting range boundaries. I first sort by the end of the ranges, then by the start. Then think about pairs of ranges:

$$
\begin{align*}
a&\textendash b \\
c&\textendash d
\end{align*}
$$

After sorting we know that $a \le c$, and if $a=c$ then $b \le d$. Overlap can only exist if $c \le b$. In this case, either the range goes from $a$ to $d$, or the full range $c\textendash d$ is embeded in the previous range.

I deal with this by tracking the start and end of the "current" range, and only counting the items if the start of the new range is after the end of the current range. Otherwise, the end of the current range may or may not need to be adjusted to the end of the new range.

``` R
library(dplyr)
lines = readr::read_lines("input.txt")
i = which(lines=="")
ranges = lines[1:(i-1)] %>%
  stringr::str_split_fixed("-",2) %>%
  as.numeric() %>% matrix(ncol=2)
# Sort on range start, and then on range end for ranges with the same start
ranges = ranges[order(ranges[,2]),]
ranges = ranges[order(ranges[,1]),]
total = 0
open_val = ranges[1,1]
close_val = ranges[1,2]
for(i in 2:nrow(ranges)){
  # close the previous range if there is no overlap
  if(ranges[i,1] > close_val){
    total = total + close_val - open_val + 1
    open_val = ranges[i,1]
    close_val = ranges[i,2]
  }else{
    close_val = max(close_val,ranges[i,2])
  }
}
# close the last range
total = total + close_val - open_val + 1
paste0(total)
```
