#ifndef CHAR_H
#define CHAR_H

#include "Elem.h"
#include <iostream>
using std::cout;

class Char : public Elem
{
public:
	char elem;

	Char() : Elem(CHAR) { elem = '\0'; }						// Default constructor.

	Char(char c) : Elem(CHAR) { this->elem = c; }					// Parametrized constructor.

	Char(string &c) : Elem(CHAR) 
	{
		if (c.size() == 3) { this->elem = c[1]; return; }
		if (c[2] == '\\')							// Construct a char object using a string rep of it.
		{
			if (c[3] == 'r') this->elem = '\r';
			else if (c[3] == '\\') this->elem == '\\';
			else if (c[3] == '\'') this->elem = '\'';
			else if (c[3] == 'n') this->elem = '\n';
			else if (c[3] == 't') this->elem = '\t';
			else if (c[3] == 'b') this->elem = '\b';
			else if (c[3] == 'f') this->elem = '\f';
			else if (c[3] == 'v') this->elem = '\v';
			else if (c[3] == '0') this->elem = '\0';
			else if (c[3] == '{' || c[3] == '(' || c[3] == '[') this->elem = c[3];
			else if (c[3] == '}' || c[3] == ')' || c[3] == ']') this->elem = c[3];
		}
		else this->elem = c[2];
	}

	bool operator==(Elem &x)							// Overloaded == operator.
	{ 
		if (x.type != CHAR) return false;					// Check for types first and foremost.
		Char *_x = (Char *)&x;
		return this->elem == _x->elem;						// Then check for value.
	} 

	bool operator<(Elem &x)								// Lexicographically compares two strings.
	{
		Char *_x = (Char *)&x;
		return this->elem < _x->elem;
	}
	bool operator<=(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem <= ((Char *)&elem)->elem;
	}
	bool operator>(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem > ((Char *)&elem)->elem;
	}
	bool operator>=(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->elem >= ((Char *)&elem)->elem;
	}
	Elem* deep_copy() { return new Char(elem); }

	string to_string_raw()
	{
		if (elem == '\r') return (string)"'\\\\" + "r'";
		if (elem == '\\') return (string)"'\\\\" + "\\";
		if (elem == '\'') return (string)"'\\\\" + "\'";
		if (elem == '\n') return (string)"'\\\\" + "n'";
		if (elem == '\t') return (string)"'\\\\" + "t'";
		if (elem == '\b') return (string)"'\\\\" + "b'";
		if (elem == '\f') return (string)"'\\\\" + "f'";
		if (elem == '\v') return (string)"'\\\\" + "v'";
		if (elem == '\0') return (string)"'\\\\" + "0'";
		if (elem == '{' || elem == '(' || elem == '['
			|| elem == '}' || elem == ')' || elem == ']') return (string)"'\\\\" + string(1, elem) + "'" ;

		return (string)"'" + string(1, elem) + "'";
	}

	string to_string()
	{
		return string(1, elem);
	}
};
#endif