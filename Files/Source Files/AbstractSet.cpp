#include "../Header Files/AbstractSet.h"
#include "../Header Files/ExpressionTree.h"

//#include <iostream>
//using std::cout;
//using std::endl;

/* Implementations for the methods in the AbstractSet class. */

// Updates all holders in criteria1 and criteria2, and unpacks them into their respective vectors.
void update_unpack_criteria(const string & criteria1, const string & criteria2, vector<string> & criteria1parts, vector<string> & criteria2parts)
{
	vector<string> * criteria = &criteria2parts;		// Will point to criteria2parts first, and then criteria1parts.
	string x = criteria2;
	int extension = 2;
	while (extension--)
	{
		int start = 0;
		bool in_string = false, in_char = false;
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
					if (!(*program_vars::identify)[candidate_holder]) // and if it's not an identifier, it's a holder.
					{
						//cout << x.substr(start, i - start);
 						criteria->push_back(x.substr(start, i - start));
						string updated_holder = candidate_holder + std::to_string(extension + 1);
						//cout << updated_holder;
						criteria->push_back(updated_holder);
						start = i = j;
					}
					else i = j;
				}
				else i = j;
			}
		}
		//cout << x.substr(start, x.size() - start) << endl;
		criteria->push_back(x.substr(start, x.size() - start));
		if (extension == 1) { x = criteria1; criteria = &criteria1parts; }
	}
}

