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

	const char* GetMemFunc() const { return m_MemberFunc; }
	void* GetTarget() const        { return m_Target; }
	FuncPtr GetFunctor() const     { return m_Func; }

protected:

	FuncBase()
	{
		ZERO_MEM(m_MemberFunc, PTR_SIZE);
		m_Func = nullptr;
		m_Target = nullptr;
	}

	FuncBase(const void* c, FuncPtr ftor, const void* memberFtor, size_t sz, bool targetAsParam = false)
	{
		ZERO_MEM(m_MemberFunc, PTR_SIZE);

		if (memberFtor)
		{
			char* mf = (char*)memberFtor;
			for (size_t i = 0; i < PTR_SIZE; ++i)
			{
				m_MemberFunc[i] = mf[i];
			}
		}
		else
		{
			m_Func = ftor;
		}
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

	void Call() const
	{
		assert(m_Thunk);
		m_Thunk(*this, GetTarget());
	}

	void operator()() const
	{
		assert(m_Thunk);
		Call();
	}

	template<class Target>
	void Call(Target* t)
	{
		assert(m_Thunk);
		assert(GetTarget() == nullptr);
		m_Thunk(*this, t);
	}

	template<class Target>
	void operator()(Target* t)
	{
		assert(m_Thunk);
		assert(GetTarget() == nullptr);
		Call(t);
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
	static void thunk_functor(const FuncBase& ftor, void* target)
	{
		(Func(ftor.GetFunctor()))();
	}

	template<class Target, class MemFunc>
	static void thunk_memberFunctor(const FuncBase& ftor, void* target)
	{
		Target* t = (Target*)target;
		MemFunc& mf = (*(MemFunc*)(void*)(ftor.GetMemFunc()));
		(t->*mf)();
	}

	template<class Target, class MemFunc>
	static void thunk_targetAsParam(const FuncBase& ftor, void* target)
	{
		Target* t = (Target*)target;
		MemFunc& mf = (*(MemFunc*)(void*)(ftor.GetMemFunc()));
		(*mf)(target);
	}


	// Thunk
	typedef void (*Thunk)(const FuncBase& ftor, void* target);

	Func0(Thunk thunk, const void* c, FuncPtr ftor, const void* mf, size_t sz, bool targetAsParam = false) :
		FuncBase(c, ftor, mf, sz, targetAsParam),
		m_Thunk(thunk)
	{
	}

protected:
	volatile Thunk m_Thunk;
};

//
// No args with return value
//
template<class RT>
class FuncRT0 : public FuncBase
{
public:

protected:

private:

};


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