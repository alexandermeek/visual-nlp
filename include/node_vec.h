#ifndef NODEVEC_H
#define NODEVEC_H
#pragma once

#include "node.h"

#include <imgui/imgui.h>

#include <algorithm>

class NodeVec {
private:
	ImVector<Node*> nodes;
public:
	NodeVec();
	virtual ~NodeVec();

	Node* GetNode(int id);
	void AddNode(Node* new_node);
	void RemoveNode(int id);
	void RemoveNode(Node* node);
	
	int Size();
	Node** begin();
	Node** end();

	Node* operator[](int pos);
};
#endif // NODEVEC_H
