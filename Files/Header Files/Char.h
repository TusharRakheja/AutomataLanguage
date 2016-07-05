#ifndef CHAR_H
#define CHAR_H

#include "Elem.h"

#define character static_pointer_cast<Char>

class Char : public Elem
{
public:
	char elem;

	Char() : Elem(CHAR) { elem = '\0'; }						// Default constructor.

	Char(char c) : Elem(CHAR) { this->elem = c; }					// Parametrized constructor.

	Char(string &c) : Elem(CHAR) 
	{
		if (c.size() == 3) { this->elem = c[1]; return; }
		if (c[1] == '\\')							// Construct a char object using a string rep of it.
		{
			if (c[2] == 'r') this->elem = '\r';
			else if (c[2] == '\\') this->elem = '\\';
			else if (c[2] == '\'') this->elem = '\'';
			else if (c[2] == 'n') this->elem = '\n';
			else if (c[2] == 't') this->elem = '\t';
			else if (c[2] == 'b') this->elem = '\b';
			else if (c[2] == 'f') this->elem = '\f';
			else if (c[2] == 'v') this->elem = '\v';
			else if (c[2] == '0') this->elem = '\0';
			else if (c[2] == '{' || c[2] == '(' || c[2] == '[') this->elem = c[2];
			else if (c[2] == '}' || c[2] == ')' || c[2] == ']') this->elem = c[2];
		}
		else this->elem = c[1];
	}

	bool operator==(Elem &e)
	{
		Char * x = (Char *)&e;
		return x->elem == this->elem;
	}

	shared_ptr<Elem> deep_copy() { return shared_ptr<Elem>{new Char(elem)}; }

	string to_string_raw()
	{
		if (elem == '\r') return (string)"'\\" + "r'";
		if (elem == '\\') return (string)"'\\" + "\\'";
		if (elem == '\'') return (string)"'\\" + "\'";
		if (elem == '\n') return (string)"'\\" + "n'";
		if (elem == '\t') return (string)"'\\" + "t'";
		if (elem == '\b') return (string)"'\\" + "b'";
		if (elem == '\f') return (string)"'\\" + "f'";
		if (elem == '\v') return (string)"'\\" + "v'";
		if (elem == '\0') return (string)"'\\" + "0'";
		if (elem == '{' || elem == '(' || elem == '['
			|| elem == '}' || elem == ')' || elem == ']') return (string)"'\\" + string(1, elem) + "'" ;

		return (string)"'" + string(1, elem) + "'";
	}

	string to_string()
	{
		return string(1, elem);
	}
};


#endif
