#include "../Header Files/ExpressionTree.h"
#include <cmath>
#include <iostream>
using std::cout;
using std::endl;
using std::static_pointer_cast;


/* Implementations for methods in the classes Token, Node, and ExpressionTree. */

// -----------------------------------------------------<CLASS TOKEN>------------------------------------------//

string token_name[] = 
{
	"INT_LIT", "LOGICAL_LIT", "CHAR_LIT", "STRING_LIT", "SET_LIT", "TUPLE_LIT", "LITERAL",
	"INDEX", "IDENTIFIER", "OP", "UNARY", "END", "ERROR", "EXPR","TYPE", "MAPPING_SYMBOL", 
	"INPUT", "OUTPUT", "IF", "ELSEIF", "ELSE", "WHILE", "DECLARE", "EQUAL_SIGN", "L_BRACE", 
	"R_BRACE", "QUIT", "DELETE", "DELETE_ELEMS", "MAP_OP", "COLON", "LET", "UNDER", "ABSTRACT", "PRINTR"
};

string Token::to_string()
{
	string rep = "TOKEN(" + lexeme + ", {";
	for (auto &type : types)
		rep += token_name[type] + ", ";
	rep += "})";
	return rep;
}

// -----------------------------------------------------</CLASS TOKEN>------------------------------------------//


// ------------------------------------------------------<CLASS NODE>-------------------------------------------//

shared_ptr<Elem> Node::parse_literal()		// Parses the token.lexeme to get a value, if the lexeme is a literal.
{
	if (this->token.types[0] != LITERAL) 
		return nullptr;			// If the token is not a literal, ignore it.
	
	if (this->token.types[1] == INT_LIT)			
		return shared_ptr<Elem>{new Int(token.lexeme)};

	if (this->token.types[1] == CHAR_LIT)			
		return shared_ptr<Elem>{new Char(token.lexeme)};

	if (this->token.types[1] == LOGICAL_LIT)
		return shared_ptr<Elem>{new Logical(token.lexeme)};
		
	if (this->token.types[1] == STRING_LIT)
		return shared_ptr<Elem>{new String(token.lexeme, 0)};	// The 0 => the string being passed is a representation of the object.
									// (as opposed to its value).
	if (this->token.types[1] == SET_LIT)
		return shared_ptr<Elem>{new Set(token.lexeme)};

	if (this->token.types[1] == TUPLE_LIT)
		return shared_ptr<Elem>{new Tuple(token.lexeme)};

	return nullptr;
}


// -----------------------------------------------------</CLASS NODE>-------------------------------------------//


// -------------------------------------------------<CLASS EXPRESSIONTREE>--------------------------------------//

void ExpressionTree::skip_whitespace()
{								
	while ((current_index < expr.size()) && (isspace(expr[current_index])))	
	{
		current_index++;				// The char will most definitely NOT be a '\n' character.
	}
}

