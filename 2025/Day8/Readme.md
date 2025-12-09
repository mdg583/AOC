# Day 8
## Part 1

This one was harder, but I got it on the second day.

The first part was not too bad. `combn` is a useful way to get all pairs of items in a list, and a simple euclidean
distance (without the square root) can be used to order the junction boxes by pairwise distances. In the end, `pts`
is the sorted pairs of boxes, starting with the ones closest together.

``` R
library(dplyr)

N = 1000 # How many conencted?
M = 3  # Largest M circuits multiplied

# Read the data
dx = readr::read_csv("input.txt",col_names=FALSE,col_types="n")
# Generate pairs of points and calculate the distances
point_pairs = t(combn(1:nrow(dx),2))
pair_distances = apply((dx[point_pairs[,1],] - dx[point_pairs[,2],])^2,1,sum)
# Get the first 10 pairs in terms of distance
pair_order = order(pair_distances)
pts = point_pairs[pair_order[1:N],]
# What are the sizes of the 3 largest circuits?
```

The second part was hard. I was trying to find a trick to avoid making a complicated loop. When I switched to trying
to implement a 'normal' approach, I was getting tripped up by the idea that every new pair of points doesn't just result
in items being added to one existing circuit or another, but instead it results in existing circuits being merged into one.

Eventually this came together using a list of vectors.

```R
circuits = as.list(unique(as.vector(pts)))
for(i in 1:nrow(pts)){
  pti = pts[i,]
  to.join = which(vapply(1:length(circuits), function(i) any(pti %in% circuits[[i]]), FALSE))
  if(length(to.join) > 1){
    circuits[[to.join[1]]] = c(circuits[[to.join[1]]], circuits[[to.join[2]]])
    circuits = circuits[-to.join[2]]
  }
}
prod(sort(vapply(circuits,length,0),decreasing = TRUE)[1:3])
```

## Part 2

Once part 1 was solved, part 2 was easy. I remove N and M, since we need all the points, and we no longer care about the largest M circuits. Then instead of a fixed-size for loop, just loop until there is only one circuit left.

``` R
library(dplyr)
# Read the data
dx = readr::read_csv("input.txt",col_names=FALSE,col_types="n")
# Generate pairs of points and calculate the distances
point_pairs = t(combn(1:nrow(dx),2))
pair_distances = apply((dx[point_pairs[,1],] - dx[point_pairs[,2],])^2,1,sum)
# Get the first 10 pairs in terms of distance
pair_order = order(pair_distances)
pts = point_pairs[pair_order,]

# Same strategy as above, but continue until there is only one circuit
circuits = as.list(unique(as.vector(pts)))
i = 1
while(i <= nrow(pts) && length(circuits) > 1){
  pti = pts[i,]
  to.join = which(vapply(1:length(circuits), function(i) any(pti %in% circuits[[i]]), FALSE))
  if(length(to.join) > 1){
    circuits[[to.join[1]]] = c(circuits[[to.join[1]]], circuits[[to.join[2]]])
    circuits = circuits[-to.join[2]]
  }
  i = i + 1
}
prod(dx[pti,"X1"])
```
