#ifndef NODE_H
#define NODE_H
#pragma once
#include "imgui.h"
#include "node_conn.h"
#include "input_conn.h"
#include "output_conn.h"
#include "node_link.h"
#include <vector>

class Node {
private:
	static int	next_id;

	ImVec2 pos;

	ImVec2 GetInputSlotPos(int slot_num);
	ImVec2 GetOutputSlotPos(int slot_num);
public:
	int					   id;
	char				   name[32];
	ImVec2				   size;
	int					   inputs_count, outputs_count;
	ImVector<InputConn*> input_conns;
	ImVector<OutputConn*> output_conns;

	Node(const char* name, ImVec2 pos, ImVec2 size, int inputs_count, int outputs_count);
	virtual ~Node();

	ImVec2 Pos();
	void Move(ImVec2 new_pos);

	NodeConn* GetConn(int slot_num, Conn_Type type);
	void AddLink(int slot_num, Conn_Type type, NodeConn* linked_conn);
};
#endif //NODE_H