shared_ptr<Elem> ExpressionTree::evaluate()
{
	if (node->value != nullptr) 
		return node->value;				// Will be triggered in case of literals and identifiers.

	if (node->token.types[0] == OP)				// If the node is an op.
	{
		if (node->token.types.size() > 1 && node->token.types[1] == UNARY)
		{
			if (node->token.lexeme == ".")				// Deep_copy operator.
			{
				node->value = node->left->evaluate()->deep_copy();	// Get the deep_copy of the value of the left tree.
			}
			else if (node->token.lexeme == "()")
			{
				node->value = node->left->evaluate();
			}
			else if (node->token.lexeme == "|")
			{
				shared_ptr<Elem> get_size_of = node->left->evaluate();
				if (get_size_of->type == SET)
				{
					shared_ptr<Set> get_size_of_set = static_pointer_cast<Set>(get_size_of);
					node->value = shared_ptr<Int>{new Int(get_size_of_set->cardinality())};
				}
				else if (get_size_of->type == TUPLE)
				{
					shared_ptr<Tuple> get_size_of_tuple = static_pointer_cast<Tuple>(get_size_of);
					node->value = shared_ptr<Int>{new Int(get_size_of_tuple->size())};
				}
				else if (get_size_of->type == STRING)
				{
					shared_ptr<String> get_size_of_string = static_pointer_cast<String>(get_size_of);
					node->value = shared_ptr<Int>{new Int(get_size_of_string->elem.size())};
				}
				else if (get_size_of->type == MAP)
				{
					shared_ptr<Map> get_num_of_mappings = static_pointer_cast<Map>(get_size_of);
					node->value = shared_ptr<Int>{new Int(get_num_of_mappings->map->size())};
				}
				else program_vars::raise_error("Expected a string or a container for \"| |\" operation.");
			}
			else if (node->token.lexeme == "!")
			{
				shared_ptr<Elem> negate = node->left->evaluate();
				if (negate->type == LOGICAL)
				{
					shared_ptr<Logical> negate_this = static_pointer_cast<Logical>(negate);
					node->value = shared_ptr<Logical>{new Logical(!negate_this->elem)};
				}
				else if (negate->type == CHAR)
				{
					shared_ptr<Char> negate_this = static_pointer_cast<Char>(negate);
					node->value = shared_ptr<Logical>{new Logical(!negate_this->elem)};
				}	
				else if (negate->type != INT)
				{
					shared_ptr<Int> negate_this = static_pointer_cast<Int>(negate);
					node->value = shared_ptr<Logical>{new Logical(!negate_this->elem)};
				}
				else program_vars::raise_error("Expected a logical (or another primitive) expression for \"!\" operation.");
			}
		}
		else
		{
			if (node->token.lexeme == "?")
			{
				shared_ptr<Elem> cond = node->left->evaluate();				
				if (cond->type == LOGICAL)
				{
					if (static_pointer_cast<Logical>(cond)->elem) node->value = node->center->evaluate();
					else node->value = node->right->evaluate();
				}
				else if (cond->type == CHAR)
				{
					if (static_pointer_cast<Char>(cond)->elem) node->value = node->center->evaluate();
					else node->value = node->right->evaluate();
				}
				else if (cond->type == INT)
				{
					if (static_pointer_cast<Int>(cond)->elem) node->value = node->center->evaluate();
					else node->value = node->right->evaluate();
				}
				else program_vars::raise_error("Expected a logical (or another primitive) expression for the \"?\" operation.");

			}

			else if (node->token.lexeme == "V")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == INT)
				{
					shared_ptr<Int> l_int = static_pointer_cast<Int>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem || r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem || r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem || r_char->elem)};
					}
					else program_vars::raise_error("Expected a logical (or another primitive) expression for the \"V\" operation");
				}
				else if (left->type == LOGICAL)
				{
					shared_ptr<Logical> l_logical = static_pointer_cast<Logical>(left);
					if (right->type == INT)
					{
						shared_ptr<Int>r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem || r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical>r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem || r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char>r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem || r_char->elem)};
					}
					else program_vars::raise_error("Expected a logical (or another primitive) expression for the \"V\" operation");
				}
				else if (left->type == CHAR)
				{
					shared_ptr<Char> l_char = static_pointer_cast<Char>(left);
					if (right->type == INT)
					{
						shared_ptr<Int>r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem || r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical>r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem || r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char>r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem || r_char->elem)};
					}
					else program_vars::raise_error("Expected a logical (or another primitive) expression for the \"V\" operation");
				}
				else program_vars::raise_error("Expected a logical (or another primitive) expression for the \"V\" operation");
			}
			else if (node->token.lexeme == "&")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == ABSTRACT_SET && right->type == ABSTRACT_SET)
				{
					shared_ptr<AbstractSet> l_set = static_pointer_cast<AbstractSet>(left);
					shared_ptr<AbstractSet> r_set = static_pointer_cast<AbstractSet>(right);
					node->value = l_set->intersection(*r_set);
				}
				else if (left->type == SET && right->type == SET)
				{
					shared_ptr<Set> l_set = static_pointer_cast<Set>(left);
					shared_ptr<Set> r_set = static_pointer_cast<Set>(right);
					node->value = l_set->intersection(*r_set);
				}
				else if (left->type == AUTO && right->type == AUTO)
				{
					shared_ptr<Auto> l_auto = static_pointer_cast<Auto>(left);
					shared_ptr<Auto> r_auto = static_pointer_cast<Auto>(right);
					node->value = l_auto->accepts_intersection(r_auto);
				}
				if (left->type == INT)
				{
					shared_ptr<Int> l_int = static_pointer_cast<Int>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem && r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem && r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem && r_char->elem)};
					}
					else program_vars::raise_error("Expected a logical (or another primitive) expression for the \"&\" operation");
				}
				else if (left->type == LOGICAL)
				{
					shared_ptr<Logical> l_logical = static_pointer_cast<Logical>(left);
					if (right->type == INT)
					{
						shared_ptr<Int>r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem && r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical>r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem && r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char>r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem && r_char->elem)};
					}
					else program_vars::raise_error("Expected a logical (or another primitive) expression for the \"&\" operation");
				}
				else if (left->type == CHAR)
				{
					shared_ptr<Char> l_char = static_pointer_cast<Char>(left);
					if (right->type == INT)
					{
						shared_ptr<Int>r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem && r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical>r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem && r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char>r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem && r_char->elem)};
					}
					else program_vars::raise_error("Expected a logical (or another primitive) expression for the \"&\" operation");
				}
				else program_vars::raise_error("Expected a set, abtract set, logical, or another primitive expression for the \"&\" operation");
			}
			else if (node->token.lexeme == "in")
			{
				shared_ptr<Elem> left = node->left->evaluate(), right = node->right->evaluate();
				if (right->type == SET)
				{
					node->value = shared_ptr<Logical>{
						new Logical(static_pointer_cast<Set>(right)->has(*left))
					};
				}
				else if (right->type == ABSTRACT_SET)
				{
					node->value = shared_ptr<Logical>{
						new Logical(static_pointer_cast<AbstractSet>(right)->has(*left))
					};
				}
				else if (right->type == TUPLE)
				{
					node->value = shared_ptr<Logical>{
						new Logical(static_pointer_cast<Tuple>(right)->has(*left))
					};
				}
				else if (right->type == STRING)
				{
					if (left->type == CHAR)
					{
						node->value = shared_ptr<Logical> { 
							new Logical (
								static_pointer_cast<String>(right)->elem.find (
									static_pointer_cast<Char>(left)->elem
								) != string::npos
							)
						};
					}
					else if (left->type == STRING)
					{
						node->value = shared_ptr<Logical> {
							new Logical(
								static_pointer_cast<String>(right)->elem.find(
									static_pointer_cast<String>(left)->elem
								) != string::npos
							)
						};
					}
					else program_vars::raise_error("Expected a char or a string on the LHS for an \"in\" operation with a string on the RHS.");
				}
				else program_vars::raise_error("Expected a set, abstract set, tuple or string on the RHS for an \"in\" operation.");
			}
			else if (node->token.lexeme == "==")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				node->value = shared_ptr<Logical>{new Logical(*left == *right)};
			}
			else if (node->token.lexeme == "!=")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				node->value = shared_ptr<Logical>{new Logical(!(*left == *right))};
			}
			else if (node->token.lexeme == "<")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				node->value = shared_ptr<Logical>{new Logical(*left < *right)};
			}
			else if (node->token.lexeme == "<=")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				node->value = shared_ptr<Logical>{new Logical(*left <= *right)};
			}
			else if (node->token.lexeme == ">")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				node->value = shared_ptr<Logical>{new Logical(*left > *right)};
			}
			else if (node->token.lexeme == ">=")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				node->value = shared_ptr<Logical>{new Logical(*left >= *right)};
			}
			else if (node->token.lexeme == "o")
			{
				shared_ptr<Elem> f = node->left->evaluate();
				shared_ptr<Elem> g = node->right->evaluate();
				if (f->type == MAP && g->type == MAP)
					node->value = static_pointer_cast<Map>(f)->composed_with(*static_pointer_cast<Map>(g));
					
				else if (f->type == ABSTRACT_MAP && g->type == ABSTRACT_MAP)
					node->value = static_pointer_cast<AbstractMap>(f)->composed_with(static_pointer_cast<AbstractMap>(g));
				
				else program_vars::raise_error("Expected map or abstract map objects for a \"o\" operation.");
			}
			else if (node->token.lexeme == "c")
			{
				shared_ptr<Elem> f = node->left->evaluate();
				shared_ptr<Elem> g = node->right->evaluate();
				if (f->type == SET && g->type == SET) 
				{
					node->value = shared_ptr < Logical > {
						new Logical (
							static_pointer_cast<Set>(f)->subset_of(*static_pointer_cast<Set>(g))
						)
					};
				}
				else if (f->type == SET && g->type == ABSTRACT_SET)
				{
					node->value = shared_ptr < Logical > {
						new Logical(
							static_pointer_cast<AbstractSet>(g)->superset_of(*static_pointer_cast<Set>(f))
						)
					};
				}
				else program_vars::raise_error("Expected set or abstract set objects for a \"c\" operation.");
			}
			else if (node->token.lexeme == "x")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == SET && right->type == SET)
					node->value = static_pointer_cast<Set>(left)->cartesian_product(
						*static_pointer_cast<Set>(right)
					);

				else if (left->type == ABSTRACT_SET && right->type == ABSTRACT_SET)
					node->value = static_pointer_cast<AbstractSet>(left)->cartesian_product(
						*static_pointer_cast<AbstractSet>(right)
					); 

				else program_vars::raise_error("Expected set or abstract set objects for a \"x\" operation.");
			}
			else if (node->token.lexeme == "U")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == SET && right->type == SET)
					node->value = static_pointer_cast<Set>(left)->_union(
						*static_pointer_cast<Set>(right)
					);

				else if (left->type == AUTO && right->type == AUTO)
					node->value = static_pointer_cast<Auto>(left)->accepts_union(
							static_pointer_cast<Auto>(right)
					);

				else if (left->type == ABSTRACT_SET && right->type == ABSTRACT_SET)
					node->value = static_pointer_cast<AbstractSet>(left)->_union(
						*static_pointer_cast<AbstractSet>(right)
					);
				else program_vars::raise_error("Expected sets, abstract sets, our automata for a \"x\" operation.");
			}
			else if (node->token.lexeme == "\\")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == SET && right->type == SET)
					node->value = static_pointer_cast<Set>(left)->exclusion(
						*static_pointer_cast<Set>(right)
					);

				else if (left->type == AUTO && right->type == AUTO)
					node->value = static_pointer_cast<Auto>(left)->accepts_exclusively(
						static_pointer_cast<Auto>(right)
					);

				else if (left->type == ABSTRACT_SET && right->type == ABSTRACT_SET)
					node->value = static_pointer_cast<AbstractSet>(left)->exclusion(
						*static_pointer_cast<AbstractSet>(right)
					);
				else program_vars::raise_error("Expected sets, abstract sets, our automata for a \"\\\" operation.");
			}
			else if (node->token.lexeme == "[]")
			{
				shared_ptr<Elem> elem = node->left->evaluate();
				shared_ptr<Elem> query = node->right->evaluate();
				if (elem->type == SET && query->type == INT)
				{
					shared_ptr<Set> e = static_pointer_cast<Set>(elem);
					shared_ptr<Int> q = static_pointer_cast<Int>(query);
					node->value = (*e)[q->elem];
				}
				else if (elem->type == SET && query->type == TUPLE)
				{
					shared_ptr<Set> e = static_pointer_cast<Set>(elem);
					shared_ptr<Tuple> q = static_pointer_cast<Tuple>(query);
					shared_ptr<Int> start = static_pointer_cast<Int>((*q)[0]);
					shared_ptr<Int> end = static_pointer_cast<Int>((*q)[1]);
					node->value = e->subset(start->elem, end->elem);				
				}
				else if (elem->type == ABSTRACT_MAP)
				{
					shared_ptr<AbstractMap> map = static_pointer_cast<AbstractMap>(elem);
					node->value = (*map)[*query];
				}
				else if (elem->type == TUPLE && query->type == INT)
				{
					shared_ptr<Tuple> e = static_pointer_cast<Tuple>(elem);
					shared_ptr<Int> q = static_pointer_cast<Int>(query);
					node->value = (*e)[q->elem];
				}
				else if (elem->type == MAP)
				{
					shared_ptr<Map> map = static_pointer_cast<Map>(elem);
					node->value = (*map)[*query];
				}
				else if (elem->type == AUTO && query->type == STRING)
				{
					shared_ptr<Auto> auto_ = static_pointer_cast<Auto>(elem);
					shared_ptr<String> q = static_pointer_cast<String>(query);
					node->value = (*auto_)[*q];
				}
				else program_vars::raise_error("Expected a suitable data type for a \"[]\" operation.");
			}
			else if (node->token.lexeme == "+")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == INT)
				{
					shared_ptr<Int> l_int = static_pointer_cast<Int>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem + r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem + r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem + r_char->elem)};
					}
					else program_vars::raise_error("Expected an int (or another primitive) for a \"+\" operation.");
				}
				else if (left->type == LOGICAL)
				{
					shared_ptr<Logical> l_logical = static_pointer_cast<Logical>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem + r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem + r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem + r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"+\" operation.");
				}
				else if (left->type == CHAR)
				{
					shared_ptr<Char> l_char = static_pointer_cast<Char>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem + r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem + r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem + r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"+\" operation.");
				}
				else if (left->type == STRING)
				{
					shared_ptr<String> l_str = static_pointer_cast<String>(left);
					if (right->type == STRING)
					{	
						shared_ptr<String> r_str = static_pointer_cast<String>(right);
						return shared_ptr<String>{new String(l_str->elem + r_str->elem)};
					}
					else if (right->type == CHAR)
					{	
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);

						shared_ptr<String> r_str = static_pointer_cast<String>(right);
						return shared_ptr<String>{new String(l_str->elem + r_str->elem)}; 
					}
					else program_vars::raise_error("Expected a string or a char for a \"+\" operation with a string.");
				}
				else program_vars::raise_error("Expected a primitive or a string for a \"+\" operation.");
			}
			else if (node->token.lexeme == "-")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == INT)
				{
					shared_ptr<Int> l_int = static_pointer_cast<Int>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem - r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem - r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem - r_char->elem)};
					}
					else program_vars::raise_error("Expected an int (or another primitive) for a \"-\" operation.");
				}
				else if (left->type == LOGICAL)
				{
					shared_ptr<Logical> l_logical = static_pointer_cast<Logical>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem - r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem - r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem - r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"-\" operation.");
				}
				else if (left->type == CHAR)
				{
					shared_ptr<Char> l_char = static_pointer_cast<Char>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem - r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem - r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem - r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"-\" operation.");
				}
				else program_vars::raise_error("Expected a primitive for a \"-\" operation.");
			}
			else if (node->token.lexeme == "*")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == INT)
				{
					shared_ptr<Int> l_int = static_pointer_cast<Int>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem * r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem * r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem * r_char->elem)};
					}
					else program_vars::raise_error("Expected an int (or another primitive) for a \"*\" operation.");
				}
				else if (left->type == LOGICAL)
				{
					shared_ptr<Logical> l_logical = static_pointer_cast<Logical>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem * r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem * r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem * r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"*\" operation.");
				}
				else if (left->type == CHAR)
				{
					shared_ptr<Char> l_char = static_pointer_cast<Char>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem * r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem * r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem * r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"*\" operation.");
				}
				else program_vars::raise_error("Expected a primitive for a \"*\" operation.");
			}
			else if (node->token.lexeme == "/")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == INT)
				{
					shared_ptr<Int> l_int = static_pointer_cast<Int>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem / r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem / r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem / r_char->elem)};
					}
					else program_vars::raise_error("Expected an int (or another primitive) for a \"/\" operation.");
				}
				else if (left->type == LOGICAL)
				{
					shared_ptr<Logical> l_logical = static_pointer_cast<Logical>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem / r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem / r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem / r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"/\" operation.");
				}
				else if (left->type == CHAR)
				{
					shared_ptr<Char> l_char = static_pointer_cast<Char>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem / r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem / r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem / r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"/\" operation.");
				}
				else program_vars::raise_error("Expected a primitive for a \"/\" operation.");
			}
			else if (node->token.lexeme == "^")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == INT)
				{
					shared_ptr<Int> l_int = static_pointer_cast<Int>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Int>{new Int(pow(l_int->elem, r_int->elem))};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Int>{new Int(pow(l_int->elem, r_logical->elem))};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Int>{new Int(pow(l_int->elem, r_char->elem))};
					}
					else program_vars::raise_error("Expected an int (or another primitive) for a \"^\" operation.");
				}
				else if (left->type == LOGICAL)
				{
					shared_ptr<Logical> l_logical = static_pointer_cast<Logical>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Logical>{new Logical(pow(l_logical->elem, r_int->elem))};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Logical>{new Logical(pow(l_logical->elem, r_logical->elem))};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Logical>{new Logical(pow(l_logical->elem, r_char->elem))};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"^\" operation.");
				}
				else if (left->type == CHAR)
				{
					shared_ptr<Char> l_char = static_pointer_cast<Char>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Char>{new Char(pow(l_char->elem, r_int->elem))};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Char>{new Char(pow(l_char->elem, r_logical->elem))};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Char>{new Char(pow(l_char->elem, r_char->elem))};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"^\" operation.");
				}
				else program_vars::raise_error("Expected a primitive for a \"^\" operation.");
			}
			else if (node->token.lexeme == "%")
			{
				shared_ptr<Elem> left = node->left->evaluate();
				shared_ptr<Elem> right = node->right->evaluate();
				if (left->type == INT)
				{
					shared_ptr<Int> l_int = static_pointer_cast<Int>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem % r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem % r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Int>{new Int(l_int->elem % r_char->elem)};
					}
					else program_vars::raise_error("Expected an int (or another primitive) for a \"%\" operation.");
				}
				else if (left->type == LOGICAL)
				{
					shared_ptr<Logical> l_logical = static_pointer_cast<Logical>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem % r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem % r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Logical>{new Logical(l_logical->elem % r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"%\" operation.");
				}
				else if (left->type == CHAR)
				{
					shared_ptr<Char> l_char = static_pointer_cast<Char>(left);
					if (right->type == INT)
					{
						shared_ptr<Int> r_int = static_pointer_cast<Int>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem % r_int->elem)};
					}
					else if (right->type == LOGICAL)
					{
						shared_ptr<Logical> r_logical = static_pointer_cast<Logical>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem % r_logical->elem)};
					}
					else if (right->type == CHAR)
					{
						shared_ptr<Char> r_char = static_pointer_cast<Char>(right);
						node->value = shared_ptr<Char>{new Char(l_char->elem % r_char->elem)};
					}
					else program_vars::raise_error("Expected an logical (or another primitive) for a \"%\" operation.");
				}
				else program_vars::raise_error("Expected a primitive for a \"%\" operation.");
			}
		}
	}
	return node->value;
}

