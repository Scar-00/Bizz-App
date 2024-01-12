#pragma once

#include "editor/window.h"
#include <memory>
#include <tuple>
#include <vector>
#include <string>
#include <tuple>
#include "types.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <unordered_map>
#include <set>

enum class Image {
    Text,
    Dir,
    Image,
};

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
