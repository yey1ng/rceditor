# include "widgets.h"
#include <imgui_node_editor/imgui_extra_math.h>

void ax::Widgets::Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color/* = ImVec4(1, 1, 1, 1)*/, const ImVec4& innerColor/* = ImVec4(0, 0, 0, 0)*/)
{
    if (ImGui::IsRectVisible(size))
    {
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto drawList  = ImGui::GetWindowDrawList();
        ax::Drawing::DrawIcon(drawList, cursorPos, cursorPos + size, type, filled, ImColor(color), ImColor(innerColor));
    }

    ImGui::Dummy(size);
}

void ax::Widgets::Icon(const ImVec2& size, const ImTextureID i_ID)
{
    if (ImGui::IsRectVisible(size))
    {
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto drawList = ImGui::GetWindowDrawList();
        ax::Drawing::DrawIcon(i_ID, drawList);
    }

    ImGui::Dummy(size);
}
