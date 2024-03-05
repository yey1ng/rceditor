# pragma once
# define IMGUI_DEFINE_MATH_OPERATORS
# include <imgui.h>

namespace ax {
namespace Drawing {

enum class IconType: ImU32 { Flow, Circle, Square, Grid, RoundSquare, Diamond, Function, ForEach, Branch };

void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor);
void DrawIcon(ImTextureID i_ID, ImDrawList* drawList);

} // namespace Drawing
} // namespace ax