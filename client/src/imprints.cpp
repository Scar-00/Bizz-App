#include "imprints.h"
#include "imgui.h"
#include "state.h"
#include "editor/ui.h"
#include "util/util.h"

static std::string PrintInt(int v) {
    char buf[64];
    itoa(v, buf, 10);
    return {buf};
}

void Imprint::Display(State &state) {
    for(const auto &tame : this->tames) {
        std::string node_name = this->acc + tame.name;
        ImGui::PushID(node_name.c_str());
        bool active = ImGui::TreeNodeEx(tame.name.c_str(), TREE_NODE_FLAGS);
        ImGui::PopID();
        if(active) {
            ImGui::Text("Loc: %s", tame.loc.c_str());
            ImGui::Text("Needs Imprint: %s", PrintTime(tame.needs_imprint, "%H:%M").c_str());
            ImGui::Text("Amount: %zu", tame.amount);
            ImGui::Text("Maybe needs food: %d", tame.watch_food);
            ImGui::TreePop();
        }
    }
}
