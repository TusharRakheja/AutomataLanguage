#include "../Header Files/Tuple.h"
#include "../Header Files/Set.h"
#include "../Header Files/ExpressionTree.h"

//#include <iostream>
//using std::cout;
//using std::endl;

/* Implementations for the methods in the class Tuple. */

vector<char> op_signs_tup = { // These characters will signify the presence of an operator.
	'+', '-', '*', '/', '^', '%', '\\', '.', 'U', 'i', '|',
	'?', 'V', '&', '=', '!', '<', '>', 'o', 'c', 'x', '[', '!'
}; // Just the first (often the only) characters in the operators. 


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
	bool in_string = false, in_char = false;
	vector<string> elements;			// We're going to extract e1, e2 ... out of x = "{ e1, e2, ... }".	
	while (x[start] != '(')	start++;		// Look for the set's opening brace.	
	start++;
	while (isspace(x[start])) start++;		// Once we've found the opening '(', remove the extra space before the first element.
	for (int i = start; i < x.size(); i++)
	{
		if (x[i] == ')' && level == 0) // Usually the closing ')' will be the last character in the string, but, just in case.
		{
			int j = i;					// Store the position of the comma.
			while (isspace(x[j - 1])) j--;			// Work back from there, to get a trimmed representation. 
			if (x.substr(start, j - 1) != "")		// If the trimmed representation isn't empty.
				elements.push_back(x.substr(start, j - start));	// Push it to the vector of representations
			break;
		}
		if (((x[i] == '"' && !in_string && !in_char) || (x[i] == '\'' && !in_char && !in_string) ||
			x[i] == '{' || x[i] == '(' || x[i] == '[') 
			&& (i == 0 || (x[i - 1] != '\\' || (x[i - 1] == '\\' && i - 2 >= 0 && x[i - 2] == '\\')))) 
		{
			level++;
			if (x[i] == '"' && !in_string && !in_char) in_string = true;
			if (x[i] == '\'' && !in_char && !in_string) in_char = true;
		}
		else if (((x[i] == '"' && in_string) || (x[i] == '\'' && in_char) ||
			x[i] == '}' || x[i] == ')' || x[i] == ']')
			&& (i == 0 || (x[i - 1] != '\\' || (x[i - 1] == '\\' && i - 2 >= 0 && x[i - 2] == '\\')))) 
		{
			level--;
			if (x[i] == '"' && in_string) in_string = false;
			if (x[i] == '\'' && in_char) in_char = false;
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
	for (auto &rep : elements) // An important thing to remember is that, the elements can still be expressions.
	{
		// So first of all, we'll check if the element we're looking at (or rep. thereof) is an expression or not.

		int level = 0;		  // An operator sign found at level 0 will tell us that we're looking at an expression.
		bool seeing_expr = false; // Will hold the result of our investigation with regards the above comment.
		bool in_string = false;	  // Helps us keep track of the level when strings are involved.
		bool in_char = false;     // Helps us keep track of the level when chars are involved.

		for (int i = 0; i < rep.size(); i++)
		{
			// If, it's level 0, and the character that we're looking at right now in the rep. is the sign of an operator.
			if (level == 0 && std::find(op_signs_tup.begin(), op_signs_tup.end(), rep[i]) != op_signs_tup.end())
			{
				seeing_expr = true;
				break;
			}
			if (((rep[i] == '"' && !in_string && !in_char) || (rep[i] == '\'' && !in_char && !in_string) ||
				rep[i] == '{' || rep[i] == '(' || rep[i] == '[')
				&&                                                                 // ... and is not escaped.
				(i == 0 || (rep[i - 1] != '\\' || (rep[i - 1] == '\\' && i - 2 >= 0 && rep[i - 2] == '\\'))))
			{
				level++;
				if (rep[i] == '"' && !in_string && !in_char) in_string = true;
				if (rep[i] == '\'' && !in_char && !in_string) in_char = true;
			}
			else if (((rep[i] == '"' && in_string) || (rep[i] == '\'' && in_char) ||
				rep[i] == '}' || rep[i] == ')' || rep[i] == ']')
				&& (i == 0 || (rep[i - 1] != '\\' || (rep[i - 1] == '\\' && i - 2 >= 0 && rep[i - 2] == '\\'))))
			{
				level--;
				if (rep[i] == '"' && in_string) in_string = false;
				if (rep[i] == '\'' && in_char) in_char = false;
			}
		}
		if (seeing_expr)
		{
			ExpressionTree expr(rep);
			this->elems->push_back(expr.evaluate());
		}
		else
		{
			if (rep[0] == '{')						// If the element to be parsed is a set ...
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