#ifndef ABSTRACT_SET_H
#define ABSTRACT_SET_H

#include "Set.h"

/* All abstract sets are represented in the set-builder notation, and the placeholder element is the keyword 'elem'. */

class AbstractSet : public Elem
{
public:
	string criteria;					   // The membership criteria for the abstract set.
	string input_format;
	unordered_map<string, string> holder_value_pairs;

	AbstractSet() : Elem(ABSTRACT_SET) { }			   // Uninteresting default constructor.
	AbstractSet(string &);					   // Construct with a string representing the criteria.
	AbstractSet(const char *);				   // Construct with a cstring representing the criteria.
	shared_ptr<AbstractSet> cartesian_product(AbstractSet &);  // Returns the cartesian product of this set and the other set.
	shared_ptr<Elem> deep_copy()				   // Return a new set with the same criteria.
	{ 
		string crit_copy = "{ ";
		crit_copy += input_format;
		crit_copy += " | ";
		crit_copy += criteria;
		crit_copy += " }";
		return shared_ptr<Elem>{new AbstractSet(crit_copy)}; 
	} 
	shared_ptr<AbstractSet> exclusion(AbstractSet &);	   // Returns a set containing elements of this, minus those of the argument.
	bool has(Elem &);					   // Looks for an element in the set.
	shared_ptr<AbstractSet> intersection(AbstractSet &);       // Intersection with a second set.
	bool superset_of(Set &);				   // Return true if this abstract set is a super set of the argument set.
	shared_ptr<AbstractSet> _union(AbstractSet &);             // Union with a second set.
	string to_string() {return "{ " + input_format + " | " + criteria + " }";} // Just return the set-builder notation.
};

#define aset static_pointer_cast<AbstractSet>

#endif