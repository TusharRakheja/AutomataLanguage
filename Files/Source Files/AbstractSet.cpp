#include "../Header Files/AbstractSet.h"
#include "../Header Files/ExpressionTree.h"

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
	string criteria_e1 = this->criteria;			// Will replace all instances of elem in this criteria with (elem[0]).
	string criteria_e2 = other.criteria;			// Will replace all instances of elem in this criteria with (elem[1]).

	vector<string> criteria1;
	int pos = criteria_e1.find("elem");
	while (pos != string::npos)
	{
		criteria1.push_back(criteria_e1.substr(0, pos));
		criteria1.push_back("(elem[0])");
		criteria_e1 = criteria_e1.substr(pos + 4, criteria_e1.size() - (pos + 4));
		pos = criteria_e1.find("elem");
	}
	criteria1.push_back(criteria_e1);

	vector<string> criteria2;
	pos = criteria_e2.find("elem");
	while (pos != string::npos)
	{
		criteria2.push_back(criteria_e2.substr(0, pos));
		criteria2.push_back("(elem[1])");
		criteria_e2 = criteria_e2.substr(pos + 4, criteria_e2.size() - (pos + 4));
		pos = criteria_e2.find("elem");
	}
	criteria2.push_back(criteria_e2);

	criteria_e1 = "", criteria_e2 = "";

	for (auto part : criteria1) criteria_e1 += part;
	for (auto part : criteria2) criteria_e2 += part;

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
	string to_be_evaluated = this->criteria;		// We'll replace all instances of 'elem' with elem->to_string().
	while (to_be_evaluated.find("elem") != string::npos)	// Iteratively replace the instances in to_be_evaluated.
	{
		int elem_pos = to_be_evaluated.find("elem");
		string part1 = to_be_evaluated.substr(0, elem_pos);
		string part2 = elem.to_string_raw();
		string part3 = to_be_evaluated.substr(elem_pos + 4, to_be_evaluated.size() - (elem_pos + 4));
		to_be_evaluated = part1 + part2 + part3;
	}
	ExpressionTree eval(to_be_evaluated);
	bool answer = (std::static_pointer_cast<Logical>(eval.evaluate()))->elem;
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