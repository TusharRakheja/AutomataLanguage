#ifndef LOGICAL_H
#define LOGICAL_H

#include "Elem.h"

class Logical : public Elem
{
public:
	bool elem;				// A boolean element is the internal representation.
	Logical();				// Default constructor.
	Logical(bool);				// Another constructor with argument.
	Logical(string &);			// Construct a logical object using its string representation.
	shared_ptr<Elem> deep_copy();		// Deep_copy method, which of course returns just a logical instance since it's atomic.
	Logical operator!();			// Overloaded negation operator.
	Logical operator&&(Logical &);		// Overloaded logical and. 
	Logical operator||(Logical &);		// Overloaded logical or.
	bool operator==(Elem &);		// Overloaded == operator.
	const Logical& operator=(Logical &);	// Overloaded logical or.
	bool operator<(Elem &x)	{return false;}	// This op is basically useless for logicals.
	string to_string();			// Virtual to_string method for display.
	~Logical() { }				// Empty destructor.
};

#endif