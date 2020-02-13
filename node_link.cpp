#include "node_link.h"
#include <iostream>

NodeLink::NodeLink(OutputConn* start, InputConn* end) : start(start), end(end) {
	start->AddLink(this);
	end->AddLink(this);
}

NodeLink::~NodeLink() {
	if (start != nullptr) {
		start->RemoveLink(this);
	}
	if (end != nullptr) {
		end->RemoveLink(this);
	}
}