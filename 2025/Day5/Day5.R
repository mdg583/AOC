# Part 1
library(dplyr)
lines = readr::read_lines("input.txt")
i = which(lines=="")
ranges = lines[1:(i-1)] %>%
  stringr::str_split_fixed("-",2) %>%
  as.numeric() %>% matrix(ncol=2)
items = lines[(i+1):length(lines)] %>%
  as.numeric()
sum(apply(outer(items,ranges[,1],`>=`) & outer(items,ranges[,2],`<=`),1,any))

# Part 2
library(dplyr)
lines = readr::read_lines("input.txt")
i = which(lines=="")
ranges = lines[1:(i-1)] %>%
  stringr::str_split_fixed("-",2) %>%
  as.numeric() %>% matrix(ncol=2)

# Sort on range start, and then on range end for ranges with the same start
ranges = ranges[order(ranges[,2]),]
ranges = ranges[order(ranges[,1]),]
# a-b
# c-d

# We know:
# a <= b, c <= d, a <= c
# if a = c, then b <= d
# 
# If we just counted the ranges, we would over-count because they can overlap
# It could be that:
# c < b and d > b
# c < b and d < b

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