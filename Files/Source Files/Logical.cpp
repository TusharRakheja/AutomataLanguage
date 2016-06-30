#include "../Header Files/Logical.h"

/* Implementations for the methods in the class Logical. */

Logical::Logical() : Elem(LOGICAL)		// Default constructor.
{
	elem = false;
}

Logical::Logical(bool t_val) : Elem(LOGICAL)	// Another constructor with argument.
{
	elem = t_val;
}

Logical::Logical(string &t_val) : Elem(LOGICAL) // Construct a logical object using its string representation.
{
	elem = (t_val == "True") ? true : false;
}

shared_ptr<Elem> Logical::deep_copy()		// Deep_copy method, which of course returns just a logical instance since it's atomic.
{
	return shared_ptr<Elem>{new Logical(elem)};	// Return the address to a new Logical object, initialized with the same value as this.
}

const Logical& Logical::operator=(Logical &x)	// Overloaded assignment operator.
{
	this->elem = x.elem;			// Basically make the truth value of the this logical element equal to that of x.
	return *this;				// Return a reference to this for cascading assignment.
}

Logical Logical::operator!()			// Overloaded negation operator.
{
	return Logical(!this->elem);		// Return a logical object constructed with the truth value negative of this.
}

Logical Logical::operator&&(Logical &x)		// Overloaded logical and. 
{
	return Logical(this->elem && x.elem);   // Return a logical object constructed with the truth value of (this->elem && x.elem).
}

Logical Logical::operator||(Logical &x)		// Overloaded logical or.
{
	return Logical(this->elem || x.elem);	// Return a logical object constructed with the truth value of (this->elem || x.elem).
}

bool Logical::operator==(Elem &x)		// Overloaded == operator.
{
	if (x.type != LOGICAL) return false;	// Check for types first and foremost.
	Logical *_x = (Logical *)&x;		
	return (this->elem == _x->elem);	// Then check for value.
}

string Logical::to_string()			// Virtual to_string method for display.
{
	return (elem == true) ? "True" : "False";
}
