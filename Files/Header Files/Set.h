#ifndef SET_H
#define SET_H

#include "Tuple.h"

/* Definition of a Set in Autolang. */

class Set : public Elem
{
public:
	vector<shared_ptr<Elem>> * elems;		// A set is internally implemented as a vector of element_pointers.
	Set();						// Default constructor.
	Set(vector<shared_ptr<Elem>> *);		// It is possible to initialize a set with an existing vector (Copy pointers).
	Set(vector<shared_ptr<Elem>> *, int);		// It is possible to initialize a set with an existing vector (Direct assign).
	Set(string &);					// Construct a set using a string representation of it.
	int cardinality();				// Returns the cardinality of the set.	
	shared_ptr<Set> cartesian_product(Set &);	// Returns the cartesian product of this set and the other set.
	shared_ptr<Elem> deep_copy();			// Returns a new set which is a deep_copy of this set.
	shared_ptr<Set> exclusion(Set &);		// Returns a set containing elements of this, minus those of the argument.
	bool has(Elem &);				// Looks for an element in the set.
	bool homoset();					// Returns true if every element of this set has the same type.
	Type homotype();				// Returns the type of every element in this set (if it is a homoset).
	shared_ptr<Set> intersection(Set &);		// Intersection with a second set.
	shared_ptr<Set> subset(int, int);		// Returns a subset of the set containing elements in [start, end).
	bool subset_of(Set &);				// Checks if this set is a subset of the candidate_superset.
	shared_ptr<Elem> operator[](int);		// L-value access.
	const shared_ptr<Elem> operator[](int) const;	// R-value access.
	bool operator==(Elem &);			// Checks two sets for equality.
	bool operator<(Elem &e) { return false; }	// This op is basically useless for sets.
	string to_string();				// Returns a string representation of the set.
	string to_string_raw();				// Returns a raw_string representation of the set.
	shared_ptr<Set> _union(Set &);			// Union with a second set.
	~Set();						// Destructor.
};

#define set static_pointer_cast<Set>

#endif