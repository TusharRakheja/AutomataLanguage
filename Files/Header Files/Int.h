#ifndef INT_H
#define INT_H

#include "Elem.h"
#include <iostream>

using std::cout;

#define integer static_pointer_cast<Int>

class Int : public Elem
{
public:
	int elem;						// Only going to implement the bare-minimum methods.
	Int() : Elem(INT) { elem = 0; }				// Default constructor.
	Int(int x) : Elem(INT) { elem = x; }			// Parameterized constructor.
	Int(string x) : Elem(INT) { elem = std::stoi(x); } 	// Construct an int object using a string representation of it.
		
	bool operator==(Elem &e)
	{
		Int* x = (Int *)&e;
		return x->elem == this->elem;
	}

	shared_ptr<Elem> deep_copy()				// Return the address of a new Int object constructed with the value of this.
	{ 
		return shared_ptr<Elem>{new Int(elem)}; 
	} 
	
	string to_string() 					// Return a string constructed using the integer elem cast to a character array.
	{
		return std::to_string(elem);			// Use C++'s own to_string function.
	} 
	~Int() { }						// Empty destructor.
};

#endif