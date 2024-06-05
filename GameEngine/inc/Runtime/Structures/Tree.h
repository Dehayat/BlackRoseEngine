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
		this->parent = nullptr;
		if (parent != nullptr) {
			parent->AddChild(this);
		}
	}
	void RemoveChild(Node* child) {
		child->parent = nullptr;
		if (children.find(child) != children.end()) {
			children.erase(child);
		}
	}
	void AddChild(Node* child) {
		if (child->parent != nullptr) {
			child->parent->RemoveChild(child);
		}
		child->parent = this;
		children.insert(child);
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