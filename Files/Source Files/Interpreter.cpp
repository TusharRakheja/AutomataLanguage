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
using std::static_pointer_cast;

unordered_map<string, shared_ptr<Elem>> * program_vars::identify = new unordered_map<string, shared_ptr<Elem>>
{ 
	{ "__prompt__", shared_ptr<String>{new String(">>> ")} } 
};
// Identifiers mapped to their objects.

void program_vars::raise_error(const char *message)
{
	cout << "ERROR: Line " << line_num-1 << ": " << message << endl;
	delete program_vars::identify;
	exit(0);
}

int program_vars::line_num = 1;

using program_vars::identify;
using program_vars::raise_error;
using program_vars::line_num;

Token current_token;		// The current token we're looking at. 
istream * program;		// The stream of text constituting the program.
bool read_right_expr = false;	// Notes whether or not you're reading an expression on the right side of an '=' sign.
bool read_map_expr = false;
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
void parse_printr();		// Parses a print raw command.
void parse_mapping();		// Parse a single mapping.
void parse_if();		// Parse an if condition.
void parse_while();		// Parse a while loop.
void trim(string &);		// Trim a string (*sigh*).
void remove_comment(string&);   // Removes the comment at the end of the line.
bool identifier(string &);	// Returns true if a string is an identifier.
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
	else if (read_map_expr)
	{
		getline(*program, lexeme, ':');			// Read the whole thing first;
		if (program == &cin)
			program->unget();
		else
			program->seekg(-1L * (int)sizeof(char), ios::cur);
		read_map_expr = false;
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
	else if (lexeme == "abstract")  return{ lexeme, { ABSTRACT} };
	else if (lexeme == "print")	return{ lexeme, { PRINT } };
	else if (lexeme == "printr")	return{ lexeme, { PRINTR} }; 
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
	if (program == &std::cin) cout << static_pointer_cast<String>((*identify)["__prompt__"])->to_string();
	current_token = get_next_token();
	while (current_token.types[0] != END)
	{	
		parse_statement();
	}
}

void parse_statement()
{
	if (current_token.types[0] == QUIT || current_token.types[0] == END) {cout << "\n"; exit(0); }				// Exit.
	else if (current_token.types[0] == DECLARE) parse_declaration();
	else if (current_token.types[0] == TYPE || current_token.types[0] == ABSTRACT) parse_initialization();
	else if (current_token.types[0] == WHILE) parse_while();
	else if (current_token.types[0] == IF) parse_if();
	else if (current_token.types[0] == PRINT) parse_print();
	else if (current_token.types[0] == PRINTR) parse_printr();
	else if (current_token.types[0] == LET) parse_assignment();
	else if (current_token.types[0] == UNDER) parse_mapping();
	if (program == &std::cin) cout << static_pointer_cast<String>((*identify)["__prompt__"])->to_string();
	current_token = get_next_token();
}


void parse_mapping()
{
	read_map_expr = true;

	Token map_expr = get_next_token();

	ExpressionTree map_expression(map_expr.lexeme);

	shared_ptr<Elem> candidate_map = map_expression.evaluate();

	if (candidate_map->type != MAP && candidate_map->type != ABSTRACT_MAP) raise_error("An expression for a map or an abstract map expected.");
	
	Token colon = get_next_token();

	if (colon.types[0] != COLON) raise_error("Missing operator \":\".");

	if (candidate_map->type == MAP)
	{
		shared_ptr<Map> map = static_pointer_cast<Map>(candidate_map);
		
		read_mapdom_expr = true;

		Token pre_image = get_next_token();

		Token mapsymb = get_next_token();

		if (mapsymb.types[0] != MAPPING_SYMBOL) raise_error("Missing operator \"-->\".");

		read_right_expr = true;

		Token image = get_next_token();

		ExpressionTree pre_im_expr(pre_image.lexeme);	// Not going to mark these two with ROOT ...
		ExpressionTree im_expr(image.lexeme);		// ... because these values will only be used for lookup.

		map->add_maping(*pre_im_expr.evaluate(), *im_expr.evaluate());
	}
	else
	{
		shared_ptr<AbstractMap> absmap = static_pointer_cast<AbstractMap>(candidate_map);
		
		read_right_expr = true;
		
		Token mapping_scheme = get_next_token();
		
		absmap->add_scheme(mapping_scheme.lexeme);
	}
}

