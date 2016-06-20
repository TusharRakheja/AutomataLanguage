#include "../Header Files/ExpressionTree.h"
#include <iostream>
#include <fstream>

using std::cout;
using std::cin;
using std::endl;
using std::istream;
using std::ios;
using std::getline;
using std::ifstream;

unordered_map<string, Elem *> * program_vars::identify = new unordered_map<string, Elem *> { { "__prompt__", new String(">>> ") } };	
// Identifiers mapped to their objects.

using program_vars::identify;

Token current_token;		// The current token we're looking at. 
istream * program;		// The stream of text constituting the program.
int line_num = 1;		// Line number that we're looking at right now.
bool read_right_expr = false;	// Notes whether or not you're reading an expression on the right side of an '=' sign.
bool read_left_expr = false;	// Notes whether or not you're reading an expression on the right side of an '=' sign.
bool read_mapdom_expr = false;	// Notes whether or not you're reading an expression in the domain of the map (left of '-->' sign).

Token get_next_token();		// Lexer.
void parse_program();		// Parse the program.
void parse_statement();		// Parse a statement.
void parse_declaration();	// Parse a declaration.
void parse_initialization();	// Parse an initialization.
void parse_assignment();	// Parse an assignment.
void parse_input();		// Parse an input command.
void parse_print();		// Parse a print command.
void parse_mapping();		// Parse a single mapping.
void parse_if();		// Parse an if condition.
void parse_while();		// Parse a while loop.
void parse_delete();		// Parse a delete command.
void trim(string &);		// Trim a string (*sigh*).
void remove_comment(string&);   // Removes the comment at the end of the line.
bool identifier(string &);	// Returns true if a string is an identifier.
void raise_error(const char *);	// Raises an error and quits the program.
bool all_spaces(string &);	// Returns true if a string is full of spaces.
void print_info();		// Prints the license and other info.

int main(int argc, char **argv) 
{
	if (argc == 1) { print_info(); program = &cin; }
	else program = new std::ifstream(argv[1]);
	parse_program();
}

Token get_next_token()						// The lexer.
{
	string lexeme;
	if (program->eof()) return{ "", {END} };
	if (read_left_expr) 
	{
		getline(*program, lexeme, '=');			// Read the whole thing first;
		if (program == &cin) 
			program->unget();
		else
			program->seekg(-1L*(int)sizeof(char), ios::cur);	
		read_left_expr = false;
	}
	else if (read_right_expr)
	{
		getline(*program, lexeme, '\n');
		line_num++;
		remove_comment(lexeme);
		read_right_expr = false;
	}
	else if (read_mapdom_expr)
	{
		lexeme = "";
		char a, b, c;
		program->get(a);
		while (isspace(a)) 
		{
			if (a == '\n') 	line_num++;
			program->get(a);
		}
		while (true)
		{
			if (a == '-')
			{
				program->get(b); program->get(c);
				if (b == '-' && c == '>') 
				{
					if (program == &cin) 
					{
						program->unget();
						program->unget();
						program->unget();
						break;
					}
					else 
					{
						program->seekg(-3L * (int)sizeof(char), ios::cur);
						break;
					}
				}
				else 
				{
					if (program == &cin) { program->unget(); program->unget(); }
					else program->seekg(-2L * (int)sizeof(char), ios::cur);
				}
			}
			else 
			{
				lexeme += a;
				program->get(a);
			}
		}
		read_mapdom_expr = false;
	}
	else
	{
		lexeme = "";
		char c; 
		program->get(c);
		while (isspace(c)) 
		{
			if (c == '\n') 	line_num++;
			program->get(c);
		}
		if (c == '#')
		{
			while (c != '\n' && !program->eof()) program->get(c);
			return get_next_token();
		}
		while (!isspace(c) && !program->eof())
		{	
			lexeme += c;
			program->get(c);
		}
	}
	trim(lexeme);
	if (all_spaces(lexeme)) { return get_next_token(); }
	if	(lexeme == "quit") 	return{lexeme, {QUIT}};			// The quit token, so quit the program.
	else if (lexeme == "delete")	return{ lexeme, { DELETE } };		// The delete token.
	else if (lexeme == "delete_elems") return{ lexeme, { DELETE_ELEMS } };	// The delete elems token.
	else if (lexeme == "declare")	return{ lexeme, { DECLARE } };
	else if	(lexeme == "set" || lexeme == "string" || lexeme == "int" ||	// If it's a data_type token.
		 lexeme == "char" || lexeme == "tuple" || lexeme == "map" ||
	       	 lexeme == "logical" || lexeme == "auto")
		 return{ lexeme, { TYPE } };
	else if (lexeme == "=")		return{ lexeme, { EQUAL_SIGN } };
	else if (lexeme == "-->")	return{ lexeme, { MAPPING_SYMBOL } };
	else if (lexeme == "let")	return{ lexeme, { LET } };
	else if (lexeme == "under")	return{ lexeme, { UNDER } };
	else if (lexeme == ":")		return{ lexeme, { COLON } };
	else if (lexeme == "input")	return{ lexeme, { INPUT } };
	else if (lexeme == "print")	return{ lexeme, { PRINT } };
	else if (lexeme == "{")		return{ lexeme, { L_BRACE } };
	else if (lexeme == "}")		return{ lexeme, { R_BRACE } };
	else if (lexeme == "while")	return{ lexeme, { WHILE } };
	else if (lexeme == "if")	return{ lexeme, { IF } };
	else if (lexeme == "else")	return{ lexeme, { ELSE } };
	else if (identifier(lexeme))	return{ lexeme, { IDENTIFIER } };
	else				return{ lexeme, { EXPR } };
}

