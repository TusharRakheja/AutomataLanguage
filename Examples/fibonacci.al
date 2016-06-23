declare abstract map fib									# Declare the fib abstract map.
under fib : x --> (((x) == 0) V ((x) == 1)) ? 1 : ((fib[(x) - 1]) + (fib[(x) - 2]))		# Mapping scheme `if x is 0 or 1 then 0 else fib[x - 1] + fib[x - 2]`
print fib[10]											# Print the 10th Fibonacci number (89).