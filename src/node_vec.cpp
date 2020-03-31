#include "node_vec.h"

NodeVec::NodeVec() {}

NodeVec::~NodeVec() {
	std::for_each(nodes.begin(), nodes.end(), [](Node* node) {delete node;});
}

Node* NodeVec::GetNode(int id) {
	for (int i = 0; i < nodes.size(); i++) {
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
	nodes.erase(std::find(nodes.begin(), nodes.end(), node));
}

void NodeVec::RemoveNode(Node* node) {
	nodes.erase(std::find(nodes.begin(), nodes.end(), node));
}

int NodeVec::Size() {
	return (int)nodes.size();
}

std::vector<Node*>::iterator NodeVec::begin() {
	return nodes.begin();
}

std::vector<Node*>::iterator NodeVec::end() {
	return nodes.end();
}

Node* NodeVec::operator[](int pos) {
	return nodes[pos];
}

Node* NodeVec::at(int pos) {
	return nodes[pos];
}