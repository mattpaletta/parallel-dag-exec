#pragma once

#include <list>
#include <mutex>
#include <condition_variable>

// Concurrent bounded FIFO queue class.
template <class T>
class queue {
public:
	// The type of each of the elements stored in the queue.
	using value_type = T;

	// An unsigned integral type used to represent sizes.
	using size_type = std::size_t;

	// A type for the status of a queue operation.
	enum class status {
		success = 0, // operation successful
		empty, // queue is empty (not currently used)
		full, // queue is full (not currently used)
		closed, // queue is closed
	};

	// A queue is not default construtable
	queue() = delete;

	queue(size_type max_size) : status_(status::empty), data_(), max_size_(max_size) {}

	// A queue is not movable or copyable.
	queue(const queue&) = delete;
	queue& operator=(const queue&) = delete; queue(queue&&) = delete;
	queue& operator=(queue&&) = delete;

	~queue() {
		this->close();
		this->clear();
	}

	status push(value_type&& x) {
		// Lock unique_lock(mu)
		std::unique_lock lock(this->mu_);
		// Wait until queue is not full
		this->c_.wait(lock, [this]() { return this->is_closed() or !this->is_full();});

		// Update the queue status if required (if not closed)
		if (this->is_closed()) {
			//this->c_.notify_one();
			return status::closed;
		}

		// emplace_back the element
		this->data_.push_front(std::move(x));
		if (this->data_.size() == this->max_size_ && !this->is_closed()) {
			this->status_ = status::full;
		}

		this->c_.notify_one();

		// return status
		return status::success;
	}

	status pop(value_type& x) {
		// Lock unique_lock(mu)
		std::unique_lock lock(this->mu_);

		// Block until queue closed or not empty
		this->c_.wait(lock, [this]() { return this->is_closed() or !this->is_empty();});

		if (this->is_empty() && this->is_closed()) {
			this->c_.notify_one();
			return status::closed;
		}

		// remove one element
		x = std::move(this->data_.back());
		this->data_.pop_back();
		this->c_.notify_one();

		if (this->data_.size() == 0 && !this->is_closed()) {
			this->status_ = status::empty;
		}
		// Return status
		return status::success;
	}

	void close() {
		std::unique_lock lock(this->mu_);
		this->status_ = status::closed;
		this->c_.notify_one();
	}

	void clear() {
		// Lock unique_lock
		std::unique_lock lock(this->mu_);

		// Empty queue
		this->data_.clear();
		// Set status to empty
		this->status_ = status::empty;
	}

	bool is_full() const {
		return this->data_.size() == this->max_size_;
	}

	bool is_empty() const {
		return this->data_.size() == 0;
	}

	bool is_closed() const {
		return this->status_ == status::closed;
	}

	size_type max_size() const {
		return this->max_size_;
	}

private:
	status status_;
	mutable std::condition_variable c_;
	mutable std::mutex mu_;
	size_type max_size_;
	std::list<value_type> data_;
};
