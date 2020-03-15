#ifndef NODECONN_H
#define NODECONN_H
#pragma once

#include <imgui/imgui.h>

class NodeLink;
class Node;

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

enum class Conn_Type { input, output };

class NodeConn {
private:
	ImVector<NodeLink*> links;
	const float RADIUS = 4.0f;
	const ImU32 COLOUR = IM_COL32(150, 150, 150, 150);
	const ImU32 HOVER_COLOUR = IM_COL32(175, 175, 175, 175);
public:
	int       slot_num;
	Conn_Type type;
	Node*     node;

	NodeConn(Node* node, int slot_num, Conn_Type type);
	virtual ~NodeConn();

	ImVec2 Pos();

	void AddLink(NodeLink*);
	void RemoveLink(NodeLink*);
	void RemoveLinks();
	ImVector<NodeLink*>* GetLinks();

	bool Hovered(ImVec2 offset);
	void Draw(ImDrawList* draw_list, ImVec2 offset);
};
#endif // NODECONN_H

