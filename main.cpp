#include <thread>
#include <iostream>

#include "Callback.h"

//
// Simple test case
//
class TestCase
{
public:
	void TestFunc0()
	{
		Func0::MakeFunc(this, &TestCase::LogFunc0).Call();
	}


private:
	void LogFunc0()
	{
		std::cout << "Test Member Func0" << std::endl;
	}
};

void test_func0() { std::cout << "Test Func0" << std::endl; }

//Entry
int main()
{
	Func0 ftor0 = Func0::MakeFunc(test_func0);
	ftor0.Call();

	TestCase test;
	test.TestFunc0();

	return 0;
}