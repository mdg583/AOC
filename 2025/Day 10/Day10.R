library(dplyr)

x = readr::read_lines("example.txt")
g = stringr::str_match(x,"(\\(.*\\)) \\{(.*)\\}")
button_string = g[,2]
joltage_string = g[,3]
# Get buttons
each_button = stringr::str_match_all(button_string, "(?<=\\()[0-9,]+(?=\\))")
each_button = lapply(each_button, function(x){
  lapply(strsplit(x,","), as.integer)
})
# Get joltages
joltages = lapply(strsplit(joltage_string,","), as.integer)

total = 0
for(i in 1:length(joltages)){
  j = joltages[[i]]
  b = each_button[[i]]
  # num rows = length joltages
  # num cols = num buttons
  M = matrix(0, nrow = length(j), ncol = length(b))
  for(k in 1:length(b)){
    M[b[[k]]+1,k] = 1
  }
  # M
  # number of each button press is a vector of length 6
  # v1 = c(1,3,5,3,1,2)
  # M %*% v1 # This is a hypothetical joltage
  # This is a system of 4 equations in 6 variables
  # e + f = 3
  # b + f = 5
  # c + d + e = 4
  # a + b + d = 7
  # a,b,c,d,e,f >= 0
  # A bit like cheating...
  # Minimize a + b + c + d + e + f
  # Subject to:
  #   a...f >= 0
  #   equations above
  res = lpSolve::lp(
    direction = "min",
    objective.in = rep(1, length(b)),
    const.mat = M,
    const.dir = rep("=", length(j)),
    const.rhs = j,
    all.int = TRUE
  )
  total = total + sum(res$solution)
}
print(total)
