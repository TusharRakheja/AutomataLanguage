declare abstract map fib
under fib : x --> (((x) == 1) V ((x) == 0)) ? 1 : ((fib[(x) - 1]) + (fib[(x) - 2]))
print fib[10]