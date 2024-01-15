#pragma once
#include "../context.h"
#include "../util/imgui_datechooser.h"
#include <misc/cpp/imgui_stdlib.h>

#include "../util/tcp.h"
#include "../util/util.h"

#include <tcp.h>

#define TREE_NODE_FLAGS (ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap)

namespace Editor {
    void BeginDocking();
    void RenderAccountsWindow(State &state);
    void RenderGeneratorsWindow(State &state);
    void RenderBreedingWindow(State &state);
    void RenderLoginWindow(Window &window, bool &logged_in, TcpConnection &server_conn, State &state);
    void RenderTodoWindow(State &state);
}
