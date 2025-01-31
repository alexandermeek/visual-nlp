#ifndef NODECONN_H
#define NODECONN_H
#pragma once

#include "module.h"

#include <imgui/imgui.h>

#include <vector>

class NodeLink;
class Node;

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

enum class Conn_Type { input, output };

class NodeConn {
private:
	std::vector<NodeLink*> links;
	const float RADIUS = 4.0f;
	const ImU32 COLOUR = IM_COL32(255, 153, 0, 255);
	const ImU32 HOVER_COLOUR = IM_COL32(204, 122, 0, 255);
	const ImU32 EDITED_COLOUR = IM_COL32(204, 0, 0, 255);
public:
	int       slot_num;
	Conn_Type type;
	Node*     node;

	NodeConn(Node* node, int slot_num, Conn_Type type);
	virtual ~NodeConn();

	ImVec2 Pos();

	bool HasLink() const;
	void AddLink(NodeLink*);
	void RemoveLink(NodeLink*);
	void RemoveLinks();
	std::vector<NodeLink*>* GetLinks();

	bool IsEdited() const;

	std::string Label() const;
	std::string DataType();

	bool Hovered(ImVec2 offset);
	void Draw(ImDrawList* draw_list, ImVec2 offset);
};
#endif // NODECONN_H

