#pragma once
#include <atomic>
#include <thread>

class Spinlock {
	std::atomic_flag flag;
public:
	Spinlock() : flag() {}

	void lock() {
		while (flag.test_and_set(std::memory_order_acquire));
	}

	void unlock() {
		flag.clear(std::memory_order_release);
	}
};

Spinlock spin;

void workOnResourceAF() {
	spin.lock();
	// shared resource
	spin.unlock();
}
