#include "input_conn.h"
#include "node_link.h"

InputConn::InputConn(Node* node, ImVec2 pos, int slot_num)
	: NodeConn(node, pos, slot_num, Conn_Type::input), link(nullptr) {}

InputConn::~InputConn() {
	if (link != nullptr) {
		delete link;
	}
}

NodeLink* InputConn::GetLink() {
	return link;
}

void InputConn::AddLink(NodeLink* new_link) {
	if (link != nullptr)
		delete link;
	link = new_link;
}

void InputConn::RemoveLink(NodeLink* link) {
	link = nullptr;
}