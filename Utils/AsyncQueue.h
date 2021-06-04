#ifndef ASYNCQUEUE_H
#define ASYNCQUEUE_H

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>

template<typename T>
class AsyncQueue
{
public:
	AsyncQueue() = default;
	AsyncQueue(const AsyncQueue<T>&) = delete;
	virtual ~AsyncQueue() { clear(); }

public:
	// Returns and maintains item at front of Queue
	T& front()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.front();
	}

	// Returns and maintains item at back of Queue
	T& back()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.back();
	}

	// Removes and returns item from front of Queue
	T pop_front()
	{
		std::scoped_lock lock(muxQueue);
		auto t = std::move(deqQueue.front());
		deqQueue.pop_front();
		return t;
	}

	// Removes and returns item from back of Queue
	T pop_back()
	{
		std::scoped_lock lock(muxQueue);
		auto t = std::move(deqQueue.back());
		deqQueue.pop_back();
		return t;
	}

	// Adds an item to back of Queue
	void push_back(const T& item)
	{
		std::scoped_lock lock(muxQueue);
		deqQueue.emplace_back(std::move(item));

		std::unique_lock<std::mutex> ul(muxBlocking);
		cvBlocking.notify_one();
	}

	// Adds an item to front of Queue
	void push_front(const T& item)
	{
		std::scoped_lock lock(muxQueue);
		deqQueue.emplace_front(std::move(item));

		std::unique_lock<std::mutex> ul(muxBlocking);
		cvBlocking.notify_one();
	}

	// Returns true if Queue has no items
	bool empty()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.empty();
	}

	// Returns number of items in Queue
	size_t count()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.size();
	}

	// Clears Queue
	void clear()
	{
		std::scoped_lock lock(muxQueue);
		deqQueue.clear();
	}

	bool wait()
	{
		while (empty())
		{
			std::unique_lock<std::mutex> ul(muxBlocking);
			cvBlocking.wait(ul);
			if (notified) {
				notified = false;
				return true;
			}
		}
		return false;
	}

	void notify()
	{
		std::unique_lock<std::mutex> ul(muxBlocking);
		notified = true;
		cvBlocking.notify_one();
	}

protected:
	std::mutex muxQueue;
	std::deque<T> deqQueue;
	std::condition_variable cvBlocking;
	std::mutex muxBlocking;

	bool notified = false;
};

#endif
