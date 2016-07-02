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
	shared_ptr<Set>domain_s, codomain_s;		// The domain and codomain sets.
	unordered_map<int, int> * _map;
	vector<int> * pi_indices;			// Pre_image indices.

	Map();						// Default constructor.
	Map(shared_ptr<Set>, shared_ptr<Set>);		// Parameterized constructor.
	void add_maping(Elem &, Elem &);		// Adds a mapping from the left element to the right under this map.
	shared_ptr<Set> codomain();			// Returns the codomain of the ordered map.
	shared_ptr<Map> composed_with(Map &);		// Retuns a new map that is this map composed with another map.
	shared_ptr<Elem>deep_copy();			// Returns a deep_copy of this map.
	void delete_elems();				// Delete elements.
	shared_ptr<Set> domain();			// Returns the domain of the ordered map.
	shared_ptr<Elem> operator[](Elem &);		// Returns the image of the pre-image [L-value].
	const shared_ptr<Elem>operator[](Elem &) const;	// Returns the image of the pre-image [R-value].
	bool operator==(Elem &);			// Compares this to another map.
	shared_ptr<Set> range();			// Returns the range of the ordered map.
	string to_string();				// Returns a string representation of the map. 
	string to_string_raw();				// Returns a raw_string representation of the map.
	~Map();						// Destructor.
};

#define map static_pointer_cast<Map>

#endif