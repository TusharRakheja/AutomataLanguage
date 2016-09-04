#include "../Header Files/Tuple.h"
#include "../Header Files/Set.h"
#include "../Header Files/ExpressionTree.h"

#include <iostream>
using std::cout;
using std::endl;

/* Implementations for the methods in the class Tuple. */

using program_vars::DUMMYv;
using program_vars::op_signs_set;
using program_vars::op_signs_set_VERBOSE;

Tuple::Tuple() : Elem(TUPLE)				// Default constructor, empty tuple.
{
	elems = new vector<shared_ptr<Elem>>;
}

Tuple::Tuple(vector<shared_ptr<Elem>> * elems) : Elem(TUPLE)	// Tuple-ize an existing vector of element_pointers.
{
	this->elems = new vector<shared_ptr<Elem>>(*elems);
}

Tuple::Tuple(string &x) : Elem(TUPLE)				// Construct a set using a string representation of it.
{
	elems = new vector < shared_ptr<Elem> >;
	int start = 0;
	vector<string> elements;			// We're going to extract e1, e2 ... out of x = "{ e1, e2, ... }".	
	while (x[start] != '(')	start++;		// Look for the tuple's opening parenthesis.	
	start++;
	int st = start;
	vector<char> delims{{ ',', ')' }};
	for (int i : program_vars::findall_at_level_0(x.substr(start), ANY, DUMMYc, delims))
	{
		int j = i + st;					// Store the position of the comma.
		while (isspace(x[j - 1])) j--;			// Work back from there, to get a trimmed representation. 
		string elem = x.substr(start, j - start);
		if (!elem.empty())				// If the trimmed representation isn't empty.
			elements.push_back(elem);		// Push it to the vector of representations
		if (x[i + st] == ')') break;
		start = i + st + 1;				// The next element's representation will usually start from i + 1.
		while (isspace(x[start])) start++;		// But it may not, in case of extra spaces.
		if (x[start] == ')') break;
	}
	for (auto &rep : elements) // An important thing to remember is that, the elements can still be expressions.
	{
		// So first of all, we'll check if the element we're looking at (or rep. thereof) is an expression or not.
		bool seeing_expr = program_vars::find_at_level_0(rep, ANY, DUMMYc, op_signs_set);
		if (seeing_expr)
		{
			ExpressionTree expr(rep);
			this->elems->push_back(expr.evaluate());
		}
		else
		{
			if (rep[0] == '{')						// If the element to be parsed is a set ...
			{	
				int i = 0;
				bool set_literal = false;
				bool pipe_at_zero = program_vars::exists_at_level_0(rep.substr(1), !ANY, '|', DUMMYv);
				if (!pipe_at_zero) // If a '|' doesn't exist in the candidate_lit ... 
				{
					this->elems->push_back(shared_ptr<Elem>{new Set(rep)});
					continue;
				}
				// Get the part between '{' and '|', and if is empty or has any operator tokens, it's a SET_LIT. 
				string last_check = rep.substr(1, program_vars::find_at_level_0(rep.substr(1), !ANY, '|', DUMMYv));
				if (last_check.empty())
				{
					this->elems->push_back(shared_ptr<Elem>{new Set(rep)});
					continue;
				}
				int pipe_pos = program_vars::find_at_level_0(rep.substr(1), !ANY, '|', DUMMYv);
				string find_in = rep.substr(1, pipe_pos);
				if (program_vars::exists_at_level_0(find_in, ANY, DUMMYc, op_signs_set))
				{
					// We have found operator signatures at level 0. Let's scan them all to make sure they're legit.
					for (int i : program_vars::findall_at_level_0(find_in, ANY, DUMMYc, op_signs_set))
					{
						if (std::find(op_signs_set_VERBOSE.begin(), op_signs_set_VERBOSE.end(), find_in[i]) == op_signs_set_VERBOSE.end())
						{ // If it's not a verbose or "word-like" operator signature (i.e. NOT 't' for typeof, 'c' for c etc.)
							this->elems->push_back(shared_ptr<Elem>{new Set(rep)});
							set_literal = true;
							break;
							// No further checks needed. It's a set literal.
						}
						else
						{ // If it IS a verbose operator signature.
							int start = i, end = i + 1;
							while (isalnum(find_in[start - 1])) start--;
							while (isalnum(find_in[end])) end++;
							string candidate = find_in.substr(start, end - start);
							if (candidate == "in" || candidate == "typeof" || candidate == "x"
								|| candidate == "o" || candidate == "U" || candidate == "V" || candidate == "c")
							{  // If it DOES structurally resemble an identifier (can be a verbose op).
								// But it really is a verbose operator.
								this->elems->push_back(shared_ptr<Elem>{new Set(rep)});
								set_literal = true;
								break;
							}
						}
					}
				}
				if (!set_literal) this->elems->push_back(shared_ptr<Elem>{new AbstractSet(rep)});
			}
			else if (rep[0] == '(')
				this->elems->push_back(shared_ptr<Elem>{new Tuple(rep)});
			else if (isdigit(rep[0]))
				this->elems->push_back(shared_ptr<Elem>{new Int(rep)});
			else if (rep[0] == '\'')
				this->elems->push_back(shared_ptr<Elem>{new Char(rep)});
			else if (rep[0] == '"')
				this->elems->push_back(shared_ptr<Elem>{new String(rep, 0)});
			else if (rep == "True" || rep == "False")
				this->elems->push_back(shared_ptr<Elem>{new Logical(rep)});
			else if (rep[0] == ':')
			{
				string lambda = rep.substr(2, rep.size() - 4);
				cout << "Here: " << lambda << "\n";
				this->elems->push_back(shared_ptr<Elem>{new AbstractMap(lambda)});
			}
			else
			{	// Surely an identifier.
				ExpressionTree expr(rep);
				this->elems->push_back(expr.evaluate());
			}
		}
	}
}

