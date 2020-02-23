#ifndef NODE_H
#define NODE_H
#pragma once
#include "imgui.h"
#include "node_conn.h"
#include "node_link.h"
#include <vector>

class Node {
private:
	static int	next_id;

	ImVec2 pos;
	ImVec2 size;

public:
	int					id;
	char				name[32];
	int					inputs_count, outputs_count;
	ImVector<NodeConn*> input_conns;
	ImVector<NodeConn*> output_conns;

	Node(const char* name, ImVec2 pos, ImVec2 size, int inputs_count, int outputs_count);
	virtual ~Node();

	ImVec2 GetSlotPos(int slot_num, Conn_Type type);
	ImVec2 Pos();
	ImVec2 Size();
	void Resize(ImVec2);
	void Move(ImVec2 new_pos);

	NodeConn* GetConn(int slot_num, Conn_Type type);
};
#endif //NODE_H
