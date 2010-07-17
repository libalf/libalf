
stats <- read.table("bounded_stats")
boxplot(V2/1000 ~ V1, stats, range = 0, main="Angluin L*", xlab="mDFA size", ylab="t / ms")

