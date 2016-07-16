#ifndef DATASINK_H
#define DATASINK_H

#include "Logical.h"
#include <iostream>
#include <fstream>

using std::ofstream;
using std::ios;

#define datasink static_pointer_cast<DataSink>

class DataSink : public Elem
{
public:
	shared_ptr<ofstream> elem;
	shared_ptr<Logical> append;
	shared_ptr<Logical> raw;

	DataSink(const char * filepath, shared_ptr<Logical> append, shared_ptr<Logical> raw) : Elem(DATASINK)
	{
		if (append->elem)
			elem = shared_ptr<ofstream>{new ofstream(filepath, ios::app)};
		else
			elem = shared_ptr<ofstream>{new ofstream(filepath)};
			
		if (!*elem) program_vars::raise_error("Failed to open file.");

		this->append = append; 
		this->raw = raw;
	}

	DataSink() : Elem(DATASINK) { }

	shared_ptr<Elem> deep_copy()
	{
		shared_ptr<DataSink> copy = shared_ptr<DataSink>{new DataSink()};
		copy->elem = this->elem;
		copy->append = logical(this->append->deep_copy());
		copy->raw = logical(this->raw->deep_copy());
		return copy;
	}
	shared_ptr<Logical> operator[](int always1or2)
	{
		if (always1or2 != 1 && always1or2 != 2) 
			program_vars::raise_error("The sink's only accessible members are the 'append' and 'raw' flags, at indices 1 and 2 respectively.");
		if (always1or2 == 1) return append;
		else return raw;
	}
	const shared_ptr<Logical> operator[](int always1or2) const
	{
		if (always1or2 != 1 && always1or2 != 2)
			program_vars::raise_error("The sink's only accessible members are the 'append' and 'raw' flags, at indices 1 and 2 respectively.");
		if (always1or2 == 1) return append;
		else return raw;
	}
	string to_string()
	{
		string representation = "Cannot print a data sink.";
		return representation;
	}
	~DataSink()
	{
		if (elem != nullptr) elem->close();
	}
	
};

#endif