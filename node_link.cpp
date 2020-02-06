#include "node_link.h"
#include <iostream>

NodeLink::NodeLink(NodeConn* input, NodeConn* output) : input(input), output(output) {
	input->link = this;
	output->link = this;
}

NodeLink::~NodeLink() {
	if (input != nullptr) {
		input->link = nullptr;
	}
	if (output != nullptr) {
		output->link = nullptr;
	}
}

void NodeLink::SwapDirection() {
	NodeConn* temp = output;
	output = input;
	input = temp;
}