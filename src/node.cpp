#include "node.h"
#include "node_conn.h"
#include <iostream>
int Node::next_id = 0;

Node::Node(const std::string name, ImVec2 pos, ImVec2 size, Module* module)
	: name(name), pos(pos), size(size), module(module) {
	this->id = next_id++;

	if (module != nullptr) {
		for (int i = 0; i < module->ParamsCount(); i++) {
			NodeConn* new_conn = new NodeConn(this, i, Conn_Type::input);
			input_conns.push_back(new_conn);
		}
		for (int i = 0; i < module->ReturnsCount(); i++) {
			NodeConn* new_conn = new NodeConn(this, i, Conn_Type::output);
			output_conns.push_back(new_conn);
		}
	}
}

Node::~Node() {
	for (int i = 0; i < input_conns.size(); i++) {
		delete input_conns[i];
	}
	input_conns.clear();
	for (int i = 0; i < output_conns.size(); i++) {
		delete output_conns[i];
	}
	output_conns.clear();

	delete module;
}

int Node::InputsCount() {
	return module->ParamsCount();
}

int Node::OutputsCount() {
	return module->ReturnsCount();
}

ImVec2 Node::GetSlotPos(int slot_num, Conn_Type type) {
	if (type == Conn_Type::input) {
		return ImVec2(pos.x, pos.y + size.y * ((float)slot_num + 1) / ((float)InputsCount() + 1));
	}
	else if (type == Conn_Type::output) {
		return ImVec2(pos.x + size.x, pos.y + size.y * ((float)slot_num + 1) / ((float)OutputsCount() + 1));
	}
	else {
		return ImVec2(0, 0);
	}
}

ImVec2 Node::Pos() {
	return pos;
}

ImVec2 Node::Size() {
	return size;
}

void Node::Resize(ImVec2 new_size) {
	size = new_size;
}

void Node::Move(ImVec2 new_pos) {
	this->pos = new_pos;
}

NodeConn* Node::GetConn(int slot_num, Conn_Type type) {
	if (type == Conn_Type::input) {
		if (slot_num >= InputsCount()) {
			return nullptr;
		}
		return input_conns[slot_num];
	} 
	else if (type == Conn_Type::output) {
		if (slot_num >= OutputsCount()) {
			return nullptr;
		}

		return output_conns[slot_num];
	}
	else {
		return nullptr;
	}
}

bool Node::Hovered(ImVec2 offset) {
	ImVec2 n_min = pos + offset;
	ImVec2 n_max = n_min + size;

	ImVec2 mp = ImGui::GetIO().MousePos;

	if ((mp.x > n_max.x) || (mp.x < n_min.x) || (mp.y > n_max.y) || (mp.y < n_min.y)) {
		return false;
	}
	else {
		return true;
	}
}

void Node::Draw(ImDrawList* draw_list, ImVec2 offset, bool hovered) {
	ImU32 node_bg_color = hovered ? HOVER_BG_COL : BG_COL;
	ImVec2 node_rect_min = offset + pos;
	ImVec2 node_rect_max = node_rect_min + size;

	draw_list->ChannelsSetCurrent(2); // Foreground

	ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
	ImGui::BeginGroup(); // Lock horizontal position
	ImGui::Text("%s", name.c_str());
	ImGui::Text("Node description...");
	json* results = Results();
	if (results != nullptr && !results->empty()) {
		ImGui::Text("Result(s): %s", results->dump().c_str());
	}
	ImGui::EndGroup();

	Resize(ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING);

	draw_list->ChannelsSetCurrent(1); // Background
	draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
	draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

	ImGui::SetCursorScreenPos(node_rect_min);
	ImGui::InvisibleButton("node", size);

	for (NodeConn* conn : input_conns) {
		conn->Draw(draw_list, offset);
	}
	for (NodeConn* conn : output_conns) {
		conn->Draw(draw_list, offset);
	}
}

void Node::CheckConns(ImVec2 offset, bool& conn_hover, NodeConn*& hovered_conn, bool& conn_drag, NodeConn*& dragged_conn) {
	for (NodeConn* conn : input_conns) {
		bool tmp_conn_hover = conn->Hovered(offset);
		if (tmp_conn_hover && ImGui::IsMouseDown(0) && !conn_drag) {
			dragged_conn = conn;
			conn_drag = true;
			break;
		}
		else if (tmp_conn_hover) {
			hovered_conn = conn;
			conn_hover = true;
		}
	}

	for (NodeConn* conn : output_conns) {
		bool tmp_conn_hover = conn->Hovered(offset);
		if (tmp_conn_hover && ImGui::IsMouseDown(0) && !conn_drag) {
			dragged_conn = conn;
			conn_drag = true;
			break;
		}
		else if (tmp_conn_hover) {
			hovered_conn = conn;
			conn_hover = true;
		}
	}
}

json* Node::Results() {
	return module->Results();
}

void Node::Run() {
	int inputs_count = InputsCount();
	if (inputs_count == 0) {
		module->Run();
	}
	else {
		json* params = new json(json::array());

		const std::vector<std::string>* param_names = module->ParamNames();
		for (int i = 0; i < inputs_count; i++) {
			Node* prev_node;
			std::vector<NodeLink*>* links = input_conns[i]->GetLinks();
			if (!links->empty()) {
				prev_node = links->at(0)->start->node;
			}
			else {
				throw std::runtime_error("Missing input link to node.");
			}
			

			// Run previous node if hasn't run already
			if (prev_node->Results() == nullptr) {
				prev_node->Run();
			}

			params->insert(params->begin() + i, *prev_node->Results());
		}
		module->Run(params);
	}
}