Token ExpressionTree::get_next_token()				// The limited lexical analyzer to parse only expressions.
{
	skip_whitespace();					// Skip whitespace, of course.

	if (current_index >= expr.size())			// If the current_index is now beyond the expr's length ...
	{
		return{ "", {END} };				// ... return a token initializer list with a blank lexeme, and type END.
	}
	else if (expr[current_index] == '|')			// Cardinality op.
	{
		current_index++;
		return{ "|", {OP, UNARY} };
	}

	else if (expr[current_index] == ':')
	{
		current_index++;
		return{ ":", {COLON} };
	}

	else if (expr[current_index] == '?')
	{
		current_index++;
		return{ "?", {OP} };
	}

	else if (expr[current_index] == '.')
	{
		current_index++;
		return{ ".", { OP, UNARY } };
	}

	//-----------------------------------------<LOGICAL OPS>-----------------------------------------------//

	else if (expr[current_index] == '!')
	{
		if ((current_index + 1 < expr.size()) && expr[current_index + 1] == '=')
		{
			current_index += 2;
			return{ "!=", { OP } };
		}
		current_index++;
		return{ "!", { OP, UNARY } };
	}

	else if (expr[current_index] == 'V' && ((current_index + 1) < expr.size()) 
		&& !isalnum(expr[current_index + 1]) && expr[current_index + 1] != '_')
	{
		current_index++;
		return{ "V", { OP } };
	}

	else if (expr[current_index] == '=' && ((current_index + 1) < expr.size()) && expr[current_index + 1] == '=')
	{
		current_index += 2;
		return{ "==", { OP } };
	}

	else if (expr[current_index] == 'i' && ((current_index + 1) < expr.size()) && expr[current_index + 1] == 'n' 
	&& ((current_index + 2) < expr.size()) && !isalnum(expr[current_index + 2]) && expr[current_index + 2] != '_')
	{
		current_index += 2;
		return{ "in", { OP } };
	}

	//----------------------------------------</LOGICAL OPS>-----------------------------------------------//

	//----------------------------------------------<MAP OP>-----------------------------------------------//

	else if (expr[current_index] == 'o' && ((current_index + 1) < expr.size())
		&& !isalnum(expr[current_index + 1]) && expr[current_index + 1] != '_')			// Map compose op.
	{
		current_index++;
		return{ "o", { OP } };
	}

	//---------------------------------------------</MAP OP>-----------------------------------------------//

	//----------------------------------------------<SET OP>-----------------------------------------------//

	else if (expr[current_index] == 'c' && ((current_index + 1) < expr.size())
		&& !isalnum(expr[current_index + 1]) && expr[current_index + 1] != '_')			// Subset op.
	{
		current_index++; return{ "c", { OP } };
	}
	else if (expr[current_index] == 'x' && ((current_index + 1) < expr.size())
		&& !isalnum(expr[current_index + 1]) && expr[current_index + 1] != '_')			// Cartesian product op.
	{
		current_index++; return{ "x", { OP } };
	}
	else if (expr[current_index] == 'U' && ((current_index + 1) < expr.size())
		&& !isalnum(expr[current_index + 1]) && expr[current_index + 1] != '_')			// Union op.
	{
		current_index++; return{ "U", { OP } };
	}
	else if (expr[current_index] == '&')			// Intersection (AND) op.
	{
		current_index++; return{ "&", { OP } };
	}
	else if (expr[current_index] == '\\')			// Exclusion op.
	{
		current_index++; return{ "\\", { OP } };
	}

	//---------------------------------------------</SET OP>----------------------------------------------//

	//--------------------------------------------<LITERALS>----------------------------------------------//
	
	else if (isdigit(expr[current_index]) || 
		(expr[current_index] == '-' && current_index + 1 < expr.size() && isdigit(expr[current_index + 1]))) // Int literal.
	{                                         
		int i = current_index + 1;				// Start looking one space ahead of the current_index ...
		while (i < expr.size() && isdigit(expr[i]))		// ... and while you're finding more digits ...
			i++;						// ... keep looking.
		int j = current_index;					// Store the current_index, because it is about to be updated.
		current_index = i;					// Update the current index.
		return{ expr.substr(j, i-j), { LITERAL, INT_LIT} };     // Return a token initializer list with the int_literal lexeme.
	} 		                          
	
	else if (expr[current_index] == 'T' || expr[current_index] == 'F') // Logical Literals
	{
		if (expr[current_index] == 'T') {
			if (current_index + 3 < expr.size() &&	// Because if expr[i] == 'T', expr[i + 3] == 'e' for a True logical literal.
		           (expr.substr(current_index, 4) == "True"))
			{
				current_index += 4;
				return{ "True", { LITERAL, LOGICAL_LIT} };
			}
		}
		else if (expr[current_index] == 'F') {
			if (current_index + 4 < expr.size() &&  // Because if expr[i] == 'F', expr[i + 4] == 'e' for a False logical literal.
				(expr.substr(current_index, 5) == "False"))
			{
				current_index += 5;
				return{ "False", { LITERAL, LOGICAL_LIT} };
			}
		}									
	}
	else if (expr[current_index] == '\'')			// Char literals.
	{
		if (current_index + 2 < expr.size() && expr[current_index + 2] == '\'')
		{
			int j = current_index;
			current_index += 3;
			return{ expr.substr(j, 3), { LITERAL, CHAR_LIT } };
		}
		else if (current_index + 4 < expr.size() && expr[current_index + 1] == '\\' && expr[current_index + 4] == '\'')
		{
			int j = current_index;		
			current_index += 5;	
			return{ expr.substr(j, 5), { LITERAL, CHAR_LIT } };
		}
		else return{ "", {ERROR} };
	}
	else if (expr[current_index] == '"')			// String literals.
	{
		int i;
		for (i = current_index + 1; i < expr.size(); i++)	// We'll look for the closing backtick.
			if (expr[i] == '"' && (i == 0 || expr[i - 1] != '\\'))	// When we find a closing backtick. that is not preceded by an escape '\',
				break;					// break.
		int j = current_index;
		current_index = i + 1;
		return{ expr.substr(j, current_index - j), { LITERAL, STRING_LIT } };
	}
	else if (expr[current_index] == '{')			// Set literals.
	{
		int level = 0, i;
		bool in_string = false, closing_brace_found = false;
		for (i = current_index + 1; i < expr.size(); i++)
		{
			if (expr[i] == '}' && level == 0 && (i == 0 || expr[i - 1] != '\\'))
			{
				closing_brace_found = true;
				break;
			}
			if (((expr[i] == '"' && !in_string) || expr[i] == '{' || expr[i] == '(' || expr[i] == '[')
				&& (i == 0 || expr[i - 1] != '\\')) {
				level++;
				if (expr[i] == '"' && !in_string) in_string = true;
			}
			else if (((expr[i] == '"' && in_string) || expr[i] == '}' || expr[i] == ')' || expr[i] == ']')
				&& (i == 0 || expr[i - 1] != '\\')) {
				level--;
				if (expr[i] == '"' && in_string) in_string = false;
			}
		}
		if (!closing_brace_found) { return{ "", {ERROR} }; }
		else
		{
			int j = current_index;
			current_index = i + 1;
			return{ expr.substr(j, i - j + 1), {LITERAL, SET_LIT} };
		}
	}
	else if (expr[current_index] == '(')	  		// Now depending on the current character, we return a token.
	{					  		// It can either be (<expr>) or a tuple literal.
		int level = 0, i;
		bool in_string = false, is_tuple = false, closing_paren_found = false;;
		for (i = current_index + 1; i < expr.size(); i++)
		{
			if (((expr[i] == '"' && !in_string) || expr[i] == '{' || expr[i] == '(' || expr[i] == '[')
				&& (i == 0 || expr[i - 1] != '\\')) {
				level++;
				if (expr[i] == '"' && !in_string) in_string = true;
			}
			else if (((expr[i] == '"' && in_string) || expr[i] == '}' || expr[i] == ')' || expr[i] == ']')
				&& (i == 0 || expr[i - 1] != '\\')) {
				level--;
				if (expr[i] == '"' && in_string) in_string = false;
			}
			else if (expr[i] == ',' && level == 0)		// It's a tuple if a comma exists at level 0.
			{
				is_tuple = true;
				break;
			}
		}			
		level = 0;						// We'll look for the closing ')' now.
		in_string = false;
		for (i = current_index + 1; i < expr.size(); i++)
		{
			if (expr[i] == ')' && level == 0 && (i == 0 || expr[i - 1] != '\\'))
			{
				closing_paren_found = true;
				break;
			}
			if (((expr[i] == '"' && !in_string) || expr[i] == '{' || expr[i] == '(' || expr[i] == '[')
				&& (i == 0 || expr[i - 1] != '\\')) {
				level++;
				if (expr[i] == '"' && !in_string) in_string = true;
			}
			else if (((expr[i] == '"' && in_string) || expr[i] == '}' || expr[i] == ')' || expr[i] == ']')
				&& (i == 0 || expr[i - 1] != '\\')) {
				level--;
				if (expr[i] == '"' && in_string) in_string = false;
			}
		}
		if (!closing_paren_found) return{ "", {ERROR} };
		else if (is_tuple)
		{
			int j = current_index;
			current_index = i + 1;
			return{ expr.substr(j, i - j + 1), { LITERAL, TUPLE_LIT } };
		}
		else
		{
			int j = current_index;
			current_index = i + 1;
			return{ expr.substr(j + 1, i - j - 1), { EXPR } };
		}
	}
	//-------------------------------------------</LITERALS>----------------------------------------------//

	else if (expr[current_index] == '[')
	{
		int i, level = 0;				// We're going to look for closing bracket ']' at level 0.
		bool closingbracket_found = false, in_string = false;
		for (i = current_index + 1; i < expr.size(); i++)
		{
			if (expr[i] == ']' && level == 0 && (i == 0 || expr[i - 1] != '\\'))
			{
				closingbracket_found = true;
				break;
			}
			if (((expr[i] == '"' && !in_string) || expr[i] == '{' || expr[i] == '(' || expr[i] == '[')
				&& (i == 0 || expr[i - 1] != '\\')) {
				level++;
				if (expr[i] == '"' && !in_string) in_string = true;
			}
			else if (((expr[i] == '"' && in_string) || expr[i] == '}' || expr[i] == ')' || expr[i] == ']')
				&& (i == 0 || expr[i - 1] != '\\')) {
				level--;
				if (expr[i] == '"' && in_string) in_string = false;
			}
		}
		if (!closingbracket_found) return{ "", { ERROR } };
		else
		{
			int j = current_index;
			current_index = i + 1;	// The next lexeme starts after ']'
			return{ expr.substr(j + 1, i - j - 1), { INDEX } };
		}
	}
	
	else if (isalpha(expr[current_index]) || expr[current_index] == '_')	// Parsing identifier.
	{
		int i = current_index + 1;					// Going to look for the end of this identifier.
		while (isalnum(expr[i]) || expr[i] == '_') i++;			// Keep going  ...
		int j = current_index;						// Store the current_index ...
		current_index = i;						// ... because it is about to be updated.
		return{ expr.substr(j, i - j), {IDENTIFIER} };			// Return the token initializer list.
	}

	//--------------------------------------------<OPS>-----------------------------------------------//

	else if (expr[current_index] == '+')
	{
		current_index++;
		return{ "+", { OP } };
	}

	else if (expr[current_index] == '-')
	{
		current_index++;
		return{ "-", { OP } };
	}
	else if (expr[current_index] == '*')
	{
		current_index++;
		return{ "*", { OP } };
	}
	else if (expr[current_index] == '/')
	{
		current_index++;
		return{ "/", { OP } };
	}
	else if (expr[current_index] == '^')
	{
		current_index++;
		return{ "^", { OP } };
	}
	else if (expr[current_index] == '%')
	{
		current_index++;
		return{ "%", { OP } };
	}
	else if (expr[current_index] == '<')
	{
		if ((current_index + 1) < expr.size() && expr[current_index + 1] == '=')
		{
			current_index += 2;
			return{ "<=", { OP } };
		}
		else
		{
			current_index++;
			return{ "<", { OP } };
		}
	}

	else if (expr[current_index] == '>')
	{
		if ((current_index + 1) < expr.size() && expr[current_index + 1] == '=')
		{
			current_index += 2;
			return{ ">=", { OP } };
		}
		else
		{
			current_index++;
			return{ ">", { OP } };
		}
	}

	//-------------------------------------------</OPS>-----------------------------------------------//

	return{ "", {ERROR} };
}

