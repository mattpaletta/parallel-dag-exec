#include <cassert>
#include <complex>
#include <string>
#include <iostream>
#include <chrono>
#include <future>
#include <atomic>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "include/jobserver.hpp"

TEST_CASE("no tasks", "[jobserver]") {
	JobServer j;
	std::atomic<int> sum{0};
	j.run();
	CHECK(sum == 0);
}

TEST_CASE("single tasks", "[jobserver]") {
	JobServer j;
	std::atomic<int> sum{0};
	j.add_task([&sum]{
		sum++;
	}, 0);
	j.run();
	CHECK(sum == 1);
}

TEST_CASE("multiple independent tasks", "[jobserver]") {
	JobServer j;
	std::atomic<int> sum{0};
	for (int i = 0; i < 10; ++i) {
		j.add_task([&sum]{
			sum++;
		}, i);
	}
	j.run();
	CHECK(sum == 10);
}

void order_of_operations(std::size_t size) {
	JobServer j;
	std::atomic<int> result{0};
	// Explicitly do: 3 + 6 x (5 + 4) / 3 - 7
	j.add_task([&result]{ result = 5 + 4; }, 0);
	j.add_task([&result]{ result = 6 * result; }, 1);
	j.depends_on(1, 0);
	j.add_task([&result]{ result = result / 3; }, 2);
	j.depends_on(2, 1);
	j.add_task([&result]{ result = result + 3; }, 3);
	j.depends_on(3, 2);
	j.add_task([&result]{ result = result - 7; }, 4);
	j.depends_on(4, 3);
	j.run(size);
	CHECK(result == 14);
}

TEST_CASE("order of operations tasks", "[jobserver]") {
	order_of_operations(1);
}

TEST_CASE("order of operations multi tasks", "[jobserver]") {
	order_of_operations(4);
}

TEST_CASE("check_parallel", "[jobserver]") {
	JobServer j;
	std::string A = "";
	std::string B = "";

	j.add_task([&B]{ B = "Hello"; }, 0);
	j.add_task([&A]{ A = "World"; std::this_thread::sleep_for(std::chrono::milliseconds(100));}, 1);
	j.add_task([&A, &B]{ std::swap(A, B); }, 2);
	j.depends_on(2, 1);
	j.depends_on(2, 0);
	j.run(2);
	CHECK(A == "Hello");
	CHECK(B == "World");
}
