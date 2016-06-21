#include "../Header Files/ExpressionTree.h"
#include <cmath>
#include <iostream>
using std::cout;
using std::endl;

/* Implementations for methods in the classes Token, Node, and ExpressionTree. */

// -----------------------------------------------------<CLASS TOKEN>------------------------------------------//

string token_name[] = 
{
	"INT_LIT", "LOGICAL_LIT", "CHAR_LIT", "STRING_LIT", "SET_LIT", "TUPLE_LIT", "LITERAL",
	"INDEX", "IDENTIFIER", "OP", "UNARY", "END", "ERROR", "EXPR","TYPE", "MAPPING_SYMBOL", 
	"INPUT", "OUTPUT", "IF", "ELSEIF", "ELSE", "WHILE", "DECLARE", "EQUAL_SIGN", "L_BRACE", 
	"R_BRACE", "QUIT", "DELETE", "DELETE_ELEMS", "MAP_OP", "COLON", "LET", "UNDER", "ABSTRACT"
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

Elem * Node::parse_literal()		// Parses the token.lexeme to get a value, if the lexeme is a literal.
{
	if (this->token.types[0] != LITERAL) 
		return nullptr;		// If the token is not a literal, ignore it.
	
	if (this->token.types[1] == INT_LIT)			
		return new Int(token.lexeme);			

	if (this->token.types[1] == CHAR_LIT)			
		return new Char(token.lexeme);

	if (this->token.types[1] == LOGICAL_LIT)
		return new Logical(token.lexeme);
		
	if (this->token.types[1] == STRING_LIT)
		return new String(token.lexeme, 0);		// The 0 means that the string being passed is a representation of the object.
								// (as opposed to its value).
	if (this->token.types[1] == SET_LIT)
		return new Set(token.lexeme);

	if (this->token.types[1] == TUPLE_LIT)
		return new Tuple(token.lexeme);

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

Elem * ExpressionTree::evaluate()
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
				Elem * get_size_of = node->left->evaluate();
				if (get_size_of->type == SET)
				{
					Set * get_size_of_set = (Set *)get_size_of;
					node->value = new Int(get_size_of_set->cardinality());
				}
				else if (get_size_of->type == TUPLE)
				{
					Tuple * get_size_of_tuple = (Tuple *)get_size_of;
					node->value = new Int(get_size_of_tuple->size());
				}
			}
			else if (node->token.lexeme == "!")
			{
				Logical * negate = (Logical *)node->left->evaluate();
				node->value = new Logical(!negate->elem);
			}
		}
		else
		{
			if (node->token.lexeme == "?")
			{
				Logical * condition = (Logical *)node->left->evaluate();
				if (condition->elem) node->value = node->center->evaluate();
				else node->value = node->right->evaluate();
			}

			else if (node->token.lexeme == "V")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == INT)
				{
					Int * l_int = (Int *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Int(l_int->elem || r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Int(l_int->elem || r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Int(l_int->elem || r_char->elem);
					}
				}
				else if (left->type == LOGICAL)
				{
					Logical * l_int = (Logical *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Logical(l_int->elem || r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Logical(l_int->elem || r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Logical(l_int->elem || r_char->elem);
					}
				}
				else if (left->type == CHAR)
				{
					Char * l_int = (Char *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Char(l_int->elem || r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Char(l_int->elem || r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Char(l_int->elem || r_char->elem);
					}
				}
			}
			else if (node->token.lexeme == "&")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == ABSTRACT_SET && right->type == ABSTRACT_SET)
					node->value = ((AbstractSet *)left)->intersection(*(AbstractSet *)right);
				else if (left->type == SET && right->type == SET)
				{
					Set * l_set = (Set *)left, *r_set = (Set *)right;
					node->value = l_set->intersection(*r_set);
				}
				else if (left->type == AUTO && right->type == AUTO)
				{
					Auto * l_auto = (Auto *)left, *r_auto = (Auto *)right;
					node->value = l_auto->accepts_intersection(r_auto);
				}
				else if (left->type == INT)
				{
					Int * l_int = (Int *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Int(l_int->elem && r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Int(l_int->elem && r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Int(l_int->elem && r_char->elem);
					}
				}
				else if (left->type == LOGICAL)
				{
					Logical * l_int = (Logical *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Logical(l_int->elem && r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Logical(l_int->elem && r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Logical(l_int->elem && r_char->elem);
					}
				}
				else if (left->type == CHAR)
				{
					Char * l_int = (Char *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Char(l_int->elem && r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Char(l_int->elem && r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Char(l_int->elem && r_char->elem);
					}
				}
			}
			else if (node->token.lexeme == "in")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (right->type == SET)
					node->value = new Logical(((Set *)right)->has(*left));
				else if (right->type == ABSTRACT_SET)
					node->value = new Logical(((AbstractSet *)right)->has(*left));
				else if (right->type == TUPLE)
					node->value = new Logical(((Tuple *)right)->has(*left));
			}
			else if (node->token.lexeme == "==")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				node->value = new Logical(*left == *right);
			}
			else if (node->token.lexeme == "<")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				node->value = new Logical(*left < *right);
			}
			else if (node->token.lexeme == "<=")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				node->value = new Logical(*left <= *right);
			}
			else if (node->token.lexeme == ">")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				node->value = new Logical(*left > *right);
			}
			else if (node->token.lexeme == ">=")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				node->value = new Logical(*left >= *right);
			}
			else if (node->token.lexeme == "o")
			{
				Elem * f = node->left->evaluate();
				Elem * g = node->right->evaluate();
				if (f->type == MAP && g->type == MAP)
					node->value = ((Map *)f)->composed_with(*(Map *)g);
				else if (f->type == ABSTRACT_MAP && g->type == ABSTRACT_MAP)
					node->value = ((AbstractMap *)f)->composed_with((AbstractMap *)g);
			}
			else if (node->token.lexeme == "c")
			{
				Elem * f = node->left->evaluate();
				Elem * g = node->right->evaluate();
				if (f->type == SET && g->type == SET)
					node->value = new Logical(((Set *)f)->subset_of(*(Set *)g));
				else if (f->type == SET && g->type == ABSTRACT_SET)
					node->value = new Logical(((AbstractSet *)g)->superset_of(*(Set *)f));
			}
			else if (node->token.lexeme == "x")
			{
				Elem * left =  node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == SET && right->type == SET)
					node->value = ((Set *)left)->cartesian_product(*(Set *)right);
				else if (left->type == ABSTRACT_SET && right->type == ABSTRACT_SET)
					node->value = ((AbstractSet *)left)->cartesian_product(*(AbstractSet *)right);
			}
			else if (node->token.lexeme == "U")
			{
				Elem * left = (Set *)node->left->evaluate();
				Elem * right = (Set *)node->right->evaluate();
				if (left->type == SET && right->type == SET)
					node->value = ((Set *)left)->_union(*(Set *)right);
				else if (left->type == AUTO && right->type == AUTO)
					node->value = ((Auto*)left)->accepts_union((Auto*)right);
				else if (left->type == ABSTRACT_SET && right->type == ABSTRACT_SET)
					node->value = ((AbstractSet *)left)->_union(*(AbstractSet *)right);
			}
			else if (node->token.lexeme == "\\")
			{
				Elem * left = (Set *)node->left->evaluate();
				Elem * right = (Set *)node->right->evaluate();
				if (left->type == SET && right->type == SET)
					node->value = ((Set *)left)->exclusion(*(Set *)right);
				else if (left->type == AUTO && right->type == AUTO)
					node->value = ((Auto*)left)->accepts_exclusively((Auto*)right);
				else if (left->type == ABSTRACT_SET && right->type == ABSTRACT_SET)
					node->value = ((AbstractSet *)left)->exclusion(*(AbstractSet *)right);
			}
			else if (node->token.lexeme == "[]")
			{
				Elem * elem = node->left->evaluate();
				Elem * query = node->right->evaluate();
				if (elem->type == SET && query->type == INT)
				{
					Set * e = (Set *)elem;
					Int * q = (Int *)query;
					node->value = (*e)[q->elem];
				}
				else if (elem->type == ABSTRACT_MAP)
				{
					AbstractMap * map = (AbstractMap *)elem;
					node->value = (*map)[*query];
				}
				else if (elem->type == TUPLE && query->type == INT)
				{
					Tuple * e = (Tuple *)elem;
					Int * q = (Int *)query;
					node->value = (*e)[q->elem];
				}
				else if (elem->type == MAP)
				{
					Map * map = (Map *)elem;
					node->value = (*map)[*query];
				}
				else if (elem->type == AUTO && query->type == STRING)
				{
					Auto * auto_ = (Auto *)elem;
					String * q = (String *)query;
					node->value = (*auto_)[*q];
				}
			}
			else if (node->token.lexeme == "+")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == INT)
				{
					Int * l_int = (Int *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Int(l_int->elem + r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Int(l_int->elem + r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Int(l_int->elem + r_char->elem);
					}
				}
				else if (left->type == LOGICAL)
				{
					Logical * l_int = (Logical *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Logical(l_int->elem + r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Logical(l_int->elem + r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Logical(l_int->elem + r_char->elem);
					}
				}
				else if (left->type == CHAR)
				{
					Char * l_int = (Char *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Char(l_int->elem + r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Char(l_int->elem + r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Char(l_int->elem + r_char->elem);
					}
				}
				else if (left->type == STRING)
				{
					if (right->type == STRING)
					{	
						string str = ((String*)left)->elem;
						str += ((String*)right)->elem;
						return new String(str);
					}
					else if (right->type == CHAR)
					{	
						string cha = ((String*)left)->elem;
						cha += ((Char*)right)->elem;
						return new String(cha);
					}
				}
			}
			else if (node->token.lexeme == "-")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == INT)
				{
					Int * l_int = (Int *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Int(l_int->elem - r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Int(l_int->elem - r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Int(l_int->elem - r_char->elem);
					}
				}
				else if (left->type == LOGICAL)
				{
					Logical * l_int = (Logical *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Logical(l_int->elem - r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Logical(l_int->elem - r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Logical(l_int->elem - r_char->elem);
					}
				}
				else if (left->type == CHAR)
				{
					Char * l_int = (Char *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Char(l_int->elem - r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Char(l_int->elem - r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Char(l_int->elem - r_char->elem);
					}
				}
			}
			else if (node->token.lexeme == "*")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == INT)
				{
					Int * l_int = (Int *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Int(l_int->elem * r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Int(l_int->elem * r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Int(l_int->elem * r_char->elem);
					}
				}
				else if (left->type == LOGICAL)
				{
					Logical * l_int = (Logical *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Logical(l_int->elem * r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Logical(l_int->elem * r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Logical(l_int->elem * r_char->elem);
					}
				}
				else if (left->type == CHAR)
				{
					Char * l_int = (Char *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Char(l_int->elem * r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Char(l_int->elem * r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Char(l_int->elem * r_char->elem);
					}
				}
			}
			else if (node->token.lexeme == "/")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == INT)
				{
					Int * l_int = (Int *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Int(l_int->elem / r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Int(l_int->elem / r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Int(l_int->elem / r_char->elem);
					}
				}
				else if (left->type == LOGICAL)
				{
					Logical * l_int = (Logical *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Logical(l_int->elem / r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Logical(l_int->elem / r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Logical(l_int->elem / r_char->elem);
					}
				}
				else if (left->type == CHAR)
				{
					Char * l_int = (Char *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Char(l_int->elem / r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Char(l_int->elem / r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Char(l_int->elem / r_char->elem);
					}
				}
			}
			else if (node->token.lexeme == "^")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == INT)
				{
					Int * l_int = (Int *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Int(pow(l_int->elem, r_int->elem));
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Int(pow(l_int->elem, r_logical->elem));
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Int(pow(l_int->elem, r_char->elem));
					}
				}
				else if (left->type == LOGICAL)
				{
					Logical * l_int = (Logical *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Logical(pow(l_int->elem, r_int->elem));
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Logical(pow(l_int->elem, r_logical->elem));
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Logical(pow(l_int->elem, r_char->elem));
					}
				}
				else if (left->type == CHAR)
				{
					Char * l_int = (Char *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Char(pow(l_int->elem, r_int->elem));
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Char(pow(l_int->elem, r_logical->elem));
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Char(pow(l_int->elem, r_char->elem));
					}
				}
			}
			else if (node->token.lexeme == "%")
			{
				Elem * left = node->left->evaluate();
				Elem * right = node->right->evaluate();
				if (left->type == INT)
				{
					Int * l_int = (Int *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Int(l_int->elem % r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Int(l_int->elem % r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Int(l_int->elem % r_char->elem);
					}
				}
				else if (left->type == LOGICAL)
				{
					Logical * l_int = (Logical *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Logical(l_int->elem % r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Logical(l_int->elem % r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Logical(l_int->elem % r_char->elem);
					}
				}
				else if (left->type == CHAR)
				{
					Char * l_int = (Char *)left;
					if (right->type == INT)
					{
						Int *r_int = (Int *)right;
						node->value = new Char(l_int->elem % r_int->elem);
					}
					else if (right->type == LOGICAL)
					{
						Logical *r_logical = (Logical *)right;
						node->value = new Char(l_int->elem % r_logical->elem);
					}
					else if (right->type == CHAR)
					{
						Char *r_char = (Char *)right;
						node->value = new Char(l_int->elem % r_char->elem);
					}
				}
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

ExpressionTree::ExpressionTree(string &expr, int is_root) : ExpressionTree(expr)
{
	this->is_root = true;
}

ExpressionTree::~ExpressionTree()
{
	if (!this->is_root)
		delete node;
	else
	{
		node->at_root = true;
		delete node;
	}
}

// -------------------------------------------------<CLASS EXPRESSIONTREE>--------------------------------------//
