#ifndef STRING_H
#define STRING_H

#include "Elem.h"

#define str static_pointer_cast<String>

class String : public Elem
{
public:
	string elem;

	String() : Elem(STRING) { elem = ""; }			// Default constructor.

	String(string &x) : Elem(STRING) { elem = x; }		// Parameterized constructor 1.

	String(string &x, int rep) : Elem(STRING) { elem = x.substr(1, x.size() - 2); }	// Construct a string object using its rep.

	String(const char* x) : Elem(STRING) { elem = x; }	// Parameterized constructor 2.

	bool operator==(Elem &e)
	{
		String * x = (String *)&e;
		return x->elem == this->elem;
	}

	shared_ptr<Elem> deep_copy()				// Since String is an atomic data type, a deep copy is very simple.
	{ 
		return shared_ptr<Elem> {new String(elem)}; 
	} 

	string to_string_raw()					// The string representation of a string is the string itself (in quotes).
	{
		string rep = "\"";
		rep += elem;
		return rep + "\"";
	}

	string to_string()
	{
		string rep = "";
		for (int i = 0; i < elem.size(); i++)
		{
			if ((elem[i] != '\\')) rep += elem[i];
			else
			{
				if (elem[i + 1] == 't') { rep += '\t'; i++; }
				else if (elem[i + 1] == 'v') { rep += '\v'; i++; }
				else if (elem[i + 1] == '\\') { rep += '\\'; i++; }
				else if (elem[i + 1] == 'n') { rep += '\n'; i++; }
				else if (elem[i + 1] == 'b') { rep += '\b'; i++; }
				else if (elem[i + 1] == 'r') { rep += '\r'; i++; }
				else if (elem[i + 1] == 'f') { rep += '\f'; i++; }
				else if (elem[i + 1] == '"') { rep += '"'; i++; }
				else if (elem[i + 1] == '{' || elem[i + 1] == '(' || elem[i + 1] == '[') rep += elem[++i];
				else if (elem[i + 1] == '}' || elem[i + 1] == ')' || elem[i + 1] == ']') rep += elem[++i];
			}
		}
		return rep;
	}

	~String() {  }						// Destructor - Delete the elem object.
};

#endif