#ifndef STRING_H
#define STRING_H

#include "Elem.h"

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
	bool operator<=(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem <= ((String *)&elem)->elem;
	}
	bool operator>(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem > ((String *)&elem)->elem;
	}
	bool operator>=(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem >= ((String *)&elem)->elem;
	}
	Elem * deep_copy() { return new String(elem); }		// Since String is an atomic data type, a deep copy is very simple.

	string to_string()					// The string representation of a string is the string itself (in quotes).
	{
		string rep = "\"";
		for (int i = 0; i < elem.size(); i++)
		{
			if (!(elem[i] == '\\')) rep += elem[i];		 
			else 
			{
				if (elem[i+1] == 't') { rep += '\t'; i++; }
				else if (elem[i + 1] == 'n') { rep += '\n'; i++; }
				else if (elem[i + 1] == 'b') { rep += '\b'; i++; }
				else if (elem[i + 1] == '"') { rep += '"'; i++; }
				else if (elem[i + 1] == '{' || elem[i + 1] == '(' || elem[i + 1] == '[') rep += elem[++i];
				else if (elem[i + 1] == '}' || elem[i + 1] == ')' || elem[i + 1] == ']') rep += elem[++i];
			}
		}
		return rep + "\"";
	}

	~String() {  }						// Destructor - Delete the elem object.
};

#endif