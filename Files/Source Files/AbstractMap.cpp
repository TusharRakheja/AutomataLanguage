#include "../Header Files/AbstractMap.h"
#include "../Header Files/ExpressionTree.h"
#include "../Header Files/ProgramVars.h"

#include <iostream>
using std::cout;
using std::endl;

/* Implementations for methods in AbstractMap. */

void AbstractMap::parse_holder_value_pairs(string &x, string &parent)
{
	if (x.find("(") == string::npos && x.find("{") == string::npos)     // Only a single holder provided (eg n -> <expr>)
	{
		int start = 0, end = x.size() - 1;
		while (isspace(x[start])) start++;
		while (isspace(x[end])) end--;
		holder_value_pairs[x.substr(start, end - start + 1)] = "(x)";
		//cout << "{ Holder = " << x.substr(start, end - start + 1) << " : Value = " << "(x)" << " }\n";
	}
	else
	{
		int start = 0, end = x.size() - 1, level = 0;
		vector<string> holders;				   // We're going to extract e1, e2 ... out of x = "(e1, (e2, e3), ... )".
		while (x[start] != '(' && x[start] != '{')
			start++;				   // Look for the opening character.

		char closing_char = (x[start] == '(') ? ')' : '}';
		start++;
		for (int i = start; i < x.size(); i++)
		{
			if (x[i] == closing_char && level == 0)   
			{ 
				int j = i - 1;
				while (isspace(x[j])) j--;
				while (isspace(x[start])) start++;
				if (x.substr(start, j - start + 1) != "")		// If the trimmed representation isn't empty.
				{	
					holders.push_back(x.substr(start, i - start));	// Push it to the vector of representations
				}				
				break;
			}
			else if (x[i] == '{' || x[i] == '(') level++;
			else if (x[i] == '}' || x[i] == ')') level--;
			else if (x[i] == ',' && level == 0)			// If we find a comma that delimits a holder representation ...
			{
				int j = i - 1;
				while (isspace(x[j])) j--;
				while (isspace(x[start])) start++;
				if (x.substr(start, j - start + 1) != "")		// If the trimmed representation isn't empty.
				{
					holders.push_back(x.substr(start, i - start));	// Push it to the vector of representations.
				}
				start = i + 1;					// The  next holder's representation will usually start from i + 1.
				while (isspace(x[start])) start++;		// But it may not, in case of extra spaces.
				if (x[start] == closing_char) break;
			}
		}
		for (int i = 0; i < holders.size(); i++)
		{
			auto &holder = holders[i];
			if (holder[0] == '_' || isalpha(holder[0]))
			{
				string value = parent;
				value += "[";
				value += std::to_string(i);
				value += "]";
				holder_value_pairs[holder] = value;
				//cout << "{ Holder = " << holder << " : Value = " << value << " }\n";
			}
			else
			{
				string new_parent = parent;
				new_parent += "[";
				new_parent += std::to_string(i);
				new_parent += "]";
				parse_holder_value_pairs(holder, new_parent);
			}
		}
	}
}

AbstractMap::AbstractMap(string &format_and_scheme) : Elem(ABSTRACT_MAP)
{
	this->domain = nullptr;
	this->codomain = nullptr;
	int start = 0;
	input_format = format_and_scheme.substr(0, format_and_scheme.find("->"));
	mapping_scheme = format_and_scheme.substr(format_and_scheme.find("->") + 2);
	string first_parent = "(x)";
	parse_holder_value_pairs(input_format, first_parent);
}

AbstractMap::AbstractMap(const char *_format_and_scheme) : Elem(ABSTRACT_MAP)
{
	string format_and_scheme(_format_and_scheme);
	this->domain = nullptr;
	this->codomain = nullptr;
	input_format = format_and_scheme.substr(0, format_and_scheme.find("->"));
	mapping_scheme = format_and_scheme.substr(format_and_scheme.find("->") + 2);
	string first_parent = "(x)";
	parse_holder_value_pairs(input_format, first_parent);
}

