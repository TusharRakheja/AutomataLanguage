#include "../Header Files/AbstractSet.h"
#include "../Header Files/Int.h"
#include "../Header Files/Tuple.h"

namespace AbstractSet_test
{
	void test_independent(AbstractSet * z_plus, AbstractSet * z_minus)
	{
		cout << "\n-------------------------Testing Z+ and Z- independently--------------------\n\n";

		Int x(1), y(-1), z(0);
		cout << "Membership criteria for Z+ : " << z_plus->criteria << endl;
		cout << x.to_string() << " in Z+ : " << (z_plus->has(x) ? "True" : "False") << endl;		// Should be True.
		cout << y.to_string() << " in Z+ : " << (z_plus->has(y) ? "True" : "False") << endl;		// Should be False.
		cout << z.to_string() << " in Z+ : " << (z_plus->has(z) ? "True" : "False") << endl << endl;    // Should be True.

		cout << "Membership criteria for Z- : " << z_minus->criteria << endl;
		cout << x.to_string() << " in Z- : " << (z_minus->has(x) ? "True" : "False") << endl;		// Should be False.
		cout << y.to_string() << " in Z- : " << (z_minus->has(y) ? "True" : "False") << endl;		// Should be True.
		cout << z.to_string() << " in Z- : " << (z_minus->has(z) ? "True" : "False") << endl << endl;   // Should be True.
	}
	
	void test_union(AbstractSet * z_plus, AbstractSet * z_minus)
	{
		cout << "\n------------------------------- Testing Z+ U Z- ----------------------------\n\n";
		AbstractSet * z = z_plus->_union(*z_minus);
		Int x(1), y(-1), zz(0);
		cout << "Membership criteria for Z = Z+ U Z- : " << z->criteria << endl;
		cout << x.to_string() << " in Z : " << (z->has(x) ? "True" : "False") << endl;		// Should be True.
		cout << y.to_string() << " in Z : " << (z->has(y) ? "True" : "False") << endl;		// Should be True.
		cout << zz.to_string() << " in Z : " << (z->has(zz)? "True" : "False") << endl << endl;  // Should be True.
		delete z;
	}

	void test_intersection(AbstractSet * z_plus, AbstractSet * z_minus)
	{
		cout << "\n------------------------------- Testing Z+ & Z- ----------------------------\n\n";
		AbstractSet * z = z_plus->intersection(*z_minus);
		Int x(1), y(-1), zz(0);
		cout << "Membership criteria for Z = Z+ & Z- : " << z->criteria << endl;
		cout << x.to_string() << " in Z : " << (z->has(x) ? "True" : "False") << endl;		// Should be False.
		cout << y.to_string() << " in Z : " << (z->has(y) ? "True" : "False") << endl;		// Should be False.
		cout << zz.to_string() << " in Z : " << (z->has(zz)? "True" : "False") << endl << endl; // Should be True.
		delete z;
	}

	void test_cartesian(AbstractSet * z_plus, AbstractSet * z_minus)
	{
		cout << "\n------------------------------- Testing Z+ x Z- ----------------------------\n\n";
		AbstractSet * z = z_plus->cartesian_product(*z_minus);

		Tuple x1(new vector<Elem *>{ new Int(1) ,  new Int(-1) }, DIRECT_ASSIGN);
		Tuple y1(new vector<Elem *>{ new Int(-1), new Int(1) }, DIRECT_ASSIGN);
		Tuple z1(new vector<Elem *>{ new Int(1), new Int(0) }, DIRECT_ASSIGN);
		Tuple x2(new vector<Elem *>{ new Int(0), new Int(1) }, DIRECT_ASSIGN);
		Tuple y2(new vector<Elem *>{ new Int(0), new Int(0) }, DIRECT_ASSIGN);
		Tuple z2(new vector<Elem *>{ new Int(1), new Int(1) }, DIRECT_ASSIGN);
		cout << "Membership criteria for Z = Z+ x Z- : " << z->criteria << endl;
		cout << x1.to_string() << " in Z : " << (z->has(x1) ? "True" : "False") << endl;		// Should be True.
		cout << y1.to_string() << " in Z : " << (z->has(y1) ? "True" : "False") << endl;		// Should be False.
		cout << z1.to_string() << " in Z : " << (z->has(z1) ? "True" : "False") << endl;		// Should be True.
		cout << x2.to_string() << " in Z : " << (z->has(x2) ? "True" : "False") << endl;		// Should be False.
		cout << y2.to_string() << " in Z : " << (z->has(y2) ? "True" : "False") << endl;		// Should be True.
		cout << z2.to_string() << " in Z : " << (z->has(z2) ? "True" : "False") << endl << endl;        // Should be False.
		delete z;
	}

	void test_exclusion(AbstractSet * z_plus, AbstractSet * z_minus)
	{
		cout << "\n------------------------------- Testing Z+ \\ Z- ----------------------------\n\n";
		AbstractSet * z = z_plus->exclusion(*z_minus);
		Int x(1), y(-1), zz(0);
		cout << "Membership criteria for Z = Z+ \\ Z- : " << z->criteria << endl;
		cout << x.to_string() << " in Z : " << (z->has(x) ? "True" : "False") << endl;		// Should be True.
		cout << y.to_string() << " in Z : " << (z->has(y) ? "True" : "False") << endl;		// Should be False.
		cout << zz.to_string() << " in Z: " << (z->has(zz) ? "True" : "False") << endl << endl; // Should be False.
		delete z;
	}

	void test_superset(AbstractSet * z_plus, AbstractSet * z_minus)
	{
		cout << "\n------------------------ Testing {1, 2, 3} c Z+ and Z------------------------\n\n";

		Set A((string)"{1, 2, 3}");
		cout << A.to_string() << " c Z+ : " << (z_plus->superset_of(A) ? "True" : "False") << endl;
		cout << A.to_string() << " c Z- : " << (z_minus->superset_of(A) ? "True" : "False") << endl << endl;
	}

	void test_all()
	{
		AbstractSet * positive_ints = new AbstractSet((string)"{ elem | elem >= 0 }");
		AbstractSet * negative_ints = new AbstractSet((string)"{ elem | elem <= 0 }");
		test_independent(positive_ints, negative_ints);
		test_cartesian(positive_ints, negative_ints);
		test_exclusion(positive_ints, negative_ints);
		test_intersection(positive_ints, negative_ints);
		test_union(positive_ints, negative_ints);
		test_superset(positive_ints, negative_ints);
		delete positive_ints, negative_ints;
	}
}