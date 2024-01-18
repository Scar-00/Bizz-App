#include "ui.h"
#include "../context.h"
#include "../state.h"
#include "../generators.h"
#include "../accounts.h"
#include "../imprints.h"

#include <imgui.h>
#include <vector>

namespace Editor {
void BeginDocking() {
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }else {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Main_Window", NULL, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if(ImGui::BeginMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Open")) { }
            if(ImGui::MenuItem("Save")) { }
            if(ImGui::MenuItem("Save As")) {  }
            if(ImGui::MenuItem("Test")) {
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Window")) {
            if(ImGui::MenuItem("Editor")) {  }
            if(ImGui::MenuItem("Test")) {

            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void RenderAccountsWindow(State &state) {
    ImGui::Begin("Accounts");
    if (ImGui::BeginTable("##Accounts", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders)) {
        for(size_t i = 0; i < state.accounts.size(); i ++) {
            if(state.accounts[i].Display(state)) {
                state.accounts.erase(state.accounts.cbegin() + i);
                state.SetUpdated();
            }
        }
        ImGui::EndTable();
    }

    if(ImGui::Button("+", {20, 20})) {
        ImGui::OpenPopup("AddAccountPopup");
    }

    auto win_size = Context::GetWindow().GetSize();
    ImGuiWindowFlags popup_flags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowViewport(ImGui::GetCurrentWindow()->ViewportId);
    ImGui::SetNextWindowPos(ImVec2(Context::GetWindow().GetPos().x, Context::GetWindow().GetPos().y) + Center({win_size.x, win_size.y}, {200, 100}), ImGuiCond_Always);
    if(ImGui::BeginPopupModal("AddAccountPopup", NULL, popup_flags)) {
        ImGui::SetWindowSize({300, 0}, ImGuiCond_Once);
        static std::string name = {0};
        static std::string password = {0};
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::Text("Name:");
        ImGui::SameLine(112);
        ImGui::InputText("##Name", &name);
        ImGui::Text("Password:");
        ImGui::SameLine(112);
        ImGui::InputText("##Password", &password);

        ImGui::PopItemWidth();
        AlignMultipleElemetsOnLine({50, 50});
        if(ImGui::Button("Ok", { 50, 0 })) {
            state.accounts.push_back(Account{name, password});
            state.SetUpdated();
            name.clear();
            password.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button("Close", { 50, 0 })) {
            name.clear();
            password.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void RenderGeneratorsWindow(State &state) {
    ImGui::Begin("Generators");
    if (ImGui::BeginTable("##Generators", 4, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("Location");
        ImGui::TableSetupColumn("Element");
        ImGui::TableSetupColumn("Last Filled");
        ImGui::TableSetupColumn("Filling Needed");
        ImGui::TableHeadersRow();
        for(size_t i = 0; i < state.generators.size(); i ++) {
            if(state.generators[i].Display()) {
                state.generators.erase(state.generators.cbegin() + i);
                state.SetUpdated();
            }
        }
        ImGui::EndTable();
    }
    if(ImGui::Button("+", {20, 20})) {
        ImGui::OpenPopup("AddGeneratorPopup");
    }

    auto win_size = Context::GetWindow().GetSize();
    ImGuiWindowFlags popup_flags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowViewport(ImGui::GetCurrentWindow()->ViewportId);
    ImGui::SetNextWindowPos(ImVec2(Context::GetWindow().GetPos().x, Context::GetWindow().GetPos().y) + Center({win_size.x, win_size.y}, {200, 100}), ImGuiCond_Always);
    if(ImGui::BeginPopupModal("AddGeneratorPopup", NULL, popup_flags)) {
        ImGui::SetWindowSize({300, 0}, ImGuiCond_Once);
        static std::string location = {0};
        static std::string element = {0};
        static tm fill_time = {0};
        static tm fill_needed = {0};
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::Text("Location:");
        ImGui::SameLine(112);
        ImGui::InputText("##Location", &location);
        ImGui::Text("Element:");
        ImGui::SameLine(112);
        ImGui::InputText("##Element", &element, ImGuiInputTextFlags_CharsDecimal);
        ImGui::Text("Filling Date:");
        ImGui::SameLine(112);
        ImGui::DateChooser("##FillTime", fill_time);
        ImGui::Text("Filling Needed:");
        ImGui::SameLine(112);
        ImGui::DateChooser("##FillNeeded", fill_needed);

        ImGui::PopItemWidth();
        AlignMultipleElemetsOnLine({50, 50});
        if(ImGui::Button("Ok", { 50, 0 })) {
            state.generators.push_back(Generator{ location, (size_t)std::stoi(element), fill_time, fill_needed });
            state.SetUpdated();
            location.clear();
            element.clear();
            fill_time = {0};
            fill_needed = {0};
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button("Close", { 50, 0 })) {
            location.clear();
            element.clear();
            fill_time = {0};
            fill_needed = {0};
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}

void RenderBreedingWindow(State &state) {
    ImGui::Begin("Breeding");
    for(auto imprint : state.imprints) {
        std::string add_tame_popup = std::string{imprint.acc} + "AddTamePopup";

        if(ImGui::BeginPopupModal(add_tame_popup.c_str())) {

            ImGui::EndPopup();
        }

        bool active = ImGui::TreeNodeEx(imprint.acc, TREE_NODE_FLAGS);
        if(ImGui::BeginPopupContextItem()) {
            if(ImGui::Button("Add")) {
                ImGui::OpenPopup(add_tame_popup.c_str());
            }
            ImGui::EndPopup();
        }
        if(active) {
            imprint.Display(state);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

#define LOGIN_PADDING 112

void RenderLoginWindow(Window &window, bool &logged_in, TcpConnection &server_conn, State &state) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::SetNextWindowPos({window.GetPos().x, window.GetPos().y}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ window.GetSize().x, window.GetSize().y });
    ImGui::Begin("Login", NULL, flags);
    static std::string ip_string = DEFAULT_IP;
    static std::string password = {0};
    ImGui::Text("Ip:");
    ImGui::SameLine(LOGIN_PADDING);
    ImGui::InputText("Ip", &ip_string);
    ImGui::Text("Password:");
    ImGui::SameLine(LOGIN_PADDING);
    ImGui::InputText("Password", &password);
    ImGui::SetCursorPosX(CenterX(window.GetSize().x, 80) + 40);
    if(ImGui::Button("Ok", {80, 0})) {
        server_conn.Connect(ip_string.c_str());
        state = server_conn.QueryState();
        logged_in = true;
        window.SetSize({1904, 1080});
    }
    ImGui::End();
}

void RenderTodoWindow(State &state) {
    ImGui::Begin("Todo's");


    ImGui::End();
}
}
