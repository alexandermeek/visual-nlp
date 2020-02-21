#include "imgui.h"
#include "node.h"
#include "node_link.h"
#include "node_vec.h"

#include <math.h> // fmodf
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

void ShowDiagnosticsWindow(bool* p_open, std::vector<std::string>* stats) {
	if (!ImGui::Begin("Node Graph Diagnostics", p_open)) {
		ImGui::End();
		return;
	}
	//ImGui::SetNextWindowSize(ImVec2(70, 50), ImGuiCond_FirstUseEver);
	ImGui::Text("Node Graph Diagnostics Tool");
	for (int i = 0; i < stats->size(); i++) {
		ImGui::Text(stats->at(i).c_str());
	}

	ImGui::End();
}

void ShowNodeGraph(bool* p_open) {
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Node Graph", p_open)) {
		ImGui::End();
		return;
	}
	static bool diagnosticsWindow = true;
	static std::vector<std::string> stats;
	if (diagnosticsWindow) ShowDiagnosticsWindow(&diagnosticsWindow, &stats);

	static NodeVec nodes;

	static bool initialised = false;
	static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	static bool show_grid = true;
	static int node_selected = -1;

	if (!initialised) {
		nodes.AddNode(new Node("Node One", ImVec2(40.0f, 50.0f), ImVec2(0.5f, 0.5f), 1, 1));
		nodes.AddNode(new Node("Node Two", ImVec2(40.0f, 150.0f), ImVec2(0.5f, 0.5f), 1, 1));
		NodeLink* link = new NodeLink(nodes[0]->GetConn(0, Conn_Type::output), nodes[1]->GetConn(0, Conn_Type::input));
		initialised = true;
	}

	bool open_context_menu = false;
	int node_hovered_in_list = -1;
	int node_hovered_in_scene = -1;

	static bool conn_hover = false;
	static bool conn_drag = false;
	static NodeConn* hovered_conn = nullptr;
	static NodeConn* dragged_conn = nullptr;

	ImGui::BeginChild("node_list", ImVec2(100, 0));
	ImGui::Text("Nodes");
	ImGui::Separator();
	for (Node* node : nodes) {
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
		if (dragged_conn->type == Conn_Type::input) {
			NodeLink* link = new NodeLink(hovered_conn, dragged_conn);
		}
		else {
			NodeLink* link = new NodeLink(dragged_conn, hovered_conn);
		}
		conn_drag = false;
	}
	else {
		conn_drag = false;
	}

	// Display nodes
	conn_hover = false;
	for (Node* node : nodes) {
		ImGui::PushID(node->id);
		ImVec2 node_rect_min = offset + node->Pos();

		// Draw links coming from output connections.
		draw_list->ChannelsSetCurrent(0);
		for (NodeConn* conn : node->input_conns) {
			ImVector<NodeLink*>* conn_links = conn->GetLinks();
			if (conn_links->Size > 0) {
				ImVec2 p1 = offset + conn_links->front()->start->pos;
				ImVec2 p2 = offset + conn_links->front()->end->pos;
				draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
			}
		}

		// Display node contents
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text("%s", node->name);
		ImGui::Text("Node description...");
		ImGui::EndGroup();

		// Save the size of what we have emitted and wether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->Resize(ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING);
		ImVec2 node_rect_max = node_rect_min + node->Size();

		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->Size());
		if (ImGui::IsItemHovered()) {
			node_hovered_in_scene = node->id;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}

		// Track line from connector to mouse
		for (NodeConn* conn : node->input_conns) {
			bool tmp_conn_hover = conn->Hovered(offset);
			if (tmp_conn_hover && ImGui::IsMouseDown(0) && !conn_drag) {
				dragged_conn = conn;
				conn_drag = true;
				break;
			}
			else if (tmp_conn_hover) {
				hovered_conn = conn;
				conn_hover = true;
			}
		}
		
		for (NodeConn* conn : node->output_conns) {
			bool tmp_conn_hover = conn->Hovered(offset);
			if (tmp_conn_hover && ImGui::IsMouseDown(0) && !conn_drag) {
				dragged_conn = conn;
				conn_drag = true;
				break;
			}
			else if (tmp_conn_hover) {
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
			if (conn->Hovered(offset)) {
				conn_colour = IM_COL32(175, 175, 175, 175);
			}
			draw_list->AddCircleFilled(conn_pos, conn->RADIUS, conn_colour);
		}
		for (NodeConn* conn : node->output_conns) {
			ImU32 conn_colour = IM_COL32(150, 150, 150, 150);
			ImVec2 conn_pos = offset + conn->pos;
			if (conn->Hovered(offset)) {
				conn_colour = IM_COL32(175, 175, 175, 175);
			}
			draw_list->AddCircleFilled(conn_pos, conn->RADIUS, conn_colour);
		}

		ImGui::PopID();
	}
	draw_list->ChannelsMerge();

	// Open context menu
	if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(0))) {
		node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		if (ImGui::IsMouseClicked(1)) {
				open_context_menu = true;
		}
	}
	if (conn_hover && ImGui::IsMouseClicked(1)) hovered_conn->RemoveLinks();
	if (open_context_menu && !conn_hover) {
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_list != -1)
			node_selected = node_hovered_in_list;
		if (node_hovered_in_scene != -1)
			node_selected = node_hovered_in_scene;
	}

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu")) {
		Node* node = nodes.GetNode(node_selected);
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node) {
			ImGui::Text("Node '%s'", node->name);
			ImGui::Separator();
			if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
			if (ImGui::MenuItem("Delete")) {
				nodes.RemoveNode(node_selected);
				node_selected = -1;
				dragged_conn = nullptr;
				hovered_conn = nullptr;
			}
			if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		else {
			if (ImGui::MenuItem("Add")) {
				nodes.AddNode(new Node("New node", scene_pos, ImVec2(0.5f, 0.5f), 2, 2));
			}
			if (ImGui::MenuItem("Paste", NULL, false, false)) {}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	// Scrolling
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;

	// Update Diagnostics
	stats.clear();
	if (conn_drag) stats.push_back("Drag = TRUE");
	else stats.push_back("Drag = FALSE");
	if (conn_hover) {
		stats.push_back("Hover = TRUE");
		std::stringstream ss;
		ss << "ConnPos(" << hovered_conn->pos.x << "," << hovered_conn->pos.y << ")";
		stats.push_back(ss.str());
	}
	else stats.push_back("Hover = FALSE");
	{
		ImVec2 wat = ImGui::GetIO().MousePos - offset;
		std::stringstream ss;
		ss << "RelMousePos(" << wat.x << "," << wat.y << ")";
		stats.push_back(ss.str());
	}
	if (node_selected != -1) {
		Node* node = nodes.GetNode(node_selected);
		std::stringstream ss;
		ss << "NodeSize(" << node->Size().x << "," << node->Size().y << ")";
		stats.push_back(ss.str());
	}

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::End();
}

