#include <iostream>

#include <list>
#include <map>
#include <utility>

#include "node.hpp"

template<class T>
class Graph {
public:
	Graph(): list() {}
	~Graph() = default;

	bool contains(const Node<T> &a) const {
		auto elem = this->list.find(a);
		return elem != this->list.end();
	}

	std::list<Node<T>> get_nodes() const {
		std::list<Node<T>> l;
		for (auto it = this->list.begin(); it != this->list.end(); ++it) {
			l.emplace_front(it->first);
		}

		return l;
	}

	std::list<Node<T>> get_edges(const Node<T> &a) const {
		return this->list.find(a)->second;
	}

	bool has_edge(const Node<T> &a, const Node<T> &b) const {
		if (this->contains(a) && this->contains(b)) {
			const auto elem = this->get_edges(a);
			return std::find(elem.begin(), elem.end(), b) != elem.end();
		}
		return false;
	}

	void add_node(Node<T> from) {
		// If it already exists, do nothing
		if (!this->contains(from)) {
			this->list.emplace(std::make_pair<Node<T>, std::list<Node<T>>>(std::move(from), {}));
		}
	}

	Node<T> get_node(const std::size_t id) const {
		for (auto it = this->list.begin(); it != this->list.end(); ++it) {
			if (it->first.get_id() == id) {
				return it->first;
			}
		}
		return {T(), 0};
	}

	void add_edge(Node<T> from, Node<T> to) {
		this->add_node(to);
		this->add_node(from);

		auto new_nodes = this->get_edges(from);
		new_nodes.push_front(to);
		this->list.insert_or_assign(from, new_nodes);
	}

private:
	std::map<Node<T>, std::list<Node<T>>> list;
};
