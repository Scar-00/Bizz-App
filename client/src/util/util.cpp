#include "util.h"

std::string PrintTime(tm t) {
    char buf[128];
    strftime(buf, 128, "%d-%m-%Y", &t);
    return {buf};
}

tm TimeNow() {
    time_t now;
    time(&now);
    return *localtime(&now);
}

tm TimeFromPtr(const char *time) {
    tm tm;
    strptime(time, "%d.%m.%Y", &tm);
    return tm;
}

f32 CenterX(f32 window_width, f32 target_width) {
    return (window_width / 2) - (target_width);
}

f32 CenterY(f32 window_height, f32 target_height) {
    return (window_height / 2) - (target_height);
}

ImVec2 Center(ImVec2 window_dims, ImVec2 target_dims) {
    return {CenterX(window_dims.x, target_dims.x), CenterY(window_dims.y, target_dims.y)};
}

void AlignForWidth(float width, float alignment)
{
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

void AlignMultipleElemetsOnLine(std::vector<f32> sizes) {
    ImGuiStyle& style = ImGui::GetStyle();
    float width = 0.0f;
    for(const auto item : sizes) {
        width += item;
        width += style.ItemSpacing.x;
    }
    AlignForWidth(width);
}
