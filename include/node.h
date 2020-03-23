#ifndef NODE_H
#define NODE_H
#pragma once

#include "node_conn.h"
#include "node_link.h"
#include "module.h"

#include <imgui/imgui.h>
#include <nlohmann/json.hpp>

#include <vector>
#include <string>

class Node {
private:
	const ImVec2		NODE_WINDOW_PADDING = ImVec2(8.0f, 8.0f);

	static int			next_id;

	ImVec2				pos;
	ImVec2				size;
public:
	int					id;
	std::string			name;
	std::vector<NodeConn*> input_conns;
	std::vector<NodeConn*> output_conns;

	Module* module;

	Node(const std::string name, ImVec2 pos, ImVec2 size, Module* module);
	virtual ~Node();

	int InputsCount();
	int OutputsCount();
	ImVec2 GetSlotPos(int slot_num, Conn_Type type);
	ImVec2 Pos();
	ImVec2 Size();
	void Resize(ImVec2);
	void Move(ImVec2 new_pos);
	bool Hovered(ImVec2 offset);

	NodeConn* GetConn(int slot_num, Conn_Type type);

	void Draw(ImDrawList* draw_list, ImVec2 offset, bool hovered);
	void CheckConns(ImVec2 offset, bool& conn_hover, NodeConn*& hovered_conn, bool& conn_drag, NodeConn*& dragged_conn);

	json* Results();
	void Run();
};
#endif //NODE_H
