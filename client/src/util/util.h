#pragma once

#include "../context.h"
#include "../util/imgui_datechooser.h"
#include "../types.h"

#include <cstddef>
#include <misc/cpp/imgui_stdlib.h>
#include <ctime>
#include <string>
#include <vector>

#define DEFAULT_IP "192.168.0.80"

std::string PrintTime(tm t, const char *format = NULL);
tm TimeNow();
tm TimeFromPtr(const char *time);
f32 CenterX(f32 window_width, f32 target_width);
f32 CenterY(f32 window_height, f32 target_height);
ImVec2 Center(ImVec2 window_dims, ImVec2 target_dims);
void AlignForWidth(float width, float alignment = 0.5f);
void AlignMultipleElemetsOnLine(std::vector<f32> sizes);
