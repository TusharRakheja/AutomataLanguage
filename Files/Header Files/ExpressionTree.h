#ifndef EXPRESSION_TREE_H            
#define EXPRESSION_TREE_H           

#define ROOT 27			// Every expression tree that we explicitly make here will have this as argument as well. 
                                     
#include "ProgramVars.h"
#include "Auto.h"
#include "AbstractMap.h"

enum Token_type 
{ 
	INT_LIT, LOGICAL_LIT, CHAR_LIT, STRING_LIT, SET_LIT, TUPLE_LIT, LITERAL,
	INDEX, IDENTIFIER, OP, UNARY, END, ERROR, EXPR, TYPE, MAPPING_SYMBOL,
	INPUT, PRINT, IF, ELSEIF, ELSE, WHILE, DECLARE, EQUAL_SIGN, L_BRACE, 
	R_BRACE, QUIT, DELETE, DELETE_ELEMS, MAP_OP, COLON, LET, UNDER, ABSTRACT, PRINTR
};

class Token
{
public:
	string lexeme;
	vector<Token_type> types;
	string to_string();
};

class ExpressionTree;

class Node
{
public:
	Token token; 
	Elem * value;					// Every node in the expression tree will have a value based on its token.
	bool operator_node;				// To be set to true if the lexeme in the unit is an OP.
	bool at_root;					// Boolean to determine if the current root is at the root of the expression tree.
	ExpressionTree *left, *right, * center;		// The center will only be used 

	Node() { left = nullptr; right = nullptr; center = nullptr; operator_node = false; value = nullptr; }

	Elem * parse_literal();		// Parses the token.lexeme to get a value, if the lexeme is a literal.
	Elem * evaluate();		// Evaluates an expression.
	~Node()
	{
		if (left != nullptr) delete left; 
		if (right != nullptr) delete right;
		if (token.types[0] != IDENTIFIER && !at_root && value != nullptr) delete value;
	}
};

class ExpressionTree
{
public:
	string expr;			// The expression that the tree is going to parse.
	int current_index;		// The current index into the expression_string.
	Node * node;			// The root tree of the expression.
	bool is_root;			// Tells us whether the tree is the one that performs the final operation.
	void skip_whitespace();		// Skips whitespace in the expression.
	ExpressionTree(string &);	// Construct the tree given the expression.
	ExpressionTree(string &, int);	// Construct the tree given the expression, and mark it the root also.
	Elem * evaluate();		// Will evaluate the expression and return the result (Elem * in the root).
	Token get_next_token();		// The lexical analyzer (lexer) for the expression.
	~ExpressionTree();		 // Will think about this later.
};


#endif 