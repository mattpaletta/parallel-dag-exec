#include "threadpool.hpp"

thread_pool::thread_pool() : q_(32) {
    int hc = std::thread::hardware_concurrency();
    this->schedule_threads(hc > 0 ? hc : 2);
}

thread_pool::thread_pool(std::size_t num_threads) : q_(32) {
    this->schedule_threads(num_threads);
}

thread_pool::~thread_pool() {
    if (!this->is_shutdown()) {
        this->shutdown();
    }
}

thread_pool::size_type thread_pool::size() const {
    return this->q_.max_size();
}

void thread_pool::schedule(std::function<void()>&& func) {
    std::unique_lock lock(this->mu_);
    this->q_.push(std::move(func));
}

void thread_pool::shutdown() {
    std::unique_lock lock(this->mu_);
    if (!q_.is_closed()) {
        q_.close();
        for (auto& th : this->threads_) {
            th.join();
        }
    }
}

bool thread_pool::is_shutdown() const {
    return q_.is_closed();
}

// Private Functions
void thread_pool::schedule_threads(std::size_t num_threads) {
    for (std::size_t t = 0; t < num_threads; ++t) {
        threads_.emplace_back([this]() {
            // While the queue is open (we have not shutdown)
            // Keep polling for new functions to run from the thread pool
            // These will be scheduled in order because of the queue.
            while (true) {
                std::function<void()> f;
                auto did_pop = false;
                if (!q_.is_closed() || (q_.is_closed() && !q_.is_empty())) {
                    if (q_.pop(std::ref(f)) == queue<std::function<void()>>::status::success) {
                        f();
                    }
                } else {
                    break;
                }
            }
        });
    }
}

