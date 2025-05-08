#include <thread>
#include <vector>
#include <numeric>
#include <algorithm>

// could use something like this too instead of the struct!
template<typename Iterator, typename T>
void accumulate_block_fn(Iterator first, Iterator last, T& result)
{
	result = std::accumulate(first, last, result);
}


template<typename Iterator, typename T>
struct accumulate_block
{
	void operator() (Iterator first, Iterator last, T& result)
	{
		result = std::accumulate(first, last, result);
	}
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	const unsigned long length{ std::distance(first, last) };

	// return initial value if length is 0
	if (!length)
	{
		return init;
	}
	
	// We don't want a thread to be created for fewer than this many elements
	const unsigned long min_per_thread{ 25 };
	// calculate how many threads to create
	const unsigned long max_threads{ (length + min_per_thread - 1) / min_per_thread };

	const unsigned long hardware_threads{ std::thread::hardware_concurrency() };

	// 2 is a generic number in case we don't support multithreading
	const unsigned long num_threads{
		std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads)
	};

	// number of entries for each thread
	// the last block (handled by this thread) takes any leftover elements

	const unsigned long block_size{ length / num_threads };

	// vector to capture result of each thread
	std::vector<T> results(num_threads);
	// vector to capture all the threads
	std::vector<std::thread> threads(num_threads - 1);

	Iterator block_start{ first };
	for (unsigned long i{ 0 }; i < (num_threads - 1); ++i)
	{
		Iterator block_end{ block_start };
		// advance the iterator to the end of the block
		std::advance(block_end, block_size);
		threads[i] = std::thread(
			accumulate_block<Iterator, T>(),
			block_start, block_end, std::ref(results[i])
		);
		// set the next block start to the end of the current
		block_start = block_end;
	}
	// run any left over entries
	accumulate_block<Iterator, T>() (
		block_start, last, results[num_threads - 1]
	);
	// wait for all threads to complete
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	// accumulate all the results
	return std::accumulate(results.begin(), results.end(), init);
}