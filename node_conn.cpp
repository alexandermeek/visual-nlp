#include "node_conn.h"
#include "node_link.h"
#include <iostream>

NodeConn::NodeConn(Node* node, ImVec2 pos, int slot_num, Conn_Type type)
	: node(node), pos(pos), slot_num(slot_num), type(type) {}

NodeConn::~NodeConn() {}

bool NodeConn::Hovered(ImVec2 offset) {
	ImVec2 mouse_pos = ImGui::GetIO().MousePos;
	float xd = mouse_pos.x - (offset.x + pos.x);
	float yd = mouse_pos.y - (offset.y + pos.y);

	return ((xd * xd) + (yd * yd)) < (RADIUS * RADIUS);
}