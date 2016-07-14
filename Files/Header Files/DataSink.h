#ifndef DATASINK_H
#define DATASINK_H

#include "Logical.h"
#include <iostream>
#include <fstream>

using std::ofstream;

#define datasink static_pointer_cast<DataSink>

class DataSink : public Elem
{
public:
	shared_ptr<ofstream> elem;
	shared_ptr<Logical> raw;

	DataSink(const char * filepath, shared_ptr<Logical> raw) : Elem(DATASINK)
	{
		elem = shared_ptr<ofstream>{new ofstream(filepath)};
		if (!*elem) program_vars::raise_error("Failed to open file.");
		this->raw = raw;
	}

	DataSink() : Elem(DATASINK) { }

	shared_ptr<Elem> deep_copy()
	{
		shared_ptr<DataSink> copy = shared_ptr<DataSink>{new DataSink()};
		copy->elem = this->elem;
		copy->raw = logical(this->raw->deep_copy());
		return copy;
	}
	shared_ptr<Logical> operator[](int always1)
	{
		if (always1 != 1) program_vars::raise_error("The sink's only accessible member is the 'raw' flag, at index 1.");
		return raw;
	}
	const shared_ptr<Logical> operator[](int always1) const
	{
		if (always1 != 1) program_vars::raise_error("The sink's only accessible member is the 'raw' flag, at index 1.");
		return raw;
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