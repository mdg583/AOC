library(dplyr)
x = readr::read_delim("input.txt",",",col_types = "c",col_names = FALSE) %>%
  stringr::str_split("-")

comp.gt = function(x,y) nchar(x) > nchar(y) || (nchar(x) == nchar(y) && x > y)
options(scipen = 999)
# i: 10000 -> 5 -> 2.5 -> 3 -> 100 -> 100100
# j: 10000 -> 5 -> 2.5 -> 2 -> 99+1 -> 100100
f = function(range){
  r = 0;
  i = 10^(max(ceiling(nchar(range[1]) / 2)-1,0))
  while(comp.gt(range[1], paste0(i,i,""))) i = i + 1
  j = 10^(max(floor(nchar(range[2]) / 2),0))
  while(comp.gt(paste0(j,j,""),range[2])) j = j - 1
  if(j < i) return(0)
  sum(as.numeric(paste0(i:j,i:j,"")))
}
y = vapply(x,f,0)
y

pasten = function(x,n) paste0(rep(x,n),collapse="")

f = function(range,n){
  r = 0;
  i = 10^(max(ceiling(nchar(range[1]) / n)-1,0))
  while(comp.gt(range[1], pasten(i,n))) i = i + 1
  j = 10^(max(floor(nchar(range[2]) / n),0))
  while(j > 0 && comp.gt(pasten(j,n),range[2])) j = j - 1
  if(j < i) return(0)
  as.numeric(vapply(i:j,function(x) pasten(x,n),""))
}

fi = function(xi){
  r = c()
  print(xi)
  for(n in 2:nchar(xi[2])){
    r = union(r, f(xi,n))
  }
  sum(r)
}
sum(vapply(x,fi,0))

f = function(range,n){
  r = 0;
  i = 10^(max(ceiling(nchar(range[1]) / n)-1,0))
  while(comp.gt(range[1], pasten(i,n))) i = i + 1
  j = 10^(max(floor(nchar(range[2]) / n),0))
  while(j > 0 && comp.gt(pasten(j,n),range[2])) j = j - 1
  if(j < i) return(0)
  as.numeric(vapply(i:j,function(x) pasten(x,n),""))
}



library(dplyr)
options(scipen = 999)

x = readr::read_delim("input.txt",",",col_types = "c",col_names = FALSE) %>%
  stringr::str_split("-")

pasten = function(x,n) paste0(rep(x,n),collapse="")
nextn = function(x,n,larger=TRUE){
  xlen = nchar(x);
  if(xlen %% n==0){
    x1 = substring(x,1,xlen/n)
    if(larger){
      if(pasten(x1,n) < x) x1 = as.numeric(x1)+1
    }else{
      if(pasten(x1,n) > x) x1 = as.numeric(x1)-1
    }
    x1
  }else{
    r = 10^(max(ceiling(xlen / n)-1,0))
    if(larger) r else r-1
  }
}
nextn(258375372,3) # first possible 3 times repeated integer >= x
nextn(25837537,3,FALSE) # first possible 3-times repeated integer <= x

f = function(range,n){
  i = nextn(range[1],n,TRUE)
  j = nextn(range[2],n,FALSE)
  if(j < i) return(0)
  as.numeric(vapply(i:j,function(x) pasten(x,n),""))
}
sum(vapply(x,function(xi){
  print(xi)
  r = c()
  for(n in 2:nchar(xi[2])) r = union(r, f(xi,n))
  sum(r)
},0))


library(dplyr)
options(scipen = 999)

x = readr::read_delim("input.txt",",",col_types = "c",col_names = FALSE) %>%
  stringr::str_split("-")

x