void parse_program()
{
	if (program == &std::cin) cout << (*identify)["__prompt__"]->to_string();
	current_token = get_next_token();
	while (current_token.types[0] != END)
	{	
		parse_statement();
	}
}

void parse_statement()
{
	if (current_token.types[0] == QUIT || current_token.types[0] == END) {cout << "\n"; exit(0); }				// Exit.
	else if (current_token.types[0] == DELETE) parse_delete();
	else if (current_token.types[0] == DELETE_ELEMS) parse_delete();
	else if (current_token.types[0] == DECLARE) parse_declaration();
	else if (current_token.types[0] == TYPE) parse_initialization();
	else if (current_token.types[0] == WHILE) parse_while();
	else if (current_token.types[0] == IF) parse_if();
	else if (current_token.types[0] == PRINT) parse_print();
	else if (current_token.types[0] == LET) parse_assignment();
	else if (current_token.types[0] == UNDER) parse_mapping();
	if (program == &std::cin) cout << (*identify)["__prompt__"]->to_string();
	current_token = get_next_token();
}

void parse_mapping()
{
	Token map_name = get_next_token();

	if (map_name.types[0] != IDENTIFIER) raise_error("An identifier for a map expected.");

	if ((*identify)[map_name.lexeme]->type != MAP) raise_error("An identifier for a map expected.");

	Token colon = get_next_token();

	if (colon.types[0] != COLON) raise_error("Missing operator \":\".");

	read_mapdom_expr = true;

	Token pre_image = get_next_token();

	Token mapsymb = get_next_token();

	if (mapsymb.types[0] != MAPPING_SYMBOL) raise_error("Missing operator \"-->\".");

	Token image = get_next_token();

	ExpressionTree pre_im_expr(pre_image.lexeme);	// Not going to mark these two with ROOT ...
	ExpressionTree im_expr(image.lexeme);		// ... because these values will only be used for lookup.

	((Map *)(*identify)[map_name.lexeme])->add_maping(*pre_im_expr.evaluate(), *im_expr.evaluate());
}

