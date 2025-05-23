#pragma once
#include <exception>
#include <memory>
#include <stack>
#include <mutex>

struct empty_stack : std::exception
{
	const char* what() const noexcept;
};

template<typename T>
class threadsafe_stack
{
public:
	threadsafe_stack() {};
	threadsafe_stack(const threadsafe_stack& other)
	{
		std::lock_guard<std::mutex> lock(other.m);
		data = other.data;
	}
	threadsafe_stack& operator=(const threadsafe_stack&) = delete;

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lock(m);
		data.push(new_value);
	}
	std::shared_ptr<T> pop()
	{
		std::lock_guard<std::mutex> lock(m);
		if (data.empty()) throw empty_stack();
		const std::shared_ptr<T> item{ std::make_shared(data.top()) };
		data.pop();
		return item;
	}
	void pop(T& value)
	{
		std::lock_guard<std::mutex> lock(m);
		if (data.empty()) throw empty_stack();
		value = data.top();
		data.pop();
	}
	bool empty() const
	{
		std::lock_guard<std::mutex> lock(m);
		return data.empty();
	}
private:
	std::stack<T> data;
	mutable std::mutex m;
};