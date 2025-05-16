#include <mutex>
#include <queue>

#include <mutex>
#include <queue>
#include <condition_variable>

std::mutex mut;
std::queue<data_chunk> data_queue;
std::condition_variable data_cond;

void data_preparation_thread()
{
	while (more_data_to_prepare())
	{
		const data_chunk data{ prepare_data() };
		std::lock_guard<std::mutex> lk{ mut }; // lock before updating the queue
		data_queue.push(data);
		data_cond.notify_one(); // notify atleast one thread
	}
}

void data_processing_thread()
{
	while (true)
	{
		// unique_lock is needed because std::condition_variable requires it (not std::lock_guard)
		std::unique_lock<std::mutex> lk{ mut };
		// Wait until there is data in the queue.
		// This call does the following:
		// 1. Atomically releases the mutex 'mut' and puts the thread to sleep.
		// 2. The thread stays asleep until either:
		//    a. Another thread calls data_cond.notify_one() or data_cond.notify_all(), AND
		//    b. The predicate (lambda) returns true when the thread wakes up.
		// 3. When the thread wakes up, it reacquires the mutex before returning.
		// 4. If the predicate returns false (e.g., due to spurious wakeup), it goes back to waiting
		data_cond.wait(
			lk, [] { return !data_queue.empty();  }
		);
		data_chunk data{ data_queue.front() };
		data_queue.pop();
		// unlock so if data processing is time consuming other threads are not blocked
		lk.unlock();
		process(data);
		if (is_last_chunk(data)) break;
	}
}

/*
 Why do spurious wakeups happen?
They happen due to the underlying implementation of condition variables in the OS. The system may internally resume a thread from its waiting state for reasons unrelated to the condition being met, including:

Optimization decisions by the OS scheduler.

Resource cleanup.

Signals or interrupts.
*/