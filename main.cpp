#include "Test.hpp"
#include <iostream>
using std::endl;
using std::wcout;

int main()
{
	std::locale::global(std::locale(""));
	Test1();
	Test2();
	Test3();
	Test4();
	Test5();
	Test6();
    Test7();
}
