#include "node_vec.h"

NodeVec::NodeVec() {}

NodeVec::~NodeVec() {}

Node* NodeVec::GetNode(int id) {
	for (int i = 0; i < nodes.Size; i++) {
		if (nodes[i]->id == id) {
			return nodes[i];
		}
	}
	return nullptr;
}

void NodeVec::AddNode(Node* node) {
	nodes.push_back(node);
}

void NodeVec::RemoveNode(int id) {
	Node* node = GetNode(id);
	delete node;
	nodes.find_erase(node);
}

void NodeVec::RemoveNode(Node* node) {
	auto it = nodes.find(node);
	delete* it;
	nodes.erase(it);
}

int NodeVec::Size() {
	return nodes.Size;
}

Node* NodeVec::operator[](int pos) {
	return nodes[pos];
}