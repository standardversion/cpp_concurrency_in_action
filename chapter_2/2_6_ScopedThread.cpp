#include <thread>
#include <stdexcept>

class scoped_thread
{
	std::thread t;
public:
	explicit scoped_thread(std::thread t_)
		: t(std::move(t_))
	{
		if (!t.joinable())
		{
			throw std::logic_error("No thread");
		}
	}
	~scoped_thread()
	{
		t.join(); // no need to check if thread is joinable
	}
	scoped_thread(const scoped_thread&) = delete;
	scoped_thread& operator=(const scoped_thread&) = delete;
};