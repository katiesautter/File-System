#pragma once
#include <mutex>
#include <condition_variable>

// Code taken from:
// https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
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

