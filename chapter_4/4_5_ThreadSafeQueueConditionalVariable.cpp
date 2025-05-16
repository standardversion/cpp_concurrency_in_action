#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mut;
	std::queue<T> data_queue;
	std::condition_variable data_cond;
public:
	threadsafe_queue()
	{ }

	threadsafe_queue(const threadsafe_queue& other)
	{
		std::lock_guard<std::mutex> lk{ other.mut };
		data_queue = other.data_queue;
	}

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lk{ mut };
		data_queue.push(new_value);
		data_cond.notify_one();
	}

	//will wait until there’s a value to retrieve
	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lk{ mut };
		data_cond.wait(lk, [this] { return !data_queue.empty(); });
		value = data_queue.front();
		data_queue.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk{ mut };
		data_cond.wait(lk, [this] { return !data_queue.empty(); });
		std::shared_ptr<T> res{ std::make_shared<T>(data_queue.front()) };
		data_queue.pop();
		return res;
	}

	/*
	tries to pop the value
	from the queue but always returns immediately (with an indication of failure) even if
	there wasn’t a value to retrieve
	*/
	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lk{ mut };
		if (data_queue.empty()) return false;
		value = data_queue.front();
		data_queue.pop();
		return true;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk{ mut };
		if (data_queue.empty()) return std::shared_ptr<T>{};
		std::shared_ptr<T> res{ std::make_shared<T>(data_queue.front()) };
		data_queue.pop();
		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk{ mut };
		return data_queue.empty();
	}
};
/*
Even though empty() is a const member function, and the other parameter to the
copy constructor is a const reference, other threads may have non-const references
to the object, and be calling mutating member functions, so we still need to lock the
mutex. Since locking a mutex is a mutating operation, the mutex object must be
marked mutable so it can be locked in empty() and in the copy constructor.
*/