/*	<expr> -->   (<expr>)
 *		   | !<expr>			# Negation.
 *		   | |<expr>|			# Cardinality.
 *		   | <expr> <op> <expr>		# Every op basically.
 *		   | <expr>[<expr>]		# Set, Tuple, String, Map, Auto query/access.
 *		   | <term>			
 *		
 *	<term> -->   <identifier>
		   | <literal>
 */	

ExpressionTree::ExpressionTree(string &expr)
{
	current_index = 0;
	this->expr = expr;
	Token t1 = this->get_next_token();
	if (t1.types[0] == END)					// End of recursion.
	{
		node = nullptr;
		return;
	}

	Token t2 = this->get_next_token();

	if (t2.types[0] == OP)					// If we have <expr> <op> <expr>
	{
		if (t2.lexeme == "?")
		{
			node = new Node();
			node->operator_node = true;
			node->token = t2;
			node->left = new ExpressionTree(t1.lexeme);		// The condition.
			Token t3 = get_next_token();
			Token t4 = get_next_token();
			if (t4.types[0] != COLON) program_vars::raise_error("Missing \":\" in the conditional operator.");
			Token t5 = get_next_token();
			node->center = new ExpressionTree(t3.lexeme);
			node->right = new ExpressionTree(t5.lexeme);
			return;
		}
		else
		{
			string rest = expr.substr(current_index, expr.size() - current_index);
			node = new Node();
			node->operator_node = true;
			node->token = t2;
			node->left = new ExpressionTree(t1.lexeme);
			node->right = new ExpressionTree(rest);
			return;
		}
	}

	if (t2.types[0] == INDEX)
	{
		string rest = expr.substr(current_index, expr.size() - current_index);
		if (rest == "")
		{
			node = new Node();
			node->operator_node = true;
			node->token = { "[]", { OP, INDEX } };
			node->left = new ExpressionTree(t1.lexeme);
			node->right = new ExpressionTree(t2.lexeme);
		}
		else
		{
			node = new Node();
			node->operator_node = true;
			node->token = { "()", { OP, UNARY, EXPR } };
			string arg = "((";
			arg += t1.lexeme;
			arg += (string)")[";
			arg += t2.lexeme;
			arg += (string)"])";
			node->left = new ExpressionTree(arg);
		}
		return;
	}

	/*
	 *	<expr> --> !<expr>
	 *	<expr> --> |<expr>|
	 */
	if (t1.types[0] == OP)							// If we've seen an operator.
	{
		if (t1.lexeme == "!")						// A NOT, so this will just precede an expression.
		{
			Token to_be_negated = t2;				// Get the expression to be negated.
			string rest {
				expr.substr(current_index, expr.size() - current_index),// Get the rest of the string.
			};
			if (rest == "") 					// rest == "" implies we're doing a primitive negation.
			{
				node = new Node();					
				node->operator_node = true;
				node->token = t1;
				node->left = new ExpressionTree(to_be_negated.lexeme);
			}
			else 
			{
				node = new Node();					 
				node->operator_node = true;
				node->token = { "()", {OP, UNARY, EXPR} };
				string arg = "(!(";
				arg += to_be_negated.lexeme;
				arg += (string)"))";
				arg += rest;
				node->left = new ExpressionTree (arg);
			}
			return;
		}
		if (t1.lexeme == "|")					// Okay, so we're gonna be checking the size of something.
		{
			Token get_size_of = t2;				// Generate the token whose size you want.
			Token get_second_pipe = get_next_token();
			string rest{
				expr.substr(current_index, expr.size() - current_index),// Get the rest of the string.
			};
			if (rest == "") 				// rest == "" implies we're doing a primitive get_size().
			{
				node = new Node();
				node->operator_node = true;
				node->token = t1;
				node->left = new ExpressionTree(get_size_of.lexeme);
			}
			else
			{
				node = new Node();
				node->operator_node = true;
				node->token = { "()", { OP, UNARY, EXPR } };
				string arg = "(|(";
				arg += get_size_of.lexeme;
				arg += (string)")|)";
				arg += rest;
				node->left = new ExpressionTree (arg);
			}
		}
		if (t1.lexeme == ".")
		{
			Token to_be_copied = t2;
			string rest{
				expr.substr(current_index, expr.size() - current_index),// Get the rest of the string.
			};
			if (rest == "") 						// rest == "" implies we're doing basic deep_copy().
			{
				node = new Node();
				node->operator_node = true;
				node->token = t1;
				node->left = new ExpressionTree(to_be_copied.lexeme);
			}
			else
			{
				node = new Node();
				node->operator_node = true;
				node->token = { "()", { OP, UNARY, EXPR } };
				string arg = "(.(";
				arg += to_be_copied.lexeme;
				arg += (string)"))";
				arg += rest;
				node->left = new ExpressionTree(arg);
			}
		}
		return;
	}
	/*
	 *	<expr> --> (<expr>)
	 */ 
	if (t1.types[0] == EXPR)							
	{
		node = new Node();				// Make a node node to hold the operator.
		node->operator_node = true;			// Mark it as an operator node.
		node->token = Token{ "()", {OP, UNARY, EXPR} }; // Let it hold the token. 
		node->left = new ExpressionTree(t1.lexeme);	// Let the left parse the <expr> in (<expr>).
		return;
	}

	if (t1.types[0] == IDENTIFIER)
	{
		node = new Node();
		node->operator_node = false;
		node->token = t1;
		node->value = (*program_vars::identify)[t1.lexeme];
		return;
	}

	if (t1.types[0] == LITERAL)
	{
		node = new Node();
		node->operator_node = false;
		node->token = t1;
		node->value = node->parse_literal();
		return;
	}
	
	// By now we've exhausted all the cases where a check was necessary on the first token.
}

ExpressionTree::~ExpressionTree()
{
	delete node;
}

// -------------------------------------------------<CLASS EXPRESSIONTREE>--------------------------------------//
