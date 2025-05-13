#include <iostream>
#include <mutex>

/*
This hierarchical_mutex class does not hold multiple locks itself,
but it is designed to enforce a locking hierarchy to allow a thread to hold multiple locks safely,
preventing deadlocks caused by out-of-order locking.
*/
class hierarchical_mutex
{
	std::mutex internal_mutex;
	const unsigned long hierarchy_value;
	unsigned long previous_hierarchy_value;
	static thread_local unsigned long this_thread_hierarchy_value;

	void check_for_hierarchy_violation()
	{
		if (this_thread_hierarchy_value <= hierarchy_value)
		{
			throw std::logic_error("mutex hierarchy violated");
		}
	}
	
	void update_hierarchy_value()
	{
		previous_hierarchy_value = this_thread_hierarchy_value;
		this_thread_hierarchy_value = hierarchy_value;
	}
public:
	explicit hierarchical_mutex(unsigned long value)
		: hierarchy_value{ value }, previous_hierarchy_value(0)
	{ }

	void lock()
	{
		check_for_hierarchy_violation();
		internal_mutex.lock();
		update_hierarchy_value();
	}
	
	void unlock()
	{
		this_thread_hierarchy_value = previous_hierarchy_value;
		internal_mutex.unlock();
	}

	bool try_lock()
	{
		check_for_hierarchy_violation();
		if (!internal_mutex.try_lock()) return false;
		update_hierarchy_value();
		return true;
	}
};
thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);

hierarchical_mutex high_level_mutex(10000); // Most general / top-level
hierarchical_mutex mid_level_mutex(6000);
hierarchical_mutex low_level_mutex(5000);   // Most specific / bottom-level

void correct_order() {
	try {
		std::lock_guard<hierarchical_mutex> high_lock(high_level_mutex); // 10000
		std::cout << "High level mutex locked\n";

		std::lock_guard<hierarchical_mutex> mid_lock(mid_level_mutex);   // 6000
		std::cout << "Mid level mutex locked\n";

		std::lock_guard<hierarchical_mutex> low_lock(low_level_mutex);   // 5000
		std::cout << "Low level mutex locked\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in correct_order(): " << e.what() << '\n';
	}
}

void wrong_order() {
	try {
		std::lock_guard<hierarchical_mutex> low_lock(low_level_mutex);   // 5000
		std::cout << "Low level mutex locked\n";

		std::lock_guard<hierarchical_mutex> high_lock(high_level_mutex); // 10000
		std::cout << "This will not be reached\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in wrong_order(): " << e.what() << '\n';
	}
}
