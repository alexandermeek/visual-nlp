#include "output_conn.h"
#include "node_link.h"

OutputConn::OutputConn(Node* node, ImVec2 pos, int slot_num)
	: NodeConn(node, pos, slot_num, Conn_Type::output) {}

OutputConn::~OutputConn() {
	for (int i = 0; i < links.Size; i++) {
		if (links[i] != nullptr) {
			delete links[i];
			//links[i] = nullptr;
		}
	}
}

#include <algorithm>

NodeLink* OutputConn::GetLink() {
	if (links.Size > 0)
		return links[0];
	else
		return nullptr;
}

void OutputConn::AddLink(NodeLink* new_link) {
	links.push_back(new_link);
}

void OutputConn::RemoveLink(NodeLink* link) {
	if (links.Size <= 1) {
		links.clear();
	}
	else {

		auto it = std::find(links.begin(), links.end(), link);
		links.insert(it, links[links.Size]);
		links.erase(links.end());
	}
}