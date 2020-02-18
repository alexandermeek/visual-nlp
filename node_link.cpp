#include "node_link.h"
#include <iostream>

NodeLink::NodeLink(NodeConn* start, NodeConn* end) : start(start), end(end) {}

NodeLink::~NodeLink() {}