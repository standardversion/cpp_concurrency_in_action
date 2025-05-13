#include <mutex>

class some_big_object;

void swap(some_big_object& lhs, some_big_object& rhs);

class X
{
private:
	some_big_object some_detail;
	std::mutex m;
public:
	X(const some_big_object& sd)
		: some_detail{sd}
	{ }

	friend void swap(X& lhs, X& rhs)
	{
		if (&lhs == &rhs) return;
		std::lock(lhs.m, rhs.m);
		/*
		std::adopt_lock parameter is supplied in
		addition to the mutex to indicate to the std::lock_guard objects that the mutexes
		are already locked, and they should just adopt the ownership of the existing lock on
		the mutex rather than attempt to lock the mutex in the constructor
		*/
		std::lock_guard<std::mutex> lock_a{ lhs.m, std::adopt_lock };
		std::lock_guard<std::mutex> lock_b{ rhs.m, std::adopt_lock };
		swap(lhs.some_detail, rhs.some_detail);
	}
};