Tuple::Tuple(vector<shared_ptr<Elem>> *elems, int direct_assign) : Elem(TUPLE) // Tuple-ize an existing vector of element_pointers (Direct Assign).
{
	this->elems = elems;
}

shared_ptr<Elem> Tuple::deep_copy()				// Returns a tuple which is a deep_copy of this tuple.
{
	shared_ptr<Tuple> clone = shared_ptr<Tuple>{new Tuple};	// Make an empty clone tuple.
	for (auto &elem_p1 : *elems)				// For every element pointed to in the vector of element_pointers in this ...
		clone->elems->push_back(elem_p1->deep_copy());  // ... tuple, add to the clone tuple a deep_copy (recursively gen) of it.
	return clone;						// Then return the clone.
}

bool Tuple::has(Elem &elem)				// Checks if a certain element is present in the tuple.
{	
	for (auto &elem_p1 : *elems)				// For every (any) element_pointer in the vector of element_pointers in this ...
		if (*elem_p1 == elem)				// ... if the element pointed to by it is equal to the query element, ...
			return true;				// ... return true.
	return false;						// Else return false.
}

const shared_ptr<Elem> Tuple::operator[](int index) const       // R-value access.
{
	return (*elems)[index];					// Return a reference to an element pointed to by the element_pointer at index. 
}

shared_ptr<Elem> Tuple::operator[](int index)			// L-value access.
{
	return (*elems)[index];					// Return a reference to an element pointed to by the element_pointer at index.
}

bool Tuple::operator==(Elem &other_tuple)			// Checks two tuples for equality.	
{
	if (other_tuple.type != TUPLE) return false;
	Tuple *other = (Tuple *)&other_tuple;
	if (size() != other->size())				// If the sizes are different ... 
		return false;					// ... then they are obviously not equal.
	for (int i{ 0 }; i < size(); i++)			// So the sizes of the tuples are the same. Now if at any index i ...
		if (!(*(*elems)[i] == *(*other->elems)[i]))	// ... the elements in the two tuples are different, then ...
			return false;				// ... then the tuples are not equal.
	return true;						// Otherwise they are.
}

int Tuple::size()					// Returns the size (or 'dimension') of this tuple.
{
	return elems->size();
}

string Tuple::to_string()				// Returns a string representation of the tuple.
{
	string representation{ "(" };
	int i{ 0 };
	for (auto &elem_p : *elems)
	{
		representation += elem_p->to_string();  // Recursive, awesome representations. ;)
		if (i != elems->size() - 1)
			representation += ", ";
		i++;
	}
	return representation + ")";
}

string Tuple::to_string_raw()				// Returns a string representation of the tuple.
{
	string representation{ "(" };
	int i{ 0 };
	for (auto &elem_p : *elems)
	{
		representation += elem_p->to_string_raw();  // Recursive, awesome representations. ;)
		if (i != elems->size() - 1)
			representation += ", ";
		i++;
	}
	if (i == 1) representation += ", ";
	return representation + ")";
}

string Tuple::to_string_eval()				// Returns a string representation of the tuple.
{
	if (identifier != "") return identifier;
	string representation{ "(" };
	int i{ 0 };
	for (auto &elem_p : *elems)
	{
		representation += elem_p->to_string_eval();  // Recursive, awesome representations. ;)
		if (i != elems->size() - 1)
			representation += ", ";
		i++;
	}
	if (i == 1) representation += ", ";
	return representation + ")";
}


Tuple::~Tuple()				  // Destructor.
{
	delete elems;			  // Delete the vector of (shared) element pointers before you delete the object.
}
