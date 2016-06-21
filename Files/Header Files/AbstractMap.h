#ifndef ABSTRACT_MAP_H
#define ABSTRACT_MAP_H

#include "AbstractSet.h"

class AbstractMap : public Elem
{
public:
	string mapping_scheme;
	AbstractSet * domain, * codomain;
	
	AbstractMap() : Elem(ABSTRACT_MAP) { }
	AbstractMap(AbstractSet *, AbstractSet *, string &);
	AbstractMap(AbstractSet *, AbstractSet *);
	AbstractMap(string &);
	void add_scheme(string &);
	AbstractMap * composed_with(AbstractMap *);
	Elem * deep_copy() 
	{
		return new AbstractMap(
			(domain == nullptr) ? nullptr : (AbstractSet *)domain->deep_copy(), 
			(codomain == nullptr) ? nullptr : (AbstractSet *)codomain->deep_copy(), 
			mapping_scheme
		); 
	};
	Elem * operator[](Elem &);
	const Elem * operator[](Elem &) const;
	string to_string() { return mapping_scheme; }
};

#endif