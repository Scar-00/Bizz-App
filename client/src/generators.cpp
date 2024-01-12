#include "generators.h"
#include "context.h"
#include "util/imgui_datechooser.h"
#include "util/util.h"
#include <misc/cpp/imgui_stdlib.h>

bool Generator::Display() {
    bool remove = false;

    std::string remove_popup_name = this->location + "GeneratorRemoveContext";
    if(ImGui::BeginPopup(remove_popup_name.c_str())) {
        if(ImGui::Button("Remove")) {
            remove = true;
        }
        ImGui::EndPopup();
    }

    ImGui::TableNextColumn();
    ImGui::Selectable(this->location.c_str());
    if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(remove_popup_name.c_str());
    }
    ImGui::TableNextColumn();
    ImVec4 col = {1, 1, 1, 1};
    if(this->element < 3) {
        col = {1, 0, 0, 1};
    }
    ImGui::TextColored(col, "%zu", this->element);
    ImGui::TableNextColumn();
    ImGui::Text("%s", PrintTime(this->date_filled).c_str());
    ImGui::TableNextColumn();
    ImGui::Text("%s", PrintTime(this->date_needs_refill).c_str());
    return remove;
}

