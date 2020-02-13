#ifndef INPUTCONN_H
#define INPUTCONN_H
#pragma once

#include "node_conn.h"

class InputConn : public NodeConn {
protected:
	NodeLink* link;
public:
	InputConn(Node* node, ImVec2 pos, int slot_num);
	virtual ~InputConn();

	NodeLink* GetLink();
	void AddLink(NodeLink* new_link);
	void RemoveLink(NodeLink* link);
};
#endif // INPUTCONN_H

