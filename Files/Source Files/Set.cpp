#include "../Header Files/Set.h"
#include "../Header Files/ExpressionTree.h"

#include <iostream>
using std::cout;
using std::endl;

/* Implementations for methods in the class Set. */

using program_vars::DUMMYv;
using program_vars::op_signs_set;
using program_vars::op_signs_set_VERBOSE;

Set::Set() : Elem(SET)								// Default constructor.
{
	elems = new vector < shared_ptr<Elem> >;				// A blank vector.
}

Set::Set(vector<shared_ptr<Elem>> *elems) : Elem(SET)				// It is possible to initialize a set with an existing vector (Copy pointers).
{
	(this->elems) = new vector<shared_ptr<Elem>>(*elems);			// Just make a new vector that contains all the pointers in the existing vector.
}

Set::Set(vector<shared_ptr<Elem>> *elems, int direct_assign) : Elem(SET)	// It is possible to initialize a set with an existing vector (Direct assign).
{
	this->elems = elems;							// Just make the set's vector_pointer point to that existing vector.
}

Set::Set(string &x) : Elem(SET)					// Construct a set using a string representation of it.
{
	this->elems = new vector<shared_ptr<Elem>>;
	int start = 0;
	vector<string> elements;			// We're going to extract e1, e2 ... out of x = "{ e1, e2, ... }".	
	while (x[start] != '{')	start++;		// Look for the set's opening brace.	
	start++;
	while (isspace(x[start])) start++;		// Once we've found the opening brace, remove the extra space before the first element.
	if (x[start] == '}') return;
	int st = start;
	vector<char> delims{{ ',', '}' }};
	for (int i : program_vars::findall_at_level_0(x.substr(start), ANY, DUMMYc, delims))
	{
		int j = i + st;					// Store the position of the comma.
		while (isspace(x[j - 1])) j--;			// Work back from there, to get a trimmed representation. 
		string elem = x.substr(start, j - start);
		if (!elem.empty())				// If the trimmed representation isn't empty.
			elements.push_back(elem);		// Push it to the vector of representations
		start = i + st + 1;				// The next element's representation will usually start from i + 1.
		while (isspace(x[start])) start++;		// But it may not, in case of extra spaces.
		if (x[start] == '}') break;
	}
	for (auto &rep : elements) // An important thing to remember is that, the elements can still be expressions.
	{	
		bool seeing_expr = program_vars::find_at_level_0(rep, ANY, DUMMYc, op_signs_set);
		if (seeing_expr) 
		{
			ExpressionTree expr(rep);
			this->elems->push_back(expr.evaluate());			
		}
		else 
		{
			if (rep[0] == '{')				// If the element to be parsed is a set or an abstract set...
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

int Set::cardinality()				// Returns the cardinality of the set.
{
        return elems->size();			// Which is just the number of elements in the set.
}

shared_ptr<Set> Set::cartesian_product(Set &other)	// Returns the cartesian product of this set and the other set.
{
	shared_ptr<Set> product = shared_ptr<Set>{new Set};	   // Make an empty set.
	for (auto &elem_p1 : *elems)				   // For every element_pointer in the vector of element_pointers in this set ...
		for (auto &elem_p2 : *(other.elems))		   // ... taken with every element pointer in the vector of element_pointers in the other ...
			product->elems->push_back(		   // ... <convoluted_code> Push into the cartesian_product (cart) ...
			shared_ptr<Elem> { new Tuple (		   // ... a pointer to a Tuple object, that is ... 
				new vector < shared_ptr<Elem> > {  // ... constructed using a vector initialized with ...    
					elem_p1, elem_p2	   // ... elem_p1 and elem_p2 ...
				},
				DIRECT_ASSIGN			   // ... and is directly assigned to the Tuple.
			)});					   // </convoluted_code>
	return product;
}

shared_ptr<Elem> Set::deep_copy()				// Returns a new set which is a deep-copy of this set.
{
	shared_ptr<Set> clone = shared_ptr<Set>{new Set};	// Make an empty clone set.
	for (auto &elem_p1 : *elems)				// For every element_pointer in the vector of element_pointers in this set ...
		clone->elems->push_back(elem_p1->deep_copy());  // ... push into the clone, a deep_copy of the object pointed to by the pointer. 
	return clone;						// Return a pointer to the clone.
}

shared_ptr<Set> Set::exclusion(Set &exclude)		// Returns a set containing the elements of this set, minus those of the argument. 
{
	shared_ptr<Set> exclusive = shared_ptr<Set>{new Set};	// Make an empty set.
	for (auto &elem_p1 : *elems)				// For every (any) element_pointer in the vector of element_pointers in this ...  
		if (!exclude.has(*elem_p1))			// ... if the element it points to is not present in the set to be excluded ...
			exclusive->elems->push_back(elem_p1);   // ... then add a pointer to that element in the exclusive set.
	return exclusive;
}

bool Set::has(Elem &elem)			// Looks for an element in the set.
{
	for (auto &elem_p : *elems)		// For each (any) element_pointer in the vector of element_pointers ...
		if (*elem_p == elem)		// ... if the object pointed to by that element_pointer is == the argument ...
			return true;		// ... then return true.
	return false;				// Else return false.
}

bool Set::homoset()				// Returns true if every element of this set has the same type.
{
	if (this->cardinality() == 0)		// Vacuously true.
		return true;	

	Type candidate_homotype = (*(this->elems))[0]->type; // The type of the first element in the set.
	for (auto &elem_p1 : *elems)			     // For every (any) element_pointer in the vector of element_pointers in this set...
		if (candidate_homotype != elem_p1->type)     // ... if the type of the object pointed to, is not equal to the candidate ...
			return false;			     // ... return false.
	return true;					     // Else return true.
}

Type Set::homotype()				// Returns the type of every element in this set (if it is a homoset).
{
	if (this->cardinality() == 0)			// If the cardinality is zero ...
		return NULLTYPE;			// ... return NULLTYPE.
	else return (*(this->elems))[0]->type;		// Otherwise return the first element's type.
}

shared_ptr<Set> Set::intersection(Set &other)		// Intersection with a second set.
{
	shared_ptr<Set> intersect = shared_ptr<Set>{new Set};      // Make an initially empty set since we don't know what we may have to add.
	for (int i{ 0 }; i < elems->size(); i++)                   // For every element_pointer in this set, check to see if ...
		if (other.has(*(*elems)[i]))                       // ... the element pointed to by it is in the other one too. If it is ...
			intersect->elems->push_back((*elems)[i]);  // ... then add a pointer to that element to their intersection.
	return intersect;
}

const shared_ptr<Elem> Set::operator[](int index) const	// R-value access.
{
	return (*elems)[index];			// Return a reference to an element pointed to by the element_pointer at index. 
}

shared_ptr<Elem> Set::operator[](int index)	// L-value access.
{
	return (*elems)[index];			// Return an element_pointer at index.     
}

bool Set::operator==(Elem &other_set)           // Checks two sets for equality.
{
	if (other_set.type != SET) return false;
	Set *other = (Set *)&other_set;
	if (cardinality() != other->cardinality())   // If the cardinalities are different ... 
		return false;                        // ... then they are obviously not equal.
	return this->subset_of(*other);              // But if they are the same, then if either is a subset of the other, they indeed are equal.
}

shared_ptr<Set> Set::subset(int start, int end)          // Returns a subset of the set containing elements in [start, end).
{
	shared_ptr<Set> subset = shared_ptr<Set>{new Set};      // Make an empty set.
	for (int i{ start }; i < end; i++)			// Start at the starting index, go up till the ending index.
		subset->elems->push_back((*elems)[i]);		// Add all the element_pointers between the limits to the subset.
	return subset;						// Then return the subset.
}

bool Set::subset_of(Set &candidate_superset)    // Checks if this set is a subset of the candidate_superset.
{
	for (auto &elem_p : *elems)                    // For every (any) element_pointer in this set's vector ...
		if (!candidate_superset.has(*elem_p))  // ... if the candidate_superset does not have a pointer to the object it refers to ...
			return false;                  // ... return false.
	return true;                                   // Else return true.
}

string Set::to_string()                         // Returns a string representation of the set.
{
	string representation{ "{" };
	int i{ 0 };
	for (auto &elem_p : *elems)
	{
		representation += elem_p->to_string();   // Recursive, awesome representations. ;)
		if (i != elems->size() - 1)
			representation += ", ";
		i++;
	}
	return representation + "}";
}

string Set::to_string_raw()                         // Returns a string representation of the set.
{
	string representation{ "{" };
	int i{ 0 };
	for (auto &elem_p : *elems)
	{
		representation += elem_p->to_string_raw();   // Recursive, awesome representations. ;)
		if (i != elems->size() - 1)
			representation += ", ";
		i++;
	}
	return representation + "}";
}

string Set::to_string_eval()				// Returns a string representation of the tuple.
{
	if (identifier != "") return identifier;
	string representation{ "{" };
	int i{ 0 };
	for (auto &elem_p : *elems)
	{
		representation += elem_p->to_string_eval();  // Recursive, awesome representations. ;)
		if (i != elems->size() - 1)
			representation += ", ";
		i++;
	}
	return representation + "}";
}

shared_ptr<Set> Set::_union(Set &other)			// Union with a second set.
{
	shared_ptr<Set> unified = shared_ptr<Set>{new Set(this->elems)};
	for (auto &elem_p : *(other.elems)) 		   // For every other element_pointer in the other set's vector ...
		if (!unified->has(*elem_p)) 		   // ... if the object pointed to by it is not already present in this set ...
			unified->elems->push_back(elem_p); // ... add a pointer to it to this set as well.
	return unified;
}

Set::~Set()					// Destructor.
{
	delete elems;				// Delete the vector of (shared) pointers to the elements before you delete the object.
}