void AbstractSet::parse_holder_value_pairs(string &x, string &parent)
{
	if (x.find("(") == string::npos)			    // Only a single holder provided (eg n -> <expr>)
	{
		int start = 0, end = x.size() - 1;
		while (isspace(x[start])) start++;
		while (isspace(x[end])) end--;
		holder_value_pairs[x.substr(start, end - start + 1)] = "(x)";
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

AbstractSet::AbstractSet(string &setbuilder) : Elem(ABSTRACT_SET)	// Construct with a string representing the criteria.
{
	int start = setbuilder.find("{") + 1, end = setbuilder.find("|") - 1;
	while (isspace(setbuilder[start])) start++;
	while (isspace(setbuilder[end])) end--;
	input_format = setbuilder.substr(start, end - start + 1);
	int index = setbuilder.find("|") + 1;				// Take the index back to separation + 1, because criteria starts there.
	while (isspace(setbuilder[index])) index++;			// Ignore extraneous spaces between '|' and the criteria.
	int cr_start = index;						// Now we must be at the index of the criteria's first character.
	index = setbuilder.size() - 1;					// We'll look for the criteria's end from the other end.
	while  (setbuilder[index] != '}')  index--;			// First look for the set's closing brace.
	index--;							// The criteria will definitely end before that.
	while (isspace(setbuilder[index])) index--;			// Ignore spaces.
	criteria = setbuilder.substr(cr_start, index - cr_start + 1);	// Phew.
	//cout << "Criteria: " << criteria << endl;
	string first_parent = "(x)";					// For matching arguments.
	parse_holder_value_pairs(input_format, first_parent);
}

AbstractSet::AbstractSet(const char * _setbuilder) : Elem(ABSTRACT_SET)	// Construct with a cstring representing the criteria.
{
	string setbuilder(_setbuilder);
	int start = setbuilder.find("{") + 1, end = setbuilder.find("|") - 1;
	while (isspace(setbuilder[start])) start++;
	while (isspace(setbuilder[end])) end--;
	input_format = setbuilder.substr(start, end - start + 1);
	int index = setbuilder.find("|") + 1;				// Take the index back to separation + 1, because criteria starts there.
	while (isspace(setbuilder[index])) index++;			// Ignore extraneous spaces between '|' and the criteria.
	int cr_start = index;						// Now we must be at the index of the criteria's first character.
	index = setbuilder.size() - 1;					// We'll look for the criteria's end from the other end.
	while (setbuilder[index] != '}')  index--;			// First look for the set's closing brace.
	index--;							// The criteria will definitely end before that.
	while (isspace(setbuilder[index])) index--;			// Ignore spaces.
	criteria = setbuilder.substr(cr_start, index - cr_start + 1);	// Phew.
	string first_parent = "(x)";					// For matching arguments.
	parse_holder_value_pairs(input_format, first_parent);
}

shared_ptr<AbstractSet> AbstractSet::cartesian_product(AbstractSet &other)	// Returns the cartesian product of this set and the other set.
{
	shared_ptr<AbstractSet> cart_p = shared_ptr<AbstractSet>{new AbstractSet()};
	int extension = 1;	     // For every holder value pair { a : v(a) } in this, put { a1 : v(a1) } in the product's holder_value map.
	for (auto& pair : this->holder_value_pairs)
	{
		string v_a1 = pair.second.substr(0, pair.second.find("(x)")); // We can get v(a1) by replacing (x) in v(a) with ((x)[0]).
		v_a1 += "((x)[0])";
		v_a1 += pair.second.substr(pair.second.find("(x)") + 3);
		//cout << "Holder: " << pair.first + std::to_string(extension) << "\tValue: " << v_a1 << endl;
		cart_p->holder_value_pairs[pair.first + std::to_string(extension)] = v_a1;
	}
	extension = 2;               // For every holder value pair { a : v(a) } in other, put { a2 : v(a2) } in the product's holder_value map.
	for (auto& pair : other.holder_value_pairs)
	{
		string v_a2 = pair.second.substr(0, pair.second.find("(x)")); // We can get v(a2) by replacing (x) in v(a) with ((x)[1]).
		v_a2 += "((x)[1])";
		v_a2 += pair.second.substr(pair.second.find("(x)") + 3);
		//cout << "Holder: " << pair.first + std::to_string(extension) << "\tValue: " << v_a2 << endl;
		cart_p->holder_value_pairs[pair.first + std::to_string(extension)] = v_a2;
	}
	// Now for all a in the keys of this->holder_value_pairs, replace a with a1.
	vector<string> criteria1parts;				// The parts of this criteria. We'll update the holders in it.
	vector<string> criteria2parts;				// The parts of other's criteria. We'll update the holders in it.
	update_unpack_criteria(this->criteria, other.criteria, criteria1parts, criteria2parts);
	cart_p->criteria = "(";
	for (int i = 0; i < criteria1parts.size(); i++)
		cart_p->criteria += criteria1parts[i];
	cart_p->criteria += ") & ("; // criteria2  comes after the &.
	for (int i = 0; i < criteria2parts.size(); i++)
		cart_p->criteria += criteria2parts[i];
	cart_p->criteria += ")";

	cart_p->input_format = "(";
	cart_p->input_format += this->input_format;
	cart_p->input_format += ", ";
	cart_p->input_format += other.input_format;
	cart_p->input_format += ")";

	return cart_p;
}


shared_ptr<AbstractSet> AbstractSet::exclusion(AbstractSet &exclude)	// Returns a set containing elements of this, minus those of the argument.
{
	shared_ptr<AbstractSet> exclusive = shared_ptr<AbstractSet>{new AbstractSet()};
	int extension = 1;	     // For every holder value pair { a : v(a) } in this, put { a1 : v(a) } in the exclusive's holder_value map.
	for (auto& pair : this->holder_value_pairs)
	{
		//cout << "Holder: " << pair.first + std::to_string(extension) << "\tValue: " << pair.second << endl;
		exclusive->holder_value_pairs[pair.first + std::to_string(extension)] = pair.second;
	}
	extension = 2;               // For every holder value pair { a : v(a) } in other, put { a2 : v(a) } in the exclusive's holder_value map.
	for (auto& pair : exclude.holder_value_pairs)
	{
		//cout << "Holder: " << pair.first + std::to_string(extension) << "\tValue: " << pair.second << endl;
		exclusive->holder_value_pairs[pair.first + std::to_string(extension)] = pair.second;
	}
	// Now for all a in the keys of this->holder_value_pairs, replace a with a1.
	vector<string> criteria1parts;				// The parts of this criteria. We'll update the holders in it.
	vector<string> criteria2parts;				// The parts of other's criteria. We'll update the holders in it.
	update_unpack_criteria(this->criteria, exclude.criteria, criteria1parts, criteria2parts);
	exclusive->criteria = "(";
	for (int i = 0; i < criteria1parts.size(); i++)
		exclusive->criteria += criteria1parts[i];
	exclusive->criteria += ") & ! ("; // criteria2 comes after the & !.
	for (int i = 0; i < criteria2parts.size(); i++)
		exclusive->criteria += criteria2parts[i];
	exclusive->criteria += ")";

	exclusive->input_format = "";
	exclusive->input_format += this->input_format;
	exclusive->input_format += " = ";
	exclusive->input_format += exclude.input_format;

	return exclusive;
}

bool AbstractSet::has(Elem &query)				// Returns true if the argument elem fulfils the membership criteria.
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
		else if ((x[i] == '_' || isalpha(x[i])) && !in_string && !in_char)
		{
			int j = i;
			while (isalnum(x[j]) || x[j] == '_') j++;
			string candidate_holder = x.substr(i, j - i);
			//cout << candidate_holder << endl;
			if (!(*program_vars::keyword_ops)[candidate_holder]) // If this is not an operator or keyword.
			{
				if (!(*program_vars::identify)[candidate_holder]) // and if it's not an identifier, it's a holder.
				{
					criteriaparts.push_back(x.substr(start, i - start));
					string &holder = candidate_holder;
					string holder_value = holder_value_pairs[holder];

					holder_value = holder_value.substr(0, holder_value.find("(x)")) +
						query.to_string_eval() +
						holder_value.substr(holder_value.find("(x)") + 3);

					ExpressionTree holder_value_expr(holder_value);
					criteriaparts.push_back(holder_value_expr.evaluate()->to_string_eval());

					start = i = j;
				}
				else i = j;
			}
			else i = j;
		}
	}
	criteriaparts.push_back(x.substr(start, x.size() - start));

	string to_be_evaluated = "";
	for (int i = 0; i < criteriaparts.size(); i++)
		to_be_evaluated += criteriaparts[i];
	
	//cout << "Evaluating: " << to_be_evaluated << endl;

	ExpressionTree eval(to_be_evaluated);
	return logical(eval.evaluate())->elem;
}

