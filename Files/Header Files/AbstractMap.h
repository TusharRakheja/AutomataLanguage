#ifndef ABSTRACT_MAP_H
#define ABSTRACT_MAP_H

#include "AbstractSet.h"

class AbstractMap : public Elem
{
public:
	string mapping_scheme;
	shared_ptr<AbstractSet> domain, codomain;
	
	AbstractMap() : Elem(ABSTRACT_MAP) { }
	AbstractMap(shared_ptr<AbstractSet>, shared_ptr<AbstractSet>, string &);
	AbstractMap(shared_ptr<AbstractSet>, shared_ptr<AbstractSet>);
	AbstractMap(string &);
	void add_scheme(string &);
	shared_ptr<AbstractMap> composed_with(shared_ptr<AbstractMap>);
	shared_ptr<Elem> deep_copy()
	{
		return shared_ptr<AbstractMap>{
			new AbstractMap (
				(domain == nullptr) ? nullptr : std::static_pointer_cast<AbstractSet>(domain->deep_copy()), 
				(codomain == nullptr) ? nullptr : std::static_pointer_cast<AbstractSet>(codomain->deep_copy()),
				mapping_scheme
			)
		}; 
	};
	shared_ptr<Elem> operator[](Elem &);
	const shared_ptr<Elem> operator[](Elem &) const;
	string to_string() { return mapping_scheme; }
};

#endif