#ifndef ABSTRACT_MAP_H
#define ABSTRACT_MAP_H

#include "AbstractSet.h"

class AbstractMap : public Elem
{
public:
	string mapping_scheme;
	string input_format;
	unordered_map<string, string> holder_value_pairs;
	shared_ptr<AbstractSet> domain, codomain;
	
	AbstractMap() : Elem(ABSTRACT_MAP) { }
	AbstractMap(shared_ptr<AbstractSet>, shared_ptr<AbstractSet>, string &);
	AbstractMap(shared_ptr<AbstractSet>, shared_ptr<AbstractSet>);
	AbstractMap(string &);
	AbstractMap(const char *);
	void add_scheme(string &);
	shared_ptr<AbstractMap> composed_with(shared_ptr<AbstractMap>);
	shared_ptr<Elem> deep_copy()
	{
		return shared_ptr<AbstractMap>{
			new AbstractMap (
				(domain == nullptr) ? nullptr : aset(domain->deep_copy()), 
				(codomain == nullptr) ? nullptr : aset(codomain->deep_copy()),
				this->to_string()
			)
		}; 
	};
	void parse_holder_value_pairs(string &, string &);
	shared_ptr<Elem> operator[](Elem &);
	const shared_ptr<Elem> operator[](Elem &) const;
	string to_string() { return input_format + " -> " + mapping_scheme; }
	string to_string_raw() { return ":: " + input_format + " -> " + mapping_scheme + " ::"; }
};

#define amap static_pointer_cast<AbstractMap>

#endif
