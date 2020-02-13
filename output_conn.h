#ifndef OUTPUTCONN_H
#define OUTPUTCONN_H
#pragma once

#include "node_conn.h"
#include "imgui.h"

class OutputConn : public NodeConn {
protected:
	ImVector<NodeLink*> links;
public:
	OutputConn(Node* node, ImVec2 pos, int slot_num);
	virtual ~OutputConn();

	NodeLink* GetLink();
	void AddLink(NodeLink* new_link);
	void RemoveLink(NodeLink* link);
};
#endif // OUTPUTCONN_H

