library(dplyr)

M = readr::read_delim("example_grid.txt", " ",col_types="i",col_names = FALSE) %>%
  as.matrix()

M = cbind(M,0)

M0 = M
total = 0
while(any(M > 0)){
  total = total + (M)[11,2]
  M = M %*% M0
}
print(total)



library(dplyr)
x = readr::read_lines("input.txt")
x.rows = strsplit(x, ": ")
rnames = c(vapply(x.rows,function(x) x[1],""),"out")
M = matrix(0,ncol=length(x)+1,nrow=length(x)+1,dimnames=list(rnames, rnames))

for(i in 1:length(x.rows)){
  cname = x.rows[[i]][1]
  rnames = strsplit(x.rows[[i]][2]," ")[[1]]
  M[rnames,cname] = 1
}

M0 = M
ways = 0
while(any(M>0)){
  ways = ways + M["out","you"]
  M = M %*% M0
}
print(ways)




# Part 2
library(dplyr)
x = readr::read_lines("input.txt")
x.rows = strsplit(x, ": ")
rnames = c(vapply(x.rows,function(x) x[1],""),"out")
Mi = matrix(0,ncol=length(x)+1,nrow=length(x)+1,dimnames=list(rnames, rnames))

for(i in 1:length(x.rows)){
  cname = x.rows[[i]][1]
  rnames = strsplit(x.rows[[i]][2]," ")[[1]]
  Mi[rnames,cname] = 1
}

# Assume we must pass through each of fft and dac exactly once
# Try solving from dac and fft, and combining with solutions to dac and fft

paths = function(M, from, to){
  M0 = M
  ways = 0
  while(any(M>0)){
    ways = ways + M[to,from]
    M = M %*% Mi
  }
  ways
}

# Solutions to fft -> dac -> out
M = Mi;
v1 = paths(M,"svr","fft")
M["fft",] = 0
v2 = paths(M,"fft","dac")
M["dac",] = 0
v3 = paths(M,"dac","out")
ways1 = v1*v2*v3

# Solutions to dac -> fft -> out
M = Mi
v1 = paths(M,"svr","dac")
M["dac",] = 0
v2 = paths(M,"dac","fft")
M["fft",] = 0
v3 = paths(M,"fft","out")
ways2 = v1*v2*v3

message(ways1 + ways2)
