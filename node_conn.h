#ifndef NODECONN_H
#define NODECONN_H
#pragma once
#include "imgui.h"

class NodeLink;
class Node;

enum class Conn_Type { input, output };

class NodeConn {
private:
	ImVector<NodeLink*> links;

public:
	const float RADIUS = 4.0f;

	int       slot_num;
	Conn_Type type;
	Node*     node;

	NodeConn(Node* node, int slot_num, Conn_Type type);
	virtual ~NodeConn();

	ImVec2 Pos();

	void AddLink(NodeLink*);
	void RemoveLink(NodeLink*);
	void RemoveLinks();
	ImVector<NodeLink*>* GetLinks();

	bool Hovered(ImVec2 offset);
};
#endif // NODECONN_H

