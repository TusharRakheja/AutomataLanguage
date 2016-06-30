#include "../Header Files/Tuple.h"
#include "../Header Files/Set.h"
#include "../Header Files/ExpressionTree.h"

/* Implementations for the methods in the class Tuple. */

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
	int level = 0, start = 0;
	bool in_string = false;
	vector<string> elements;			// We're going to extract e1, e2 ... out of x = "{ e1, e2, ... }".	
	while (x[start] != '(')	start++;		// Look for the set's opening brace.	
	start++;
	while (isspace(x[start])) start++;		// Once we've found the opening '(', remove the extra space before the first element.
	for (int i = start; i < x.size(); i++)
	{
		if (x[i] == ')' && (i <= 0 || x[i] != '\\') && level == 0) // Usually the closing ')' will be the last character in the string, but, just in case.
		{
			int j = i;					// Store the position of the comma.
			while (isspace(x[j - 1])) j--;			// Work back from there, to get a trimmed representation. 
			if (x.substr(start, j - 1) != "")		// If the trimmed representation isn't empty.
				elements.push_back(x.substr(start, j - start));	// Push it to the vector of representations
			break;
		}
		if (((x[i] == '"' && !in_string) || x[i] == '{' || x[i] == '(' || x[i] == '[')
			&& (i == 0 || x[i - 1] != '\\')) {
			level++;
			if (x[i] == '"' && !in_string) in_string = true;
		}
		else if (((x[i] == '"' && in_string) || x[i] == '}' || x[i] == ')' || x[i] == ']')
			&& (i == 0 || x[i - 1] != '\\')) {
			level--;
			if (x[i] == '"' && in_string) in_string = false;
		}
		else if (x[i] == ',' && level == 0)		// If we find a comma that delimits an elements representation ...
		{
			int j = i;					// Store the position of the comma.
			while (isspace(x[j - 1])) j--;			// Work back from there, to get a trimmed representation. 
			if (x.substr(start, j - 1) != "")		// If the trimmed representation isn't empty.
				elements.push_back(x.substr(start, j - start));	// Push it to the vector of representations.
			start = i + 1;					// The next element's representation will usually start from i + 1.
			while (isspace(x[start])) start++;		// But it may not, in case of extra spaces.
			if (x[start] == ')') break;
		}
	}
	for (auto &rep : elements)
	{
		if (rep[0] == '{')				// If the element to be parsed is a set ...
			this->elems->push_back(shared_ptr<Elem>{new Set(rep)});	// ... recursively parse that too.
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
		else
		{
			ExpressionTree expr(rep);
			this->elems->push_back(expr.evaluate());
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
		representation += ", ";
		i++;
	}
	return representation + ")";
}


Tuple::~Tuple()				  // Destructor.
{
	delete elems;			  // Delete the vector of (shared) element pointers before you delete the object.
}