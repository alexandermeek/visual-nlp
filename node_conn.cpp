#include "node_conn.h"
#include "node_link.h"
#include <iostream>

NodeConn::NodeConn(Node* node, ImVec2 pos, int slot_num, Conn_Type type)
	: node(node), pos(pos), slot_num(slot_num), type(type) {}

NodeConn::~NodeConn() {
	RemoveLinks();
}

void NodeConn::AddLink(NodeLink* link) {
	if (type == Conn_Type::input) {
		RemoveLinks();
		links.push_back(link);
	}
	else if (type == Conn_Type::output) {
		links.push_back(link);
	}
}

void NodeConn::RemoveLink(NodeLink* link) {
	links.find_erase(link);
}

void NodeConn::RemoveLinks() {
	for (NodeLink* link : links) {
		delete link;
	}
	links.clear();
}

ImVector<NodeLink*>* NodeConn::GetLinks() {
	return &links;
}

bool NodeConn::Hovered(ImVec2 offset) {
	ImVec2 mouse_pos = ImGui::GetIO().MousePos;
	float xd = mouse_pos.x - (offset.x + pos.x);
	float yd = mouse_pos.y - (offset.y + pos.y);

	return ((xd * xd) + (yd * yd)) < (RADIUS * RADIUS);
}