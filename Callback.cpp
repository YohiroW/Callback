#include <cassert>
#include <thread>

// For test
#include <iostream>

#define PTR_SIZE  sizeof(void*)
#define MEMFUNC_SIZE  PTR_SIZE
#define ZERO_MEM(_ptr_, _size_)  memset(_ptr_, 0, _size_)

class DummyVirtualFuncBase
{
	virtual ~DummyVirtualFuncBase() {}
	virtual void VFunc() = 0;
};

class FuncBase
{
public:
	// Move ctor to protected, for FuncBase can not be instantiated

	typedef void (*FuncPtr)();	
	typedef void (DummyVirtualFuncBase::*MemberFunc)();
	
	void SetTarget(void* target) { m_Target = target; }
	FuncPtr GetFunctor() { return m_Func; }

protected:

	FuncBase()
	{
		ZERO_MEM(m_MemberFunc, PTR_SIZE);
		m_Func = nullptr;
		m_Target = nullptr;
	}

	FuncBase(const void* c, FuncPtr ftor, const void* memberFtor, size_t sz, bool targetAsParam = false)
	{
		assert(ftor != nullptr);

		char* mf = (char*)memberFtor;

		if (mf)
		{
			for (size_t i = 0; i < PTR_SIZE; ++i)
			{
				m_MemberFunc[i] = mf[i];
			}
		}
		else
		{
			//m_MemberFunc = nullptr;
		}
		m_Func = ftor;
	}
	
    //
	union
	{
		FuncPtr m_Func;
		char m_MemberFunc[PTR_SIZE];
	};

	void*  m_Target;
};


//
// No args and no return value
//
class Func0 : public FuncBase
{
public:
	// Ctors in protected

	void Call()
	{
		assert(m_Func != nullptr);
		m_Thunk(*this, GetFunctor());
	}

	void operator()()
	{
		Call();
	}

	template<class Target>
	void Call(Target* t)
	{
		assert(t != nullptr);
		m_Thunk(*this, t);
	}

	template<class Target>
	void operator()(Target* t)
	{
		Call(t);
	}

	template<class Target, class MemFunc>
	void Call(Target* t, const MemFunc& memFtor)
	{
		assert(t != nullptr);
		(t->*memFtor)();
	}
	
	//
	template<class Target, class MemFunc>
	static Func0 MakeFuncTargetAsParam(Target* t, const MemFunc& memFtor)
	{
		return Func0(thunk_targetAsParam<Target, MemFunc>, nullptr, (FuncPtr)ftor, nullptr, sizeof(MemFunc), true);
	}

	template<class Target, class MemFunc>
	static Func0 MakeFunc(Target* t, const MemFunc& memFtor)
	{
		return Func0(thunk_memberFunctor<Target, MemFunc>, t, nullptr, &memFtor, sizeof(MemFunc));
	}

	template<class Func>
	static Func0 MakeFunc(Func ftor)
	{
		return Func0(thunk_functor<Func>, nullptr, (FuncPtr)ftor, nullptr, 0);
	}

protected:
	//
	// Internal static functions
	//
	template<class Func>
	static void thunk_functor(Func func)
	{
		(Func(*func))();
	}

	template<class Target, class MemFunc>
	static void thunk_memberFunctor(const MemFunc& mftor, const void* target)
	{
		Target* t = (Target*)target;
		MemFunc& mf = (*(MemFunc*))(void*)(mftor);
		(t->*mf)();
	}

	template<class Target, class MemFunc>
	static void thunk_targetAsParam(const MemFunc& mftor, const void* target)
	{

	}

	typedef void (*Thunk)(const FuncBase&, const void*);

	Func0(Thunk thunk, const void* c, FuncPtr ftor, const void* mf, size_t sz, bool targetAsParam = false) :
		FuncBase(c, ftor, mf, sz, targetAsParam),
		m_Thunk(thunk)
	{
	}

protected:
	volatile Thunk m_Thunk;
};


//
// Simple test case
//
class TestCase
{
public:
	void TestFunc0() 
	{
		//Func0::MakeFunc(this, &TestCase::LogFunc0).Call(); 
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

	//TestCase test;
	//test.TestFunc0();

	return 0;
}