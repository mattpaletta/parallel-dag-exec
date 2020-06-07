#pragma once

#include <thread>
#include <iostream>

#include "graph.hpp"
#include "threadpool.hpp"
#include "queue.hpp"

class JobServer {
public:
	using Function = std::function<void()>;

    JobServer();
	~JobServer() = default;

    std::size_t add_task(Function&& f, std::size_t id);

    Node<Function> get_node(std::size_t id);

    void depends_on(std::size_t from, std::size_t to);

    void depends_on(Node<Function> from, Node<Function> to);

    void run(queue<int>::size_type size = 4);

private:
	Graph<Function> g;
};
