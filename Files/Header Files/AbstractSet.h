#ifndef ABSTRACT_SET_H
#define ABSTRACT_SET_H

#include "Set.h"

/* All abstract sets are represented in the set-builder notation, and the placeholder element is the keyword 'elem'. */

class AbstractSet : Elem
{
public:
	string criteria;					   // The membership criteria for the abstract set.
	AbstractSet() : Elem(ABSTRACT_SET) { }			   // Uninteresting default constructor.
	AbstractSet(string &);					   // Construct with a string representing the criteria.
	AbstractSet * cartesian_product(AbstractSet &);            // Returns the cartesian product of this set and the other set.
	Elem * deep_copy()
	{ return new AbstractSet("{ elem | " + criteria + " }"); } // Return a new set with the same criteria.
	AbstractSet * exclusion(AbstractSet &);			   // Returns a set containing elements of this, minus those of the argument.
	bool has(Elem &);					   // Looks for an element in the set.
	AbstractSet * intersection(AbstractSet &);                 // Intersection with a second set.
	bool superset_of(Set &);				   // Return true if this abstract set is a super set of the argument set.
	AbstractSet * _union(AbstractSet &);                       // Union with a second set.
	string to_string() {return "{ elem | " + criteria + " }";} // Just return the set-builder notation.
};

#endif