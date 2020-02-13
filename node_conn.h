#ifndef NODECONN_H
#define NODECONN_H
#pragma once
#include "imgui.h"

class NodeLink;
class Node;

enum class Conn_Type { input, output };

class NodeConn {
public:
	ImVec2    pos;
	int       slot_num;
	Conn_Type type;
	Node*     node;

	NodeConn(Node* node, ImVec2 pos, int slot_num, Conn_Type type);
	virtual ~NodeConn();

	virtual NodeLink* GetLink() = 0;
	virtual void AddLink(NodeLink* new_link) = 0;
	virtual void RemoveLink(NodeLink* link) = 0;
};
#endif // NODECONN_H

