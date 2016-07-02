#ifndef INT_H
#define INT_H

#include "Elem.h"
#include <iostream>
using std::cout;
using std::endl;

class Int : public Elem
{
public:
	int elem;						// Only going to implement the bare-minimum methods.
	Int() : Elem(INT) { elem = 0; }				// Default constructor.
	Int(int x) : Elem(INT) { elem = x; }			// Parameterized constructor.
	Int(string x) : Elem(INT) { elem = std::stoi(x); } 	// Construct an int object using a string representation of it.
		
	shared_ptr<Elem> deep_copy()				// Return the address of a new Int object constructed with the value of this.
	{ 
		return shared_ptr<Elem>{new Int(elem)}; 
	} 
	bool operator==(Elem &x)
	{
		if (x.type != INT) return false;		// Check for types first and foremost.
		Int *_x = (Int *)&x;
		return (_x->elem == this->elem);
	}	
	bool operator<(Elem &x)					// Compares two ints.
	{
		Int *_x = (Int *)&x;
		return this->elem < _x->elem;
	}
	bool operator<=(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem <= ((Int *)&elem)->elem;
	}
	bool operator>(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem > ((Int *)&elem)->elem;
	}
	bool operator>=(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem >= ((Int *)&elem)->elem;
	}
	string to_string() 					// Return a string constructed using the integer elem cast to a character array.
	{
		return std::to_string(elem);			// Use C++'s own to_string function.
	} 
	~Int() { }						// Empty destructor.
};

#define integer static_pointer_cast<Int>

#endif