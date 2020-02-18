#ifndef NODEVEC_H
#define NODEVEC_H
#pragma once

#include "node.h"
#include "imgui.h"

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

	Node* operator[](int pos);
};
#endif // NODEVEC_H

