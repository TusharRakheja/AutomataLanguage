#ifndef DATASOURCE_H
#define DATASOURCE_H

#include "Char.h"
#include <iostream>
#include <fstream>
#include <iterator>

using std::ifstream;
using std::istream;

#define datasource static_pointer_cast<DataSource>
#define filesource static_pointer_cast<ifstream>

class DataSource : public Elem
{
public:
	shared_ptr<istream> elem;
	shared_ptr<Char> delimiter;

	DataSource(const char * filepath, shared_ptr<Char> delimiter) : Elem(DATASOURCE)
	{
		elem = shared_ptr<ifstream>{new ifstream(filepath)};
		if (!*elem) program_vars::raise_error("Failed to open file.");
		this->delimiter = delimiter;
	}

	DataSource(int console, shared_ptr<Char> delimiter) : Elem(DATASOURCE) 
	{
		elem = shared_ptr<istream>{&std::cin};
		this->delimiter = delimiter;
	}

	DataSource() : Elem(DATASOURCE) { }

	shared_ptr<Elem> deep_copy()
	{
		shared_ptr<DataSource> copy = shared_ptr<DataSource>{new DataSource()};
		copy->elem = this->elem; 
		copy->delimiter = character(copy->delimiter->deep_copy());
		return copy;
	}
	shared_ptr<Char> operator[](int always1)
	{
		if (always1 != 1) program_vars::raise_error("The source's only accessible member is the 'delimiter', at index 1.");
		return delimiter;
	}
	const shared_ptr<Char> operator[](int always1) const
	{
		if (always1 != 1) program_vars::raise_error("The source's only accessible member is the 'delimiter', at index 1.");
		return delimiter;
	}
	string to_string()
	{
		string representation((std::istreambuf_iterator<char>(*elem)), std::istreambuf_iterator<char>());
		elem->get();
		return representation;
	}
	~DataSource()
	{
		if (elem != nullptr && elem != shared_ptr < istream > {&std::cin})
		{
			ifstream &e = static_cast<ifstream &>(*elem);
			e.close();
		}
	}
};

#endif