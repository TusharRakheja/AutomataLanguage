#include "../Header Files/ExpressionTree.h" 

using std::cout;
using std::cin;
using std::endl;
using std::getline;
using std::ios;
using std::istream;

unordered_map<string, shared_ptr<Elem>> * program_vars::identify = new unordered_map<string, shared_ptr<Elem>>
{ 
	{ "__prompt__", shared_ptr<String>{new String(">>> ")}},
	{ "console", shared_ptr<DataSource>{new DataSource(0, shared_ptr<Char>{ new Char('\n') } ) } }  // Console input source.
};
// Identifiers mapped to their objects.

int program_vars::line_num = 1;

using program_vars::identify;
using program_vars::raise_error;
using program_vars::line_num;

Token current_token;		// The current token we're looking at. 
istream * program;		// The stream of text constituting the program.
bool read_right_expr = false;	// Notes whether or not you're reading an expression on the right side of an '=' sign.
bool read_map_expr = false;     // Notes whether or not you're reading an expression that represents a map.
bool read_update_expr = false;	// Notes whether or not you're reading an expression on the left side of an '=' sign.
bool read_mapdom_expr = false;	// Notes whether or not you're reading an expression in the domain of the map (left of '->' sign).
bool read_getfs_expr = false;	// Notes whether or not you're reading an expression on the left side of an '<-' sign.

Token get_next_token();		// Lexer.
void parse_program();		// Parse the program.
void parse_statement();		// Parse a statement.
void parse_declaration();	// Parse a declaration.
void parse_initialization();	// Parse an initialization.
void parse_assignment();	// Parse an assignment.
void parse_fromsource();	// Parse a `get` from source command.
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

void program_vars::raise_error(const char *message)
{
	cout << "ERROR: ";
	if (program != &cin) cout << " Line" << (line_num - 1) << ": ";
	cout << message << endl;
	delete program_vars::identify;
	exit(0);
}

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
	if (read_update_expr) 
	{
		getline(*program, lexeme, '=');			// Read the whole thing first;
		if (program == &cin) 
		{
			if (!isspace(lexeme[lexeme.size() - 1]))
			{
				program->unget(); program->unget();
				lexeme = lexeme.substr(0, lexeme.size() - 1);
			}
			else program->unget();
		}
		else
		{
			if (!isspace(lexeme[lexeme.size() - 1]))
			{
				program->seekg(-2L * (int)sizeof(char), ios::cur);
				lexeme = lexeme.substr(0, lexeme.size() - 1);
			}
			else program->seekg(-1L*(int)sizeof(char), ios::cur);	
		}
		read_update_expr = false;
	}	
	else if (read_getfs_expr)
	{
		getline(*program, lexeme, '<');			// Read the whole thing first;
		if (program == &cin)
			program->unget();
		else
			program->seekg(-1L * (int)sizeof(char), ios::cur);
		read_getfs_expr = false;
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
		char a, b;
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
				program->get(b);
				if (b == '>') 
				{
					if (program == &cin) 
					{
						program->unget();
						program->unget();
						break;
					}
					else 
					{
						program->seekg(-2L * (int)sizeof(char), ios::cur);
						break;
					}
				}
				else 
				{
					if (program == &cin) program->unget();
					else program->seekg(-1L * (int)sizeof(char), ios::cur);
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
	       	 lexeme == "logical" || lexeme == "auto" || lexeme == "source" || 
		 lexeme == "sink")      return{ lexeme, { TYPE } };
	else if (lexeme == "=")		return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "&=")	return{ lexeme, { UPDATE_OP} };
	else if (lexeme == "U=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "\\=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "x=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "V=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "+=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "-=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "*=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "/=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "^=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "%=")	return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "o=")        return{ lexeme, { UPDATE_OP } };
	else if (lexeme == "<-")        return{ lexeme, { SOURCE_OP } };
	else if (lexeme == "->")	return{ lexeme, { MAPPING_SYMBOL } };
	else if (lexeme == "let")	return{ lexeme, { LET } };
	else if (lexeme == "get")       return{ lexeme, { GET } };
	else if (lexeme == "under")	return{ lexeme, { UNDER } };
	else if (lexeme == ":")		return{ lexeme, { COLON } };
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
	if (program == &std::cin) cout << str((*identify)["__prompt__"])->to_string();
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
	else if (current_token.types[0] == GET) parse_fromsource();
	else if (current_token.types[0] == UNDER) parse_mapping();
	if (program == &std::cin) cout << str((*identify)["__prompt__"])->to_string();
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
		shared_ptr<Map> m = map(candidate_map);
		
		read_mapdom_expr = true;

		Token pre_image = get_next_token();

		Token mapsymb = get_next_token();

		if (mapsymb.types[0] != MAPPING_SYMBOL) raise_error("Missing operator \"->\".");

		read_right_expr = true;

		Token image = get_next_token();

		ExpressionTree pre_im_expr(pre_image.lexeme);	

		ExpressionTree im_expr(image.lexeme);		

		m->add_maping(*pre_im_expr.evaluate(), *im_expr.evaluate());
	}
	else if (candidate_map->type == ABSTRACT_MAP)
	{
		shared_ptr<AbstractMap> absmap = amap(candidate_map);
		
		read_right_expr = true;
		
		Token mapping_scheme = get_next_token();
		
		absmap->add_scheme(mapping_scheme.lexeme);
	}
	else raise_error("Expected a map or an abstract map for a \":\" operation.");
}

