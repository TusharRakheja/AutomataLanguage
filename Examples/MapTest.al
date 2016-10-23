map f : {1, 2, 3} -> {1, 2, 3}
under f : 1 -> 2
under f : 2 -> 3
under f : 3 -> 1

print "Map f: ", f

print "\nMap f o f: ", f o f

print "\nMap f ^ 2: ", f ^ 2

print "\nMap f o f o f: ", f o f o f

print "\nMap f ^ 3: ", f ^ 3

let f ^= 3

print "\nMap f ^= 3: ", f

quit