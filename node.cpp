#include "node.h"
#include <iostream>
int Node::next_id = 0;

Node::Node(const char* name, ImVec2 pos, ImVec2 size, int inputs_count, int outputs_count)
	: pos(pos), size(size), inputs_count(inputs_count), outputs_count(outputs_count)
{
	this->id = next_id++;
	strncpy_s(this->name, name, 31);
	this->name[31] = 0;
}

ImVec2 Node::GetInputSlotPos(int slot_num) {
	return ImVec2(pos.x, pos.y + size.y * ((float)slot_num + 1) / ((float)inputs_count + 1));
}

ImVec2 Node::GetOutputSlotPos(int slot_num) {
	return ImVec2(pos.x + size.x, pos.y + size.y * ((float)slot_num + 1) / ((float)outputs_count + 1));
}
