#ifndef NODELINK_H
#define NODELINK_H
#pragma once

#include "node_conn.h"

#include <imgui/imgui.h>

class NodeLink {
public:
	ImU32 COLOUR = IM_COL32(200, 200, 100, 255);

	NodeConn* start;
	NodeConn* end;

	NodeLink(NodeConn* start, NodeConn* end);
	virtual ~NodeLink();

	void Draw(ImDrawList* draw_list, ImVec2 offset);
};
#endif // NODELINE_H