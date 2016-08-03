#ifndef PROGRAM_VARS_H
#define PROGRAM_VARS_H

#include "Elem.h"

namespace program_vars
{
	extern unordered_map<string, shared_ptr<Elem>> * identify; // Stores the objects against their identifiers.
	extern int line_num;
	extern void raise_error(const char *);
	extern unordered_map<string, bool> * keyword_ops;
}

#endif