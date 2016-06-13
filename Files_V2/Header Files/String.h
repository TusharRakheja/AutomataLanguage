#ifndef STRING_H
#define STRING_H

#include "Elem.h"

using std::string;

class String : public Elem
{
public:
	string elem;

	String() : Elem(STRING) { elem = ""; }			// Default constructor.

	String(string &x) : Elem(STRING) { elem = x; }		// Parameterized constructor 1.

	String(string &x, int rep) : Elem(STRING) { elem = x.substr(1, x.size() - 2); }	// Construct a string object using its rep.

	String(const char* x) : Elem(STRING) { elem = x; }	// Parameterized constructor 2.

	bool operator==(Elem &x)				// Overloaded == operator.
	{ 
		if (x.type != STRING) return false;		// Check for types first and foremost.
		String* _x = (String *)&x;
		return this->elem == _x->elem;			// Then check for value.
	}
	bool operator<(Elem &x)					// Lexicographically compares two strings.
	{
		String *_x = (String *)&x;
		return this->elem < _x->elem;
	}
	Elem * deep_copy() { return new String(elem); }		// Since String is an atomic data type, a deep copy is very simple.

	string to_string() { return "\"" + elem + "\""; }	// The string representation of a string is the string itself (in quotes).

	~String() {  }						// Destructor - Delete the elem object.
};

#endif