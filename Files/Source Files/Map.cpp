#include "../Header Files/Map.h"
#include "../Header Files/ExpressionTree.h"

/* Implementations for methods in the class Map. */

Map::Map() : Elem(MAP)					// Default constructor.
{
	domain_s = codomain_s = nullptr;
	pi_indices = nullptr; _map = nullptr;
}

Map::Map(shared_ptr<Set> domain_s, shared_ptr<Set> codomain_s) : Elem(MAP)	// Parameterized constructor.
{
	this->domain_s = domain_s; this->codomain_s = codomain_s;
	_map = new unordered_map<int, int>();
	pi_indices = new vector<int>();
}

void Map::add_maping(Elem &pre_image, Elem &image)	// Adds a mapping from the left element to the right under this map.
{
	if (!domain_s->has(pre_image) || !codomain_s->has(image))
		return; 

	int pre_image_index {0}, image_index {0};

	for (auto &elem_p1 : *domain_s->elems)		// For every (any) element_pointer in the domain's vector of element_pointers ...
		if (*elem_p1 == pre_image)		// ... if the element pointed to by it == pre_image ...
			break;				// ... stop looking.
		else
			pre_image_index++;		// ... otherwise look at the next element_pointer.

	for (auto &elem_p2 : *codomain_s->elems)	// For every (any) element_pointer in the codomain's vector of element_pointers ...
		if (*elem_p2 == image)			// ... if the element pointed to by it == image ...
			break;
		else
			image_index++;			// ... otherwise look at the next element_pointer.

	(*_map)[pre_image_index] = image_index;		// Once both the pre_image_index as well as the image_index are found, add the mapping.

	pi_indices->push_back(pre_image_index);		// Also add the pre_image_index to the vector (to note which pre_images are mapped).
}

shared_ptr<Map> Map::composed_with(Map &other_map)	// Returns a reference to a map object that is this->composed_with(other_map).
{
	if (!other_map.range()->subset_of(*domain_s))	// If the range of g is !c domain of f, the compose operation is not possible.
		return nullptr;

	shared_ptr<Map> fog { new Map((other_map.domain_s), codomain_s) };   // If the compose is possible, let's make a map the apt sets.

	for (auto &index : *(other_map._map)) 				     // THE UGLIEST if-condition follows.

		if (std::find(pi_indices->begin(), pi_indices->end(), index.second) != pi_indices->end())

			(*fog->_map)[index.first] = (*this->_map)[index.second];		// fog(x) = f(g(x))

	return fog;							     // Return the composition.		
}

shared_ptr<Set> Map::codomain() { return codomain_s; }		// Returns a pointer to the codomain set.
shared_ptr<Set> Map::domain()   { return  domain_s; }		// Returns a pointer to the domain set.

shared_ptr<Elem> Map::deep_copy()				// Returns a deep_copy of this map (NOTE: Also deep_copies the codomain and domain).
{
	shared_ptr<Set> deep_domain   { std::set(domain_s->deep_copy())   };	// Deep_copy the domain.
	shared_ptr<Set> deep_codomain { std::set(codomain_s->deep_copy()) };	// Deep_copy the codomain.

	shared_ptr<Map> deep_map {new Map(deep_domain, deep_codomain)};	// Make a new map object with the deep_domain and deep_range.

	*deep_map->_map = *this->_map;			// Deep_copy the unordered map of pre_image_indices and image_indices.
	*deep_map->pi_indices = *this->pi_indices;	// Deep_copy the vector of pre_image_indices.

	return deep_map;				// Return the deeply_copied map.
}

void Map::delete_elems()
{
	delete pi_indices, _map;				// Delete the mappings and pi_indices.
}

shared_ptr<Elem> Map::operator[](Elem &pre_image)	// Returns the image of the pre-image [L-value].
{
	shared_ptr<Elem> default_l_val = nullptr;
	int pre_image_index{ 0 };			// Initialize the pre_image('s)_index to 0.
	for (auto &elem_p : *domain_s->elems)		// For every (any) element_pointer in the domain set's vector of element_pointers ...
		if (*elem_p == pre_image)		// ... if the element pointed to by it == the pre_image ...
			break;				// ... stop and look no more.
		else					// ... else ...
			pre_image_index++;		// ... look at the next element_pointer.

	if (pre_image_index == domain_s->cardinality()) // If we didn't find the pre_image in the domain ...
		return default_l_val;			// ... return nullptr.

	return (*codomain_s)[(*_map)[pre_image_index]];	// Otherwise of course return the image.
}

const shared_ptr<Elem> Map::operator[](Elem &pre_image) const	// Returns the image of the pre-image [R-value].
{
	int pre_image_index { 0 };			// Initialize the pre_image('s)_index to 0.
	for (auto &elem_p : *domain_s->elems)		// For every (any) element_pointer in the domain set's vector of element_pointers ...
		if (*elem_p == pre_image)		// ... if the element pointed to by it == the pre_image ...
			break;				// ... stop and look no more.
		else					// ... else ...
			pre_image_index++;		// ... look at the next element_pointer.

	if (pre_image_index == domain_s->cardinality()) // If we didn't find the pre_image in the domain ...
		return nullptr;				// ... return nullptr.

	return (*codomain_s)[_map->at(pre_image_index)];	// Otherwise of course return the image.
}

bool Map::operator==(Elem &elem)			// Compares this to another map.
{
	if (elem.type != MAP) return false;		// Return false if the type of the element isn't MAP.

	Map * other_map = (Map *)&elem;			// Now we can cast the map. 

	if (*other_map->domain_s == *this->domain_s &&		// Maps are only equal if they have the same domain ...
	    *other_map->codomain_s == *this->codomain_s &&	// ... the same co-domain ...
	    *other_map->_map == *this->_map)			// ... and the same pre_image-image mappings.
	    return true;					// All those things together imply the same range as well!

	return false;					// Otherwise they're not equal.
}

shared_ptr<Set> Map::range()					// The set of all values in the codomain that are mapped to.
{
	shared_ptr<Set> x { new Set() };				        // Make a new empty set.
	for (auto &mapping : *this->_map)				// For every (pre_image_index,image_index) pair in the set ...
		x->elems->push_back((*codomain_s)[mapping.second]);	// ... put the element in the codomain at the image_index  
	return x;
}

string Map::to_string()					// The virtual to_string() method for display.
{
	string representation = "{";	
	int i{ 0 };
	for (auto &index : *_map)
	{		
		representation += "(";
		representation += domain_s->operator[](index.first)->to_string();   // Recursive, awesome representations. ;)
		representation += ", ";
		representation += codomain_s->operator[](index.second)->to_string();
		representation += ")";
		if (i != _map->size() - 1)
			representation += ", ";
		i++;
	}
	return representation + "}";
}

string Map::to_string_raw()					// The virtual to_string() method for display.
{
	string representation = "{";
	int i{ 0 };
	for (auto &index : *_map)
	{
		representation += "(";
		representation += domain_s->operator[](index.first)->to_string_raw();   // Recursive, awesome representations. ;)
		representation += ", ";
		representation += codomain_s->operator[](index.second)->to_string_raw();
		representation += ")";
		if (i != _map->size() - 1)
			representation += ", ";
		i++;
	}
	return representation + "}";
}

Map::~Map()
{
	delete _map; 
	delete pi_indices;
}