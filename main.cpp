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

	void TestFuncRT0()
	{
		FuncRT0<float> functorRT = FuncRT0<float>::MakeFunc(this, &TestCase::LogFuncRT0);
		std::cout<< "Test Member FuncRT0, value: "<< functorRT.Call()<< std::endl;
	}

private:
	void LogFunc0()
	{
		std::cout << "Test Member Func0" << std::endl;
	}

	float LogFuncRT0()
	{
		return 66.6f;
	}
};

void test_func0() { std::cout << "Test Func0" << std::endl; }
int  test_funcRT0() { return 23; }

//Entry
int main()
{
	Func0 ftor0 = Func0::MakeFunc(test_func0);
	ftor0.Call();

	FuncRT0<int> ftorRT = FuncRT0<int>::MakeFunc(test_funcRT0);
	std::cout<< "Test FuncRT0, value:"<< ftorRT.Call()<< std::endl;

	TestCase test;
	test.TestFunc0();
	test.TestFuncRT0();

	return 0;
}