bool AbstractSet::superset_of(Set &candidate_subset)
{
	for (auto &elem : *candidate_subset.elems)
		if (!this->has(*elem))
			return false;
	return true;
}

shared_ptr<AbstractSet> AbstractSet::intersection(AbstractSet &with) // Returns a set containing elements of this, minus those of the argument.
{
	shared_ptr<AbstractSet> intersect = shared_ptr<AbstractSet>{new AbstractSet()};
	int extension = 1;	     // For every holder value pair { a : v(a) } in this, put { a1 : v(a) } in the intersect's holder_value map.
	for (auto& pair : this->holder_value_pairs)
	{
		//cout << "Holder: " << pair.first + std::to_string(extension) << "\tValue: " << pair.second << endl;
		intersect->holder_value_pairs[pair.first + std::to_string(extension)] = pair.second;
	}
	extension = 2;               // For every holder value pair { a : v(a) } in other, put { a2 : v(a) } in the intersect's holder_value map.
	for (auto& pair : with.holder_value_pairs)
	{
		//cout << "Holder: " << pair.first + std::to_string(extension) << "\tValue: " << pair.second << endl;
		intersect->holder_value_pairs[pair.first + std::to_string(extension)] = pair.second;
	}

	// Now for all a in the keys of this->holder_value_pairs, replace a with a1.
	vector<string> criteria1parts;				// The parts of this criteria. We'll update the holders in it.
	vector<string> criteria2parts;				// The parts of other's criteria. We'll update the holders in it.
	update_unpack_criteria(this->criteria, with.criteria, criteria1parts, criteria2parts);
	intersect->criteria = "(";
	for (int i = 0; i < criteria1parts.size(); i++)
		intersect->criteria += criteria1parts[i];
	intersect->criteria += ") & ("; // criteria2 comes after the & !.
	for (int i = 0; i < criteria2parts.size(); i++)
		intersect->criteria += criteria2parts[i];
	intersect->criteria += ")";

	intersect->input_format = "";
	intersect->input_format += this->input_format;
	intersect->input_format += " = ";
	intersect->input_format += with.input_format;
	return intersect;
}

shared_ptr<AbstractSet> AbstractSet::_union(AbstractSet &with) 	// Returns a set containing elements of this, minus those of the argument.
{
	shared_ptr<AbstractSet> unified = shared_ptr<AbstractSet>{new AbstractSet()};
	int extension = 1;	     // For every holder value pair { a : v(a) } in this, put { a1 : v(a) } in the intersect's holder_value map.
	for (auto& pair : this->holder_value_pairs)
	{
		//cout << "Holder: " << pair.first + std::to_string(extension) << "\tValue: " << pair.second << endl;
		unified->holder_value_pairs[pair.first + std::to_string(extension)] = pair.second;
	}

	extension = 2;               // For every holder value pair { a : v(a) } in other, put { a2 : v(a) } in the intersect's holder_value map.
	for (auto& pair : with.holder_value_pairs)
	{
		//cout << "Holder: " << pair.first + std::to_string(extension) << "\tValue: " << pair.second << endl;
		unified->holder_value_pairs[pair.first + std::to_string(extension)] = pair.second;
	}

	// Now for all a in the keys of this->holder_value_pairs, replace a with a1.
	vector<string> criteria1parts;				// The parts of this criteria. We'll update the holders in it.
	vector<string> criteria2parts;				// The parts of other's criteria. We'll update the holders in it.
	update_unpack_criteria(this->criteria, with.criteria, criteria1parts, criteria2parts);
	unified->criteria = "(";
	for (int i = 0; i < criteria1parts.size(); i++)
		unified->criteria += criteria1parts[i];
	unified->criteria += ") V ("; // criteria2 comes after the & !.
	for (int i = 0; i < criteria2parts.size(); i++)
		unified->criteria += criteria2parts[i];
	unified->criteria += ")";
	
	unified->input_format = "";
	unified->input_format += this->input_format;
	unified->input_format += " = ";
	unified->input_format += with.input_format;
	return unified;
}