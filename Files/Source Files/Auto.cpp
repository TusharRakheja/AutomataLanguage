#define _CRT_SECURE_NO_WARNINGS

#include "../Header Files/Auto.h"
#include <cstdlib>
#include <iostream>
#include "../Header Files/ProgramVars.h"

using std::cout;
using std::endl;

/* Implementations for the methods in the class Auto. */

Auto::Auto(shared_ptr<Set> states, shared_ptr<Set> sigma, shared_ptr<Elem>start, shared_ptr<Map>delta, shared_ptr<Set> accepting) : Elem(AUTO)			// Constructor (Copy elements).
{
	if (!states->homoset() || !sigma->homoset())				// The set of states and the alphabet must be homogeneous.
		program_vars::raise_error("The set of states or the alphabet is not homogeneous.");
	if (sigma->homotype() != CHAR)						// forall x in sigma x.type = CHAR  
		program_vars::raise_error("The alphabet is not a homoset set of characters.");
	if (!accepting->subset_of(*states))					// The set of accepting states must be a subset of states.
		program_vars::raise_error("The set of accepting states is not a subset of the set of states.");

	if (!delta->domain()->subset_of(*states->cartesian_product(*sigma)) ||	// If the domain of the map is not a subset of states X sigma...
		!delta->codomain()->subset_of(*states))				// ... or if the codomain is not a subset of states ...
		program_vars::raise_error("The transition function has improper domain or codomain.");

	this->states    = std::static_pointer_cast<Set>(states->deep_copy());
	this->sigma     = std::static_pointer_cast<Set>(sigma->deep_copy());
	this->start     = start->deep_copy();
	this->delta     = std::static_pointer_cast<Map>(delta->deep_copy());
	this->accepting = std::static_pointer_cast<Set>(accepting->deep_copy());
}

Auto::Auto(shared_ptr<Set> states, shared_ptr<Set> sigma, shared_ptr<Elem>start, shared_ptr<Map>delta, shared_ptr<Set> accepting, int) : Elem(AUTO)	     // Constructor (Direct assign).
{
	if (!states->homoset() || !sigma->homoset())				// The set of states and the alphabet must be homogeneous.
		program_vars::raise_error("The set of states or the alphabet is not homogeneous.");
	if (sigma->homotype() != CHAR)						// forall x in sigma x.type = CHAR  
		program_vars::raise_error("The alphabet is not a homoset set of characters.");
	if (!accepting->subset_of(*states))					// The set of accepting states must be a subset of states.
		program_vars::raise_error("The set of accepting states is not a subset of the set of states.");

	if (!delta->domain()->subset_of(*states->cartesian_product(*sigma)) ||	// If the domain of the map is not a subset of states X sigma...
		!delta->codomain()->subset_of(*states))				// ... or if the codomain is not a subset of states ...
		program_vars::raise_error("The transition function has improper domain or codomain.");

	this->states = states;
	this->sigma = sigma;
	this->start = start;
	this->delta = delta;
	this->accepting = accepting;
}

shared_ptr<Logical> Auto::operator[](String &query)
{
	shared_ptr<Elem> current_state = start;			// Initially, the current_state is the starting state 'start'.
	for (auto &character : query.elem)		// For every character in the query string ...
	{	
		Tuple t ( 
			new vector< shared_ptr< Elem > > {	        // Make a tuple t ...
				current_state->deep_copy(), 
				shared_ptr<Char>(new Char(character))   // ... that contains pointers to the deep_copies of the current_state...
			}, 
			DIRECT_ASSIGN
		);							// ... and a Char object constructed with the current character.
		if (((*delta)[t]) == nullptr) 				// If there does not exist any mapping from the current (state, char) pair ...
			return shared_ptr<Logical>{new Logical(false)};	// ... return false because we're going to a dump state.

		current_state = (*delta)[t];		    // Use this tuple as input to the transition map 'delta', and update the current_state.
	}						    // Finally we'll have current_state as the final state of the query.
	return shared_ptr<Logical>{			    // If the final state is in the set of accepting states, return True, else return False.
		new Logical(accepting->has(*current_state))
	}; 
}

const shared_ptr<Logical> Auto::operator[](String &query) const
{
	shared_ptr<Elem> current_state = start;			// Initially, the current_state is the starting state 'start'.
	for (auto &character : query.elem)		// For every character in the query string ...
	{
		Tuple t( new vector< shared_ptr< Elem > >{	        // Make a tuple t ...
			current_state,					// ... that is (q_current, c). 
			shared_ptr<Char>(new Char(character))		
			},
			DIRECT_ASSIGN
		);							
		if (((*delta)[t]) == nullptr) 				// If there does not exist any mapping from the current (state, char) pair ...
			return shared_ptr<Logical>{new Logical(false)};	// ... return false because we're going to a dump state.

		current_state = (*delta)[t];		// Use this tuple as input to the transition map 'delta', and update the current_state.
	}						// Finally we'll have current_state as the final state of the query.
	return shared_ptr<Logical>{			// If the final state is in the set of accepting states, return True, else return False.
		new Logical(accepting->has(*current_state))
	};
}

