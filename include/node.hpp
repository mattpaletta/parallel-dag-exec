#include <cstddef>

template<class T>
class Node {
public:
	Node(T _value, std::size_t _id) : value(_value), id(_id) {}
	~Node() = default;

	T get_value() const {
		return this->value;
	}

	std::size_t get_id() const {
		return this->id;
	}

	bool operator==(const Node<T> &b) const {
		return b.id == this->id && b.value == this->value;
	}

	bool operator<(const Node<T> &b) const {
		return this->id < b.id;
	}

private:
	T value;
	std::size_t id;
};
