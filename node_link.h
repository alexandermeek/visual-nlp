#ifndef NODELINK_H
#define NODELINK_H
#pragma once
#include "imgui.h"
#include "node_conn.h"

class NodeLink {
public:
	NodeConn* input;
	NodeConn* output;

	NodeLink(NodeConn* input, NodeConn* output);
	virtual ~NodeLink();
	void SwapDirection();
};
#endif // NODELINE_H