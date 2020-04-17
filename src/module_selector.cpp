#include "node_vec.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

void ShowModuleSelector(bool* p_open, NodeVec* nodes) {
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
    for (int i = 0; i < 100; i++)
    {
        char label[128];
        sprintf_s(label, "MyObject %d", i);
        if (ImGui::Selectable(label, selected == i))
            selected = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // right
    ImGui::BeginGroup();
    ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
    ImGui::Text("MyObject: %d", selected);
    ImGui::Separator();
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Description"))
        {
            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Details"))
        {
            ImGui::Text("ID: 0123456789");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
    if (ImGui::Button("Revert")) {}
    ImGui::SameLine();
    if (ImGui::Button("Save")) {}
    ImGui::EndGroup();

	ImGui::End();
}