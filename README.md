# Autolang [![Build Status](https://travis-ci.org/TusharRakheja/Autolang.svg?branch=master)](https://travis-ci.org/TusharRakheja/Autolang)

**Autolang** is a high-level programming language supporting multiple paradigms, with syntax rooted in mathematical notation.

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

`$ ./auto filename.al`

### Examples

The real joy of Autolang is its very math-oriented syntax. Here are some cool examples you can try. 

 * [Sets](https://github.com/TusharRakheja/Autolang#1-sets)
 * [Tuples](https://github.com/TusharRakheja/Autolang#2-tuples)
 * [Abstract Sets](https://github.com/TusharRakheja/Autolang#3-abstract-sets)
 * [Maps](https://github.com/TusharRakheja/Autolang#4-maps)
 * [Abstract Maps (&lambda;)](https://github.com/TusharRakheja/Autolang#5-abstract-maps-λ)
 * [Notes](https://github.com/TusharRakheja/Autolang#6-notes)



##### 1. Sets

The key data structure in Autolang is a  **set** - a (possibly heterogeneous) collection of elements. Most set operations are supported. Here are a few examples.

**Basic Operations**
```perl
>>> print {1, 2} U {1, 3}                                          # Union of two sets.
{1, 2, 3}
>>> print {1, 2} & {1, 3}                                          # Intersection.
{1}
>>> print {1, 2} \ {1, 3}                                          # Exclusion.
{2}
```

**Advanced Operations** 

```perl
>>> set A = {1, 2, 3} x {'A', 'B'}      		                   # Cartesian Product
>>> print A
{(1, 'A'), (1, 'B'), (2, 'A'), (2, 'B'), (3, 'A'), (3, 'B')} 
>>> print |A|                                                      # Print the cardinality of set A.
6
```

**Subset Query**
```perl
>>> print {(1, 'B')} c A                		                   # Is this set a subset of A?
True
```
In addition to the standard set operations, it is possible to access a member of a set at a specific position, using the **`[]`** operator (may also be used with *tuples*).

**Access Query**
```perl
>>> print A[1]                          		                   # Access the element of A at index 1.
{(1, 'B')}
>>> print A[(1, 3)]                                                # Access the subset of A between [1, 3).
{(1, 'B'), (2, 'A')}
```

The **`in`** operator returns a *logical* value if the left argument is present in the right argument. Just like the access query operator, it may also be used with tuples.

**Membership Query**
```perl
>>> print ('2' in {1, {'2'}, 3}[1])                  # The 'in' and '[]' operators in action.
True
```

##### 2. Tuples

Much like sets in that they are containers too, **tuples** are *immutable*, i.e., the contents of a tuple object cannot be change. The interface for working with tuples, as a result, is quite limited, but they useful wherever a general ordered collection is needed.

```perl
>>> tuple A = (1, )                                  # A single-element tuple needs a trailing comma.
>>> print A[0]
1
```

```perl
>>> string given = "Tushar"
>>> string family = "Rakheja"
>>> tuple entry = (given, family)         		     # Identifiers can be used as elements.
>>> print entry
("Tushar", "Rakheja")
```

##### 3. Abstract Sets
One very powerful concept Autolang supports is that of an **abstract set**. Unlike a normal set, an *abstract* set does not have fixed members, but rather, a membership criteria. 

The criteria is supposed to be a *logical* expression, which is evaluated for every query on the set when needed. One must use the **`elem`** *keyword* as a placeholder for the incoming query. 

If you get creative, there's a lot that suddenly became possible. For instance.
```perl
>>> set A = {1, 2, 3} 		                         # Autolang also supports the notion of 'abstract' sets.
>>> abstract set PowA = { elem | elem c A }          # Define the PowA as the power set of A.
>>> print ({1, 3} in PowA)
True
>>> print ({1, 4} in PowA)
False
```

Most set operations will also work with abstract sets, except the [] operator (since an abstract set can be potentially uncountably infinite in size). However, an operation between an abstract set and a normal set is __not__ possible (for now), except a subset operation (which, (un)interestingly, cannot be performed on two abstract sets).

**Abstract Set Operations**
```perl
>>> declare abstract set Inter
>>> abstract set EvenP = { elem | (((elem[0]) * (elem[1])) % 2) == 0 } 
>>> let Inter = PowA & EvenP
>>> print {1, 2} in Inter
True
>>> print {1, 3} in Inter
False
>>> print {{1, 2}, {2, 3}} c Inter                  # A subset op between a normal and an abstract set.
True
```
Of course, one can make new abstract sets by taking unions, exclusions, and cartesian products of two abstract sets as well.

##### 4. Maps

Containers that store mappings between two sets of elements are called **maps**. Their syntax is inspired by the mathematical definition of a function.

```perl
>>> map f : {"One", "Zero"} --> {True, False}		 # Initialize a map with a given domain and codomain.
>>> under f : "One"  --> True					
>>> under f : "Zero" --> False				         # `Under f, "Zero" goes to False.`
>>> print f["One"]						
True
>>> print f["Zero"]					                 # Maps can be queried this way.
False
```

Just like regular mathematical functions, maps can be composed with each other, given that the domains and ranges of the arguments match appropriately.

```perl
>>> map g : {1, 0} --> {"One", "Zero"}				
>>> under g : 1 --> "One"
>>> under g : 0 --> "Zero"
>>> declare map fog					                 # Declare a map fog and let it equal ...
>>> let fog = f o g					                 # ...  the composition of maps f and g.
>>> print fog
{(1, True), (0, False)}				                 # One can also print a map this way.
```
Maps can also be used to implement **associative arrays**. See the `Examples/` directory for an example.

##### 5. Abstract Maps (&lambda;)

Autolang supports a certain *flavor* of lambda expressions in the form of **abstract maps**.

**Factorial**
```perl
>>> declare abstract map fact                       # An abstract map to compute the factorial.
>>> under fact : x --> ((x) == 0) ? (1) : ((x) * fact[(x) - 1])
>>> print fact[5]
120
```

We used a *conditional operator* in the `fact` map, which is the only ternary operator in Autolang. Abstract maps can be composed too, but for now, they don't have domains and codomains restricting their input, so any composition between two abstract maps is possible. However, a composition between an abstract and a normal map is not possible (again, for now).

Again, like abstract sets, abstract maps work by replacing a certain keyword with the value of the query. This *keyword*, which is more like a key-expression, is in the mapping scheme (the part to the right of the mapping operator `-->`), exclusively represented by **`(x)`**. This combination of these characters, `(x)`, does not occur in Autolang anywhere else.

To see an example of how the while loop (the only looping construct in the language, right now) and some other things in the language work, check `Examples/example7.al` out. A language specification is coming soon.

##### 6. Notes

Operators in Autolang are *always* right-associative by default. There is no concept of operator precedence currently. Hence, ***parentheses are important*** to get the results you need.

More generally, a thing to keep in mind is that though Autolang tries to approximate mathematical notation wherever possible, certain expressions *may* be tricky. For instance:

```perl
>>> set P = {1, 2} x {'a', 'b'} x {True}    # We have tuples of the form (e, (f, g)), not (e, f, g) as expected.
>>> print P
{(1, ('a', True)), (1, ('b', True)), (2, ('a', True)), (2, ('b', True))}
>>> print |(P[0])|                                                         # Will not print 3, but 2.
2
```

Depending on your moral values and whether or not you believe there is any justice in the world, this may or may not have been what you expected. But for now, this is the result.

Autolang is imperfect. The parser is brittle, there are memory leaks etc. But perfection is the goal. And perfection, is a journey unto itself.

### Future Plans

 * [ ] Lexical scoping.
 
 * [ ] Casting.
 
 * [ ] Automatic memory management.

 * [ ] Support for graphics, modularity etc.
 
### Completed

 * [x] Abstract Maps (&lambda;).

 * [x] Abstract Sets.
 

### License

Copyright (c) 2016 Tushar Rakheja (The MIT License)
