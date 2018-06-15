#include <cassert>
#include <thread>

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
	typedef void (*MemberFunc)();
	
	void SetTarget(void* target) { m_Target = target; }
	void SetFunctor(FuncPtr ftor) { m_Func = ftor; }


	void Call()
	{
		assert(m_Func != nullptr, "No functor binded");
		(*m_Func)();
	}

	template<class Target>
	void Call(Target* t)
	{
		assert(t != nullptr, "Invoker is null pointer");
		t->FuncPtr();
	}

protected:

	FuncBase()
	{
		ZERO_MEM(m_MemberFunc, PTR_SIZE);
		m_Func = nullptr;
	}

	FuncBase(FuncPtr ftor, const char* memberFtor)
	{
		assert(ftor != nullptr, "Invaild functor binded");
		if (memberFtor)
		{
			for (size_t i = 0; i < PTR_SIZE; ++i)
			{
				m_MemberFunc[i] = memberFtor[i];
			}
		}
		m_Func = ftor;
	}
	
    //
	union
	{
		FuncPtr m_Func;
		void*  m_Target;
	};

	char m_MemberFunc[PTR_SIZE];
};
