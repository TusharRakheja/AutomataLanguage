#include "../Header Files/AbstractMap.h"
#include <iostream>
using std::cout;
using std::endl;

namespace AbstractMap_test
{
	void test_all()
	{
		AbstractMap * f = new AbstractMap((string)"x --> (x) + 2");
		AbstractMap * g = new AbstractMap((string)"x --> (x) * 3");

		Int x(3); Char a('a'), excl('!');

		cout << "\n-----------------------Testing maps f and g --------------------\n\n";
		cout << "f[3] = " << (*f)[x]->to_string() << endl;				// Should be 5.
		cout << "f['a'] = " << (*f)[a]->to_string() << endl << endl;			// Should be 'c'.

		cout << "g[3] = " << (*g)[x]->to_string() << endl;				// Should be 9.
		cout << "g['!'] = " << (*g)[excl]->to_string() << endl << endl;			// Should be 'c'.

		AbstractMap * fog = f->composed_with(g);

		cout << "fog[3] = " << (*fog)[x]->to_string() << endl;				// Should be 11.
		cout << "fog['!'] = " << (*fog)[excl]->to_string() << endl << endl;		// Should be 'e'.

		delete f, g, fog;
	}
}
