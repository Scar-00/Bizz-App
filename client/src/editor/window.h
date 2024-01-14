#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Window {
public:
    Window(glm::vec2 size);
    ~Window();
    void RenderBegin();
    void RenderEnd();
    GLFWwindow *GetHandle() const { return handle; }
    bool ShouldClose() { return glfwWindowShouldClose(handle); }
    void SetSize(glm::vec2 size) {
        this->size = size;
        glfwSetWindowSize(this->handle, size.x, size.y);
    }
    glm::vec2 GetSize() const { return size; }
    glm::vec2 GetPos() const {
        int xpos, ypos;
        glfwGetWindowPos(this->handle, &xpos, &ypos);
        return {xpos, ypos};
    }
private:
    GLFWwindow *handle;
    glm::vec2 size;
};
