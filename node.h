#ifndef NODE_H
#define NODE_H
#pragma once
#include "imgui.h"
class Node {
private:
	static int	next_id;
public:
	int			id;
	char		name[32];
	ImVec2		pos, size;
	int			inputs_count, outputs_count;

	Node(const char* name, ImVec2 pos, ImVec2 size, int inputs_count, int outputs_count);

	ImVec2 GetInputSlotPos(int slot_num);
	ImVec2 GetOutputSlotPos(int slot_num);
};
#endif //NODE_H
