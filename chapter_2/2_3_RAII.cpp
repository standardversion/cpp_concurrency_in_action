#include <thread>
#include <iostream>

void do_something([[maybe_unused]] int& i)
{

}

void do_something_in_current_thread()
{

}

struct func
{
	int& i;
	func(int& i_)
		: i{ i_ }
	{ }
	void operator()()
	{
		for (unsigned j{ 0 }; j < 1000000; ++j)
		{
			do_something(i);
		}
	}
};

class thread_guard
{
	std::thread& t;
public:
	explicit thread_guard(std::thread& t_)
		: t{ t_ }
	{ }
	~thread_guard()
	{
		if (t.joinable())
		{
			t.join();
		}
	}
	/*The copy constructor and copy - assignment operator are marked = delete to
	ensure that they’re not automatically provided by the compiler.Copying or assigning
	such an object would be dangerous, because it might then outlive the scope of the
	thread it was joining.By declaring them as deleted, any attempt to copy a thread_
	guard object will generate a compilation error.*/

	thread_guard(thread_guard const&) = delete;
	thread_guard& operator=(thread_guard const&) = delete;
};

void f()
{
	int some_local_state{ 0 };
	func my_func(some_local_state);
	std::thread t(my_func);
	thread_guard g(t);

	do_something_in_current_thread();
}


int main()
{
	f();
	// after f terminates thread_guard will be destroyed first and the thread is joined
	// with in the destructor. This even happens if the function exits because do_something_in_current_thread
	// throws and exception.
	return 0;
}