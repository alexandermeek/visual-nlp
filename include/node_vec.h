#ifndef NODEVEC_H
#define NODEVEC_H
#pragma once

#include "node.h"

#include <imgui/imgui.h>

#include <algorithm>

class NodeVec {
private:
	std::vector<Node*> nodes;
public:
	NodeVec();
	virtual ~NodeVec();

	Node* GetNode(int id);
	void AddNode(Node* new_node);
	void RemoveNode(int id);
	void RemoveNode(Node* node);
	
	int Size();
	std::vector<Node*>::iterator begin();
	std::vector<Node*>::iterator end();

	Node* operator[](int pos);
	Node* at(int pos);
};
#endif // NODEVEC_H