void parse_assignment()
{
	read_update_expr = true;
		
	Token update_this = get_next_token();

	ExpressionTree update_expr(update_this.lexeme);

	shared_ptr<Elem> update = update_expr.evaluate();

	Token op = get_next_token();

	if (op.types[0] != UPDATE_OP) raise_error("Missing operator for assignment/update.");

	read_right_expr = true;

	Token expression = get_next_token();

	ExpressionTree expr(expression.lexeme);
	
	shared_ptr<Elem> new_value = expr.evaluate();

	if (op.lexeme == "=")
	{
		if (update->type == SET)
		{
			if (new_value->type == SET)							   // Gon' A = B right here.
			{
				delete set(update)->elems;						   // Get rid of A's vector.
				set(update)->elems = new vector<shared_ptr<Elem>>(*set(new_value)->elems); // Give it a new vector with B's elems.
			}
			else raise_error("Expected a set on the RHS for a \"=\" operation with a set on the LHS.");
		}
		else if (update->type == TUPLE)
		{
			if (new_value->type == TUPLE)							   // Gon' A = B right here.
			{
				delete _tuple(update)->elems;						   // Get rid of A's vector.
				_tuple(update)->elems = new vector<shared_ptr<Elem>>(*_tuple(new_value)->elems);
			}
			else raise_error("Expected a tuple on the RHS for a \"=\" operation with a tuple on the LHS.");
		}
		else if (update->type == INT)
		{
			if (new_value->type == INT)
			{
				integer(update)->elem = integer(new_value)->elem;
			}
			else if (new_value->type == CHAR)
			{
				integer(update)->elem = character(new_value)->elem;
			}
			else if (new_value->type == LOGICAL)
			{
				integer(update)->elem = logical(new_value)->elem;
			}
			else raise_error("Expected an integer or a primitive on the RHS for a \"=\" operation with an integer on the LHS.");
		}
		else if (update->type == LOGICAL)
		{
			if (new_value->type == INT)
			{
				logical(update)->elem = integer(new_value)->elem;
			}
			else if (new_value->type == CHAR)
			{
				logical(update)->elem = character(new_value)->elem;
			}
			else if (new_value->type == LOGICAL)
			{
				logical(update)->elem = logical(new_value)->elem;
			}
			else raise_error("Expected a logical or a primitive on the RHS for a \"=\" operation with a logical on the LHS.");
		}
		else if (update->type == CHAR)
		{
			if (new_value->type == INT)
			{
				character(update)->elem = integer(new_value)->elem;
			}
			else if (new_value->type == CHAR)
			{
				character(update)->elem = character(new_value)->elem;
			}
			else if (new_value->type == LOGICAL)
			{
				character(update)->elem = logical(new_value)->elem;
			}
			else raise_error("Expected a character or a primitive on the RHS for a \"=\" operation with a character on the LHS.");
		}
		else if (update->type == STRING)
		{
			if (new_value->type == STRING)
			{
				str(update)->elem = str(new_value)->elem;
			}
			else raise_error("Expected a string on the RHS for a \"=\" operation with a string in the LHS");
		}
		else if (update->type == MAP)
		{
			if (new_value->type == MAP)
			{
				shared_ptr<Map> updating = map(update), new_map = map(new_value);
				updating->domain_s = new_map->domain_s;
				updating->codomain_s = new_map->codomain_s;
				delete updating->pi_indices;
				updating->pi_indices = new vector<int>(*new_map->pi_indices);
				delete updating->_map;
				updating->_map = new unordered_map<int, int>(*new_map->_map);
			}
			else raise_error("Expected a string on the RHS for a \"=\" operation with a string in the LHS");
		}
		else if (update->type == AUTO)
		{
			if (new_value->type == AUTO)
			{
				shared_ptr<Auto> updating = automaton(update), new_auto = automaton(new_value);
				updating->states = new_auto->states;
				updating->sigma = new_auto->sigma;
				updating->start = new_auto->start;
				updating->delta = new_auto->delta;
				updating->accepting = new_auto->accepting;
			}
			else if (new_value->type == TUPLE)
			{
				if (_tuple(new_value)->size() != 5) raise_error("An automaton is a 5-tuple.");
				shared_ptr<Auto> updating = automaton(update);
				shared_ptr<Tuple> new_auto = _tuple(new_value);
				updating->states = set((*new_auto)[0]);
				updating->sigma = set((*new_auto)[1]);
				updating->start = (*new_auto)[2];
				updating->delta = map((*new_auto)[3]);
				updating->accepting = set((*new_auto)[4]);
			}
			else raise_error("Expected an automaton or a tuple on the RHS for a \"=\" operation with an automaton in the LHS");
		}
		else if (update->type == ABSTRACT_SET)
		{
			if (new_value->type == ABSTRACT_SET)
			{
				// Just replace A's criteria with that of B. 
				aset(update)->criteria = aset(update)->criteria;
			}
			else raise_error("Expected an abstract set on the RHS for a \"=\" operation with an abstract set in the LHS");
		}
		else if (update->type == ABSTRACT_MAP)
		{
			if (new_value->type == ABSTRACT_MAP)
			{
				// Just replace A's scheme with that of B.
				amap(update)->mapping_scheme = amap(new_value)->mapping_scheme;
			}
			else raise_error("Expected an abstract map on the RHS for a \"=\" operation with an abstract map in the LHS");
		}
		else if (update->type == DATASOURCE)
		{
			if (new_value->type == DATASOURCE)
			{
				datasource(update)->elem = datasource(new_value)->elem;
				datasource(update)->delimiter = datasource(new_value)->delimiter;
			}
			else if (new_value->type == TUPLE)
			{
				if (_tuple(new_value)->size() != 2) 
					raise_error("A source must be a 2-tuple.");
				if ((*_tuple(new_value)->elems)[0]->type != STRING) 
					raise_error("A source's first element must be a string (filepath).");
				if ((*_tuple(new_value)->elems)[1]->type != CHAR)
					raise_error("A source's second element must be a char (delimiter).");

				if (datasource(update)->elem != nullptr) filesource(datasource(update)->elem)->close();
				datasource(update)->elem = shared_ptr<ifstream>{ 
					new ifstream(str((*_tuple(new_value)->elems)[0])->elem.c_str())
				};
				datasource(update)->delimiter = character((*_tuple(new_value)->elems)[1]);
			}
		}
		else if (update->type == DATASINK)
		{
			if (new_value->type == DATASINK)
			{
				datasink(update)->elem = datasink(new_value)->elem;
				datasink(update)->raw = datasink(new_value)->raw;
			}
			else if (new_value->type == TUPLE)
			{
				if (_tuple(new_value)->size() != 2)
					raise_error("A sink must be a 2-tuple.");
				if ((*_tuple(new_value)->elems)[0]->type != STRING)
					raise_error("A sink first element must be a string (filepath).");
				if ((*_tuple(new_value)->elems)[1]->type != LOGICAL)
					raise_error("A sink second element must be a logical (raw flag).");

				if (datasink(update)->elem != nullptr) datasink(update)->elem->close();
				datasink(update)->elem = shared_ptr<ofstream> {
					new ofstream(str((*_tuple(new_value)->elems)[0])->elem.c_str())
				};
				datasink(update)->raw = logical((*_tuple(new_value)->elems)[1]);
			}
		}
	}
	else 
	{
		if (op.lexeme == "&=")
		{	
			if (update->type == LOGICAL)
			{
				if (new_value->type == LOGICAL)
					logical(update)->elem = logical(update)->elem && logical(new_value)->elem;

				else if (new_value->type == INT)
					logical(update)->elem = logical(update)->elem && integer(new_value)->elem;

				else if (new_value->type == CHAR)
					logical(update)->elem = logical(update)->elem && character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"&=\" operation with a logical on the LHS.");
			}
			else if (update->type == INT)
			{
				if (new_value->type == LOGICAL)
					integer(update)->elem = integer(update)->elem && logical(new_value)->elem;

				else if (new_value->type == INT)
					integer(update)->elem = integer(update)->elem && integer(new_value)->elem;

				else if (new_value->type == CHAR)
					integer(update)->elem = integer(update)->elem && character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"&=\" operation with an int on the LHS.");
			}
			else if (update->type == CHAR)
			{
				if (new_value->type == LOGICAL)
					character(update)->elem = character(update)->elem && logical(new_value)->elem;

				else if (new_value->type == INT)
					character(update)->elem = character(update)->elem && integer(new_value)->elem;

				else if (new_value->type == CHAR)
					character(update)->elem = character(update)->elem && character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"&=\" operation with a char on the LHS.");
			}
			else if (update->type == SET)
			{
				if (new_value->type == SET)
				{
					shared_ptr<Set> updating = set(update), intersect = set(new_value);	// We need A = A & B

					for (int i {0}; i < updating->elems->size(); i++)			// So for all elements in A ...
						if (!intersect->has(*(*updating->elems)[i]))			// if any one is not in B.
							updating->elems->erase(updating->elems->begin() + i);   // ... remove it from the set.
				}
				else raise_error("Expected a set on the RHS for a \"&=\" operation with a set on the LHS.");
			}
			else if (update->type == ABSTRACT_SET)
			{
				if (new_value->type == ABSTRACT_SET)
				{
					shared_ptr<AbstractSet> updating = aset(update), intersect = aset(new_value);	// We need A = A & B

					shared_ptr<AbstractSet> intersection = updating->intersection(*intersect);	// Let C = A & B.

					updating->criteria = intersection->criteria;		// Now just replace A's criteria by that of C.
				}
				else raise_error("Expected an abstract set on the RHS for a \"&=\" operation with an abstract set on the LHS.");
			}
			else if (update->type == AUTO)
			{
				if (new_value->type == AUTO)
				{
					shared_ptr<Auto> updating = automaton(update), intersect = automaton(new_value);
					shared_ptr<Auto> intersection = updating->accepts_intersection(intersect);
					updating->states = intersection->states;		// Simple assignments, no pressure.
					updating->start = intersection->start;
					updating->delta = intersection->delta;
					updating->accepting = intersection->accepting;
				}				
				else raise_error("Expected an automaton on the RHS for a \"&=\" operation with an automaton on the LHS.");
			}
			else raise_error("Expected a primitive, set, abstract set, or automaton for a \"&=\" operation.");
		}
		else if (op.lexeme == "U=")
		{
			if (update->type == SET)
			{
				if (new_value->type == SET)
				{
					shared_ptr<Set> updating = set(update), unifywith = set(new_value);	// We need A = A U B

					for (int i{ 0 }; i < unifywith->elems->size(); i++)			// So for all elements in B ...
						if (!updating->has(*(*unifywith->elems)[i]))			// if any one is not in A.
							updating->elems->push_back((*unifywith->elems)[i]);     // ... add it to A.
				}				
				else raise_error("Expected a set on the RHS for a \"U=\" operation with a set on the LHS.");
			}
			else if (update->type == ABSTRACT_SET)
			{
				if (new_value->type == ABSTRACT_SET)
				{
					shared_ptr<AbstractSet> updating = aset(update), unifywith = aset(new_value);	// We need A = A U B

					shared_ptr<AbstractSet> unified = updating->_union(*unifywith);	// Let C = A U B.

					updating->criteria = unified->criteria;			// Now just replace A's criteria by that of C.
				}
				else raise_error("Expected an abstract set on the RHS for a \"U=\" operation with an abstract set on the LHS.");
			}
			else if (update->type == AUTO)
			{
				if (new_value->type == AUTO)
				{
					shared_ptr<Auto> updating = automaton(update), unifywith = automaton(new_value);

					shared_ptr<Auto> unified = updating->accepts_union(unifywith);

					updating->states = unified->states;		// Simple assignments, no pressure.
					updating->start = unified->start;
					updating->delta = unified->delta;
					updating->accepting = unified->accepting;
				}
				else raise_error("Expected an automaton on the RHS for a \"U=\" operation with an automaton on the LHS.");
			}
			else raise_error("Expected a set, abstract set, or automaton for a \"U=\" operation.");
		}
		else if (op.lexeme == "\\=")
		{
			if (update->type == SET)
			{
				if (new_value->type == SET)
				{
					shared_ptr<Set> updating = set(update), exclude = set(new_value);	// We need A = A U B

					for (int i{ 0 }; i < updating->elems->size(); i++)			// So for all elements in B ...
						if (exclude->has(*(*updating->elems)[i]))			// ... if there is any that is not in A.
							updating->elems->erase(updating->elems->begin() + i);   // ... add it to A.
				}
				else raise_error("Expected a set on the RHS for a \"\\=\" operation with a set on the LHS.");
			}
			else if (update->type == ABSTRACT_SET)
			{
				if (new_value->type == ABSTRACT_SET)
				{
					shared_ptr<AbstractSet> updating = aset(update), exclude = aset(new_value);	// We need A = A U B

					shared_ptr<AbstractSet> exclusive = updating->exclusion(*exclude);	// Let C = A U B.

					updating->criteria = exclusive->criteria;				// Now just replace A's criteria by that of C.
				}
				else raise_error("Expected an abstract set on the RHS for a \"\\=\" operation with an abstract set on the LHS.");
			}
			else if (update->type == AUTO)
			{
				if (new_value->type == AUTO)
				{
					shared_ptr<Auto> updating = automaton(update), exclude = automaton(new_value);

					shared_ptr<Auto> exclusive = updating->accepts_exclusively(exclude);

					updating->states = exclusive->states;		// Simple assignments, no pressure.
					updating->start = exclusive->start;
					updating->delta = exclusive->delta;
					updating->accepting = exclusive->accepting;
				}
				else raise_error("Expected an automaton on the RHS for a \"\\=\" operation with an automaton on the LHS.");
			}
			else raise_error("Expected a set, abstract set, or automaton on the RHS for a \"\\=\" operation.");
		}
		else if (op.lexeme == "x=")
		{
			if (update->type == SET)
			{
				if (new_value->type == SET)
				{
					shared_ptr<Set> updating = set(update), prodwith = set(new_value);	// We need A = A x B

					shared_ptr<Set> product = updating->cartesian_product(*prodwith);	// Just let C = A x B

					delete updating->elems;							// Get rid of A's vector.

					// And give it a new vector initialized with the elements in C's vector.
					updating->elems = new vector<shared_ptr<Elem>>(*prodwith->elems);	
				}
				else raise_error("Expected a set on the RHS for a \"x=\" operation with a set on the LHS.");
			}
			else if (update->type == ABSTRACT_SET)
			{	
				if (new_value->type == ABSTRACT_SET)
				{
					shared_ptr<AbstractSet> updating = aset(update), prodwith = aset(new_value);	// We need A = A x B

					shared_ptr<AbstractSet> product = updating->cartesian_product(*prodwith);	// Let C = A x B.

					updating->criteria = product->criteria;		   // Now just replace A's criteria by that of C.
				}
				else raise_error("Expected an abstract set on the RHS for a \"x=\" operation with an abstract set on the LHS.");
			}
			else raise_error("Expected a set or an abstract set a \"x=\" operation.");
		}
		else if (op.lexeme == "V=")
		{
			if (update->type == LOGICAL)
			{
				if (new_value->type == LOGICAL)
					logical(update)->elem = logical(update)->elem || logical(new_value)->elem;

				else if (new_value->type == INT)
					logical(update)->elem = logical(update)->elem || integer(new_value)->elem;

				else if (new_value->type == CHAR)
					logical(update)->elem = logical(update)->elem || character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"V=\" operation with a logical on the LHS.");
			}
			else if (update->type == INT)
			{
				if (new_value->type == LOGICAL)
					integer(update)->elem = integer(update)->elem || logical(new_value)->elem;

				else if (new_value->type == INT)
					integer(update)->elem = integer(update)->elem || integer(new_value)->elem;

				else if (new_value->type == CHAR)
					integer(update)->elem = integer(update)->elem || character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"V=\" operation with an int on the LHS.");
			}
			else if (update->type == CHAR)
			{
				if (new_value->type == LOGICAL)
					character(update)->elem = character(update)->elem || logical(new_value)->elem;

				else if (new_value->type == INT)
					character(update)->elem = character(update)->elem || integer(new_value)->elem;

				else if (new_value->type == CHAR)
					character(update)->elem = character(update)->elem || character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"V=\" operation with a char on the LHS.");
			}
			else raise_error("Expected a primitive for a \"V=\" operation.");
		}
		else if (op.lexeme == "+=")
		{
			if (update->type == LOGICAL)
			{
				if (new_value->type == LOGICAL)
					logical(update)->elem = logical(update)->elem + logical(new_value)->elem;

				else if (new_value->type == INT)
					logical(update)->elem = logical(update)->elem + integer(new_value)->elem;

				else if (new_value->type == CHAR)
					logical(update)->elem = logical(update)->elem + character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"+=\" operation with a logical on the LHS.");
			}
			else if (update->type == INT)
			{
				if (new_value->type == LOGICAL)
					integer(update)->elem = integer(update)->elem + logical(new_value)->elem;

				else if (new_value->type == INT)
					integer(update)->elem = integer(update)->elem + integer(new_value)->elem;

				else if (new_value->type == CHAR)
					integer(update)->elem = integer(update)->elem + character(new_value)->elem;
					
				else raise_error("Expected a primitive on the RHS for a \"+=\" operation with an int on the LHS.");
			}
			else if (update->type == CHAR)
			{
				if (new_value->type == LOGICAL)
					character(update)->elem = character(update)->elem + logical(new_value)->elem;

				else if (new_value->type == INT)
					character(update)->elem = character(update)->elem + integer(new_value)->elem;

				else if (new_value->type == CHAR)
					character(update)->elem = character(update)->elem + character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"+=\" operation with a char on the LHS.");
			}
			else if (update->type == STRING)
			{
				if (new_value->type == STRING)
					str(update)->elem = str(update)->elem + str(new_value)->elem;

				else if (new_value->type == CHAR)
					str(update)->elem = str(update)->elem + character(new_value)->elem;

				else raise_error("Expected a string or a char on the RHS for a \"+=\" operation with a string on the LHS.");
			}
			else if (update->type == DATASINK)
			{
				if (datasink(update)->raw->elem) // If the raw flag for the sink is set to true, we'll print the raw string.
					*datasink(update)->elem << new_value->to_string_raw();
				else
					*datasink(update)->elem << new_value->to_string();
			}
			else if (update->type == DATASOURCE)
			{
				if (new_value->type == INT)
				{
					if (datasource(update)->elem->eof())
					{
						datasource(update)->elem->clear();
						datasource(update)->elem->seekg(0, ios::beg);
					}
					datasource(update)->elem->seekg(integer(new_value)->elem, ios::cur);
				}
				else raise_error("Expected an integer on the RHS for a \"+=\" update with a source on the LHS.");
			}
			else raise_error("Expected a primitive, string, source, or sink for a \"+=\" operation.");
		}
		else if (op.lexeme == "-=")
		{
			if (update->type == LOGICAL)
			{
				if (new_value->type == LOGICAL)
					logical(update)->elem = logical(update)->elem - logical(new_value)->elem;

				else if (new_value->type == INT)
					logical(update)->elem = logical(update)->elem - integer(new_value)->elem;

				else if (new_value->type == CHAR)
					logical(update)->elem = logical(update)->elem - character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"-=\" operation with a logical on the LHS.");
			}
			else if (update->type == INT)
			{
				if (new_value->type == LOGICAL)
					integer(update)->elem = integer(update)->elem - logical(new_value)->elem;

				else if (new_value->type == INT)
					integer(update)->elem = integer(update)->elem - integer(new_value)->elem;

				else if (new_value->type == CHAR)
					integer(update)->elem = integer(update)->elem - character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"-=\" operation with an int on the LHS.");
			}
			else if (update->type == CHAR)
			{
				if (new_value->type == LOGICAL)
					character(update)->elem = character(update)->elem - logical(new_value)->elem;

				else if (new_value->type == INT)
					character(update)->elem = character(update)->elem - integer(new_value)->elem;

				else if (new_value->type == CHAR)
					character(update)->elem = character(update)->elem - character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"-=\" operation with a char on the LHS.");
			}
			else if (update->type == DATASOURCE)
			{
				if (new_value->type == INT)
				{
					if (datasource(update)->elem->eof()) 
					{
						datasource(update)->elem->clear();
						datasource(update)->elem->seekg(0, ios::end);
					}
					datasource(update)->elem->seekg(-integer(new_value)->elem, ios::cur);
				}

				else raise_error("Expected an integer on the RHS for a \"+=\" update with a source on the LHS.");
			}
			else raise_error("Expected a primitive or a source for a \"-=\" operation.");
		}
		else if (op.lexeme == "*=")
		{
			if (update->type == LOGICAL)
			{
				if (new_value->type == LOGICAL)
					logical(update)->elem = logical(update)->elem * logical(new_value)->elem;

				else if (new_value->type == INT)
					logical(update)->elem = logical(update)->elem * integer(new_value)->elem;

				else if (new_value->type == CHAR)
					logical(update)->elem = logical(update)->elem * character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"*=\" operation with a logical on the LHS.");
			}
			else if (update->type == INT)
			{
				if (new_value->type == LOGICAL)
					integer(update)->elem = integer(update)->elem * logical(new_value)->elem;

				else if (new_value->type == INT)
					integer(update)->elem = integer(update)->elem * integer(new_value)->elem;

				else if (new_value->type == CHAR)
					integer(update)->elem = integer(update)->elem * character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"*=\" operation with an int on the LHS.");
			}
			else if (update->type == CHAR)
			{
				if (new_value->type == LOGICAL)
					character(update)->elem = character(update)->elem * logical(new_value)->elem;

				else if (new_value->type == INT)
					character(update)->elem = character(update)->elem * integer(new_value)->elem;

				else if (new_value->type == CHAR)
					character(update)->elem = character(update)->elem * character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"*=\" operation with a char on the LHS.");
			}

			else raise_error("Expected a primitive for a \"*=\" operation.");
		}
		else if (op.lexeme == "/=")
		{
			if (update->type == LOGICAL)
			{
				if (new_value->type == LOGICAL)
					logical(update)->elem = logical(update)->elem / logical(new_value)->elem;

				else if (new_value->type == INT)
					logical(update)->elem = logical(update)->elem / integer(new_value)->elem;

				else if (new_value->type == CHAR)
					logical(update)->elem = logical(update)->elem / character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"/=\" operation with a logical on the LHS.");
			}
			else if (update->type == INT)
			{
				if (new_value->type == LOGICAL)
					integer(update)->elem = integer(update)->elem / logical(new_value)->elem;

				else if (new_value->type == INT)
					integer(update)->elem = integer(update)->elem / integer(new_value)->elem;

				else if (new_value->type == CHAR)
					integer(update)->elem = integer(update)->elem / character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"/=\" operation with an int on the LHS.");
			}
			else if (update->type == CHAR)
			{
				if (new_value->type == LOGICAL)
					character(update)->elem = character(update)->elem / logical(new_value)->elem;

				else if (new_value->type == INT)
					character(update)->elem = character(update)->elem / integer(new_value)->elem;

				else if (new_value->type == CHAR)
					character(update)->elem = character(update)->elem / character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"/=\" operation with a char on the LHS.");
			}

			else raise_error("Expected a primitive for a \"/=\" operation.");
		}
		else if (op.lexeme == "^=")
		{
			if (update->type == LOGICAL)
			{
				if (new_value->type == LOGICAL)
					logical(update)->elem = pow(logical(update)->elem, logical(new_value)->elem);

				else if (new_value->type == INT)
					logical(update)->elem = pow(logical(update)->elem, integer(new_value)->elem);

				else if (new_value->type == CHAR)
					logical(update)->elem = pow(logical(update)->elem, character(new_value)->elem);

				else raise_error("Expected a primitive on the RHS for a \"^=\" operation with a logical on the LHS.");
			}
			else if (update->type == INT)
			{
				if (new_value->type == LOGICAL)
					integer(update)->elem = pow(integer(update)->elem, logical(new_value)->elem);

				else if (new_value->type == INT)
					integer(update)->elem = pow(integer(update)->elem, integer(new_value)->elem);

				else if (new_value->type == CHAR)
					integer(update)->elem = pow(integer(update)->elem, character(new_value)->elem);

				else raise_error("Expected a primitive on the RHS for a \"^=\" operation with an int on the LHS.");
			}
			else if (update->type == CHAR)
			{
				if (new_value->type == LOGICAL)
					character(update)->elem = pow(character(update)->elem, logical(new_value)->elem);

				else if (new_value->type == INT)
					character(update)->elem = pow(character(update)->elem, integer(new_value)->elem);

				else if (new_value->type == CHAR)
					character(update)->elem = pow(character(update)->elem, character(new_value)->elem);

				else raise_error("Expected a primitive on the RHS for a \"^=\" operation with a char on the LHS.");
			}
			else if (update->type == MAP)
			{
				if (new_value->type == INT || new_value->type == CHAR || new_value->type == LOGICAL)
				{
					int power = 0;			// Compose the map with itself power times

					if (new_value->type == LOGICAL)  power =  integer(new_value)->elem - 1;	
					else if (new_value->type = INT)  power =  logical(new_value)->elem - 1;
					else if (new_value->type = CHAR) power = character(new_value)->elem- 1;

					shared_ptr<Map> original_map = map(update->deep_copy());

					while (power--)
					{
						shared_ptr<Map> temp = map(update)->composed_with(*original_map);
						delete map(update)->pi_indices;
						delete map(update)->_map;
						map(update)->pi_indices = new vector<int>(*temp->pi_indices);
						map(update)->_map = new unordered_map<int, int>(*temp->_map);
					}
				}	
				else raise_error("Expected an integer or another primitive on the RHS for a \"^=\" operation with a map on the LHS");
			}
			else if (update->type == ABSTRACT_MAP)
			{
				if (new_value->type == INT || new_value->type == CHAR || new_value->type == LOGICAL)
				{
					int power = 0;			// Compose the abstract map with itself power times

					if (new_value->type == LOGICAL)  power = integer(new_value)->elem - 1;
					else if (new_value->type = INT)  power = logical(new_value)->elem - 1;
					else if (new_value->type = CHAR) power = character(new_value)->elem - 1;

					while (power--)
					{
						shared_ptr<AbstractMap> raised_map = amap(update)->composed_with(amap(update));
						amap(update)->mapping_scheme = raised_map->mapping_scheme;
					}
				}
				else raise_error("Expected an integer or another primitive on the RHS for a \"^=\" operation with an abstract map on the LHS");
			}
			else raise_error("Expected a primitive, map, or abstract map for a \"^=\" operation.");
		}
		else if (op.lexeme == "%=")
		{
			if (update->type == LOGICAL)
			{
				if (new_value->type == LOGICAL)
					logical(update)->elem = logical(update)->elem % logical(new_value)->elem;

				else if (new_value->type == INT)
					logical(update)->elem = logical(update)->elem % integer(new_value)->elem;

				else if (new_value->type == CHAR)
					logical(update)->elem = logical(update)->elem % character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"%=\" operation with a logical on the LHS.");
			}
			else if (update->type == INT)
			{
				if (new_value->type == LOGICAL)
					integer(update)->elem = integer(update)->elem % logical(new_value)->elem;

				else if (new_value->type == INT)
					integer(update)->elem = integer(update)->elem % integer(new_value)->elem;

				else if (new_value->type == CHAR)
					integer(update)->elem = integer(update)->elem % character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"%=\" operation with an int on the LHS.");
			}
			else if (update->type == CHAR)
			{
				if (new_value->type == LOGICAL)
					character(update)->elem = character(update)->elem % logical(new_value)->elem;

				else if (new_value->type == INT)
					character(update)->elem = character(update)->elem % integer(new_value)->elem;

				else if (new_value->type == CHAR)
					character(update)->elem = character(update)->elem % character(new_value)->elem;

				else raise_error("Expected a primitive on the RHS for a \"%=\" operation with a char on the LHS.");
			}

			else raise_error("Expected a primitive for a \"%=\" operation.");
		}
		else if (op.lexeme == "o=")
		{
			if (update->type == MAP)
			{
				if (new_value->type == MAP)
				{
					shared_ptr<Map> raised_map = map(update)->composed_with(*map(new_value));
					delete map(update)->pi_indices;
					delete map(update)->_map;
					map(update)->domain_s = map(new_value)->domain_s;
					map(update)->pi_indices = new vector<int>(*raised_map->pi_indices);
					map(update)->_map = new unordered_map<int, int>(*raised_map->_map);					
				}
				else raise_error("Expected a map on the RHS for a \"o=\" operation with a map on the LHS");
			}
			else if (update->type == ABSTRACT_MAP)
			{
				if (new_value->type == ABSTRACT_MAP)
				{
					shared_ptr<AbstractMap> raised_map = amap(update)->composed_with(amap(new_value));
					amap(update)->mapping_scheme = raised_map->mapping_scheme;
				}
				else raise_error("Expected an abstract map on the RHS for a \"o=\" operation with an abstract map on the LHS");
			}
			else raise_error("Expected a map or an abstract map for a \"o=\" operation.");
		}
	}
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

		if      (data_type.lexeme ==   "set"  ) (*identify)[new_identifier.lexeme] = shared_ptr<   Set    >{new Set()};
		else if (data_type.lexeme ==  "tuple" ) (*identify)[new_identifier.lexeme] = shared_ptr<  Tuple   >{new Tuple()};
		else if (data_type.lexeme ==   "map"  ) (*identify)[new_identifier.lexeme] = shared_ptr<   Map    >{new Map(nullptr, nullptr)};
		else if (data_type.lexeme ==   "int"  ) (*identify)[new_identifier.lexeme] = shared_ptr<   Int    >{new Int()};
		else if (data_type.lexeme ==   "char" ) (*identify)[new_identifier.lexeme] = shared_ptr<   Char   >{new Char()};
		else if (data_type.lexeme ==  "string") (*identify)[new_identifier.lexeme] = shared_ptr<  String  >{new String()};
		else if (data_type.lexeme == "logical") (*identify)[new_identifier.lexeme] = shared_ptr<  Logical >{new Logical()};
		else if (data_type.lexeme ==   "auto" ) (*identify)[new_identifier.lexeme] = shared_ptr<   Auto   >{new Auto()};
		else if (data_type.lexeme ==  "source") (*identify)[new_identifier.lexeme] = shared_ptr<DataSource>{new DataSource()};
		else if (data_type.lexeme ==   "sink")  (*identify)[new_identifier.lexeme] = shared_ptr< DataSink >{new DataSink()};
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

