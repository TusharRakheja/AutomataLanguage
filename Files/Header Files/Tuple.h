#ifndef TUPLE_H
#define TUPLE_H

#define DIRECT_ASSIGN 2		/* Shall be used whenever the constructor at line 24 is to be evoked (And for other similar constructors). */

#include "Int.h"
#include "Char.h"
#include "Logical.h"
#include "String.h"
#include <vector>

using std::vector;
using std::string;

class Set;

class Tuple : public Elem
{
public:
	vector<shared_ptr<Elem>> * elems;
	Tuple();					// Default constructor, empty tuple.
	Tuple(vector<shared_ptr<Elem>> *);		// Tuple-ize an existing vector of element_pointers (Copy pointers).
	Tuple(string &);				// Make the tuple using the string representation of it.
	Tuple(vector<shared_ptr<Elem>> *, int);		// Tuple-ize an existing vector of element_pointers (Direct assign).
	shared_ptr<Elem> deep_copy();			// Returns a tuple which is a deep_copy of this tuple.
	bool has(Elem &);				// Checks if a given element is present in the tuple.
	const shared_ptr<Elem> operator[](int) const;	// R-value access.
	shared_ptr<Elem> operator[](int);		// L-value access.
	bool operator==(Elem &);			// Checks two tuples for equality.
	bool operator<(Elem &e) { return false; }	// This op is basically useless for tuples.
	int size();					// Returns the size (or 'dimension') of this tuple.
	string to_string();				// Returns a string representation of the tuple.
	string to_string_raw();				// Returns a raw string representation of the tuple.
	~Tuple();					// Destructor.
};

#define _tuple static_pointer_cast<Tuple>

#endif