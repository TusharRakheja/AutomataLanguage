# Autolang [![Build Status](https://travis-ci.org/TusharRakheja/Autolang.svg?branch=master)](https://travis-ci.org/TusharRakheja/Autolang)

**Autolang** is a procedural, interpreted programming language.

It supports primitive data types (`int`, `char`, and `logical`), containers (`sets`, `tuples`, and `maps`), and `strings`.

### Building

**Dependencies**

 - `gcc` 4.8 or later
 - GNU `make`

Clone (or alternatively download) the repo.
```
$ git clone https://github.com/TusharRakheja/Autolang
```
Then navigate into the directory and open the Makefile. Set `CXX=g++`. Depending on your version of `gcc`, you may also need to edit the `-std` flag. Once done, run make.
```
$ cd Autolang
$ make
$ make clean
```

### Usage

Autolang can be used either with a file, or interactively. The filename argument is optional.

`$ auto filename.al`

### Examples

The real joy of Autolang is its very math-oriented syntax. Here are some cool examples you can try. 

##### 1.
```perl
>>> set A = {1, 2, 3} x {'A', 'B'}      		     # Cartesian Product
>>> print A
{(1, 'A'), (1, 'B'), (2, 'A'), (2, 'B'), (3, 'A'), (3, 'B')} 
```

##### 2. 
```perl
>>> print {(1, 'B')} c A                		     # Is this set a subset of A?
True
```

##### 3.
```perl
>>> print (2 + 10 * 2) == (2 * 10 + 2)  		     # Beware, no operator precedence!
False
```

##### 4.
```perl
>>> string first = "Tushar"
>>> string last = "Rakheja"
>>> tuple entry = (first, last)         		     # Identifiers can be used as elements.
>>> print entry
("Tushar", "Rakheja")
```

##### 5.
```perl
>>> map f : {"One", "Zero"} --> {True, False}		 # Declare a map with a given domain and codomain.
>>> under f : "One"  --> True					
>>> under f : "Zero" --> False				         # `Under f, "Zero" goes to False.`
>>> print f["One"]						
True
>>> print f["Zero"]					                 # Maps can be queried this way.
False
```

##### 6.
```perl
>>> map g : {1, 0} --> {"One", "Zero"}				
>>> under g : 1 --> "One"
>>> under g : 0 --> "Zero"
>>> declare map fog					                 # Declare a map fog and let it equal ...
>>> let fog = f o g					                 # ...  the composition of maps f and g.
>>> print fog
{1 --> True, 0 --> False}				             # One can also print a map this way.
```

To see an example of how the while loop (the only looping construct in the language, right now) and some other things in the language work, check `Examples/example6.al` out. A technical guide is coming soon.

### Future Plans/Ideas

 * [ ] Lexical scoping.
 
 * [ ] Casting.
 
 * [ ] Automatic memory management.

 * [ ] Support for graphics, modularity etc.
 
 * [ ] Abstract Maps/Lambda Expressions (Mapping criteria instead of fixed mappings, eg x --> x + 2)

 * [ ] Abstract Sets (Membership criteria instead of fixed members). Probably can be simulated via Abstract Maps.
 

### License

Copyright (c) 2016 Tushar Rakheja (The MIT License)
