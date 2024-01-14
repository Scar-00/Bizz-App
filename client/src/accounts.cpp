#include "accounts.h"
#include "context.h"
#include "util/util.h"
#include <misc/cpp/imgui_stdlib.h>
#include "state.h"

void Account::ChangePassword(std::string &new_password) {
    password = new_password;
}

bool Account::Display(State &state) {
    bool remove = false;
    auto win_size = Context::GetWindow().GetSize();
    std::string change_popup_name = name + "PasswordChangeContext";
    ImGuiWindowFlags popup_flags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::SetNextWindowPos(ImVec2(Context::GetWindow().GetPos().x, Context::GetWindow().GetPos().y) + Center({win_size.x, win_size.y}, {200, 100}), ImGuiCond_Always);
    if(ImGui::BeginPopupModal(change_popup_name.c_str(), NULL, popup_flags)) {
        ImGui::SetWindowSize({200, 60}, ImGuiCond_Once);
        static std::string new_password = {0};
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputText("##TextPassword", &new_password);
        ImGui::PopItemWidth();
        AlignMultipleElemetsOnLine({50, 50});
        if(ImGui::Button("Ok", { 50, 0 })) {
            this->ChangePassword(new_password);
            state.SetUpdated();
            new_password.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button("Close", { 50, 0 })) {
            new_password.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    std::string remove_popup_name = name + "AccountRemoveContext";
    if(ImGui::BeginPopup(remove_popup_name.c_str())) {
        if(ImGui::Button("Remove")) {
            remove = true;
        }
        ImGui::EndPopup();
    }

    ImGui::TableNextColumn();
    ImGui::Selectable(name.c_str());
    if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(remove_popup_name.c_str());
    }
    ImGui::TableNextColumn();
    if(ImGui::Selectable(password.c_str())) {
        ImGui::OpenPopup(change_popup_name.c_str());
    }
    return remove;
}
