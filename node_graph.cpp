#include "imgui.h"
#include "node.h"
#include "node_link.h"

#include <math.h> // fmodf
#include <iostream>
#include <memory>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

static bool isConnectorHovered(ImVec2 con_pos, const float con_radius) {
	ImVec2 mousePos = ImGui::GetIO().MousePos;

	float xd = mousePos.x - con_pos.x;
	float yd = mousePos.y - con_pos.y;

	return ((xd * xd) + (yd * yd)) < (con_radius * con_radius);
}

void ShowNodeGraph(bool* p_open) {
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Node Graph", p_open)) {
		ImGui::End();
		return;
	}

	static ImVector<Node*> nodes;

	static bool initialised = false;
	static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	static bool show_grid = true;
	static int node_selected = -1;

	if (!initialised) {
		nodes.push_back(new Node("Node One", ImVec2(40, 50), ImVec2(0.5f, 0.5f), 1, 1));
		nodes.push_back(new Node("Node Two", ImVec2(40, 150), ImVec2(0.5f, 0.5f), 1, 1));
		nodes[0]->AddLink(0, Conn_Type::output, nodes[1]->GetConn(0, Conn_Type::input));
		initialised = true;
	}

	bool open_context_menu = false;
	int node_hovered_in_list = -1;
	int node_hovered_in_scene = -1;

	static bool conn_hover = false;
	static bool conn_drag = false;
	static NodeConn* hovered_conn = nullptr;
	static NodeConn* dragged_conn = nullptr;
	
	/*if (hovered_conn != nullptr) {
		std::cout << "HoveredConn:" << hovered_conn->node->name << ", Input?:" << (hovered_conn->type == Conn_Type::input ? "input" : "output") << ", Slot:" << hovered_conn->slot_num;
	}
	if (dragged_conn != nullptr) {
		std::cout << " | DraggedConn:" << dragged_conn->node->name << ", Input?:" << (dragged_conn->type == Conn_Type::input ? "input" : "output") << ", Slot:" << dragged_conn->slot_num << "\r";
	}
	else {
		std::cout << "\r";
	}*/

	ImGui::BeginChild("node_list", ImVec2(100, 0));
	ImGui::Text("Nodes");
	ImGui::Separator();
	for (int node_id = 0; node_id < nodes.Size; node_id++) {
		Node* node = nodes[node_id];
		ImGui::PushID(node->id);
		if (ImGui::Selectable(node->name, node->id == node_selected)) {
			node_selected = node->id;
		}
		if (ImGui::IsItemHovered()) {
			node_hovered_in_list = node->id;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginGroup();

	const float NODE_SLOT_RADIUS = 4.0f;
	const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

	// Create child canvas
	ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
	ImGui::SameLine(ImGui::GetWindowWidth() - 210);
	ImGui::Checkbox("Show grid", &show_grid);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.0f);

	ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Display grid
	if (show_grid) {
		const ImU32 GRID_COLOUR = IM_COL32(200, 200, 200, 40);
		const float GRID_SIZE = 64.0f;
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_size = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SIZE); x < canvas_size.x; x += GRID_SIZE)
			draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_size.y) + win_pos, GRID_COLOUR);
		for (float y = fmodf(scrolling.y, GRID_SIZE); y < canvas_size.y; y += GRID_SIZE)
			draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_size.x, y) + win_pos, GRID_COLOUR);
	}

	// Split frame into two layers, one for links, and one for nodes.
	draw_list->ChannelsSplit(2);

	// Draw links to mouse
	if (conn_drag && ImGui::IsMouseDown(0)) {
		draw_list->ChannelsSetCurrent(1);
		ImVec2 p1, p2;
		if (dragged_conn->type == Conn_Type::input) {
			p1 = ImGui::GetIO().MousePos;
			p2 = offset + dragged_conn->pos;
		}
		else {
			p1 = offset + dragged_conn->pos;
			p2 = ImGui::GetIO().MousePos;
		}

		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
	} else if (conn_drag && conn_hover && !ImGui::IsMouseDown(0) && dragged_conn->type != hovered_conn->type) {
		std::cout << "wub";
		NodeLink* new_link;
		if (dynamic_cast<InputConn*>(dragged_conn)) {
			new_link = new NodeLink((OutputConn*)hovered_conn, (InputConn*)dragged_conn);
		}
		else {
			new_link = new NodeLink((OutputConn*)dragged_conn, (InputConn*)hovered_conn);
		}
		conn_drag = false;
	}
	else {
		conn_drag = false;
	}

	// Display nodes
	for (int node_id = 0; node_id < nodes.Size; node_id++) {
		Node* node = nodes[node_id];
		ImGui::PushID(node->id);
		ImVec2 node_rect_min = offset + node->Pos();

		// Draw links coming from output connections.
		draw_list->ChannelsSetCurrent(0);
		for (int i = 0; i < node->input_conns.Size; i++) {
			NodeLink* link = node->input_conns[i]->GetLink();
			if (link != nullptr) {
				ImVec2 p1 = offset + link->start->pos;
				ImVec2 p2 = offset + link->end->pos;
				draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
			}
		}

		// Display node contents
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text("%s", node->name);
		ImGui::Separator();
		ImGui::Text("Node description...");
		ImGui::EndGroup();

		// Save the size of what we have emitted and wether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->size;

		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->size);
		if (ImGui::IsItemHovered()) {
			node_hovered_in_scene = node->id;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}

		// Track line from connector to mouse
		conn_hover = false;
		for (int j = 0; j < node->inputs_count; j++) {
			NodeConn* conn = node->GetConn(j, Conn_Type::input);
			ImVec2 conn_pos = offset + conn->pos;
			if (isConnectorHovered(conn_pos, NODE_SLOT_RADIUS) && ImGui::IsMouseDown(0) && !conn_drag) {
				dragged_conn = conn;
				conn_drag = true;
				break;
			}
			else if (isConnectorHovered(conn_pos, NODE_SLOT_RADIUS)) {
				hovered_conn = conn;
				conn_hover = true;
			}
		}
		
		for (int j = 0; j < node->outputs_count; j++) {
			NodeConn* conn = node->GetConn(j, Conn_Type::output);
			ImVec2 conn_pos = offset + conn->pos;
			if (isConnectorHovered(conn_pos, NODE_SLOT_RADIUS) && ImGui::IsMouseDown(0) && !conn_drag) {
				dragged_conn = conn;
				conn_drag = true;
				break;
			}
			else if (isConnectorHovered(conn_pos, NODE_SLOT_RADIUS)) {
				hovered_conn = conn;
				conn_hover = true;
			}
		}

		bool node_moving_active = ImGui::IsItemActive();

		if (node_widgets_active || node_moving_active)
			node_selected = node->id;
		if (node_moving_active && ImGui::IsMouseDragging(0) && !conn_hover && !conn_drag)
			node->Move(node->Pos() + ImGui::GetIO().MouseDelta);

		ImU32 node_bg_color = (node_hovered_in_list == node->id || node_hovered_in_scene == node->id || (node_hovered_in_list == -1 && node_selected == node->id)) ? IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
		for (NodeConn* conn : node->input_conns) {
			ImU32 conn_colour = IM_COL32(150, 150, 150, 150);
			ImVec2 conn_pos = offset + conn->pos;
			if (isConnectorHovered(conn_pos, NODE_SLOT_RADIUS)) {
				conn_colour = IM_COL32(175, 175, 175, 175);
			}
			draw_list->AddCircleFilled(conn_pos, NODE_SLOT_RADIUS, conn_colour);
		}
		for (NodeConn* conn : node->output_conns) {
			ImU32 conn_colour = IM_COL32(150, 150, 150, 150);
			ImVec2 conn_pos = offset + conn->pos;
			if (isConnectorHovered(conn_pos, NODE_SLOT_RADIUS)) {
				conn_colour = IM_COL32(175, 175, 175, 175);
			}
			draw_list->AddCircleFilled(conn_pos, NODE_SLOT_RADIUS, conn_colour);
		}

		ImGui::PopID();
	}
	draw_list->ChannelsMerge();

	// Open context menu
	if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) {
		node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		open_context_menu = true;
	}
	if (open_context_menu) {
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_list != -1)
			node_selected = node_hovered_in_list;
		if (node_hovered_in_scene != -1)
			node_selected = node_hovered_in_scene;
	}

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu")) {
		Node* node = node_selected != -1 ? nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node) {
			ImGui::Text("Node '%s'", node->name);
			ImGui::Separator();
			if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
			if (ImGui::MenuItem("Delete")) {
				delete nodes[node_selected];
				nodes.erase(nodes.begin() + node_selected);
				dragged_conn = nullptr;
				hovered_conn = nullptr;
			}
			if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		else {
			if (ImGui::MenuItem("Add")) {
				nodes.push_back(new Node("New node", scene_pos, ImVec2(0.5f, 0.5f), 2, 2));
			}
			if (ImGui::MenuItem("Paste", NULL, false, false)) {}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	// Scrolling
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::End();
}

