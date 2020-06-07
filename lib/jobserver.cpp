#include "jobserver.hpp"

JobServer::JobServer(): g() {}

std::size_t JobServer::add_task(Function&& f, std::size_t id) {
    this->g.add_node({std::move(f), id});
    return id;
}

Node<JobServer::Function> JobServer::get_node(std::size_t id) {
    return this->g.get_node(id);
}

void JobServer::depends_on(std::size_t from, std::size_t to) {
    this->depends_on(this->get_node(from), this->get_node(to));
}

void JobServer::depends_on(Node<Function> from, Node<Function> to) {
    if (g.contains(from) && g.contains(to)) {
        g.add_edge(from, to);
    }
}

void JobServer::run(queue<int>::size_type size) {
    auto nodes = g.get_nodes();
    queue<std::size_t> queue{nodes.size()};
    std::map<std::size_t, bool> completed;
    for (const auto& n : nodes) {
        queue.push(n.get_id());
        completed.emplace(n.get_id(), false);
    }
    auto execute = [&](std::size_t id) {
        const auto node = this->g.get_node(id);
        auto d_id = node.get_id();
        if (!completed.at(d_id)) {
            auto func = node.get_value();
            func();
            completed[d_id] = true;
            //                completed.emplace(std::move(d_id), true);
        }
    };

    auto ready_to_execute = [&](std::size_t id) {
        auto node = this->g.get_node(id);
        auto deps = this->g.get_edges(std::move(node));
        bool can_process = true;
        for (const auto& d : deps) {
            const auto d_id = d.get_id();
            const auto ready = completed.at(d_id);
            if (ready) {
                execute(d_id);
            }
            can_process = can_process && completed.at(d_id);
        }
        if (can_process) {
            execute(id);
        } else {
            auto tmp = id;
            queue.push(std::move(tmp));
        }

        return can_process;
    };

    // If single thread, don't use the thread pool
    while (!queue.is_empty() && size <= 1) {
        std::size_t next;
        queue.pop(next);
        ready_to_execute(next);
    }

    if (size > 1) {
        // Launch the pool with four threads.
        thread_pool pool(size);
        pool.schedule([&]{
            while (!queue.is_empty()) {
                std::size_t next;
                queue.pop(next);
                ready_to_execute(next);
            }
        });
        pool.shutdown();
    }
}
