#ifndef NODELINK_H
#define NODELINK_H
#pragma once
#include "imgui.h"
class NodeLink {
public:
	int		input_id;
	int		input_slot;
	int		output_id;
	int		output_slot;

	NodeLink(int input_id, int input_slot, int output_id, int output_slot);
	void SwapDirection();
};
#endif  // NODELINE_H