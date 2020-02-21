#include "node_link.h"

NodeLink::NodeLink(NodeConn* start, NodeConn* end) : start(start), end(end) {
	start->AddLink(this);
	end->AddLink(this);
}

NodeLink::~NodeLink() {
	start->RemoveLink(this);
	end->RemoveLink(this);
}