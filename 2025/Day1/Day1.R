install.packages("readr")

# 0 - 99
# L45 R16
# R = +, L = -
# Starts at 50


# x = readr::read_lines("example.txt")
x = readr::read_lines("input.txt")
xdir = substring(x,1,1)
xlen = as.numeric(substring(x,2))
sum((50+cumsum(xlen*ifelse(xdir=="L",-1,1))) %% 100 == 0)

# 0x434C49434B

x = readr::read_lines("input.txt")
xdir = substring(x,1,1)
xlen = as.numeric(substring(x,2))
xcum = cumsum(c(50,xlen*ifelse(xdir=="L",-1,1)))
0.5 * sum(
  abs(diff(floor(xcum / 100))) + 
  abs(diff(floor((xcum-1) / 100)))
)
