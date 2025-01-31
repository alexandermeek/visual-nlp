#include "node_conn.h"
#include "node_link.h"
#include "node.h"
#include <iostream>

NodeConn::NodeConn(Node* node, int slot_num, Conn_Type type)
	: node(node), slot_num(slot_num), type(type) {}

NodeConn::~NodeConn() {
	RemoveLinks();
}

ImVec2 NodeConn::Pos() {
	return node->GetSlotPos(slot_num, type);
}

bool NodeConn::HasLink() const {
	return links.size() > 0;
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
	links.erase(std::find(links.begin(), links.end(), link));
}

void NodeConn::RemoveLinks() {
	for (NodeLink* link : links) {
		delete link;
	}
	links.clear();
}

std::vector<NodeLink*>* NodeConn::GetLinks() {
	return &links;
}

bool NodeConn::IsEdited() const {
	return node->module->HasCustomParam(Label());
}

std::string NodeConn::Label() const {
	if (type == Conn_Type::input) {
		return (*node->module->ParamNames())[slot_num];
	}
	else {
		return "";
	}
}

std::string NodeConn::DataType() {
	json::value_t data_type = json::value_t::null;
	if (type == Conn_Type::input) {
		data_type = (*node->module->ParamTypes())[slot_num];
	}
	else if (type == Conn_Type::output) {
		data_type = (*node->module->ReturnTypes())[slot_num];
	}
	return node->module->TypeToString(data_type);
}

bool NodeConn::Hovered(ImVec2 offset) {
	ImVec2 mouse_pos = ImGui::GetIO().MousePos;
	ImVec2 conn_pos = node->GetSlotPos(slot_num, type);
	float xd = mouse_pos.x - (offset.x + conn_pos.x);
	float yd = mouse_pos.y - (offset.y + conn_pos.y);

	return ((xd * xd) + (yd * yd)) < (RADIUS * RADIUS);
}

void NodeConn::Draw(ImDrawList* draw_list, ImVec2 offset) {
	ImU32 conn_colour = COLOUR;
	ImVec2 conn_pos = offset + Pos();

	if (Hovered(offset)) {
		conn_colour = HOVER_COLOUR;
	}

	if (IsEdited()) {
		conn_colour = EDITED_COLOUR;
		RemoveLinks();
	}

	draw_list->ChannelsSetCurrent(2);
	draw_list->AddCircleFilled(conn_pos, RADIUS, conn_colour);

	// Draw node links, if this connector is an input conn.
	if (type == Conn_Type::input && links.size() > 0) {
		links.front()->Draw(draw_list, offset);
	}
}