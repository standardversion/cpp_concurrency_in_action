#include <condition_variable>
#include <mutex>
#include <chrono>

std::condition_variable cv;
bool done;
std::mutex m;

bool wait_loop()
{
	const auto timeout{ std::chrono::steady_clock::now() + std::chrono::milliseconds(500) };
	std::unique_lock<std::mutex> lk{ m };
	while (!done)
	{
		if (cv.wait_until(lk, timeout) == std::cv_status::timeout) break;
	}
	return;
}

/*
This is the recommended way to wait for condition variables with a time limit, if you’re
not passing a predicate to the wait. This way, the overall length of the loop is bounded.
You need to loop when using condition variables if you
don’t pass in the predicate, in order to handle spurious wakeups. If you use
wait_for() in a loop, you might end up waiting almost the full length of time before
a spurious wake, and the next time through the wait time starts
*/