#ifndef PROGRAM_VARS_H
#define PROGRAM_VARS_H

#include "Elem.h"
#include <unordered_map>

namespace program_vars
{
	extern std::unordered_map<string, Elem *> * identify; // Stores the objects against their identifiers.
	extern int line_num;
	extern void raise_error(const char *);
}

#endif