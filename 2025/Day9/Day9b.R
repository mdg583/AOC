library(dplyr)
x = readr::read_csv("input.txt",col_names=FALSE, col_types="i") %>% as.matrix()
xdiff = abs(outer(x[,1],x[,1],"-"))+1
ydiff = abs(outer(x[,2],x[,2],"-"))+1
max(xdiff*ydiff)

# Part 2: wow much more difficult
# Walk the boundary. Each time you turn "right" add 1.
# Each time you turn "left" subtract 1. See if you end up at -4 or +4.
# This tells you if "inside" is on your left or your right

library(dplyr)
map = readr::read_csv("input.txt",col_names=FALSE, col_types="i") %>% as.matrix()
map = map + 1
# Make a flood-fill
w = max(map[,1])+1
h = max(map[,2])+1
mmap = matrix(0,ncol=w,nrow=h)
map2 = rbind(map,map[1,])
for(i in 1:(nrow(map2)-1)){
  y = map2[i,2]:map2[i+1,2]
  x = map2[i,1]:map2[i+1,1]
  mmap[y,x] = 1
}
mmap

# Flood from 1,1
flood = function(x,y){
  message(paste0(x,",",y))
  mmap[y,x] <<- 2
  if(x > 1 && mmap[y,x-1] == 0) flood(x-1,y)
  if(x < w && mmap[y,x+1] == 0) flood(x+1,y)
  if(y > 1 && mmap[y-1,x] == 0) flood(x,y-1)
  if(y < h && mmap[y+1,x] == 0) flood(x,y+1)
}
flood(1,1)
mmap[mmap==0] = 1
mmap[mmap==2] = 0
mmap

# Check each box
i.comb = t(combn(1:nrow(map),2))
xdiff = abs(outer(map[,1],map[,1],"-"))
ydiff = abs(outer(map[,2],map[,2],"-"))
areas = (xdiff+1) * (ydiff+1)
i.order = order(areas[i.comb],decreasing = TRUE)
areas[i.comb[i.order,]]

# These are the order of combinations to check.
# As soon as I find one inside red/green tiles, we are done

mmap2 = mmap;
for(i in i.order){
  j = i.comb[i,1]; k = i.comb[i,2]
  x1 = map[j,1]; x2 = map[k,1]
  y1 = map[j,2]; y2 = map[k,2]
  a = (abs(x2-x1)+1)*(abs(y2-y1)+1)
  message(paste0(i,": (",x1,",",y1,"),(",x2,",",y2,") : ", a))
  mmap2[,] = 0
  for(x in x1:x2){
    mmap2[y1:y2,x] = 1
  }
  if(all((mmap & mmap2) == mmap2)){
    message(paste0("Area: ", a))
    break;
  }
}
