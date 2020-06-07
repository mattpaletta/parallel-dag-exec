#include <cassert>
#include <complex>
#include <string>
#include <iostream>
#include <chrono>
#include <future>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "graph.hpp"

TEMPLATE_TEST_CASE("contains empty", "[graph]", int, long, double, std::string) {
	const Graph<TestType> g;
	CHECK(!g.contains({TestType(), 1}));
}

TEMPLATE_TEST_CASE("insertion", "[graph]", int, long, double, std::string) {
	Graph<TestType> g;
	g.add_edge({TestType(), 1}, {TestType(), 2});
	CHECK(g.contains({TestType(), 1}));
	CHECK(g.contains({TestType(), 2}));
}

TEMPLATE_TEST_CASE("has_edges empty", "[graph]", int, long, double, std::string) {
	Graph<TestType> g;
	g.add_node({TestType(), 1});
	CHECK(g.contains({TestType(), 1}));
	CHECK(g.get_edges({TestType(), 1}).size() == 0);
}

TEMPLATE_TEST_CASE("has_edges single", "[graph]", int, long, double, std::string) {
	Graph<TestType> g;
	g.add_node({TestType(), 1});
	g.add_node({TestType(), 2});
	g.add_edge({TestType(), 1}, {TestType(), 2});
	CHECK(g.contains({TestType(), 1}));
	CHECK(g.contains({TestType(), 2}));
	CHECK(g.get_edges({TestType(), 1}).size() == 1);
	CHECK(g.has_edge({TestType(), 1}, {TestType(), 2}));
}

TEMPLATE_TEST_CASE("has_edges multiple", "[graph]", int, long, double, std::string) {
	Graph<TestType> g;
	g.add_node({TestType(), 1});
	g.add_node({TestType(), 2});
	g.add_node({TestType(), 3});
	g.add_edge({TestType(), 1}, {TestType(), 2});
	g.add_edge({TestType(), 1}, {TestType(), 3});
	CHECK(g.contains({TestType(), 1}));
	CHECK(g.contains({TestType(), 2}));
	CHECK(g.contains({TestType(), 3}));
	CHECK(g.get_edges({TestType(), 1}).size() == 2);
	CHECK(g.has_edge({TestType(), 1}, {TestType(), 2}));
	CHECK(g.has_edge({TestType(), 1}, {TestType(), 3}));
}

TEMPLATE_TEST_CASE("has_edges multiple new-node", "[graph]", int, long, double, std::string) {
	Graph<TestType> g;
	g.add_edge({TestType(), 1}, {TestType(), 2});
	g.add_edge({TestType(), 1}, {TestType(), 3});
	CHECK(g.contains({TestType(), 1}));
	CHECK(g.contains({TestType(), 2}));
	CHECK(g.contains({TestType(), 3}));
	CHECK(g.get_edges({TestType(), 1}).size() == 2);
	CHECK(g.has_edge({TestType(), 1}, {TestType(), 2}));
	CHECK(g.has_edge({TestType(), 1}, {TestType(), 3}));
}
