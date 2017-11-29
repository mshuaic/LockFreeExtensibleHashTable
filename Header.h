#pragma once
#include <unordered_set>
#include <mutex>

template <typename T>
class testSet
{
private:
	std::unordered_set<T> set;
	std::mutex mylock;
public:
	void add(T item)
	{
		std::lock_guard<std::mutex> lock(mylock);
		set.insert(item);
	}

	void remove(T item)
	{
		std::lock_guard<std::mutex> lock(mylock);
		set.erase(item);
	}

	void contains(T item)
	{
		std::lock_guard<std::mutex> lock(mylock);
		set.find(item);
	}
};