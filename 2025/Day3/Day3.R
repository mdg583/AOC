library(dplyr)

get_joltage = function(xi){
  r = 0; j = 1
  for(k in seq(12,1,-1)){
    i = j - 1 + which.max(xi[j:(length(xi)-k+1)])
    r = r + xi[i]*(10^(k-1))
    j = i + 1
  }
  r
}

x = readr::read_lines("input.txt") %>% 
  lapply(function(a) as.numeric(stringr::str_split_1(a,"")))
y = vapply(x, get_joltage,0)
y
sum(y)
paste0(sum(y))
