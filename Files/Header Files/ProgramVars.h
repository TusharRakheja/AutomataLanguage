#ifndef PROGRAM_VARS_H
#define PROGRAM_VARS_H

#define ANY true
#define DUMMYc '\0'

#include "Elem.h"
#include <vector>

namespace program_vars
{
	extern std::vector<unordered_map<string, shared_ptr<Elem>>> * scopewise_identifiers; // Stores the objects against their scope and names.
	extern unordered_map<string, shared_ptr<Elem>> * identify;	      // Points to the most immediate scope's identifiers.
	extern int line_num;
	extern int scope_level;
	extern void raise_error(const char *);
	extern unordered_map<string, bool> * keyword_ops;
	extern int find_at_level_0(const string &, bool, char, std::vector<char> &);
	extern bool exists_at_level_0(const string &, bool, char, std::vector<char> &);
	extern std::vector<int> findall_at_level_0(const string &, bool, char, std::vector<char> &);
	extern std::vector<char> DUMMYv;
	extern std::vector<char> op_signs_set;
}

#endif