void AbstractMap::add_scheme(string &format_and_scheme)
{
	input_format = format_and_scheme.substr(0, format_and_scheme.find("->"));
	mapping_scheme = format_and_scheme.substr(format_and_scheme.find("->") + 2);
	string first_parent = "(x)";
	parse_holder_value_pairs(input_format, first_parent);
}

AbstractMap::AbstractMap(shared_ptr<AbstractSet> domain, shared_ptr<AbstractSet> codomain) : Elem(ABSTRACT_MAP)
{
	this->domain = domain;
	this->codomain = codomain;
}

AbstractMap::AbstractMap(shared_ptr<AbstractSet> domain, shared_ptr<AbstractSet> codomain, string &format_and_scheme) : Elem(ABSTRACT_MAP)
{	
	this->domain = domain; 
	this->codomain = codomain;
	input_format = format_and_scheme.substr(0, format_and_scheme.find("->"));
	mapping_scheme = format_and_scheme.substr(format_and_scheme.find("->") + 2);
	string first_parent = "(x)";
	parse_holder_value_pairs(input_format, first_parent);
}

shared_ptr<AbstractMap> AbstractMap::composed_with(shared_ptr<AbstractMap> g)	// Returns an abstract_map (this composed with g).
{
	// For the comments that follow, this == f. Just for convenience, really.
	
	// under f : elem -> f[elem], where f[elem] represents some transformation.
	// under g : elem -> g[elem], where g[elem] represents some other transformation.
	// We need to return f o g. under f o g : elem -> f[g[elem]]

	// In the whole of f, wherever we see an (x), we'll replace that with (g[(x)]).

	shared_ptr<AbstractMap> fog = shared_ptr<AbstractMap>{new AbstractMap()};

	for (auto pair : this->holder_value_pairs)
	{
		string fog_holder_value = g->to_string_eval() + "[";
		fog_holder_value += pair.second.substr(0, pair.second.find("(x)"));
		fog_holder_value += "(x)]";
		fog_holder_value += pair.second.substr(pair.second.find("(x)") + 3);
		fog->holder_value_pairs[pair.first] = fog_holder_value;
	}
	fog->input_format = g->input_format;
	fog->mapping_scheme = this->mapping_scheme;
	fog->domain = g->domain;
	fog->codomain = this->codomain;
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
		else if ((x[i] == '_' || isalpha(x[i])) && !in_string && !in_char)
		{
			int j = i;
			while (isalnum(x[j]) || x[j] == '_') j++;
			string candidate_holder = x.substr(i, j - i);
			if (!(*program_vars::keyword_ops)[candidate_holder]) // If this is not an operator or keyword.
			{
				//cout << x.substr(start, i - start);
				string &holder = candidate_holder;
				if (!holder_value_pairs[holder].empty())     // If it is empty, it'll be an identifier.
				{
					schemeparts.push_back(x.substr(start, i - start));
					string holder_value = holder_value_pairs[holder];

					holder_value = holder_value.substr(0, holder_value.find("(x)")) + 
						pre_image.to_string_eval() +
					holder_value.substr(holder_value.find("(x)") + 3);

					ExpressionTree holder_value_expr(holder_value);
					//cout << holder_value_expr.evaluate()->to_string_eval();
					schemeparts.push_back(holder_value_expr.evaluate()->to_string_eval());

					start = i = j;
				}
				else i = j;
			}
			else i = j;
		}
	}
	//cout << x.substr(start, x.size() - start) << endl;
	schemeparts.push_back(x.substr(start, x.size() - start));

	string to_be_evaluated = "";
	for (int i = 0; i < schemeparts.size(); i++)
		to_be_evaluated += schemeparts[i];

	//cout << "Evaluating: " << to_be_evaluated << endl;
	ExpressionTree eval(to_be_evaluated);
	shared_ptr<Elem> image = eval.evaluate();
	if (codomain != nullptr && !codomain->has(*image)) program_vars::raise_error("Mapping unsuccessful. Image not found in domain."); 
	return image;
}

const shared_ptr<Elem> AbstractMap::operator[](Elem & pre_image) const
{
	return (*this)[pre_image];
}
