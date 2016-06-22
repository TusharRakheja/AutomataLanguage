#ifndef MAP_H 
#define MAP_H

#include "Set.h"
#include "Tuple.h"
#include <algorithm>
#include <unordered_map>

using std::unordered_map;

class Map : public Elem
{
public:
	Set *domain_s, *codomain_s;		// The domain and codomain sets.
	unordered_map<int, int> *map;
	vector<int> *pi_indices;		// Pre_image indices.

	Map();					// Default constructor.
	Map(Set *, Set *);			// Parameterized constructor.
	void add_maping(Elem &, Elem &);	// Adds a mapping from the left element to the right under this map.
	Set *codomain();			// Returns the codomain of the ordered map.
	Map *composed_with(Map &);		// Retuns a new map that is this map composed with another map.
	Elem *deep_copy();			// Returns a deep_copy of this map.
	void delete_elems();			// Delete elements.
	Set * domain();				// Returns the domain of the ordered map.
	Elem * operator[](Elem &);		// Returns the image of the pre-image [L-value].
	const Elem * operator[](Elem &) const;	// Returns the image of the pre-image [R-value].
	bool operator==(Elem &);		// Compares this to another map.
	Set *range();				// Returns the range of the ordered map.
	string to_string();			// Returns a string representation of the map. 
	string to_string_raw();			// Returns a raw_string representation of the map.
	~Map();					// Destructor.
};

#endif