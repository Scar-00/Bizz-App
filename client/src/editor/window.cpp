#include "window.h"
#include "GLFW/glfw3.h"
#include <cstdlib>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad.h>
#include <stdio.h>
#include "../context.h"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

Window::Window(glm::vec2 size): size(size) {
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit())
        std::exit(1);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    handle = glfwCreateWindow(size.x, size.y, "BizZ Manager", NULL, NULL);
    if(!handle)
        std::exit(1);


    glfwMakeContextCurrent(handle);
    glfwSwapInterval(1);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "error initializing GLAD\n");
        glfwTerminate();
        std::exit(1);
    }
}

Window::~Window() {
    //glfwDestroyWindow(handle);
    glfwTerminate();
}

void Window::RenderBegin() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::RenderEnd() {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(handle, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    auto io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(handle);
}
