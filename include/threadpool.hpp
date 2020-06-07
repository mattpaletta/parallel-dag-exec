#pragma once

#include <thread>
#include <functional>
#include <mutex>
#include <list>

#include "queue.hpp"

class thread_pool {
public:
	using size_type = std::size_t;

    thread_pool();
    thread_pool(std::size_t num_threads);

	// A thread pool is not copyable or movable.
	thread_pool(const thread_pool&) = delete;
	thread_pool& operator=(const thread_pool&) = delete;
	thread_pool(thread_pool&&) = delete;
	thread_pool& operator=(thread_pool&&) = delete;

    ~thread_pool();

    size_type size() const;

    void schedule(std::function<void()>&& func);

    void shutdown();

    bool is_shutdown() const;

private:
	mutable std::mutex mu_;
	std::list<std::thread> threads_;
	queue<std::function<void()>> q_;

    void schedule_threads(std::size_t num_threads);
};