void parse_assignment()
{
	read_left_expr = true;
		
	Token update = get_next_token();

	Token eq_sign = get_next_token();

	if (eq_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

	read_right_expr = true;

	Token expression = get_next_token();

	ExpressionTree expr(expression.lexeme, ROOT);
	
	Elem * new_value = expr.evaluate();
	 
	delete (*identify)[update.lexeme];

	(*identify)[update.lexeme] = new_value;
}

void parse_print()
{
	read_right_expr = true;
	Token print = get_next_token();

	ExpressionTree expr(print.lexeme, ROOT);	 
	Elem * to_be_printed = expr.evaluate();

	cout << to_be_printed->to_string();
	if (program == &cin) cout << endl << endl;

	if (expr.node->token.types[0] == LITERAL) delete expr.node->value;
}

void parse_delete()		// Parse a delete command.
{
	read_right_expr = true;
	Token delete_this = get_next_token();
	if (delete_this.types[0] == IDENTIFIER && (*identify)[delete_this.lexeme] != nullptr)	// If it's an identifier ...
		delete (*identify)[delete_this.lexeme];						// ... then delete the object it maps to.
}

void parse_delete_elems()	// Parse a delete_elems command.
{
	read_right_expr = true;
	Token delete_elemsof_this = get_next_token();
	if (delete_elemsof_this.types[0] == IDENTIFIER && (*identify)[delete_elemsof_this.lexeme] != nullptr)
	{
		Elem * /*delete_elements*/of_this = (*identify)[delete_elemsof_this.lexeme];	// Get a pointer to the object.

		if (of_this->type == SET) ((Set  *)of_this)->delete_elems();			// Cast it appropriately ...
		else if (of_this->type == TUPLE) ((Tuple*)of_this)->delete_elems();		// ... and call the method needed.
		else if (of_this->type == MAP) ((Map  *)of_this)->delete_elems();
	}
}

void parse_declaration()	// Parse a declaration.
{
	Token data_type = get_next_token();

	if (data_type.types[0] != TYPE) raise_error("Data type not supported.");

	Token new_identifier = get_next_token();

	if (new_identifier.types[0] != IDENTIFIER)	   raise_error("Please use a valid name for the identifier.");
	if ((*identify)[new_identifier.lexeme] != nullptr) raise_error("Identifier already in use. Cannot re-declare.");

	if (data_type.lexeme == "set") (*identify)[new_identifier.lexeme] = new Set();
	else if (data_type.lexeme == "tuple")(*identify)[new_identifier.lexeme] = new Tuple();
	else if (data_type.lexeme == "map") (*identify)[new_identifier.lexeme] = new Map(nullptr, nullptr);
	else if (data_type.lexeme == "int") (*identify)[new_identifier.lexeme] = new Int();
	else if (data_type.lexeme == "char") (*identify)[new_identifier.lexeme] = new Char();
	else if (data_type.lexeme == "string") (*identify)[new_identifier.lexeme] = new String();
	else if (data_type.lexeme == "logical") (*identify)[new_identifier.lexeme] = new Logical();
}

void parse_initialization()
{
	Token & data_type = current_token;

	if (data_type.types[0] != TYPE) raise_error("Data type not supported.");

	Token new_identifier = get_next_token();

	if (new_identifier.types[0] != IDENTIFIER)	   raise_error("Please use a valid name for the identifier.");
	if ((*identify)[new_identifier.lexeme] != nullptr) raise_error("Identifier already in use. Cannot re-declare.");

	if (data_type.lexeme == "set")  
	{
		Token equal_sign = get_next_token();

		if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

		read_right_expr = true;

		Token value = get_next_token();			// The value to be assigned to the identifier.

		ExpressionTree value_expr(value.lexeme, ROOT);

		Elem * val = value_expr.evaluate();

		if (val->type != SET) raise_error("Cannot assign a non-set object to a set identifier.");

		(*identify)[new_identifier.lexeme] = val;
	}
	else if (data_type.lexeme == "tuple")
	{
		Token equal_sign = get_next_token();

		if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

		read_right_expr = true;
		Token value = get_next_token();			// The value to be assigned to the identifier.
		
		ExpressionTree value_expr(value.lexeme, ROOT);

		Elem * val = value_expr.evaluate();

		if (val->type != TUPLE) raise_error("Cannot assign a non-tuple object to a tuple identifier.");

		(*identify)[new_identifier.lexeme] = val;
	}
	else if (data_type.lexeme == "map")
	{
		Token colon = get_next_token();
		if (colon.types[0] != COLON) raise_error("A map identifier must be followed by a \":\".");

		read_mapdom_expr = true;
		Token domain = get_next_token();
		Token mapsymb = get_next_token();
		read_right_expr = true;
		Token codomain = get_next_token();

		Elem * map_domain = nullptr, *map_codomain = nullptr;	// Actual pointers that will be used in the map's constructor.

		if (mapsymb.types[0] != MAPPING_SYMBOL) raise_error("Missing mapping operator \"-->\".");

		if (domain.types[0] == IDENTIFIER)
			if ((*identify)[domain.lexeme] == nullptr)
				raise_error("The domain identifier doesn't refer to any object.");
			else if ((*identify)[domain.lexeme]->type != SET)
				raise_error("The domain of a map must be a set.");
			else
				map_domain = (*identify)[domain.lexeme];

		else if (domain.types[0] == EXPR)
		{
			ExpressionTree domain_expr(domain.lexeme, ROOT);
			map_domain = domain_expr.evaluate();
			if (map_domain->type != SET)
				raise_error("The domain of a map must be a set.");
		}

		if (codomain.types[0] == IDENTIFIER)
			if ((*identify)[codomain.lexeme] == nullptr)
				raise_error("The codomain identifier doesn't refer to any object.");
			else if ((*identify)[codomain.lexeme]->type != SET)
				raise_error("The codomain of a map must be a set.");
			else
				map_codomain = (*identify)[codomain.lexeme];

		else if (codomain.types[0] == EXPR)
		{
			ExpressionTree codomain_expr(codomain.lexeme, ROOT);
			map_codomain = codomain_expr.evaluate();
			if (map_codomain->type != SET)
				raise_error("The codomain of a map must be a set.");
		}
		(*identify)[new_identifier.lexeme] = new Map((Set *)map_domain, (Set *)map_codomain);
	}
	else if (data_type.lexeme == "int") 
	{
		Token equal_sign = get_next_token();
		
		if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

		read_right_expr = true;
		Token int_expression = get_next_token();

		ExpressionTree int_expr(int_expression.lexeme, ROOT);

		Elem * val = int_expr.evaluate();

		if (val->type != INT) raise_error("Cannot assign a non-int value to an int identifier.");

		(*identify)[new_identifier.lexeme] = val;
	}
	else if (data_type.lexeme == "char")
	{
		Token equal_sign = get_next_token();

		if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

		read_right_expr = true;
		Token char_expression = get_next_token();

		ExpressionTree char_expr(char_expression.lexeme, ROOT);

		Elem * val = char_expr.evaluate();

		if (val->type != CHAR) raise_error("Cannot assign a non-char value to a char identifier.");

		(*identify)[new_identifier.lexeme] = val;
	}
	else if (data_type.lexeme == "string") 
	{
		Token equal_sign = get_next_token();

		if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

		read_right_expr = true;
		Token str_expression = get_next_token();

		ExpressionTree str_expr(str_expression.lexeme, ROOT);

		Elem * val = str_expr.evaluate();

		if (val->type != STRING) raise_error("Cannot assign a non-string value to a string identifier.");

		(*identify)[new_identifier.lexeme] = val;
	}
	else if (data_type.lexeme == "logical")
	{
		Token equal_sign = get_next_token();

		if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

		Token logic_expression = get_next_token();

		read_right_expr = true;
		ExpressionTree logic_expr(logic_expression.lexeme, ROOT);

		Elem * val = logic_expr.evaluate();

		if (val->type != LOGICAL) raise_error("Cannot assign a non-logical value to a logical identifier.");

		(*identify)[new_identifier.lexeme] = val;
	}
	else if (data_type.lexeme == "auto")
	{
		Token equal_sign = get_next_token();

		if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

		read_right_expr = true;
		Token tuple_expression = get_next_token();

		ExpressionTree tuple_expr(tuple_expression.lexeme, ROOT);

		Elem * val = tuple_expr.evaluate();

		if (val->type != TUPLE) raise_error("Initializing an automaton needs a tuple.");

		Tuple * val_ = (Tuple *)val;

		if (val_->size() != 5) raise_error("Initializing an automaton needs a 5-tuple.");

		(*identify)[new_identifier.lexeme] = new Auto(	// Make a new automaton object.
			(Set *)(*val_)[0], (Set *)(*val_)[1], (*val_)[2], (Map *)(*val_)[3], (Set *)(*val_)[4], DIRECT_ASSIGN
		);
	}
}

void parse_while()
{
	read_right_expr = true;

	Token condition = get_next_token();

	if (condition.types[0] != EXPR) raise_error("Expected an expression.");

	Token starting_brace = get_next_token();
	
	if (starting_brace.types[0] != L_BRACE) raise_error("Missing '{'.");

	std::streampos loop_from = program->tellg();		// We will restart the stream of tokens from this point.
	int restore_line = line_num;

	ExpressionTree * logical_condition = new ExpressionTree(condition.lexeme, ROOT);
	Elem * do_or_not = logical_condition->evaluate();

	if (do_or_not->type != LOGICAL) raise_error("Expected a logical expression.");

	while (((Logical *)do_or_not)->elem)
	{
		if (logical_condition != nullptr) delete logical_condition;	// We have no use for the old parse tree of the condition ...
		if (do_or_not != nullptr) delete do_or_not;			// ... or this value, because the identifiers may have changed.

		current_token = get_next_token();

		while (current_token.types[0] != R_BRACE)			// As long as you don't see the end of the while loop ...
			parse_statement();					// ... keep parsing statements.

		program->seekg(loop_from, ios::beg);				// Go back to the beginning of statements when you're done.
		line_num = restore_line;
		logical_condition = new ExpressionTree(condition.lexeme, ROOT);	// Re-parse the condition ...
		do_or_not = logical_condition->evaluate();			// ... and re-evaluate it.

		if (do_or_not->type != LOGICAL) raise_error("Expected a logical expression (non-logical sometime during the iterations).");
	}		
	
	delete logical_condition;	// Finally once you're done with the loop ... 
	delete do_or_not;		// ... you can delete these objects.
	
	bool toss_tokens = true;	// Now we're literally going to keep tossing out tokens until we find "}".
	int level = 0;

	while (toss_tokens)
	{
		Token token = get_next_token();
		if (token.types[0] == LET) read_left_expr = true;
		if (token.types[0] == EQUAL_SIGN || token.types[0] == MAPPING_SYMBOL || token.types[0] == PRINT
		    || token.types[0] == WHILE || token.types[0] == IF) read_right_expr = true;
		if (token.types[0] == COLON) read_mapdom_expr = true;
		if (token.types[0] == L_BRACE) level++;
		if (token.types[0] == R_BRACE) 
			if (level == 0)
				toss_tokens = false;
			else level--;
	}
}

void parse_if()		// Parsing if statements.
{
	read_right_expr = true;

	Token condition = get_next_token();

	if (condition.types[0] != EXPR) raise_error("Expected an expression.");

	Token starting_brace = get_next_token();

	if (starting_brace.types[0] != L_BRACE) raise_error("Missing '{'.");

	ExpressionTree * logical_condition = new ExpressionTree(condition.lexeme, ROOT);

	Elem * do_or_not = logical_condition->evaluate();

	if (do_or_not->type != LOGICAL) raise_error("Expected a logical expression.");

	if (((Logical *)do_or_not)->elem)
	{
		delete logical_condition;	// We have no use for the old parse tree of the condition ...
		delete do_or_not;			// ... or this value, because the identifiers may have changed.

		current_token = get_next_token();

		while (current_token.types[0] != R_BRACE)			// As long as you don't see the end of the if block ...
			parse_statement();					// ... keep parsing statements.

		Token else_ = get_next_token();

		if (else_.types[0] != ELSE) raise_error("else block expected.");

		Token left_brace = get_next_token();

		if (left_brace.types[0] != L_BRACE) raise_error("'{' expected.");

		// But we have to do nothing with the else block. We just need to ignore it.

		bool toss_tokens = true;	// Now we're literally going to keep tossing out tokens until we find "}".
		int level = 0;

		while (toss_tokens)
		{
			Token token = get_next_token();
			if (token.types[0] == LET) read_left_expr = true;
			if (token.types[0] == EQUAL_SIGN || token.types[0] == MAPPING_SYMBOL || token.types[0] == PRINT
				|| token.types[0] == WHILE || token.types[0] == IF) read_right_expr = true;
			if (token.types[0] == COLON) read_mapdom_expr = true;
			if (token.types[0] == L_BRACE) level++;
			if (token.types[0] == R_BRACE)
				if (level == 0)
					toss_tokens = false;
				else level--;
		}
	}
	else
	{
		delete logical_condition;	// Finally once you're done with the loop ... 
		delete do_or_not;
		
		bool toss_tokens = true;	// Now we're literally going to keep tossing out tokens until we find "}".
		int level = 0;

		while (toss_tokens)
		{
			Token token = get_next_token();

			if (token.types[0] == LET) read_left_expr = true;
			if (token.types[0] == EQUAL_SIGN || token.types[0] == MAPPING_SYMBOL || token.types[0] == PRINT
				|| token.types[0] == WHILE || token.types[0] == IF) read_right_expr = true;
			if (token.types[0] == COLON) read_mapdom_expr = true;
			if (token.types[0] == L_BRACE) level++;
			if (token.types[0] == R_BRACE)
				if (level == 0)
					toss_tokens = false;
				else level--;
		}
		Token else_ = get_next_token();

		if (else_.types[0] != ELSE) raise_error("else block expected.");

		Token l_brace = get_next_token();
		if (l_brace.types[0] != L_BRACE) raise_error("'{' expected.");

		current_token = get_next_token();
			
		while (current_token.types[0] != R_BRACE)			// As long as you don't see the end of the else block ...
			parse_statement();					// ... keep parsing statements.
	}
}

void trim(string &str)
{
	if (all_spaces(str)) return;
	int start = 0, end = str.size() - 1;
	while (isspace(str[start])) start++;
	while (isspace(str[ end ]))  end-- ;
	if (start >= end) str == "";
	str = str.substr(start, end - start + 1);
}

bool identifier(string &str)
{
	if (str[0] == '_' || isalpha(str[0])) {
		for (int i = 1; i < str.size(); i++)
			if (!isalnum(str[i]) && str[i] != '_')
				return false;
		return true;
	}
	else return false;
}

void raise_error(const char * message)
{
	cout << "ERROR: Line " << line_num << ": " << message << endl;
	exit(0);
}

bool all_spaces(string& str)
{
	for (auto& c : str)
		if (!isspace(c))
			return false;
	return true;
}

void print_info()
{
	cout << "Autolang, Version 2.0 \nCopyright (c) 2016 Tushar Rakheja (The MIT License)." << endl << endl;
	cout << "Please contribute to Autolang if you find it useful." << endl;
	cout << "For more info, visit https://github.com/TusharRakheja/Autolang." << endl << endl;
	cout << "To change the prompt, use the env. variable \"__prompt__\"." << endl << endl;	
}

void remove_comment(string &str)
{
	int level = 0, i = 0, comment_found_at = 0;
	bool in_string = false, comment_found = false;
	for (i = 0; i < str.size(); i++)
	{
		if (((str[i] == '"' && !in_string) || str[i] == '{' || str[i] == '(' || str[i] == '[')
			&& (i == 0 || str[i - 1] != '\\')) {
			level++;
			if (str[i] == '"' && !in_string) in_string = true;
		}
		else if (((str[i] == '"' && in_string) || str[i] == '}' || str[i] == ')' || str[i] == ']')
			&& (i == 0 || str[i - 1] != '\\')) {
			level--;
			if (str[i] == '"' && in_string) in_string = false;
		}
		else if (str[i] == '#' && level == 0)		// It's a tuple if a comma exists at level 0.
		{
			comment_found = true;
			comment_found_at = i;
			break;
		}
	}
	if (comment_found) str = str.substr(0, comment_found_at);
} 
