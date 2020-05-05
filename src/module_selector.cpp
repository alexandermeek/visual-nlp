#include "node_vec.h"
#include "module_loader.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <sstream>

void ShowModuleSelector(bool* p_open, ModuleLoader* module_loader, NodeVec* nodes) {
	ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Module Selector", p_open, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // left
    static int selected = 0;
    ImGui::BeginChild("left pane", ImVec2(150, 0), true);
    for (int i = 0; i < module_loader->NumModules(); i++)
    {
        if (ImGui::Selectable(module_loader->Modules()[i]->FunctionName().c_str(), selected == i))
            selected = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // right
	Module* selected_module = module_loader->Modules()[selected];
    ImGui::BeginGroup();
    ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
    ImGui::Text(selected_module->FunctionName().c_str());
    ImGui::Separator();
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Description"))
        {
			std::stringstream ss;
			ss << "From script file: " << selected_module->ScriptFile();
			if (dynamic_cast<ModulePy*>(selected_module)) ss << ".py" << std::endl << "Module Type: python";
            ImGui::TextWrapped(ss.str().c_str());
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Details"))
        {
			if (selected_module) {
				ImGui::Text("Parameters:");
				{
					const std::vector<std::string>* p_names = selected_module->ParamNames();
					std::vector<std::string> p_types = selected_module->ParamTypesToString();

					if (p_names->size() <= 0) {
						ImGui::SameLine();
						ImGui::Text("None");
					}
					else {
						ImGui::Separator();
						ImGui::Columns(2);
						for (int i = 0; i < p_names->size(); i++) {
							ImGui::Text(p_names->at(i).c_str());
							ImGui::NextColumn();
							ImGui::Text(p_types[i].c_str());
							ImGui::NextColumn();
						}
					}
				}

				ImGui::Columns(1);
				ImGui::Dummy(ImVec2(2, 2));
				ImGui::Text("Returns:");
				{
					std::vector<std::string> r_types = selected_module->ReturnTypesToString();

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
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
    if (ImGui::Button("Create Node")) {
		if (selected_module) {
			nodes->AddNode(new Node("New Node", new ModulePy(*(ModulePy*)selected_module)));
		}
	}
    ImGui::SameLine();
    if (ImGui::Button("Save")) {}
    ImGui::EndGroup();

	ImGui::End();
}