#ifndef EXPRESSION_TREE_H            
#define EXPRESSION_TREE_H           

#include "ProgramVars.h"
#include "Auto.h"
#include "DataSource.h"
#include "DataSink.h"
#include "AbstractMap.h"
#include <cmath>

enum Token_type 
{ 
	INT_LIT, LOGICAL_LIT, CHAR_LIT, STRING_LIT, SET_LIT, TUPLE_LIT, LITERAL,
	INDEX, IDENTIFIER, OP, UNARY, END, ERROR, EXPR, TYPE, MAPPING_SYMBOL, SOURCE_OP,
	PRINT, IF, ELSEIF, ELSE, WHILE, DECLARE, L_BRACE, UPDATE_OP, GET,
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
	shared_ptr<Elem> value;				// Every node in the expression tree will have a value based on its token.
	bool operator_node;				// To be set to true if the lexeme in the unit is an OP.
	bool at_root;					// Boolean to determine if the current root is at the root of the expression tree.
	ExpressionTree *left, *right, * center;		// The center will only be used 

	Node() { left = nullptr; right = nullptr; center = nullptr; operator_node = false; value = nullptr; }

	shared_ptr<Elem> parse_literal();	// Parses the token.lexeme to get a value, if the lexeme is a literal.
	shared_ptr<Elem> evaluate();		// Evaluates an expression.
	~Node()
	{
		if (left != nullptr) delete left; 
		if (right != nullptr) delete right;
	}
};

class ExpressionTree
{
public:
	string expr;			// The expression that the tree is going to parse.
	int current_index;		// The current index into the expression_string.
	Node * node;			// The root tree of the expression.
	void skip_whitespace();		// Skips whitespace in the expression.
	ExpressionTree(string &);	// Construct the tree given the expression.
	shared_ptr<Elem> evaluate();	// Will evaluate the expression and return the result (Elem * in the root).
	Token get_next_token();		// The lexical analyzer (lexer) for the expression.
	~ExpressionTree();	        // Will think about this later.
};


#endif 