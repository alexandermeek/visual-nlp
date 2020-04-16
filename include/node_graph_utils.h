#include "node.h"
#include "exceptions.h"
#include "module.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <vector>
#include <string>
#include <sstream>
#include <tuple>

using ModuleValue = std::tuple<Node*, Conn_Type, std::string>;

// Forward declarations
void ShowDiagnosticsWindow(bool* p_open, std::vector<std::string>* stats);
void ValueEditor(bool* p_open, ModuleValue* value_to_edit);
void ShowNodeEditor(bool* p_open, Node* node, bool* show_error_popup, bool* show_param_editor, ModuleValue* value_to_edit, std::exception** ex);
void ErrorPopups(bool* show_error_popup, std::exception** ex);
void RunMenu(Node* node, bool* show_error_popup, std::exception** ex);
void RunNode(Node* node, bool force_reruns, bool* show_error_popup, std::exception** ex);

void ShowDiagnosticsWindow(bool* p_open, std::vector<std::string>* stats) {
	if (!ImGui::Begin("Node Graph Diagnostics", p_open)) {
		ImGui::End();
		return;
	}
	for (int i = 0; i < stats->size(); i++) {
		ImGui::Text(stats->at(i).c_str());
	}

	ImGui::End();
}

void ValueEditor(bool* p_open, ModuleValue* value_to_edit) {
	if (!ImGui::Begin("Value Editor", p_open)) {
		ImGui::End();
		return;
	}

	static ModuleValue old_value;

	Node* node;
	Conn_Type conn_type;
	std::string value_name;

	std::tie(node, conn_type, value_name) = *value_to_edit;

	if (node) {
		static char input[200];

		if (conn_type == Conn_Type::input) {
			json* custom_params = node->module->CustomParams();
			if (std::get<0>(old_value) != nullptr && (std::get<0>(old_value)->id != node->id || std::get<2>(old_value) != value_name)) {
				if (custom_params != nullptr && custom_params->find(value_name) != custom_params->end()) {
					strcpy_s(input, custom_params->at(value_name).dump(4).c_str());
				}
				else {
					strcpy_s(input, "");
				}
			}
		}


		ImGui::Text(node->name.c_str());
		ImGui::Separator();
		ImGui::Text(value_name.c_str());

		ImGui::InputTextMultiline("", input, 200);
		if (ImGui::SmallButton("Save")) {
			if (std::strlen(input) <= 0) {
				node->module->RemoveCustomParam(value_name);
			}
			else {
				std::stringstream ss;
				ss << "{ \"" << value_name << "\": " << input << " }";
				node->module->SetCustomParam(json::parse(ss.str()));
			}
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Clear")) {
			node->module->RemoveCustomParam(value_name);
		}
	}
	old_value = *value_to_edit;
	ImGui::End();
}

void ShowNodeEditor(bool* p_open, Node* node, bool* show_error_popup, bool* show_param_editor, ModuleValue* value_to_edit, std::exception** ex) {
	if (!ImGui::Begin("Node Editor", p_open)) {
		ImGui::End();
		return;
	}

	if (node) {
		if (ImGui::Selectable(node->name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick,
			ImVec2(ImGui::GetWindowWidth() - 70, ImGui::GetTextLineHeight()))) {
			if (ImGui::IsMouseDoubleClicked(0)) {
				std::cout << "double click";
			}
		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 50);
		RunMenu(node, show_error_popup, ex);

		ImGui::Separator();
		ImGui::Text("Function: %s", node->module->FunctionName().c_str());
		ImGui::Dummy(ImVec2(2, 2));
		ImGui::Text("Parameters:");
		{
			const std::vector<std::string>* p_names = node->module->ParamNames();
			std::vector<std::string> p_types = node->module->ParamTypesToString();

			if (p_names->size() <= 0) {
				ImGui::SameLine();
				ImGui::Text("None");
			}
			else {
				ImGui::Separator();
				ImGui::Columns(3);
				for (int i = 0; i < p_names->size(); i++) {
					ImGui::Text(p_names->at(i).c_str());
					ImGui::NextColumn();
					ImGui::Text(p_types[i].c_str());
					ImGui::NextColumn();
					std::string button_name = "set##" + std::to_string(i);
					if (ImGui::SmallButton(button_name.c_str())) {
						*show_param_editor = true;
						*value_to_edit = std::make_tuple(node, Conn_Type::input, p_names->at(i));
					}
					ImGui::NextColumn();
				}
			}
		}
		ImGui::Columns(1);
		ImGui::Dummy(ImVec2(2, 2));
		ImGui::Text("Returns:");
		{
			std::vector<std::string> r_types = node->module->ReturnTypesToString();

			if (r_types.size() <= 0) {
				ImGui::SameLine();
				ImGui::Text("None");
			}
			else {
				ImGui::Separator();
				ImGui::Columns(2);
				for (int i = 0; i < r_types.size(); i++) {
					ImGui::NextColumn();
					ImGui::Text(r_types[i].c_str());
					ImGui::NextColumn();
				}
			}
		}
	}

	ImGui::End();
}

void ErrorPopups(bool* show_error_popup, std::exception** ex) { // TODO: add exception details to error popup.
	if (ImGui::BeginPopupModal("Missing Input", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::stringstream msg;
		msg << "Module/node is missing input data." << std::endl;
		if (dynamic_cast<MissingInputException*>(*ex)) {
			MissingInputException* mi_ex = (MissingInputException*)*ex;
			msg.str("");
			msg << "The module: " << mi_ex->module->FunctionName() << " is missing input data." << std::endl
				<< mi_ex->msg << std::endl;
		}

		ImGui::Text(msg.str().c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			if (*ex != nullptr) delete* ex;
			*show_error_popup = false;
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

void RunMenu(Node* node, bool* show_error_popup, std::exception** ex) {
	if (ImGui::BeginMenu("Run")) {
		if (ImGui::IsMouseClicked(0)) {
			RunNode(node, false, show_error_popup, ex);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Run missing")) {
			RunNode(node, false, show_error_popup, ex);
		}
		if (ImGui::MenuItem("Run all (force reruns)")) {
			RunNode(node, true, show_error_popup, ex);
		}
		ImGui::EndMenu();
	}
}

void RunNode(Node* node, bool force_reruns, bool* show_error_popup, std::exception** ex) {
	try {
		node->Run(force_reruns);
	}
	catch (MissingInputException e) {
		*ex = new MissingInputException(e);
		*show_error_popup = true;
	}
}