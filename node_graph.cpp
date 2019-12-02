#include "imgui.h"
#include "node.h"
#include "node_link.h"
#include <math.h> // fmodf

#include <iostream>

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

	static ImVector<Node> nodes;
	static ImVector<NodeLink> links;

	static bool initialised = false;
	static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	static bool show_grid = true;
	static int node_selected = -1;

	if (!initialised) {
		nodes.push_back(Node("Node One", ImVec2(40, 50), ImVec2(0.5f, 0.5f), 1, 1));
		nodes.push_back(Node("Node Two", ImVec2(40, 150), ImVec2(0.5f, 0.5f), 1, 1));
		links.push_back(NodeLink(0, 0, 1, 0));
		initialised = true;
	}

	bool open_context_menu = false;
	int node_hovered_in_list = -1;
	int node_hovered_in_scene = -1;

	static bool connector_hovered = false;
	static Node* hover_connector = nullptr;
	static bool hover_connector_is_input = true;
	static int hover_connector_slot = -1;

	static bool connection_drag = false;
	static Node* drag_connector_node = nullptr;
	static bool drag_connector_is_input = true;
	static int drag_connector_slot= -1;

	ImGui::BeginChild("node_list", ImVec2(100, 0));
	ImGui::Text("Nodes");
	ImGui::Separator();
	for (int node_id = 0; node_id < nodes.Size; node_id++) {
		Node* node = &nodes[node_id];
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

	// Display links
	draw_list->ChannelsSplit(2);
	draw_list->ChannelsSetCurrent(0);
	for (int link_id = 0; link_id < links.Size; link_id++) {
		NodeLink* link = &links[link_id];
		Node* node_inp = &nodes[link->input_id];
		Node* node_out = &nodes[link->output_id];
		ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->input_slot);
		ImVec2 p2 = offset + node_out->GetInputSlotPos(link->output_slot);
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
	}

	// Draw links to mouse
	if (connection_drag && ImGui::IsMouseDown(0)) {
		draw_list->ChannelsSetCurrent(1);
		ImVec2 p1, p2;
		if (drag_connector_is_input) {
			p1 = ImGui::GetIO().MousePos;
			p2 = offset + drag_connector_node->GetInputSlotPos(drag_connector_slot);
		}
		else {
			p1 = offset + drag_connector_node->GetOutputSlotPos(drag_connector_slot);
			p2 = ImGui::GetIO().MousePos;
		}

		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
	} else if (connection_drag && connector_hovered && !ImGui::IsMouseDown(0)) {
		NodeLink new_link(drag_connector_node->id, drag_connector_slot, hover_connector->id, hover_connector_slot);
		if (!drag_connector_is_input) {
			new_link.SwapDirection();
		}
		links.push_back(new_link);
		connection_drag = false;
	}
	else {
		connection_drag = false;
	}

	// Display nodes
	for (int node_id = 0; node_id < nodes.Size; node_id++) {
		Node* node = &nodes[node_id];
		ImGui::PushID(node->id);
		ImVec2 node_rect_min = offset + node->pos;

		// Display node contents
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text("%s", node->name);
		ImGui::Separator();
		ImGui::Text("Hello this is a node");
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

		// Draw line from connector to mouse
		connector_hovered = false;
		for (int j = 0; j < node->inputs_count; j++) {
			ImVec2 con_pos = offset + node->GetInputSlotPos(j);
			if (isConnectorHovered(con_pos, NODE_SLOT_RADIUS) && ImGui::IsMouseDown(0) && !connection_drag) {
					drag_connector_node = node;
					connection_drag = true;
					drag_connector_is_input = true;
					drag_connector_slot = j;
					break;
			}
			else if (isConnectorHovered(con_pos, NODE_SLOT_RADIUS)) {
				connector_hovered = true;
				hover_connector = node;
				hover_connector_is_input = true;
				hover_connector_slot = j;
			}
		}
		
		for (int j = 0; j < node->outputs_count; j++) {
			ImVec2 con_pos = offset + node->GetOutputSlotPos(j);
			if (isConnectorHovered(con_pos, NODE_SLOT_RADIUS) && ImGui::IsMouseDown(0) && !connection_drag) {
				drag_connector_node = node;
				connection_drag = true;
				drag_connector_is_input = false;
				drag_connector_slot = j;
				break;
			}
			else if (isConnectorHovered(con_pos, NODE_SLOT_RADIUS)) {
				connector_hovered = true;
				hover_connector = node;
				hover_connector_is_input = false;
				hover_connector_slot = j;
			}
		}

		bool node_moving_active = ImGui::IsItemActive();

		if (node_widgets_active || node_moving_active)
			node_selected = node->id;
		if (node_moving_active && ImGui::IsMouseDragging(0) && !connector_hovered && !connection_drag)
			node->pos = node->pos + ImGui::GetIO().MouseDelta;

		ImU32 node_bg_color = (node_hovered_in_list == node->id || node_hovered_in_scene == node->id || (node_hovered_in_list == -1 && node_selected == node->id)) ? IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
		for (int slot_id = 0; slot_id < node->inputs_count; slot_id++) {
			ImU32 con_colour = IM_COL32(150, 150, 150, 150);
			ImVec2 con_pos = offset + node->GetInputSlotPos(slot_id);
			if (isConnectorHovered(con_pos, NODE_SLOT_RADIUS)) {
				con_colour = IM_COL32(175, 175, 175, 175);
			}
			draw_list->AddCircleFilled(con_pos, NODE_SLOT_RADIUS, con_colour);
		}
		for (int slot_id = 0; slot_id < node->outputs_count; slot_id++) {
			ImU32 con_colour = IM_COL32(150, 150, 150, 150);
			ImVec2 con_pos = offset + node->GetOutputSlotPos(slot_id);
			if (isConnectorHovered(con_pos, NODE_SLOT_RADIUS)) {
				con_colour = IM_COL32(175, 175, 175, 175);
			}
			draw_list->AddCircleFilled(con_pos, NODE_SLOT_RADIUS, con_colour);
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
		Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node) {
			ImGui::Text("Node '%s'", node->name);
			ImGui::Separator();
			if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
			if (ImGui::MenuItem("Delete")) {
				for (int i = 0; i < links.Size; i++) {
					if (links[i].input_id == node->id || links[i].output_id == node->id) {
						links.erase(&links[i]);
					}
				}
				nodes.erase(node);
			}
			if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		else {
			if (ImGui::MenuItem("Add")) {
				nodes.push_back(Node("New node", scene_pos, ImVec2(0.5f, 0.5f), 2, 2));
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

