#ifndef LOGICAL_H
#define LOGICAL_H

#include "Elem.h"

#define logical static_pointer_cast<Logical>

class Logical : public Elem
{
public:
	bool elem;				// A boolean element is the internal representation.
	Logical() : Elem(LOGICAL)		// Default constructor.
	{
		elem = false;
	}

	Logical(bool t_val) : Elem(LOGICAL)	// Another constructor with argument.
	{
		elem = t_val;
	}

	Logical(string &t_val) : Elem(LOGICAL)  // Construct a logical object using its string representation.
	{
		elem = (t_val == "True") ? true : false;
	}

	bool operator==(Elem &e)
	{
		Logical * x = (Logical *)&e;
		return x->elem == this->elem;
	}

	shared_ptr<Elem> deep_copy()		// Deep_copy method, which of course returns just a logical instance since it's atomic.
	{
		return shared_ptr<Elem>{new Logical(elem)};	// Return the address to a new Logical object, initialized with the same value as this.
	}
	string to_string()			// Virtual to_string method for display.
	{
		return (elem) ? "True" : "False";
	}
};


#endif