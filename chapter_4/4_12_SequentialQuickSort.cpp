#include <list>
#include <algorithm>

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
	if (input.empty())
	{
		return input;
	}
	std::list<T> result;
	// take the first element as the pivot by slicing it off the front of the list using splice()
	result.splice(result.begin(), input, input.begin());
	const T& pivot{ *result.begin() };

	// std::partition() rearranges the list in place and returns an iterator marking the 
	// first element that’s not less than the pivot value.
	auto divide_point{ std::partition(input.begin(), input.end(), [&](const T& t) { return t < pivot; }) };

	std::list<T> lower_part;
	// move the values from input up to the divide_point into a new list
	/*
	 Since lower_part is initially empty, both lower_part.begin() and lower_part.end() refer to the same position
	 — the beginning and end of an empty list. So at this exact moment, yes, lower_part.begin() == lower_part.end().
	 end() is used for semantic reasons
	*/
	lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

	auto new_lower{ sequential_quick_sort(std::move(lower_part)) };
	auto new_higher{ sequential_quick_sort(std::move(input)) };

	result.splice(result.end(), new_higher); // add after the pivot
	result.splice(result.begin(), new_lower); // add before the pivot
	return result;
}