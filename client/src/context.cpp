#include "context.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <memory>
#include <tuple>
#include <vector>

static Context *self = nullptr;

Context::Context(Window &window): window(window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    //font = io.Fonts->AddFontFromFileTTF("C:/Users/fiffi/OneDrive/Desktop/Coding/gui/res/fonts/consola.ttf", 18);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.GetHandle(), true);
    ImGui_ImplOpenGL3_Init();
}

Context::~Context() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

Context *Context::Create(Window &window) {
    if(self == nullptr) {
        self = new Context(window);
    }
    return self;
}

void Context::Destroy() {
    delete self;
}

const Window &Context::GetWindow() {
    return self->window;
}
