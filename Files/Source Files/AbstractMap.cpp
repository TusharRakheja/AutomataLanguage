#include "../Header Files/AbstractMap.h"
#include "../Header Files/ExpressionTree.h"
#include "../Header Files/ProgramVars.h"

#include <iostream>
using std::cout;
using std::endl;

/* Implementations for methods in AbstractMap. */

AbstractMap::AbstractMap(string &mapping_scheme_full) : Elem(ABSTRACT_MAP)
{
	this->domain = nullptr;
	this->codomain = nullptr;
	int start = mapping_scheme_full.find("->") + 2;
	while (isspace(mapping_scheme_full[start])) start++;
	this->mapping_scheme = mapping_scheme_full.substr(start);
}

AbstractMap::AbstractMap(const char *_mapping_scheme_full) : Elem(ABSTRACT_MAP)
{
	string mapping_scheme_full(_mapping_scheme_full);
	this->domain = nullptr;
	this->codomain = nullptr;
	int start = mapping_scheme_full.find("->") + 2;
	while (isspace(mapping_scheme_full[start])) start++;
	this->mapping_scheme = mapping_scheme_full.substr(start);
}

void AbstractMap::add_scheme(string &mapping_scheme_full)
{
	int start = mapping_scheme_full.find("->") + 2;
	while (isspace(mapping_scheme_full[start])) start++;
	this->mapping_scheme = mapping_scheme_full.substr(start);	
}

AbstractMap::AbstractMap(shared_ptr<AbstractSet> domain, shared_ptr<AbstractSet> codomain) : Elem(ABSTRACT_MAP)
{
	this->domain = domain;
	this->codomain = codomain;
}

AbstractMap::AbstractMap(shared_ptr<AbstractSet> domain, shared_ptr<AbstractSet> codomain, string &mapping_scheme_full) : Elem(ABSTRACT_MAP)
{	
	this->domain = domain; 
	this->codomain = codomain;
	int start = mapping_scheme_full.find("->") + 2;
	while (isspace(mapping_scheme_full[start])) start++;
	this->mapping_scheme = mapping_scheme.substr(start, mapping_scheme.size() - start);
}

shared_ptr<AbstractMap> AbstractMap::composed_with(shared_ptr<AbstractMap> g)	// Returns an abstract_map (this composed with g).
{
	// For the comments that follow, this == f. Just for convenience, really.
	
	// under f : elem -> f[elem], where f[elem] represents some transformation.
	// under g : elem -> g[elem], where g[elem] represents some other transformation.
	// We need to return f o g. under f o g : elem -> f[g[elem]]

	string scheme_fog = this->mapping_scheme;

	// In the mapping scheme of f, wherever we see an (x), we'll replace that with (g[(x)]).

	vector<string> scheme_parts;
	int pos = scheme_fog.find("(x)");
	while (pos != string::npos)
	{
		scheme_parts.push_back(scheme_fog.substr(0, pos));
		scheme_parts.push_back("(" + g->mapping_scheme + ")");
		scheme_fog = scheme_fog.substr(pos + 3, scheme_fog.size() - (pos + 3));
		pos = scheme_fog.find("(x)");
	}
	scheme_parts.push_back(scheme_fog);
	scheme_fog = "";
	for (auto &part : scheme_parts) scheme_fog += part;
	
	shared_ptr<AbstractMap> fog = shared_ptr<AbstractMap>{new AbstractMap()};
	fog->domain = g->domain;
	fog->codomain = this->codomain;
	fog->mapping_scheme = scheme_fog;
	return fog;
}

shared_ptr<Elem> AbstractMap::operator[](Elem & pre_image)
{
	if (domain != nullptr && !domain->has(pre_image)) program_vars::raise_error("Mapping unsuccessful. Pre-image not found in domain.");
	string x = this->mapping_scheme;			// We'll replace all instances of 'elem' at level 0 with elem->to_string().
	int start = 0;
	bool in_string = false, in_char = false;
	vector<string> schemeparts;				// The parts of the criteria without 'elem'
	for (int i = 0; i < x.size(); i++)
	{
		if ((x[i] == '"' && !in_string && !in_char) || (x[i] == '\'' && !in_char && !in_string)
			&& (i == 0 || (x[i - 1] != '\\' || (x[i - 1] == '\\' && i - 2 >= 0 && x[i - 2] == '\\'))))
		{
			if (x[i] == '"' && !in_string && !in_char) in_string = true;
			if (x[i] == '\'' && !in_char && !in_string) in_char = true;
		}
		else if ((x[i] == '"' && in_string) || (x[i] == '\'' && in_char)
			&& (i == 0 || (x[i - 1] != '\\' || (x[i - 1] == '\\' && i - 2 >= 0 && x[i - 2] == '\\'))))
		{
			if (x[i] == '"' && in_string) in_string = false;
			if (x[i] == '\'' && in_char) in_char = false;
		}
		else if (x[i] == '(' && i + 2 < x.size() && x.substr(i, 3) == "(x)" && !in_string)
		{
			schemeparts.push_back(x.substr(start, i - start));
			i += 3;
			start = i;
		}
	}
	schemeparts.push_back(x.substr(start, x.size() - start));

	string to_be_evaluated = schemeparts[0] + pre_image.to_string_eval();
	for (int i = 1; i <schemeparts.size() - 1; i++)
		to_be_evaluated += schemeparts[i] + pre_image.to_string_eval();
	to_be_evaluated += schemeparts[schemeparts.size() - 1];

	ExpressionTree eval(to_be_evaluated);
	shared_ptr<Elem> image = eval.evaluate();
	if (codomain != nullptr && !codomain->has(*image)) program_vars::raise_error("Mapping unsuccessful. Pre-image not found in domain."); 
	return image;
}

const shared_ptr<Elem> AbstractMap::operator[](Elem & pre_image) const
{
	if (domain != nullptr && !domain->has(pre_image)) program_vars::raise_error("Mapping unsuccessful. Pre-image not found in domain.");
	string to_be_evaluated = this->mapping_scheme;		// We'll replace all instances of 'elem' with elem->to_string().
	while (to_be_evaluated.find("(x)") != string::npos)	// Iteratively replace the instances in to_be_evaluated.
	{
		int elem_pos = to_be_evaluated.find("(x)");
		string part1 = to_be_evaluated.substr(0, elem_pos);
		string part2 = "(" + pre_image.identifier + ")";
		string part3 = to_be_evaluated.substr(elem_pos + 3, to_be_evaluated.size() - (elem_pos + 3));
		to_be_evaluated = part1 + part2 + part3;
	}
	ExpressionTree eval(to_be_evaluated);
	shared_ptr<Elem> image = eval.evaluate();
	if (codomain != nullptr && !codomain->has(*image)) program_vars::raise_error("Mapping unsuccessful. Pre-image not found in domain.");
	return image;
}
