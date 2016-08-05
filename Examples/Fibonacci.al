declare abstract map fib									# Declare the fib abstract map.
under fib : n -> (((n) == 0) V ((n) == 1)) ? 1 : ((fib[(n) - 1]) + (fib[(n) - 2]))	# Mapping scheme `if n is 0 or 1 then 0 else fib[n - 1] + fib[n - 2]`
print fib[10]										# Print the 10th Fibonacci number (89).
quit											# Done.
