#include "../Header Files/ProgramVars.h"
#include "../Header Files/ExpressionTree.h"

/* Implementations and assignments for all the nethods and objects in the namespace program_vars.*/

std::vector<unordered_map<string, shared_ptr<Elem>>> * program_vars::scopewise_identifiers = new std::vector<unordered_map<string, shared_ptr<Elem>>>
{
	{
		{ "__prompt__", shared_ptr<String>{ new String(">>> ") } },
		{ "console", shared_ptr<DataSource>{ new DataSource(0, shared_ptr<Char>{ new Char('\n') }) } },
		{ "apply", shared_ptr<AbstractMap>{ new AbstractMap(
		"(am, s) -> (|s| == 1) ? { am[s[0]] } : ({ am[s[0]] } U apply[(am, s[(1, |s|)])])"
		) } },
		{ "fold",  shared_ptr<AbstractMap>{ new AbstractMap(
		"(am, s) -> (|s| == 1) ? (s[0]) : (am[(s[0], fold[(am, s[(1, |s|)])])])"
		) } },
		{ "All", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | True }") } },
		{ "Set", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"set\" }") } },
		{ "Int", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"int\" }") } },
		{ "Map", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"map\" }") } },
		{ "Tuple", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"tuple\" }") } },
		{ "String", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"string\" }") } },
		{ "Source", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"source\" }") } },
		{ "Sink", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"sink\" }") } },
		{ "AMap", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"abstract map\" }") } },
		{ "ASet", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"abstract set\" }") } },
		{ "Sets", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | \"set\" in typeof elem }") } },
		{ "Auto", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"auto\" }") } },
		{ "Char", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"char\" }") } },
		{ "Logical", shared_ptr<AbstractSet>{ new AbstractSet("{ elem | typeof elem == \"logical\" }") } },
	}
};

unordered_map<string, shared_ptr<Elem>> * program_vars::identify = &(*program_vars::scopewise_identifiers)[0];

unordered_map<string, bool> * program_vars::keyword_ops = new unordered_map<string, bool>
{
	{ "abstract", true },
	{ "set", true },
	{ "int", true },
	{ "char", true },
	{ "string", true },
	{ "auto", true },
	{ "map", true },
	{ "source", true },
	{ "sink", true },
	{ "in", true },
	{ "x", true },
	{ "o", true },
	{ "U", true },
	{ "V", true },
	{ "c", true },
	{ "typeof", true },
	{ "True", true },
	{ "False", true }
};

bool program_vars::exists_at_level_0(const string & in, bool any, char find_this, std::vector<char> & anyof)
{
	int level = 0;
	bool in_string = false;	  // Helps us keep track of the level when strings are involved.
	bool in_char = false;     // Helps us keep track of the level when chars are involved.
	//cout << "Elem: " << rep << endl;
	for (int i = 0; i < in.size(); i++)
	{
		// If, it's level 0, and the character that we're looking at right now in the in. is the sign of an operator.
		if (level == 0)
		{
			if (any && std::find(anyof.begin(), anyof.end(), in[i]) != anyof.end())
				return true;
			else if (!any && in[i] == find_this)
				return true;
		}
		if (((in[i] == '"' && !in_string && !in_char) || (in[i] == '\'' && !in_char && !in_string) ||
			in[i] == '{' || in[i] == '(' || in[i] == '[')
			&&                                                                 // ... and is not escaped.
			(i == 0 || (in[i - 1] != '\\' || (in[i - 1] == '\\' && i - 2 >= 0 && in[i - 2] == '\\'))))
		{
			level++;
			if (in[i] == '"' && !in_string && !in_char) in_string = true;
			if (in[i] == '\'' && !in_char && !in_string) in_char = true;
		}
		else if (((in[i] == '"' && in_string) || (in[i] == '\'' && in_char) ||
			in[i] == '}' || in[i] == ')' || in[i] == ']')
			&& (i == 0 || (in[i - 1] != '\\' || (in[i - 1] == '\\' && i - 2 >= 0 && in[i - 2] == '\\'))))
		{
			level--;
			if (in[i] == '"' && in_string) in_string = false;
			if (in[i] == '\'' && in_char) in_char = false;
		}
	}
	return false;
}