shared_ptr<mytuple> Auto::make_super_automata(shared_ptr<Auto> other)	// Makes the states = Q1 x Q2, and M((q1, q2), c) = (M1(q1, c), M2(q2, c)).
{
	if (!(*other->sigma == *this->sigma)) 				// If the alphabets of the two automata are different ...
		program_vars::raise_error("The alphabets of the two automata must match.");

	// The new set of states is a set of 2-tuples of states from the two automata.
	shared_ptr<Set> new_states = this->states->cartesian_product(*other->states);	

	// The domain of new delta is a set of 2-tuples of new states and input alphabet characters.
	shared_ptr<Set> new_delta_domain = new_states->cartesian_product(*this->sigma);	

	// The codomain of new delta is just the set of new states.
	shared_ptr<Set> new_delta_codomain = new_states;				

	shared_ptr<Tuple> new_start { 
		new Tuple(							// We'll now make the Tuple representing the starting state ... 
		new vector <shared_ptr<Elem>> { this->start, other->start }	// ... of the new automaton, (q_01, q_02).
	)};

	shared_ptr<Map> new_delta {new Map(new_delta_domain, new_delta_codomain)};	// Let's make the map, and then we'll add the mappings.

	for (auto &pre_image : *new_delta_domain->elems)				// Now we'll add mappings M((q1, q2), c) = (M1(q1, c), M2(q2, c)) 
	{
		shared_ptr<Tuple> preimage = std::static_pointer_cast<Tuple>(pre_image);	// *preimage = ((q1, q2), c)
		shared_ptr<Tuple> state_pair = std::static_pointer_cast<Tuple>((*preimage)[0]);	// *state_pair = (q1, q2)
		shared_ptr<Char>  sigma = std::static_pointer_cast<Char>((*preimage)[1]);	// *sigma = c 

		Tuple first_state_char_pair (					// first_state_char_pair = (q1, c)
			new vector < shared_ptr<Elem> > {
				(*state_pair)[0],				// Here we're making putting q1 into the tuple.
				sigma						// And here we're putting c. 
			},							// So now the tuple is (q1, c),
			DIRECT_ASSIGN						// Because we want to use this vector here, and not a copy.
		);

		// Note that this tuple is not dynamic, because it will only be used for lookup in this->delta,
		// and we actually want it to be destroyed.

		shared_ptr<Elem> mapped_first_state_char_pair {			// M1(q1, c)
			(*this->delta)[first_state_char_pair]
		};   

		Tuple second_state_char_pair(					// second_state_char_pair = (q2, c)
			new vector < shared_ptr<Elem> >{
				(*state_pair)[1],				// Here we're making a deep-copy of q2. 
				sigma						// And here one of sigma.
			},							// So now the tuple is (q2, c)
			DIRECT_ASSIGN						// Because we want to use this vector here, and not a copy.
		);

		shared_ptr<Elem> mapped_second_state_char_pair {		// M2(q2, c)
			(*other->delta)[second_state_char_pair] 
		};

		Tuple image(						// Now we're making the tuple (M1(q1, c), M2(q2, c))
			new vector < shared_ptr<Elem> >{		// So we'll make a tuple object initialized with a vector ...
				mapped_first_state_char_pair,		// ... containing M1(q1, c), and ...
				mapped_second_state_char_pair		// ... M2(q2, c).
			},						// Since we only need this tuple for lookup and it will be ...
			DIRECT_ASSIGN					// ... destroyed, we direct assign the vector ...
		);							// ... so that it is destroyed along with the tuple.
		new_delta->add_maping(*preimage, image);		// Now finally, add the mapping.
	}
	return shared_ptr<mytuple>{ new mytuple{ new_states, this->sigma, new_start, new_delta }};
	// Return the components of the super automaton, without the accepting set of states.
}

shared_ptr<Auto> Auto::accepts_union(shared_ptr<Auto> other)	// Returns an automaton that accepts the union of the languages of this and other automaton.
{
	shared_ptr<Set> new_states, new_alphabet;
	shared_ptr<Elem> new_start; shared_ptr<Map> new_delta;

	shared_ptr<mytuple> components = this->make_super_automata(other);		// We make the super automaton's components ...
	std::tie(new_states, new_alphabet, new_start, new_delta) = *components;		// ... put them all in their rightful place ...
	
	shared_ptr<Set> new_accepting { new Set };		// Make an empty set of accepting states for now.
	for (auto &state_pair : *new_states->elems)		// For every state in the set of states of the union(ized) automaton ...
	{
		// ... cast it into (q1, q2) for C++, because it's too dumb to do it itself.
		shared_ptr<Tuple> statepair = std::static_pointer_cast<Tuple>(state_pair); 

		if (this->accepting->has(*(*statepair)[0]) ||   // Now, if the q1 is in the set of accepting states of this automaton, or if ...
		    other->accepting->has(*(*statepair)[1]))	// ... q2 is in the accepting states of the other automaton ...
		{
		    new_accepting->elems->push_back(statepair); // ... then (q1, q2) ought to be in the set of accepting states of the union. 
		}
	}
	// Return the unionized automaton.
	return shared_ptr<Auto>{new Auto(new_states, new_alphabet, new_start, new_delta, new_accepting, DIRECT_ASSIGN)};	
}

