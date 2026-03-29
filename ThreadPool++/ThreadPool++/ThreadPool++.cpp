// ThreadPool++.cpp : Defines the entry point for the application.
//

#include "ThreadPool++.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;

	TP_CPU_CLASS * tp = new TP_CPU_CLASS();

	cout << tp->get_implementation_() << endl;

	delete tp;

	return 0;
}
