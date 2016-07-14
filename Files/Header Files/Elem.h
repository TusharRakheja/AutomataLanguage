#ifndef ELEM_H
#define ELEM_H

#include <string>
#include <memory>

using std::string;
using std::shared_ptr;
using std::static_pointer_cast;

/* The definition of a generic Elem(ent) in Autolang. */

// Enum for the data types.
enum Type { NULLTYPE, ABSTRACT_SET, ABSTRACT_MAP, AUTO, CHAR, INT, LOGICAL, MAP, SET, STRING, TUPLE, DATASOURCE, DATASINK };  

class Elem {	                              // An element can only actively represent one of data fields at a time.
public:
	Type type;			      // Type of data stored in this element.
	Elem(Type type)                       // Generic type-setting constructor.
	{
		this->type = type;
	}
	virtual bool operator==(Elem &e)
	{
		if (this->type != e.type) return false;
		else return *this == e;
	}
	virtual string to_string() = 0;					// Virtual to_string method for display.
	virtual string to_string_raw() { return this->to_string(); }	// Only ever makes sense to use this for characters and strings.
	virtual shared_ptr<Elem> deep_copy() = 0;			// Virtual deep_copy method for making a 'deep clone' of the object.
};

#endif