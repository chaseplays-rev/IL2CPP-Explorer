#include "menu.h"

void Menu::Draw()
{
    ImGui::SetNextWindowSize(
        ImVec2(760, 500),
        ImGuiCond_FirstUseEver
    );

    ImGui::Begin(
        "IL2CPP Runtime Explorer",
        nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );

    ImVec2 windowSize =
        ImGui::GetContentRegionAvail();

    ImGui::SameLine();

    ImGui::BeginChild(
        "##MainContent",
        ImVec2(0, windowSize.y),
        false
    );

    ImGui::Dummy(ImVec2(0, 12));

    ImGui::EndChild();
    ImGui::End();
}
