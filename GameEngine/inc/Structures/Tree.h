#pragma once
#include <unordered_set>
#include <unordered_map>

template<typename T>
struct Node {
	Node* parent;
	T element;
	std::unordered_set<Node*> children;
	Node(T element, Node* parent = nullptr) {
		this->element = element;
		this->parent = parent;
	}
	void SetParent(Node* parent) {
		if (this->parent != nullptr) {
			this->parent->children.erase(this);
		}
		this->parent = parent;
		this->parent->children.insert(this);
	}
	void RemoveChild(Node* child) {
		if (children.find(child)) {
			children.erase(child);
		}
	}
	void DeleteChildren() {
		for (auto p : children) {
			delete p;
		}
		children.clear();
	}
	~Node() {
	}
};