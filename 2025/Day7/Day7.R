# Part 1
library(dplyr)
x = readr::read_lines("example.txt")
grid = lapply(x,function(xi){
  stringr::str_split_1(xi,"") %>% case_match(
    "." ~  0,
    "S" ~  1,
    "^" ~  2
  )
}) %>% do.call(rbind,.)

rshift = function(x) c(0,x[1:length(x)-1])
lshift = function(x) c(x[2:length(x)],0)

splits = 0
r1 = grid[1,]
for(i in 2:nrow(grid)){
  r2 = grid[i,]
  flow = (r2==0 & r1==1)
  splits = splits + sum(r2==2 & r1==1)
  split.right = (r2==0 & lshift(r1)==1 & lshift(r2)==2)
  split.left  = (r2==0 & rshift(r1)==1 & rshift(r2)==2)
  r1 = r2 + (flow | split.right | split.left)
}
splits


# Part 2
library(dplyr)
x = readr::read_lines("example.txt")
grid = lapply(x,function(xi){
  stringr::str_split_1(xi,"") %>% case_match(
    "." ~  0, "S" ~  1, "^" ~  2
  )
}) %>% do.call(rbind,.)

rshift = function(x) c(0,x[1:length(x)-1])
lshift = function(x) c(x[2:length(x)],0)

splits = 0
r1 = 1*(grid[1,]==1)
for(i in 2:nrow(grid)){
  g2 = grid[i,]
  flow = (g2==0) * r1
  split.right = (g2==0) * (lshift(g2)==2) * lshift(r1)
  split.left  = (g2==0) * (rshift(g2)==2) * rshift(r1)
  r1 = flow + split.right + split.left # number beams in diff universes
}
paste(sum(r1))
