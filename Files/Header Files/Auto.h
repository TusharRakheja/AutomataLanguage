#ifndef AUTO_H
#define AUTO_H

#include "Map.h"

#define mytuple std::tuple<shared_ptr<Set>,shared_ptr<Set>,shared_ptr<Elem>,shared_ptr<Map>> 

class Auto : public Elem			 // DFA.
{
private: 
	shared_ptr<mytuple> make_super_automata(shared_ptr<Auto>); // Makes the states = Q1 x Q2, and M((q1, q2), c) = (M1(q1, c), M2(q2, c)).
public:
	shared_ptr<Set> sigma;			 // The alphabet of the automaton. (Homotype == Character).
	shared_ptr<Set> states;			 // The set of states (labels really). (Homoset).
	shared_ptr<Map> delta;			 // The transition function from sigma to states.
	shared_ptr<Elem> start;			 // The label of (or tuples of labels for) the starting state.
	shared_ptr<Set> accepting;		 // The set of accepting states (labels thereof).
										    
	Auto(shared_ptr<Set>, shared_ptr<Set>, shared_ptr<Elem>, shared_ptr<Map>, shared_ptr<Set>);		// Constructor (Copy elements).
	Auto(shared_ptr<Set>, shared_ptr<Set>, shared_ptr<Elem>, shared_ptr<Map>, shared_ptr<Set>, int);	// Constructor (Direct assign).

	shared_ptr<Logical> operator[](String &);		// Returns True if the automaton is in an accepting state after running the query string.
	const shared_ptr<Logical> operator[](String &) const;	// Returns True if the automaton is in an accepting state after running the query string.
	shared_ptr<Auto> accepts_union(shared_ptr<Auto>);	// Returns an automaton that accepts the union of the languages of this and other automaton.
	shared_ptr<Auto> accepts_intersection(shared_ptr<Auto>);// Returns an automaton that accepts the intersection of the languages of this and other auto.
	shared_ptr<Auto> accepts_exclusively(shared_ptr<Auto>);	// Returns an automaton that accepts all string in L(this) that are not in L(other).
	shared_ptr<Elem> deep_copy();			// Returns a deep_copy of this automaton.
	bool operator==(Elem &);		// The overloaded == operator for Automata.
	bool operator<(Elem &e) {return false;} // This op is basically useless for Automata.
	string to_string();			// Returns a string representation of the automaton.
};

#endif