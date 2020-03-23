#include "node.h"
#include "node_link.h"
#include "node_vec.h"
#include "module.h"
#include "module_py.h"

#include <imgui/imgui.h>

#include <math.h> // fmodf
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

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

void ShowNodeGraph(bool* p_open, bool* debug, NodeVec* nodes) {
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Node Graph", p_open)) {
		ImGui::End();
	}

	static std::vector<std::string> stats;
	if (*debug) ShowDiagnosticsWindow(debug, &stats);

	static bool initialised = false;
	static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	static bool show_grid = true;
	static int node_selected = -1;

	if (!initialised) {
		nodes->AddNode(new Node("Node X", ImVec2(40.0f, 50.0f), ImVec2(0.5f, 0.5f), new ModulePy("value")));
		nodes->AddNode(new Node("Node Y", ImVec2(40.0f, 250.0f), ImVec2(0.5f, 0.5f), new ModulePy("value")));
		nodes->AddNode(new Node("Node ADD", ImVec2(250.0f, 150.0f), ImVec2(0.5f, 0.5f), new ModulePy("add")));
		NodeLink* link_1 = new NodeLink(nodes->at(0)->GetConn(0, Conn_Type::output), nodes->at(2)->GetConn(0, Conn_Type::input));
		NodeLink* link_2 = new NodeLink(nodes->at(1)->GetConn(0, Conn_Type::output), nodes->at(2)->GetConn(1, Conn_Type::input));
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
	for (Node* node : *nodes) {
		ImGui::PushID(node->id);
		if (ImGui::Selectable(node->name.c_str(), node->id == node_selected)) {
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
	const ImU32 CANVAS_BG_COL = IM_COL32(24, 24, 24, 255);

	// Create child canvas
	ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
	ImGui::SameLine(ImGui::GetWindowWidth() - 210);
	ImGui::Checkbox("Show grid", &show_grid);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, CANVAS_BG_COL);
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

	// Split frame into three layers, one for links, and two for nodes.
	draw_list->ChannelsSplit(3);

	// Draw links to mouse
	if (conn_drag && ImGui::IsMouseDown(0)) {
		draw_list->ChannelsSetCurrent(2);
		ImVec2 p1, p2;
		if (dragged_conn->type == Conn_Type::input) {
			p1 = ImGui::GetIO().MousePos;
			p2 = offset + dragged_conn->Pos();
		}
		else {
			p1 = offset + dragged_conn->Pos();
			p2 = ImGui::GetIO().MousePos;
		}

		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(255, 153, 0, 255), 3.0f);
	} else if (conn_drag && conn_hover && !ImGui::IsMouseDown(0) && dragged_conn->type != hovered_conn->type) {
		if (dragged_conn->node != hovered_conn->node) {
			if (dragged_conn->type == Conn_Type::input) {
				NodeLink* link = new NodeLink(hovered_conn, dragged_conn);
			}
			else {
				NodeLink* link = new NodeLink(dragged_conn, hovered_conn);
			}
		}
		conn_drag = false;
	}
	else {
		conn_drag = false;
	}

	// Display nodes
	conn_hover = false;
	for (Node* node : (*nodes)) {
		ImGui::PushID(node->id);

		bool node_widgets_active = ImGui::IsAnyItemActive();

		if (node->Hovered(offset)) {
			node_hovered_in_scene = node->id;
			if (ImGui::IsMouseClicked(1)) open_context_menu = true;
		}

		bool node_hovered = (node_hovered_in_list == node->id || node_hovered_in_scene == node->id || (node_hovered_in_list == -1 && node_selected == node->id));
		node->Draw(draw_list, offset, node_hovered);

		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
			node_selected = node->id;
		if (node_moving_active && ImGui::IsMouseDragging(0) && !conn_hover && !conn_drag)
			node->Move(node->Pos() + ImGui::GetIO().MouseDelta);

		node->CheckConns(offset, conn_hover, hovered_conn, conn_drag, dragged_conn);

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
		Node* node = nodes->GetNode(node_selected);
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node) {
			ImGui::Text("Node '%s'", node->name.c_str());
			ImGui::Separator();
			if (ImGui::MenuItem("Run >")) {
				node->Run();
			}
			if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
			if (ImGui::MenuItem("Delete")) {
				nodes->RemoveNode(node_selected);
				node_selected = -1;
				dragged_conn = nullptr;
				hovered_conn = nullptr;
			}
			if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		else {
			if (ImGui::MenuItem("Add")) {
				nodes->AddNode(new Node("New ADD", scene_pos, ImVec2(0.5f, 0.5f), new ModulePy("add")));
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
		ImVec2 conn_pos = hovered_conn->Pos();
		ss << "ConnPos(" << conn_pos.x << "," << conn_pos.y << ")";
		stats.push_back(ss.str());
	}
	else stats.push_back("Hover = FALSE");
	{
		ImVec2 mouse_pos = ImGui::GetIO().MousePos - offset;
		std::stringstream ss;
		ss << "RelMousePos(" << mouse_pos.x << "," << mouse_pos.y << ")";
		stats.push_back(ss.str());
	}
	if (node_selected != -1) {
		Node* node = nodes->GetNode(node_selected);
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

