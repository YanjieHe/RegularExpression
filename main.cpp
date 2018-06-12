#include "Test.hpp"
#include <iostream>
using std::cout;
using std::endl;

int main()
{
	std::locale::global(std::locale(""));
	wcout << "fuck the world!" << endl;
	wcout << "test1" << endl;
	Test1();
	wcout << "test2" << endl;
	Test2();
	wcout << "test3" << endl;
	Test3();
	wcout << "test4" << endl;
	Test4();
}
