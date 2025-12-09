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

# This is much harder
# Strategy:
# - Start with n different 'circuits' of one item each, and no connections
# - for each new connection, find the circuits of the two points
# - combine those circuits unless they are already combined

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
