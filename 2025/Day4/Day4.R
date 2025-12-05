library(dplyr)
# install.packages("gsignal")

lines = readr::read_lines("input.txt")

rolls = lapply(lines,function(x){
  1*(stringr::str_split_1(x,"") == "@")
}) %>% do.call(rbind, .)

smooth = rbind(c(1,1,1),c(1,0,1),c(1,1,1))

grid_neighbours = gsignal::conv2(rolls,smooth,shape="same")
sum(rolls & grid_neighbours < 4)


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

system.time({  
  removed = 0
  to_remove = 0
  while(TRUE){
    to_remove = can_remove(rolls)
    if(sum(to_remove) == 0) break;
    removed = removed + sum(to_remove)
    rolls = rolls - to_remove
  }
  removed
})




count_neighbors <- function(rolls) {
  rows <- nrow(rolls)
  cols <- ncol(rolls)
  
  neighbors <- matrix(0, rows, cols)
  
  # Sum neighbors from all 8 directions
  for(dr in -1:1) {
    for(dc in -1:1) {
      if(dr == 0 && dc == 0) next
      
      r_idx <- row(rolls) + dr
      c_idx <- col(rolls) + dc
      r_i = cbind(r_idx, c_idx)
      
      neighbors <- neighbors + rolls[cbind(r_idx, c_idx)]
    }
  }
  
  sum(rolls == 1 & neighbors < 4)
}

count_neighbors(rolls)
