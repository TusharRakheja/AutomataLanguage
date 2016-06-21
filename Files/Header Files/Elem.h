#ifndef ELEM_H
#define ELEM_H

#include <string>

using std::string;

/* The definition of a generic Elem(ent) in Autolang. */

enum Type { NULLTYPE, ABSTRACT_SET, ABSTRACT_MAP, AUTO, CHAR, INT, LOGICAL, MAP, SET, STRING, TUPLE };    // Enum for the data types.

class Elem {	                              // An element can only actively represent one of data fields at a time.
public:
	Type type;			      // Type of data stored in this element.
	Elem(Type type)                       // Generic type-setting constructor.
	{
		this->type = type;
	}
	virtual bool operator==(Elem &elem)   // The equality comparison between two elements.
	{
		return (this->type != elem.type) ? false : this->operator==(elem);
	}
	virtual bool operator<(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->operator<(elem);
	}
	virtual bool operator<=(Elem &elem)
	{
		return (this->type != elem.type) ? false : this->operator<(elem) || this->operator==(elem); 
	}
	virtual bool operator>(Elem &elem)
	{
		return (this->type != elem.type) ? false : !this->operator<=(elem);
	}	
	virtual bool operator>=(Elem &elem)
	{
		return (this->type != elem.type) ? false : !this->operator<(elem);
	}
	virtual string to_string() = 0;       // Virtual to_string method for display.
	virtual Elem* deep_copy() = 0;        // Virtual deep_copy method for making a 'deep clone' of the object.
};

#endif