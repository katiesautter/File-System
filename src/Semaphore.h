#pragma once
#include <mutex>
#include <condition_variable>

class Semaphore
{
private:
	std::mutex mutex_;
	std::condition_variable condition_;
	unsigned long count_ = 1; // Initialized as 1 available

public:
	void notify();
	void wait();
	bool try_wait();
};

