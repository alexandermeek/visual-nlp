#include "node_conn.h"
#include "node_link.h"
#include <iostream>

NodeConn::NodeConn(Node* node, ImVec2 pos, int slot_num, Conn_Type type)
	: node(node), pos(pos), slot_num(slot_num), type(type) {}

NodeConn::~NodeConn() {}