shared_ptr<Auto> Auto::accepts_intersection(shared_ptr<Auto> other)	// Returns an automaton accepting the intersection of the languages of this and other automaton.
{
	shared_ptr<Set> new_states, new_alphabet;
	shared_ptr<Elem> new_start; shared_ptr<Map> new_delta;

	shared_ptr<mytuple> components = this->make_super_automata(other);		// We make the super automaton's components ...
	std::tie(new_states, new_alphabet, new_start, new_delta) = *components;		// ... put them all in their rightful place ...

	shared_ptr<Set> new_accepting{ new Set };		// Make an empty set of accepting states for now.
	for (auto &state_pair : *new_states->elems)		// For every state in the set of states of the union(ized) automaton ...
	{
		// ... cast it into (q1, q2) for C++, because it's too dumb to do it itself.
		shared_ptr<Tuple> statepair = std::static_pointer_cast<Tuple>(state_pair);

		if (this->accepting->has(*(*statepair)[0]) &&    // Now, if the q1 is in the set of accepting states of this automaton, and if ...
			other->accepting->has(*(*statepair)[1])) // ... q2 is in the accepting states of the other automaton ...
		{
			new_accepting->elems->push_back(statepair); // ... then (q1, q2) ought to be in the set of accepting states of the union. 
		}
	}
	// Return the intersect automaton.
	return shared_ptr<Auto>{new Auto(new_states, new_alphabet, new_start, new_delta, new_accepting, DIRECT_ASSIGN)};
}

shared_ptr<Auto> Auto::accepts_exclusively(shared_ptr<Auto> other) // Returns an automaton that accepts all string in L(this) that are not in L(other).
{
	shared_ptr<Set> new_states, new_alphabet;
	shared_ptr<Elem> new_start; shared_ptr<Map> new_delta;

	shared_ptr<mytuple> components = this->make_super_automata(other);		// We make the super automaton's components ...
	std::tie(new_states, new_alphabet, new_start, new_delta) = *components;		// ... put them all in their rightful place ...

	shared_ptr<Set> new_accepting{ new Set };		// Make an empty set of accepting states for now.
	for (auto &state_pair : *new_states->elems)		// For every state in the set of states of the union(ized) automaton ...
	{
		// ... cast it into (q1, q2) for C++, because it's too dumb to do it itself.
		shared_ptr<Tuple> statepair = std::static_pointer_cast<Tuple>(state_pair);
		if (this->accepting->has(*(*statepair)[0]) &&   // Now, if the q1 is in the set of accepting states of this automaton, and ...
		   (!other->accepting->has(*(*statepair)[1])))	// ... q2 is NOT in the accepting states of the other automaton ...
		{
		    new_accepting->elems->push_back(statepair); // ... then (q1, q2) ought to be in the set of accepting states of the union. 
		}
	}
	// Return the exclusive automaton.
	return shared_ptr<Auto>{new Auto(new_states, new_alphabet, new_start, new_delta, new_accepting, DIRECT_ASSIGN)};
}

shared_ptr<Elem> Auto::deep_copy()					// Return a deep_copy() of this automaton.
{
	return shared_ptr<Auto>
	{
		new Auto (					// Simply just construct a new automaton ... 
			std::static_pointer_cast<Set>(this->states->deep_copy()), // ... with deep_copies of all its components ...
			std::static_pointer_cast<Set>(this->sigma->deep_copy()),
			this->start->deep_copy(),
			std::static_pointer_cast<Map>(this->delta->deep_copy()),
			std::static_pointer_cast<Set>(this->accepting->deep_copy()),
			DIRECT_ASSIGN						  // ... directly assign to its corresponding fields.
		)
	};
}

bool Auto::operator==(Elem &other)				// The overloaded == operator for Automata.
{
	if (other.type != AUTO)					// If the other element is not an automaton, it  won't be equal to this.
		return false;	
	Auto * that = (Auto *) & other;				// We can introduce a pointer to other, called that, with an Auto interface.
	return							// Now return the result of this expression.
	(
		(this->states == that->states) &&		// Between you and me, this probably will never be used. But well, why not.
		( this->sigma == that->sigma ) &&
		( this->start == that->start ) &&
		( this->delta == that->delta ) &&
		(this->accepting == that->accepting)
	);
}

string Auto::to_string()					// Returns a string representation of this automaton.
{
	return "Cannot print automata.";
}

