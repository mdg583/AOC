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

removed = 0
to_remove = 0
while(TRUE){
  to_remove = can_remove(rolls)
  if(sum(to_remove) == 0) break;
  removed = removed + sum(to_remove)
  rolls = rolls - to_remove
}
removed
