#include "node_link.h"

NodeLink::NodeLink(int input_id, int input_slot, int output_id, int output_slot)
	: input_id(input_id), input_slot(input_slot), output_id(output_id), output_slot(output_slot) {}

void NodeLink::SwapDirection() {
	int temp_input_id = input_id;
	int temp_input_slot = input_slot;
	input_id = output_id;
	input_slot = output_slot;
	output_id = temp_input_id;
	output_slot = temp_input_slot;
}