void parse_fromsource()
{
	read_getfs_expr = true;

	Token get_this = get_next_token();

	ExpressionTree get_expr(get_this.lexeme);

	shared_ptr<Elem> get = get_expr.evaluate();

	Token op = get_next_token();

	if (op.types[0] != SOURCE_OP) raise_error("Expected an input/source operator.");

	read_right_expr = true;

	Token expression = get_next_token();

	ExpressionTree expr(expression.lexeme);

	shared_ptr<Elem> new_value = expr.evaluate();

	if (new_value->type == DATASOURCE)
	{
		if (datasource(new_value)->elem->eof())
		{
			cout << "WARNING: All the data from the source has already been read. ";
			cout << "Please update the source using the \"-=\" or \"+=\" updaters before reading data again.\n\n";
		}
		else
		{
			if (get->type == INT)
			{
				*datasource(new_value)->elem >> integer(get)->elem;
			}
			else if (get->type == CHAR)
			{
				datasource(new_value)->elem->get(character(get)->elem);
			}
			else if (get->type == LOGICAL)
			{
				string truthval;
				getline(*datasource(new_value)->elem, truthval, datasource(new_value)->delimiter->elem);
				logical(get)->elem = (truthval.find("True") != string::npos);
			}
			else if (get->type == STRING)
			{
				// Read a string from the file till the delimiter, and assign it to the 'elem' attr. of the obj to be updated.
				getline(*datasource(new_value)->elem, str(get)->elem, datasource(new_value)->delimiter->elem);
			}
			else if (get->type == SET)
			{
				string setrep;
				// Read a string from the file till the delimiter.
				getline(*datasource(new_value)->elem, setrep, datasource(new_value)->delimiter->elem);
				// Make a new set using the string.
				shared_ptr<Set> tempset = shared_ptr<Set>{ new Set(setrep) };
				if (set(get)->elems != nullptr) delete set(get)->elems;
				set(get)->elems = tempset->elems;
			}
			else if (get->type == TUPLE)
			{
				string tuprep;
				// Read a string from the file till the delimiter.	
				getline(*datasource(new_value)->elem, tuprep, datasource(new_value)->delimiter->elem);
				// Make a new tuple using the string.
				shared_ptr<Tuple> temptup = shared_ptr<Tuple>{ new Tuple(tuprep) };
				if (_tuple(get)->elems != nullptr) delete _tuple(get)->elems;
				_tuple(get)->elems = temptup->elems;
			}
			else if (get->type == ABSTRACT_SET)
			{
				// So, since the criteria for an abstract set is a ready-to-execute logical expression that doesn't ... 
				// ... need to be parsed, but readily executed, we don't really need a raw representation of the string.
				// (hope I'm right about this).
				getline(*datasource(new_value)->elem, aset(get)->criteria, datasource(new_value)->delimiter->elem);
			}
			else if (get->type == ABSTRACT_MAP)
			{
				// Same for maps.
				getline(*datasource(new_value)->elem, amap(get)->mapping_scheme, datasource(new_value)->delimiter->elem);
			}
			else raise_error("Expected a non-source/sink type for reading via a source.");
		}
	}
	else if (new_value->type == STRING) // Strings can also act as sources.
	{
		if (get->type == INT)
		{
			integer(get)->elem = std::stoi(str(new_value)->elem);
		}
		else if (get->type == CHAR) // There's no reason to ever use this really. But well, to each their own.
		{
			character(get)->elem = (str(new_value)->elem)[0];
		}
		else if (get->type == LOGICAL)
		{
			logical(get)->elem = (str(new_value)->elem.find("True") != string::npos);
		}
		else if (get->type == SET)
		{
			// Make a new set passing the raw string representation of the requested set to the constructor.
			string setrep = str(new_value)->to_string_raw();
			shared_ptr<Set> tempset = shared_ptr<Set>{ new Set(setrep) };
			if (set(get)->elems != nullptr) delete set(get)->elems;
			set(get)->elems = tempset->elems;
		}
		else if (get->type == TUPLE)
		{
			string tuprep = str(new_value)->to_string_raw();
			shared_ptr<Tuple> temptup = shared_ptr<Tuple>{ new Tuple(tuprep) };
			if (_tuple(get)->elems != nullptr) delete _tuple(get)->elems;
			_tuple(get)->elems = temptup->elems;
		}
		else if (get->type == ABSTRACT_SET)
		{
			// So, since the criteria for an abstract set is a ready-to-execute logical expression that doesn't ... 
			// ... need to be parsed, but readily executed, we don't really need a raw representation of the string.
			// (hope I'm right about this).
			aset(get)->criteria = str(new_value)->elem;
		}
		else if (get->type == ABSTRACT_MAP)
		{
			// Same for maps.
			amap(get)->mapping_scheme = str(new_value)->elem;
		}
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
			Token init_op = get_next_token();

			read_right_expr = true;

			if (init_op.lexeme == "=") 
			{
				Token value = get_next_token();			// The value to be assigned to the identifier.

				ExpressionTree value_expr(value.lexeme);

				shared_ptr<Elem> val = value_expr.evaluate();

				if (val->type != SET) raise_error("Cannot assign a non-set object to a set identifier.");

				(*identify)[new_identifier.lexeme] = val;
			}
			else if (init_op.lexeme == "<-")
			{
				Token source = get_next_token();		// The source from which the set is to be read.

				ExpressionTree source_expr(source.lexeme);

				shared_ptr<Elem> src = source_expr.evaluate();

				string setrep;

				if (src->type == DATASOURCE)
				{
					getline(*datasource(src)->elem, setrep, datasource(src)->delimiter->elem);

					(*identify)[new_identifier.lexeme] = shared_ptr<Set>{ new Set(setrep) };
				}
				else if (src->type == STRING)
				{
					setrep = str(src)->elem;

					(*identify)[new_identifier.lexeme] = shared_ptr<Set>{ new Set(setrep) };
				}
				else raise_error("Expected a source or a string for a \"<-\" operation.");
			}
			else raise_error("Expected operator \"=\" or \"<-\".");
		}
		else if (data_type.lexeme == "tuple")
		{
			Token init_op = get_next_token();

			read_right_expr = true;

			if (init_op.lexeme == "=")
			{
				Token value = get_next_token();			// The value to be assigned to the identifier.

				ExpressionTree value_expr(value.lexeme);

				shared_ptr<Elem> val = value_expr.evaluate();

				if (val->type != TUPLE) raise_error("Cannot assign a non-tuple object to a tuple identifier.");

				(*identify)[new_identifier.lexeme] = val;
			}
			else if (init_op.lexeme == "<-")
			{
				Token source = get_next_token();		// The source from which the tuple is to be read.

				ExpressionTree source_expr(source.lexeme);

				shared_ptr<Elem> src = source_expr.evaluate();

				string tuprep;

				if (src->type == DATASOURCE)
				{
					getline(*datasource(src)->elem, tuprep, datasource(src)->delimiter->elem);

					(*identify)[new_identifier.lexeme] = shared_ptr<Tuple>{ new Tuple(tuprep) };
				}
				else if (src->type == STRING)
				{
					tuprep = str(src)->elem;
						
					(*identify)[new_identifier.lexeme] = shared_ptr<Tuple>{ new Tuple(tuprep) };
				}
				else raise_error("Expected a source or a string for a \"<-\" operation.");
			}
			else raise_error("Expected operator \"=\" or \"<-\".");
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

			if (mapsymb.types[0] != MAPPING_SYMBOL) raise_error("Missing mapping operator \"->\".");

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
				set(map_domain),
				set(map_codomain)
			)};
		}
		else if (data_type.lexeme == "int") 
		{
			Token init_op = get_next_token();

			read_right_expr = true;

			if (init_op.lexeme == "=")
			{
				Token int_expression = get_next_token();

				ExpressionTree int_expr(int_expression.lexeme);

				shared_ptr<Elem> val = int_expr.evaluate();

				if (val->type != INT) raise_error("Cannot assign a non-int value to an int identifier.");

				(*identify)[new_identifier.lexeme] = integer(val);
			}
			else if (init_op.lexeme == "<-")
			{
				Token src_expression = get_next_token();

				ExpressionTree src_expr(src_expression.lexeme);

				shared_ptr<Elem> src = src_expr.evaluate();

				if (src->type == DATASOURCE)
				{
					int val;
					*datasource(src)->elem >> val;
					(*identify)[new_identifier.lexeme] = shared_ptr<Int>{ new Int(val) };
				}
				else if (src->type == STRING)
				{
					string val;
					getline(*datasource(src)->elem, val, datasource(src)->delimiter->elem);
					(*identify)[new_identifier.lexeme] = shared_ptr<Int>{ new Int(std::stoi(val)) };
				}
				else raise_error("Expected a source or a string for a \"<-\" operation.");
			}
			else raise_error("Expected operators \"=\" or \"<-\".");
		}
		else if (data_type.lexeme == "char")
		{
			Token init_op = get_next_token();

			read_right_expr = true;

			if (init_op.lexeme == "=")
			{
				Token char_expression = get_next_token();

				ExpressionTree char_expr(char_expression.lexeme);

				shared_ptr<Elem> val = char_expr.evaluate();

				if (val->type != CHAR) raise_error("Cannot assign a non-char value to a char identifier.");

				(*identify)[new_identifier.lexeme] = character(val);
			}
			else if (init_op.lexeme == "<-")
			{
				Token src_expression = get_next_token();

				ExpressionTree src_expr(src_expression.lexeme);

				shared_ptr<Elem> src = src_expr.evaluate();

				if (src->type == DATASOURCE)
				{
					char val;
					datasource(src)->elem->get(val);
					(*identify)[new_identifier.lexeme] = shared_ptr<Char>{ new Char(val) };
				}
				else if (src->type == STRING)
				{
					string val;
					getline(*datasource(src)->elem, val, datasource(src)->delimiter->elem);
					(*identify)[new_identifier.lexeme] = shared_ptr<Char>{ new Char(val[0]) };
				}
				else raise_error("Expected a source or a string for a \"<-\" operation.");
			}
			else raise_error("Expected operators \"=\" or \"<-\".");
		}
		else if (data_type.lexeme == "string") 
		{
			Token init_op = get_next_token();

			read_right_expr = true;

			if (init_op.lexeme == "=")
			{
				Token string_expression = get_next_token();

				ExpressionTree string_expr(string_expression.lexeme);

				shared_ptr<Elem> val = string_expr.evaluate();

				if (val->type != STRING) raise_error("Cannot assign a non-string value to a string identifier.");

				(*identify)[new_identifier.lexeme] = str(val);
			}
			else if (init_op.lexeme == "<-")
			{
				Token src_expression = get_next_token();

				ExpressionTree src_expr(src_expression.lexeme);

				shared_ptr<Elem> src = src_expr.evaluate();

				if (src->type == DATASOURCE)
				{
					string val;
					getline(*datasource(src)->elem, val, datasource(src)->delimiter->elem);
					(*identify)[new_identifier.lexeme] = shared_ptr<String>{ new String(val) };
				}
				else if (src->type == STRING)
				{
					string val = str(src)->elem;
					(*identify)[new_identifier.lexeme] = shared_ptr<String>{ new String(val) };
				}
				else raise_error("Expected a source or a string for a \"<-\" operation.");
			}
			else raise_error("Expected operators \"=\" or \"<-\".");
		}
		else if (data_type.lexeme == "logical")
		{
			Token init_op = get_next_token();

			read_right_expr = true;

			if (init_op.lexeme == "=")
			{
				Token logical_expression = get_next_token();

				ExpressionTree logical_expr(logical_expression.lexeme);

				shared_ptr<Elem> val = logical_expr.evaluate();

				if (val->type != LOGICAL) raise_error("Cannot assign a non-logical value to a logical identifier.");

				(*identify)[new_identifier.lexeme] = logical(val);
			}
			else if (init_op.lexeme == "<-")
			{
				Token src_expression = get_next_token();

				ExpressionTree src_expr(src_expression.lexeme);

				shared_ptr<Elem> src = src_expr.evaluate();

				if (src->type == DATASOURCE)
				{
					string val;
					getline(*datasource(src)->elem, val, datasource(src)->delimiter->elem);
					(*identify)[new_identifier.lexeme] = shared_ptr<Logical>{
						 new Logical(val.find("True") != string::npos) 
					};
				}
				else if (src->type == STRING)
				{
					string val = str(src)->elem;
					(*identify)[new_identifier.lexeme] = shared_ptr<Logical>{
						new Logical(val.find("True") != string::npos)
					};
				}
				else raise_error("Expected a source or a string for a \"<-\" operation.");
			}
			else raise_error("Expected operators \"=\" or \"<-\".");
		}
		else if (data_type.lexeme == "auto")
		{
			Token equal_sign = get_next_token();

			if (equal_sign.lexeme != "=") raise_error("Missing operator \"=\".");

			read_right_expr = true;

			Token tuple_expression = get_next_token();

			ExpressionTree tuple_expr(tuple_expression.lexeme);

			shared_ptr<Elem> val = tuple_expr.evaluate();

			if (val->type == TUPLE) 
			{
				shared_ptr<Tuple> val_ = _tuple(val);

				if (val_->size() != 5) raise_error("Initializing an automaton needs a 5-tuple.");

				(*identify)[new_identifier.lexeme] = shared_ptr<Auto>{new Auto(	// Make a new automaton object.
					set((*val_)[0]), 
					set((*val_)[1]), 
					(*val_)[2], 
					map((*val_)[3]), 
					set((*val_)[4]),
					DIRECT_ASSIGN
				)};
			}
			else if (val->type == AUTO)
			{
				(*identify)[new_identifier.lexeme] = automaton(val);
			}
			else raise_error("Initializing an automaton needs an automaton or a tuple.");
		}
		else if (data_type.lexeme == "source")
		{
			Token equal_sign = get_next_token();

			if (equal_sign.lexeme != "=") raise_error("Missing operator \"=\".");

			read_right_expr = true;

			Token tuple_expression = get_next_token();

			ExpressionTree tuple_expr(tuple_expression.lexeme);

			shared_ptr<Elem> val = tuple_expr.evaluate();

			if (val->type == TUPLE)
			{
				shared_ptr<Tuple> val_ = _tuple(val);

				if (val_->size() != 2) raise_error("Initializing a source needs a 2-tuple.");

				if ((*val_)[0]->type != STRING) raise_error("Expected a 2-tuple with a string as the first element.");

				if ((*val_)[1]->type != CHAR) raise_error("Expected a 2-tuple with a char as the second element.");

				(*identify)[new_identifier.lexeme] = shared_ptr<DataSource> {new DataSource(	// Make a new automaton object.
					str((*val_)[0])->elem.c_str(),					
					character((*val_)[1])
				)};
			}
			else if (val->type == DATASOURCE)
			{
				(*identify)[new_identifier.lexeme] = datasource(val);
			}
			else raise_error("Initializing a source needs a source or a tuple.");
		}
		else if (data_type.lexeme == "sink")
		{
			Token equal_sign = get_next_token();

			if (equal_sign.lexeme != "=") raise_error("Missing operator \"=\".");

			read_right_expr = true;

			Token tuple_expression = get_next_token();

			ExpressionTree tuple_expr(tuple_expression.lexeme);

			shared_ptr<Elem> val = tuple_expr.evaluate();

			if (val->type == TUPLE)
			{
				shared_ptr<Tuple> val_ = _tuple(val);

				if (val_->size() != 2) raise_error("Initializing a sink needs a 2-tuple.");

				if ((*val_)[0]->type != STRING) raise_error("Expected a 2-tuple with a string as the first element.");

				if ((*val_)[1]->type != LOGICAL) raise_error("Expected a 2-tuple with a logical as the second element.");

				(*identify)[new_identifier.lexeme] = shared_ptr<DataSink> {new DataSink(	// Make a new automaton object.
					str((*val_)[0])->elem.c_str(),
					logical((*val_)[1])
				)};
			}
			else if (val->type == DATASINK)
			{
				(*identify)[new_identifier.lexeme] = datasink(val);
			}
			else raise_error("Initializing a sink needs a sink or a tuple.");
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
			Token init_op = get_next_token();

			read_right_expr = true;

			if (init_op.lexeme == "=") 
			{
				Token abstract_set = get_next_token();

				(*identify)[new_identifier.lexeme] = shared_ptr<AbstractSet>{new AbstractSet(abstract_set.lexeme)};
			}
			else if (init_op.lexeme == "<-")
			{
				Token src_expression = get_next_token();

				ExpressionTree src_expr(src_expression.lexeme);

				shared_ptr<Elem> src = src_expr.evaluate();

				if (src->type == DATASOURCE)
				{
					string val;
					getline(*datasource(src)->elem, val, datasource(src)->delimiter->elem);
					(*identify)[new_identifier.lexeme] = shared_ptr<AbstractSet>{new AbstractSet(val)};					
				}
				else if (src->type == STRING)
				{
					string val = str(src)->elem;
					(*identify)[new_identifier.lexeme] = shared_ptr<AbstractSet>{new AbstractSet(val)};
				}
			}
			else raise_error("Missing operator \"=\" or \"<-\".");
		}
		else if (type.lexeme == "map")
		{
			Token init_op = get_next_token();

			if (init_op.types[0] == COLON) 
			{
				read_mapdom_expr = true;

				Token abstract_map_domain = get_next_token();

				Token mapssymb = get_next_token();

				if (mapssymb.types[0] == MAPPING_SYMBOL) raise_error("Missing operator \"->\".");

				read_right_expr = true;

				Token abstract_map_codomain = get_next_token();
 
				ExpressionTree abstract_mapdom_expr(abstract_map_domain.lexeme);

				ExpressionTree abstract_mapcodom_expr(abstract_map_codomain.lexeme);

				shared_ptr<Elem> domain = abstract_mapdom_expr.evaluate(), codomain = abstract_mapcodom_expr.evaluate();

				if (domain->type != ABSTRACT_SET || codomain->type != ABSTRACT_SET) raise_error("Abstract sets expected.");

				(*identify)[new_identifier.lexeme] = shared_ptr<AbstractMap> { new AbstractMap (
					aset(domain), 
					aset(codomain)
				)};
			}
			else if (init_op.lexeme == "<-")
			{
				Token src_expression = get_next_token();

				ExpressionTree src_expr(src_expression.lexeme);

				shared_ptr<Elem> src = src_expr.evaluate();

				if (src->type == DATASOURCE)
				{
					string val1 = "x -> ", val2;
					getline(*datasource(src)->elem, val2, datasource(src)->delimiter->elem);
					val1 += val2;
					(*identify)[new_identifier.lexeme] = shared_ptr<AbstractMap>{new AbstractMap(val1)};
				}
				else if (src->type == STRING)
				{
					string val1 = "x -> ", val2 = str(src)->elem;
					val1 += val2;
					(*identify)[new_identifier.lexeme] = shared_ptr<AbstractMap>{new AbstractMap(val1)};
				}
				else raise_error("Expected a source or a string for a \"<-\" operation.");
			}
			else raise_error("Missing operator \":\" or \"<-\".");
		}
		else raise_error("Only sets and maps can be abstract.");
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

	while (logical(do_or_not)->elem)
	{
		if (logical_condition != nullptr) delete logical_condition;	// We have no use for the old parse tree of the condition ...

		current_token = get_next_token();

		while (current_token.types[0] != R_BRACE)			// As long as you don't see the end of the while loop ...
			parse_statement();					// ... keep parsing statements.

		if (program != &cin)
			program->seekg(loop_from, ios::beg);			// Go back to the beginning of statements when you're done.
		
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
		if (token.types[0] == LET) read_update_expr = true;
		if (token.types[0] == GET) read_getfs_expr = true;
		if (token.types[0] == UNDER) read_map_expr = true;
		if (token.types[0] == UPDATE_OP || token.types[0] == COLON || token.types[0] == PRINT
		    || token.types[0] == WHILE || token.types[0] == IF || token.types[0] == SOURCE_OP) read_right_expr = true;
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

	if (logical(do_or_not)->elem)
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
			if (token.types[0] == LET) read_update_expr = true;
			if (token.types[0] == GET) read_getfs_expr = true;
			if (token.types[0] == UNDER) read_map_expr = true;
			if (token.types[0] == UPDATE_OP || token.types[0] == COLON || token.types[0] == PRINT
				|| token.types[0] == WHILE || token.types[0] == IF || token.types[0] == SOURCE_OP) read_right_expr = true;
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

			if (token.types[0] == LET) read_update_expr = true;
			if (token.types[0] == GET) read_getfs_expr = true;
			if (token.types[0] == UNDER) read_map_expr = true;
			if (token.types[0] == UPDATE_OP || token.types[0] == COLON || token.types[0] == PRINT
				|| token.types[0] == WHILE || token.types[0] == IF || token.types[0] == SOURCE_OP) read_right_expr = true;
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

void trim(string &s)
{
	if (all_spaces(s)) return;
	int start = 0, end = s.size() - 1;
	while (isspace(s[start])) start++;
	while (isspace(s[ end ]))  end-- ;
	if (start >= end) s == "";
	s = s.substr(start, end - start + 1);
}

bool identifier(string &s)
{
	if (s[0] == '_' || isalpha(s[0])) {
		for (int i = 1; i < s.size(); i++)
			if (!isalnum(s[i]) && s[i] != '_')
				return false;
		return true;
	}
	else return false;
}

bool all_spaces(string& s)
{
	for (auto& c : s)
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

void remove_comment(string &x)
{
	int level = 0, i = 0, comment_found_at = 0;
	bool in_string = false, in_char = false, comment_found = false;
	for (i = 0; i < x.size(); i++)
	{
		if (((x[i] == '"' && !in_string && !in_char) || (x[i] == '\'' && !in_char && !in_string) ||
			x[i] == '{' || x[i] == '(' || x[i] == '[')
			&&                                                                 // ... and is not escaped.
			(i == 0 || (x[i - 1] != '\\' || (x[i - 1] == '\\' && i - 2 >= 0 && x[i - 2] == '\\'))))
		{
			level++;
			if (x[i] == '"' && !in_string && !in_char) in_string = true;
			if (x[i] == '\'' && !in_char && !in_string) in_char = true;
		}
		else if (((x[i] == '"' && in_string) || (x[i] == '\'' && in_char) ||
			x[i] == '}' || x[i] == ')' || x[i] == ']')
			&& (i == 0 || (x[i - 1] != '\\' || (x[i - 1] == '\\' && i - 2 >= 0 && x[i - 2] == '\\')))) {
			level--;
			if (x[i] == '"' && in_string) in_string = false;
			if (x[i] == '\'' && in_char) in_char = false;
		}
		else if (x[i] == '#' && level == 0)		// It's a tuple if a comma exists at level 0.
		{
			comment_found = true;
			comment_found_at = i;
			break;
		}
	}
	if (comment_found) x = x.substr(0, comment_found_at);
} 
