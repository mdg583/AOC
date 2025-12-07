library(dplyr)

x = readr::read_lines("input.txt") %>%
  trimws() %>% stringr::str_split(" +")
nums = matrix(as.numeric(do.call(cbind, x[1:(length(x)-1)])),nrow=length(x[[1]]))
ops = x[[length(x)]]
ifelse(
  ops=="+",
  apply(nums,1,sum),
  apply(nums,1,prod)
) %>% sum() %>% paste()


library(dplyr)
x = readr::read_lines("input.txt")
ops = (x[[length(x)]] %>% trimws() %>% stringr::str_split(" +"))[[1]]
num.chars = x[1:length(x)-1] %>% lapply(
  function(xi) stringr::str_split_1(xi,"")
) %>% do.call(cbind,.)
# A blank line indicates a new op
op_i = 1; nums = c(); r = 0
for(i in 1:(nrow(num.chars))){
  if(all(num.chars[i,] == " ")){
    r = r + if(ops[op_i] == "+") { sum(nums) } else { prod(nums) }
    nums = c()
    op_i = op_i + 1
  }else{
    nums = c(nums,as.numeric(paste0(num.chars[i,],collapse="")))
  }
}
r = r + if(ops[op_i] == "+") { sum(nums) } else { prod(nums) }
paste(r)
