#ifndef PROGRAM_VARS_H
#define PROGRAM_VARS_H

#define ANY true
#define DUMMYc '\0'
#define DUMMYv vector<char>{{}}

#include "Elem.h"

namespace program_vars
{
	extern unordered_map<string, shared_ptr<Elem>> * identify; // Stores the objects against their identifiers.
	extern int line_num;
	extern void raise_error(const char *);
	extern unordered_map<string, bool> * keyword_ops;
	extern int find_at_level_0(const string &, bool, char, std::vector<char> &);
	extern bool exists_at_level_0(const string &, bool, char, std::vector<char> &);
	extern std::vector<int> findall_at_level_0(const string &, bool, char, std::vector<char> &);
}

#endif