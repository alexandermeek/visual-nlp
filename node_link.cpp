#include "node_link.h"

NodeLink::NodeLink(NodeConn* start, NodeConn* end) : start(start), end(end) {
	start->AddLink(this);
	end->AddLink(this);
}

NodeLink::~NodeLink() {
	start->RemoveLink(this);
	end->RemoveLink(this);
}

void NodeLink::Draw(ImDrawList* draw_list, ImVec2 offset) {
	ImVec2 p1 = offset + start->Pos();
	ImVec2 p2 = offset + end->Pos();

	draw_list->ChannelsSetCurrent(0);
	draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, COLOUR, 3.0f);
}