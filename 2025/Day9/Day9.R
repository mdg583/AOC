library(dplyr)
x = readr::read_csv("example.txt",col_names=FALSE, col_types="i") %>% as.matrix()
xdiff = outer(x[,1],x[,1],"-")+1
ydiff = outer(x[,2],x[,2],"-")+1
max(abs(xdiff*ydiff))

# Part 2: wow much more difficult
# Walk the boundary. Each time you turn "right" add 1.
# Each time you turn "left" subtract 1. See if you end up at -4 or +4.
# This tells you if "inside" is on your left or your right

library(dplyr)
map = readr::read_csv("input.txt",col_names=FALSE, col_types="i") %>% as.matrix()
shifty = function(x,i=1){
  if(i>0){
    rbind(x[(nrow(x)-i+1):nrow(x),],x[1:(nrow(x)-i),])
  }else{
    i = -i; rbind(x[(1+i):nrow(x),],x[1:i,])
  }
}
motions = cbind(map-shifty(map,1))
motions.ext = rbind(motions,motions[1,])
dirs = vapply(1:nrow(motions), function(i){
  2*(det(motions.ext[i:(i+1),]) > 1) - 1
},0)
# if the sum of dirs is 4, inside is on the right
# if the sum of dirs is -4, inside is on the left
sum(dirs)
inside.dir = 2*(sum(dirs) > 0) - 1 # Is "inside" on the left or right?

# As we walk the boundary, the movement from
# the previous loc to the new loc will either:
# - both be on a boundary (fine)
# - go from off a boundary to on a boundary (fine)
# - go from on a boundary to off a boundary (check)

# When we go from on a boundary to off a boundary, we need to ask:
# - did we travel vertically or horizontally? This determines whether to look
#   at horizontal boundaries or vertical.
# - which direction was this boundary going, and is inside on the left or right?
#   This tells us if the new location is "inside" or "outside"
outside = function(z,mul){
  z = diff(z)
  z = rep(2*(z[seq(1,length(z),2)] > 0) - 1,each=2)
  z*inside.dir*mul
}

if(map[1,1] == map[2,1]) {
  # first boundary is vertical
  v_bnds = map
  h_bnds = rbind(map[2:nrow(map),],map[1,])
}else{
  v_bnds = rbind(map[2:nrow(map),],map[1,])
  h_bnds = map
}
v_side = outside(v_bnds[,2],1)
h_side = outside(h_bnds[,1],-1)

# For each horizontal boundary, is 'inside' up (-1) or down (+1)?
h_bnds
h_side
# For each vertical boundary, is 'inside' left (-1) or right (+1)?
v_bnds
v_side

# Which horizontal boundary is this on?
h.bnd = function(x,y){
  cnrs = which(h_bnds[,2] == y)
  if(length(cnrs)>0){
    Filter(
      function(i) {
        xs = h_bnds[i:(i+1),1]
        (x >= min(xs)) && (x <= max(xs))
      },
      cnrs[seq(1,length(cnrs),2)]
    )
  }else{
    return(as.integer(c()))
  }
}

# Which vertical boundary is this on?
v.bnd = function(x,y){
  cnrs = which(v_bnds[,1] == x)
  if(length(cnrs)>0){
    Filter(
      function(i) {
        ys = v_bnds[i:(i+1),2]
        (y >= min(ys)) && (y <= max(ys))
      },
      cnrs[seq(1,length(cnrs),2)]
    )
  }else{
    return(as.integer(c()))
  }
}
h_bnds
h.bnd(8,7)
h.bnd(9,7)
h.bnd(10,7)
h.bnd(11,7)
h.bnd(12,7)
h_side[3] # outside is up

v_bnds
v.bnd(9,4)
v.bnd(9,5)
v.bnd(9,6)
v.bnd(9,7)
v.bnd(9,8)
v_side[3] # outside is left

# Now walk the boundary. Keep track if the previous loc was on a hor and ver boundary.
# If we go from either to none, note which one changed (only one can), and check if
# we just stepped outside or inside

area = function(i,j){
  x = map[i,1]
  y = map[i,2]
  x.end = map[j,1]
  y.end = map[j,2]
  # message(sprintf("Area from (%s,%s) to (%s,%s) is %s", x,y,x.end,y.end,(abs(x.end - x)+1)*(abs(y.end - y)+1)))
  (abs(x.end - x)+1)*(abs(y.end - y)+1)
}

boundary = function(i,j){
  x = map[i,1]
  y = map[i,2]
  x.end = map[j,1]
  y.end = map[j,2]
  if(x > x.end){ t=x;x=x.end;x.end=t; }
  if(y > y.end){ t=y;y=y.end;y.end=t; }
  rbind(
    cbind(x:(x.end-1),y),
    cbind(x.end,y:(y.end-1)),
    cbind(x.end:(x+1),y.end),
    cbind(x,y.end:(y+1))
  )
}

print.map = function(pnts,w,h){
  mp = matrix(0,nrow=w,ncol=h)
  mp[pnts] = 1
  lapply(1:h,function(i){
    paste0(ifelse(mp[,i],'X','.'),collapse="")
  }) %>% paste0(collapse="\n") %>% message()
}

boundary_inside = function(bnd){
  bnd2 = rbind(bnd,bnd[1,])
  h.bnds = h.bnd(bnd2[1,1],bnd2[1,2])
  v.bnds = v.bnd(bnd2[1,1],bnd2[1,2])
  was.bnd = length(h.bnds) > 0 || length(v.bnds) > 0
  for(k in 2:nrow(bnd2)){
    h.bnds2 = h.bnd(bnd2[k,1],bnd2[k,2])
    v.bnds2 = v.bnd(bnd2[k,1],bnd2[k,2])
    is.bnd = length(h.bnds2) > 0 || length(v.bnds2) > 0
    if(was.bnd && !is.bnd){
      wx = bnd2[k,1] - bnd2[k-1,1]
      wy = bnd2[k,2] - bnd2[k-1,2]
      # message(sprintf("Stepping at %s: (%s,%s) towards (%s,%s)", k, bnd2[k-1,1],bnd2[k-1,2],wx,wy))
      if(wx != 0){
        # Stepped off a vertical boundary
        if(wx == v_side[v.bnds]) return(FALSE)
      }else{
        # Stepped off a horizontal boundary
        if(wy == h_side[h.bnds]) return(FALSE)
      }
    }
    was.bnd = is.bnd
    h.bnds = h.bnds2
    v.bnds = v.bnds2
  }
  return(TRUE)
}

# bnd = boundary(1,4)
# print.map(bnd,11,7)
# print.map(map,11,7)
# boundary_inside(bnd)

rect.size = function(i,j){
  bnd = boundary(i,j)
  if(abs(i-j) == 1 || boundary_inside(bnd)){
    area(i,j)
  }else{
    0
  }
}

# Check all pairs of vectors, not adjacent
rsize = 0
i.pairs = t(combn(1:nrow(map),2))
for(pi in 1:nrow(i.pairs)){
  this.size = rect.size(i.pairs[pi,1],i.pairs[pi,2])
  message(sprintf("%s: %s,%s = %s", pi, i.pairs[pi,1], i.pairs[pi,2], this.size))
  if(this.size > rsize) rsize = this.size
}
message(rsize)
