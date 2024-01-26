#include "imprints.h"
#include "state.h"
#include "editor/ui.h"
#include "util/util.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

static std::string PrintInt(int v) {
    return std::to_string(v);
}

static std::string SizeAsTime(size_t time) {
    char buf[32];
    snprintf(buf, 32, "%zu:%.2zu", time/60, time%60);
    return {buf};
}

void Imprint::Display(State &state) {
    bool remove = false;
    for(size_t i = 0; i < this->tames.size(); i++) {
        auto &tame = this->tames[i];
        std::string node_name = this->acc + tame.name;
        ImGui::PushID(node_name.c_str());
        bool active = ImGui::TreeNodeEx(tame.name.c_str(), TREE_NODE_FLAGS);
        ImGui::PopID();
        if(ImGui::BeginPopupContextItem()) {
            std::string edit_popup_name = this->acc + tame.name + "EditPopup";
            float x_avail = ImGui::GetContentRegionAvail().x;
            if(ImGui::Button("Edit##Tame", {x_avail, 0})) {
                ImGui::OpenPopup(edit_popup_name.c_str());
            }
            if(ImGui::Button("Delete##Tame", {x_avail, 0})) {
                remove = true;
            }
            auto win_size = Context::GetWindow().GetSize();
            ImGuiWindowFlags popup_flags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
            ImGui::SetNextWindowViewport(ImGui::GetCurrentWindow()->ViewportId);
            ImGui::SetNextWindowPos(ImVec2(Context::GetWindow().GetPos().x, Context::GetWindow().GetPos().y) + Center({win_size.x, win_size.y}, {200, 100}), ImGuiCond_Always);
            if(ImGui::BeginPopupModal(edit_popup_name.c_str(), NULL, popup_flags)) {
                ImGui::SetWindowSize({400, 0}, ImGuiCond_Once);
                size_t hours = tame.needs_imprint / 60;
                size_t minutes = tame.needs_imprint % 60;
                bool changed = false;
                ImGui::Text("Loc:");
                ImGui::SameLine(112);
                if(ImGui::InputText("##TameLocEdit", &tame.loc)) {
                    changed = true;
                }
                ImGui::Text("Needs Imprint:");
                ImGui::SameLine(112);
                if(ImGui::DurationEdit("##TameNeedImprintEdit", (int *)&hours, (int *)&minutes)) {
                    changed = true;
                }
                ImGui::Text("Amount:");
                ImGui::SameLine(112);
                if(ImGui::InputInt("##TameAmountEdit", (int*)&tame.amount, 1, 100, ImGuiInputTextFlags_CharsDecimal)) {
                    changed = true;
                }
                ImGui::Text("Needs food:");
                ImGui::SameLine(112);
                if(ImGui::Checkbox("##TameFoodEdit", &tame.watch_food)) {
                    changed = true;
                }

                if(changed) {
                    tame.needs_imprint = hours * 60 + minutes;
                    state.SetUpdated();
                }

                AlignMultipleElemetsOnLine({50});
                if(ImGui::Button("Close", { 50, 0 })) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::EndPopup();
        }

        if(active) {
            ImGui::Text("Loc: %s", tame.loc.c_str());
            ImGui::Text("Needs Imprint: %s", SizeAsTime(tame.needs_imprint).c_str());
            ImGui::Text("Amount: %zu", tame.amount);
            ImGui::Text("Maybe needs food: %s", tame.watch_food ? "true" : "false");
            ImGui::TreePop();
        }

        if(remove) {
            this->tames.erase(this->tames.cbegin() + i);
            state.SetUpdated();
        }
    }
}
