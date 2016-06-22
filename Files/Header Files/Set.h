#ifndef SET_H
#define SET_H

#include "Tuple.h"

/* Definition of a Set in Autolang. */

class Set : public Elem
{
public:
	vector<Elem *> * elems;                    // A set is internally implemented as a vector of element_pointers.
	Set();                                     // Default constructor.
	Set(vector<Elem *> *);                     // It is possible to initialize a set with an existing vector (Copy pointers).
	Set(vector<Elem *> *, int);		   // It is possible to initialize a set with an existing vector (Direct assign).
	Set(string &);				   // Construct a set using a string representation of it.
	int cardinality();                         // Returns the cardinality of the set.	
	Set * cartesian_product(Set &);            // Returns the cartesian product of this set and the other set.
	Elem * deep_copy();			   // Returns a new set which is a deep_copy of this set.
	void delete_elems();			   // Delete elements.
	Set * exclusion(Set &);                    // Returns a set containing elements of this, minus those of the argument.
	bool has(Elem &);                          // Looks for an element in the set.
	bool homoset();				   // Returns true if every element of this set has the same type.
	Type homotype();			   // Returns the type of every element in this set (if it is a homoset).
	Set * intersection(Set &);                 // Intersection with a second set.
	Set * subset(int, int);                    // Returns a subset of the set containing elements in [start, end).
	bool subset_of(Set &);                     // Checks if this set is a subset of the candidate_superset.
	Elem * operator[](int);			   // L-value access.
	const Elem * operator[](int) const;	   // R-value access.
	bool operator==(Elem &);                   // Checks two sets for equality.
	bool operator<(Elem &e) { return false; }  // This op is basically useless for sets.
	string to_string();                        // Returns a string representation of the set.
	string to_string_raw();		           // Returns a raw_string representation of the set.
	Set * _union(Set &);                       // Union with a second set.
	~Set();					   // Destructor.
};


#endif