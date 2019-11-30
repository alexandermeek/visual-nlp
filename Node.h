#ifndef NODE_H
#define NODE_H
#pragma once
#include "imgui.h"
class Node
{
private:
	int id;
	ImVec2 pos, size;
	int inputs_count, outputs_count;
public:
	Node(int id, ImVec2 pos, ImVec2 size, int inputsCount, int outputsCount);

	ImVec2 GetInputSlotPos(int slotNum);
	ImVec2 GetOutputSlotPos(int slotNum);
};
#endif //NODE_H