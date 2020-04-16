#include "node.h"
#include "node_conn.h"
#include <iostream>
int Node::next_id = 0;

Node::Node(const std::string name, ImVec2 pos, ImVec2 size, Module* module)
	: name(name), pos(pos), size(size), module(module) {
	this->id = next_id++;

	if (module) {
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
	//ImGui::Text("Node description...");
	json* results = Results();
	if (results && !results->empty()) {
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

void Node::CheckConns(ImVec2 offset, bool& conn_hover, NodeConn*& hovered_conn, bool& conn_drag, NodeConn*& dragged_conn, bool& node_drag) {
	for (NodeConn* conn : input_conns) {
		bool tmp_conn_hover = conn->Hovered(offset);
		if (tmp_conn_hover && ImGui::IsMouseDown(0) && !conn_drag && !node_drag) {
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
		if (tmp_conn_hover && ImGui::IsMouseDown(0) && !conn_drag && !node_drag) {
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

void Node::Run(bool force_rerun) {
	int inputs_count = InputsCount();
	if (inputs_count == 0) {
		module->Run(); // Run module with no parameters.
	}
	else {
		json* params = new json(json::array());

		const std::vector<std::string>* param_names = module->ParamNames();
		json* custom_params = module->CustomParams();

		for (int i = 0; i < inputs_count; i++) {
			Node* prev_node;
			std::vector<NodeLink*>* links = input_conns[i]->GetLinks();

			if (links->size() <= 0 && !input_conns[i]->IsEdited()) {
				throw MissingInputException("No input links detected.", module);
			}
			else {
				// Check if there exists a custom parameter, if so insert in place. Otherwise run previous node
				if (input_conns[i]->IsEdited()) {
					params->push_back(custom_params->at(param_names->at(i)));
				}
				else {
					prev_node = links->at(0)->start->node;

					if (prev_node->Results() == nullptr || force_rerun) {
						prev_node->Run(force_rerun);
					}

					params->push_back(*prev_node->Results());
				}
			}
		}
		module->Run(params);
	}
}