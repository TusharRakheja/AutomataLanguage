#include "../Header Files/AbstractSet.h"
#include "../Header Files/ExpressionTree.h"

#include <iostream>

using std::cout;
using std::endl;

/* Implementations for the methods in the AbstractSet class. */

AbstractSet::AbstractSet(string &setbuilder) : Elem(ABSTRACT_SET)	// Construct with a string representing the criteria.
{
	int index = 0;
	while  (setbuilder[index] != '|')  index++;			// Now until you find the '|' character, keep going.
	index++;							// Take the index back to separation + 1, because criteria starts there.
	while (isspace(setbuilder[index])) index++;			// Ignore extraneous spaces between '|' and the criteria.
	int cr_start = index;						// Now we must be at the index of the criteria's first character.
	index = setbuilder.size() - 1;					// We'll look for the criteria's end from the other end.
	while  (setbuilder[index] != '}')  index--;			// First look for the set's closing brace.
	index--;							// The criteria will definitely end before that.
	while (isspace(setbuilder[index])) index--;			// Ignore spaces.
	criteria = setbuilder.substr(cr_start, index - cr_start + 1);	// Phew.
}

shared_ptr<AbstractSet> AbstractSet::cartesian_product(AbstractSet &other)	// Returns the cartesian product of this set and the other set.
{
	string x = this->criteria;				// We'll replace all instances of 'elem' at level 0 with elem->to_string().
	int start = 0;
	bool in_string = false, in_char = false;
	vector<string> criteria1parts;				// The parts of the criteria without 'elem'
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
		else if (x[i] == 'e'
			&& (i == 0 || (!isalnum(x[i - 1]) && x[i - 1] != '_'))
			&& i + 3 < x.size()
			&& x.substr(i, 4) == "elem"
			&& !in_string
			&& (i + 4 >= x.size() || (!isalnum(x[i + 4]) && x[i + 4] != '_')))
		{
			criteria1parts.push_back(x.substr(start, i - start));
			i += 4;
			start = i;
		}
	}
	criteria1parts.push_back(x.substr(start, x.size() - start));

	string criteria_e1 = criteria1parts[0] + "((elem[0]))";
	for (int i = 1; i < criteria1parts.size() - 1; i++)
		criteria_e1 += criteria1parts[i] + "((elem)[0]))";
	criteria_e1 += criteria1parts[criteria1parts.size() - 1];

	x = other.criteria;
	start = 0;
	in_string = false; in_char = false;

	vector<string> criteria2parts;
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
		else if (x[i] == 'e'
			&& (i == 0 || (!isalnum(x[i - 1]) && x[i - 1] != '_'))
			&& i + 3 < x.size()
			&& x.substr(i, 4) == "elem"
			&& !in_string
			&& (i + 4 >= x.size() || (!isalnum(x[i + 4]) && x[i + 4] != '_')))
		{
			criteria2parts.push_back(x.substr(start, i - start));
			i += 4;
			start = i;
		}
	}
	criteria2parts.push_back(x.substr(start, x.size() - start));

	string criteria_e2 = criteria2parts[0] + "((elem[1]))";
	for (int i = 1; i < criteria2parts.size() - 1; i++)
		criteria_e2 += criteria2parts[i] + "((elem[1]))";
	criteria_e2 += criteria2parts[criteria2parts.size() - 1];

	shared_ptr<AbstractSet> cartesian = shared_ptr<AbstractSet>{new AbstractSet};
	cartesian->criteria = "(" + criteria_e1 + ") & (" + criteria_e2 + ")";
	return cartesian;
}


shared_ptr<AbstractSet> AbstractSet::exclusion(AbstractSet &exclude)	// Returns a set containing elements of this, minus those of the argument.
{
	shared_ptr<AbstractSet> exclusive = shared_ptr<AbstractSet>{new AbstractSet()};
	exclusive->criteria = "(" + this->criteria + ") & !(" + exclude.criteria + ")";
	return exclusive;
}

bool AbstractSet::has(Elem &elem)				// Returns true if the argument elem fulfils the membership criteria.
{
	string x = this->criteria;				// We'll replace all instances of 'elem' at level 0 with elem->to_string().
	int start = 0;
	bool in_string = false, in_char = false;
	vector<string> criteriaparts;				// The parts of the criteria without 'elem'
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
		else if (x[i] == 'e'
			 && (i == 0 || (!isalnum(x[i - 1]) && x[i - 1] != '_')) 
			 && i + 3 < x.size()
			 && x.substr(i, 4) == "elem" 
			 && !in_string 
			 && (i + 4 >= x.size() || (!isalnum(x[i + 4]) && x[i + 4] != '_')))
		{
			criteriaparts.push_back(x.substr(start, i - start)); 
			i += 4;
			start = i;		     
		}
	}
	criteriaparts.push_back(x.substr(start, x.size() - start));

	string to_be_evaluated = criteriaparts[0] + ((elem.identifier == "") ? elem.to_string_eval() : elem.identifier);
	for (int i = 1; i < criteriaparts.size() - 1; i++)
		to_be_evaluated += criteriaparts[i] + ((elem.identifier == "") ? elem.to_string_eval() : elem.identifier);
	to_be_evaluated += criteriaparts[criteriaparts.size() - 1];

	ExpressionTree eval(to_be_evaluated);
	bool answer = logical(eval.evaluate())->elem;
	return answer;
}

bool AbstractSet::superset_of(Set &candidate_subset)
{
	for (auto &elem : *candidate_subset.elems)
		if (!this->has(*elem))
			return false;
	return true;
}

shared_ptr<AbstractSet> AbstractSet::intersection(AbstractSet &exclude)	// Returns a set containing elements of this, minus those of the argument.
{
	shared_ptr<AbstractSet> intersection = shared_ptr<AbstractSet>{new AbstractSet()};
	intersection->criteria = "(" + this->criteria + ") & (" + exclude.criteria + ")";
	return intersection;
}

shared_ptr<AbstractSet> AbstractSet::_union(AbstractSet &exclude)		// Returns a set containing elements of this, minus those of the argument.
{
	shared_ptr<AbstractSet> _unified = shared_ptr<AbstractSet>{new AbstractSet()};
	_unified->criteria = "(" + this->criteria + ") V (" + exclude.criteria + ")";
	return _unified;
}