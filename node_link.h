#ifndef NODELINK_H
#define NODELINK_H
#pragma once
#include "imgui.h"
#include "node_conn.h"

class NodeLink {
public:
	NodeConn* start;
	NodeConn* end;

	NodeLink(NodeConn* start, NodeConn* end);
	virtual ~NodeLink();
};
#endif // NODELINE_H