void parse_assignment()
{
	read_left_expr = true;
		
	Token update = get_next_token();

	Token eq_sign = get_next_token();

	if (eq_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

	read_right_expr = true;

	Token expression = get_next_token();

	ExpressionTree expr(expression.lexeme);
	
	shared_ptr<Elem> new_value = expr.evaluate();
	
	(*identify)[update.lexeme] = new_value;
}

void parse_print()
{
	read_right_expr = true;
	Token print = get_next_token();

	ExpressionTree expr(print.lexeme);	 
	shared_ptr<Elem> to_be_printed = expr.evaluate();
	
	cout << to_be_printed->to_string();
	if (program == &cin) cout << endl << endl;
}

void parse_printr()
{
	read_right_expr = true;
	Token print = get_next_token();

	ExpressionTree expr(print.lexeme);
	shared_ptr<Elem> to_be_printed = expr.evaluate();

	cout << to_be_printed->to_string_raw();
	if (program == &cin) cout << endl << endl;
}

void parse_declaration()	// Parse a declaration.
{
	Token data_type = get_next_token();

	if (data_type.types[0] != TYPE && data_type.types[0] != ABSTRACT) raise_error("Data type not supported.");

	if (data_type.types[0] == TYPE) 
	{
		Token new_identifier = get_next_token();

		if (new_identifier.types[0] != IDENTIFIER)	   raise_error("Please use a valid name for the identifier.");
		if ((*identify)[new_identifier.lexeme] != nullptr) raise_error("Identifier already in use. Cannot re-declare.");

		if      (data_type.lexeme ==   "set"  ) (*identify)[new_identifier.lexeme] = shared_ptr<  Set  >{new Set()};
		else if (data_type.lexeme ==  "tuple" ) (*identify)[new_identifier.lexeme] = shared_ptr< Tuple >{new Tuple()};
		else if (data_type.lexeme ==   "map"  ) (*identify)[new_identifier.lexeme] = shared_ptr<  Map  >{new Map(nullptr, nullptr)};
		else if (data_type.lexeme ==   "int"  ) (*identify)[new_identifier.lexeme] = shared_ptr<  Int  >{new Int()};
		else if (data_type.lexeme ==   "char" ) (*identify)[new_identifier.lexeme] = shared_ptr<  Char >{new Char()};
		else if (data_type.lexeme ==  "string") (*identify)[new_identifier.lexeme] = shared_ptr< String>{new String()};
		else if (data_type.lexeme == "logical") (*identify)[new_identifier.lexeme] = shared_ptr<Logical>{new Logical()};
	}
	else if (data_type.types[0] == ABSTRACT)
	{
		Token type = get_next_token();

		if (type.lexeme != "set" && type.lexeme != "map") raise_error("Only sets and maps can be abstract.");

		Token new_identifier = get_next_token();

		if ((*identify)[new_identifier.lexeme] != nullptr) raise_error("Identifier already in use. Cannot re-declare.");

		if (type.lexeme == "set")
			(*identify)[new_identifier.lexeme] = shared_ptr<AbstractSet>{new AbstractSet()};
		else                     
			(*identify)[new_identifier.lexeme] = shared_ptr<AbstractMap>{new AbstractMap()};
	}
}
void parse_initialization()
{
	Token & data_type = current_token;

	if (data_type.types[0] != TYPE && data_type.types[0] != ABSTRACT) raise_error("Data type not supported.");

	if (data_type.types[0] == TYPE) 
	{
		Token new_identifier = get_next_token();

		if (new_identifier.types[0] != IDENTIFIER)	   raise_error("Please use a valid name for the identifier.");
		if ((*identify)[new_identifier.lexeme] != nullptr) raise_error("Identifier already in use. Cannot re-declare.");

		if (data_type.lexeme == "set")  
		{
			Token equal_sign = get_next_token();

			if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

			read_right_expr = true;

			Token value = get_next_token();			// The value to be assigned to the identifier.

			ExpressionTree value_expr(value.lexeme);

			shared_ptr<Elem> val = value_expr.evaluate();

			if (val->type != SET) raise_error("Cannot assign a non-set object to a set identifier.");

			(*identify)[new_identifier.lexeme] = val;
		}
		else if (data_type.lexeme == "tuple")
		{
			Token equal_sign = get_next_token();

			if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

			read_right_expr = true;

			Token value = get_next_token();			// The value to be assigned to the identifier.
		
			ExpressionTree value_expr(value.lexeme);

			shared_ptr<Elem> val = value_expr.evaluate();

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

			shared_ptr<Elem> map_domain = nullptr, map_codomain = nullptr;	// Actual pointers that will be used in the map's constructor.

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
				ExpressionTree domain_expr(domain.lexeme);
				map_domain = domain_expr.evaluate();
				if (map_domain->type != SET) raise_error("The domain of a map must be a set.");
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
				ExpressionTree codomain_expr(codomain.lexeme);
				map_codomain = codomain_expr.evaluate();
				if (map_codomain->type != SET)
					raise_error("The codomain of a map must be a set.");
			}
			(*identify)[new_identifier.lexeme] = shared_ptr<Map> {new Map(
				static_pointer_cast<Set>(map_domain),
				static_pointer_cast<Set>(map_codomain)
			)};
		}
		else if (data_type.lexeme == "int") 
		{
			Token equal_sign = get_next_token();
		
			if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

			read_right_expr = true;

			Token int_expression = get_next_token();

			ExpressionTree int_expr(int_expression.lexeme);

			shared_ptr<Elem> val = int_expr.evaluate();

			if (val->type != INT) raise_error("Cannot assign a non-int value to an int identifier.");

			(*identify)[new_identifier.lexeme] = static_pointer_cast<Int>(val);
		}
		else if (data_type.lexeme == "char")
		{
			Token equal_sign = get_next_token();

			if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

			read_right_expr = true;
			Token char_expression = get_next_token();

			ExpressionTree char_expr(char_expression.lexeme);

			shared_ptr<Elem> val = char_expr.evaluate();

			if (val->type != CHAR) raise_error("Cannot assign a non-char value to a char identifier.");

			(*identify)[new_identifier.lexeme] = static_pointer_cast<Char>(val);
		}
		else if (data_type.lexeme == "string") 
		{
			Token equal_sign = get_next_token();

			if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

			read_right_expr = true;

			Token str_expression = get_next_token();

			ExpressionTree str_expr(str_expression.lexeme);

			shared_ptr<Elem> val = str_expr.evaluate();

			if (val->type != STRING) raise_error("Cannot assign a non-string value to a string identifier.");

			(*identify)[new_identifier.lexeme] = static_pointer_cast<String>(val);
		}
		else if (data_type.lexeme == "logical")
		{
			Token equal_sign = get_next_token();

			if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

			Token logic_expression = get_next_token();

			read_right_expr = true;

			ExpressionTree logic_expr(logic_expression.lexeme);

			shared_ptr<Elem> val = logic_expr.evaluate();

			if (val->type != LOGICAL) raise_error("Cannot assign a non-logical value to a logical identifier.");

			(*identify)[new_identifier.lexeme] = static_pointer_cast<Logical>(val);
		}
		else if (data_type.lexeme == "auto")
		{
			Token equal_sign = get_next_token();

			if (equal_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

			read_right_expr = true;
			Token tuple_expression = get_next_token();

			ExpressionTree tuple_expr(tuple_expression.lexeme);

			shared_ptr<Elem> val = tuple_expr.evaluate();

			if (val->type != TUPLE) raise_error("Initializing an automaton needs a tuple.");

			shared_ptr<Tuple> val_ = static_pointer_cast<Tuple>(val);

			if (val_->size() != 5) raise_error("Initializing an automaton needs a 5-tuple.");

			(*identify)[new_identifier.lexeme] = shared_ptr<Auto>{new Auto(	// Make a new automaton object.
				static_pointer_cast<Set>((*val_)[0]), 
				static_pointer_cast<Set>((*val_)[1]), 
				(*val_)[2], 
				static_pointer_cast<Map>((*val_)[3]), 
				static_pointer_cast<Set>((*val_)[4]),
				DIRECT_ASSIGN
			)};
		}
	}
	else if (data_type.types[0] == ABSTRACT)
	{
		Token type = get_next_token();

		if (type.lexeme != "set" && type.lexeme != "map") raise_error("Only sets and maps can be abstract.");

		Token new_identifier = get_next_token();

		if ((*identify)[new_identifier.lexeme] != nullptr) raise_error("Identifier already in use. Cannot re-declare.");

		if (type.lexeme == "set")
		{ 
			Token eq_sign = get_next_token();

			if (eq_sign.types[0] != EQUAL_SIGN) raise_error("Missing operator \"=\".");

			read_right_expr = true;

			Token abstract_set = get_next_token();

			(*identify)[new_identifier.lexeme] = shared_ptr<AbstractSet>{new AbstractSet(abstract_set.lexeme)};
		}
		else
		{
			Token colon = get_next_token();

			if (colon.types[0] != COLON) raise_error("Missing operator \":\".");

			read_mapdom_expr = true;

			Token abstract_map_domain = get_next_token();

			Token mapssymb = get_next_token();

			if (mapssymb.types[0] == MAPPING_SYMBOL) raise_error("Missing operator \"-->\".");

			read_right_expr = true;

			Token abstract_map_codomain = get_next_token();
 
			ExpressionTree abstract_mapdom_expr(abstract_map_domain.lexeme);

			ExpressionTree abstract_mapcodom_expr(abstract_map_codomain.lexeme);

			shared_ptr<Elem> domain = abstract_mapdom_expr.evaluate(), codomain = abstract_mapcodom_expr.evaluate();

			if (domain->type != ABSTRACT_SET || codomain->type != ABSTRACT_SET) raise_error("Abstract sets expected.");

			(*identify)[new_identifier.lexeme] = shared_ptr<AbstractMap> { new AbstractMap (
				static_pointer_cast<AbstractSet>(domain), 
				static_pointer_cast<AbstractSet>(codomain)
			)};
		}
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

	ExpressionTree * logical_condition = new ExpressionTree(condition.lexeme);

	shared_ptr<Elem> do_or_not = logical_condition->evaluate();

	if (do_or_not->type != LOGICAL) raise_error("Expected a logical expression.");

	while (static_pointer_cast<Logical>(do_or_not)->elem)
	{
		if (logical_condition != nullptr) delete logical_condition;	// We have no use for the old parse tree of the condition ...

		current_token = get_next_token();

		while (current_token.types[0] != R_BRACE)			// As long as you don't see the end of the while loop ...
			parse_statement();					// ... keep parsing statements.

		program->seekg(loop_from, ios::beg);				// Go back to the beginning of statements when you're done.
		line_num = restore_line;
		logical_condition = new ExpressionTree(condition.lexeme);	// Re-parse the condition ...
		do_or_not = logical_condition->evaluate();			// ... and re-evaluate it.

		if (do_or_not->type != LOGICAL) raise_error("Expected a logical expression (non-logical sometime during the iterations).");
	}		
	
	delete logical_condition;	// Finally once you're done with the loop ...
	
	bool toss_tokens = true;	// Now we're literally going to keep tossing out tokens until we find "}".
	int level = 0;

	while (toss_tokens)
	{
		Token token = get_next_token();
		if (token.types[0] == LET) read_left_expr = true;
		if (token.types[0] == UNDER) read_map_expr = true;
		if (token.types[0] == EQUAL_SIGN || token.types[0] == COLON || token.types[0] == PRINT
		    || token.types[0] == WHILE || token.types[0] == IF) read_right_expr = true;
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

	ExpressionTree * logical_condition = new ExpressionTree(condition.lexeme);

	shared_ptr<Elem> do_or_not = logical_condition->evaluate();

	if (do_or_not->type != LOGICAL) raise_error("Expected a logical expression.");

	if (static_pointer_cast<Logical>(do_or_not)->elem)
	{
		delete logical_condition;	// We have no use for the old parse tree of the condition ...

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
			if (token.types[0] == UNDER) read_map_expr = true;
			if (token.types[0] == EQUAL_SIGN || token.types[0] == COLON || token.types[0] == PRINT
				|| token.types[0] == WHILE || token.types[0] == IF) read_right_expr = true;
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
		
		bool toss_tokens = true;	// Now we're literally going to keep tossing out tokens until we find "}".
		int level = 0;

		while (toss_tokens)
		{
			Token token = get_next_token();

			if (token.types[0] == LET) read_left_expr = true;
			if (token.types[0] == UNDER) read_map_expr = true;
			if (token.types[0] == EQUAL_SIGN || token.types[0] == COLON || token.types[0] == PRINT
				|| token.types[0] == WHILE || token.types[0] == IF) read_right_expr = true;
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