int program_vars::find_at_level_0(const string & in, bool any, char find_this, std::vector<char> & anyof)
{
	int level = 0;
	bool in_string = false;	  // Helps us keep track of the level when strings are involved.
	bool in_char = false;     // Helps us keep track of the level when chars are involved.
	//cout << "Elem: " << rep << endl;
	for (int i = 0; i < in.size(); i++)
	{
		// If, it's level 0, and the character that we're looking at right now in the in. is the sign of an operator.
		if (level == 0)
		{
			if (any && std::find(anyof.begin(), anyof.end(), in[i]) != anyof.end())
				return i;
			else if (!any && in[i] == find_this)
				return i;
		}
		if (((in[i] == '"' && !in_string && !in_char) || (in[i] == '\'' && !in_char && !in_string) ||
			in[i] == '{' || in[i] == '(' || in[i] == '[')
			&&                                                                 // ... and is not escaped.
			(i == 0 || (in[i - 1] != '\\' || (in[i - 1] == '\\' && i - 2 >= 0 && in[i - 2] == '\\'))))
		{
			level++;
			if (in[i] == '"' && !in_string && !in_char) in_string = true;
			if (in[i] == '\'' && !in_char && !in_string) in_char = true;
		}
		else if (((in[i] == '"' && in_string) || (in[i] == '\'' && in_char) ||
			in[i] == '}' || in[i] == ')' || in[i] == ']')
			&& (i == 0 || (in[i - 1] != '\\' || (in[i - 1] == '\\' && i - 2 >= 0 && in[i - 2] == '\\'))))
		{
			level--;
			if (in[i] == '"' && in_string) in_string = false;
			if (in[i] == '\'' && in_char) in_char = false;
		}
	}
	return in.size();
}

vector<int> program_vars::findall_at_level_0(const string & in, bool any, char find_this, vector<char> & anyof)
{
	vector<int> indices;
	int level = 0;
	bool in_string = false;	  // Helps us keep track of the level when strings are involved.
	bool in_char = false;     // Helps us keep track of the level when chars are involved.
	//cout << "Elem: " << rep << endl;
	for (int i = 0; i < in.size(); i++)
	{
		// If, it's level 0, and the character that we're looking at right now in the in. is the sign of an operator.
		if (level == 0)
		{
			if (any && std::find(anyof.begin(), anyof.end(), in[i]) != anyof.end())
				indices.push_back(i);
			else if (!any && in[i] == find_this)
				indices.push_back(i);
		}
		if (((in[i] == '"' && !in_string && !in_char) || (in[i] == '\'' && !in_char && !in_string) ||
			in[i] == '{' || in[i] == '(' || in[i] == '[')
			&&                                                                 // ... and is not escaped.
			(i == 0 || (in[i - 1] != '\\' || (in[i - 1] == '\\' && i - 2 >= 0 && in[i - 2] == '\\'))))
		{
			level++;
			if (in[i] == '"' && !in_string && !in_char) in_string = true;
			if (in[i] == '\'' && !in_char && !in_string) in_char = true;
		}
		else if (((in[i] == '"' && in_string) || (in[i] == '\'' && in_char) ||
			in[i] == '}' || in[i] == ')' || in[i] == ']')
			&& (i == 0 || (in[i - 1] != '\\' || (in[i - 1] == '\\' && i - 2 >= 0 && in[i - 2] == '\\'))))
		{
			level--;
			if (in[i] == '"' && in_string) in_string = false;
			if (in[i] == '\'' && in_char) in_char = false;
		}
	}
	return indices;
}

std::vector<char> program_vars::DUMMYv = { {} };
std::vector<char> program_vars::op_signs_set = { // These characters will signify the presence of an operator.
	'+', '-', '*', '/', '^', '%', '\\', '.', 'U', 'i', '|',
	'?', 'V', '&', '=', '!', '<', '>', 'o', 'c', 'x', '[', '!'
}; // Just the first (often the only) characters in the operators. 
int program_vars::line_num = 1;

int program_vars::scope_level = 0;