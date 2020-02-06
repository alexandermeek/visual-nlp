#include "node.h"
#include <iostream>
int Node::next_id = 0;

Node::Node(const char* name, ImVec2 pos, ImVec2 size, int inputs_count, int outputs_count)
	: pos(pos), size(size), inputs_count(inputs_count), outputs_count(outputs_count)
{
	this->id = next_id++;
	strncpy_s(this->name, name, 31);
	this->name[31] = 0;

	for (int i = 0; i < inputs_count; i++) {
		NodeConn* new_conn = new NodeConn(this, GetInputSlotPos(i), i, Conn_Type::input);
		input_conns.push_back(new_conn);
	}
	for (int i = 0; i < outputs_count; i++) {
		NodeConn* new_conn = new NodeConn(this, GetOutputSlotPos(i), i, Conn_Type::output);
		output_conns.push_back(new_conn);
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
}

ImVec2 Node::GetInputSlotPos(int slot_num) {
	return ImVec2(pos.x, pos.y + size.y * ((float)slot_num + 1) / ((float)inputs_count + 1));
}

ImVec2 Node::GetOutputSlotPos(int slot_num) {
	return ImVec2(pos.x + size.x, pos.y + size.y * ((float)slot_num + 1) / ((float)outputs_count + 1));
}

ImVec2 Node::Pos() {
	return this->pos;
}

void Node::Move(ImVec2 new_pos) {
	this->pos = new_pos;

	for (int i = 0; i < input_conns.size(); i++) {
		input_conns[i]->pos = GetInputSlotPos(i);
	}
	for (int i = 0; i < output_conns.size(); i++) {
		output_conns[i]->pos = GetOutputSlotPos(i);
	}
}

NodeConn* Node::GetConn(int slot_num, Conn_Type type) {
	if (type == Conn_Type::input) {
		if (slot_num >= inputs_count) {
			return nullptr;
		}
		return input_conns[slot_num];
	} 
	else if (type == Conn_Type::output) {
		if (slot_num >= outputs_count) {
			return nullptr;
		}

		return output_conns[slot_num];
	}
}

void Node::AddLink(int slot_num, Conn_Type type, NodeConn* linked_conn) {
	NodeConn* my_conn = nullptr;
	if (type == Conn_Type::input) {
		if (slot_num < inputs_count) {
			my_conn = input_conns[slot_num];
		}
	}
	else if (type == Conn_Type::output) {
		if (slot_num < outputs_count) {
			my_conn = output_conns[slot_num];
		}
	}

	NodeLink* new_link = new NodeLink(my_conn, linked_conn);
	std::cout << linked_conn->link->output->node->name;
}