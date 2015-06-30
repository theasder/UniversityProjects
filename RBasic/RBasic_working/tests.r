> (1.1 + 2.2) * (3.3 - 4.4) / (-5.5)
[1] 0.66
> c(1,2,c(3,4))
[1] 1 2 3 4
> c(length(c("abc", "bce")) , length(c()), length(c(1,2, FALSE)))
[1] 2 0 3
> x <- c(c(TRUE, FALSE) , c(1.1, 3.14), c("aaa", "bce", c()))
> x[c(TRUE, FALSE, TRUE)]
[1] "TRUE" "1.1"  "3.14" "bce" 
> x[1] <- 1
> x[2] <- FALSE
> x[c(3,5,8,9)] <- x[c(1,2)]
> x[x > 1] # strings are compared only
[1] "FALSE" "3.14" "FALSE" "bce"
> y <- c(NULL, 1, NULL, 0)
> y + 1.43
[1] NULL 2.43 NULL 1.43
> y == "sdgrf"
[1] FALSE FALSE
> y == "NULL"
[1] FALSE FALSE
>x<-c(TRUE,2,3,c(4,6,NULL,0,1+2,1.05/0),3.14159,-2);x+c(1,2)
2 4 4 6 7 NULL 1 5 NULL 5.14159 -1 
