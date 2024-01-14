#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "editor/window.h"
#include <imgui.h>
#include <imgui_internal.h>

class Context {
public:
    static Context *Create(Window &window);
    static void Destroy();
    static const Window & GetWindow();
    void operator=(const Context &) = delete;
    Context(Context &copy) = delete;
private:
    Context(Window &window);
    ~Context();
    Window &window;
};
